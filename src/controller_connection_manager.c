/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Flora License, Version 1.1 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "controller_connection_manager.h"
#include "messages/message_manager.h"
#include "messages/message_command.h"
#include "messages/message_ack.h"
#include "messages/message_factory.h"
#include <string.h>
#include <glib.h>
#include "log.h"
#include "assert.h"

#define KEEP_ALIVE_CHECK_ATTEMPTS 5
#define HELLO_ACCEPT_ATTEMPTS 5
#define HELLO_ACCEPT_INTERVAL 1000 //In ms
#define KEEP_ALIVE_CHECK_INTERVAL 1000 //In ms

#define SAFE_SOURCE_REMOVE(source)\
do { \
	if(source) { \
		g_source_remove(source); \
	} \
	source = 0; \
} while(0)

typedef struct _controller_connection_manager_info {
	controller_connection_state_e state;
	char *controller_address;
	int controller_port;
	connection_state_cb state_cb;
	command_received_cb command_cb;
	int keep_alive_check_attempts_left;
	int connect_accept_attempts_left;
	guint connect_accept_timer;
	guint keep_alive_check_timer;
	unsigned long long int last_serial;
	message_factory_t *message_factory;
} _controller_connection_manager_s;

static _controller_connection_manager_s s_info = {
	.state = CONTROLLER_CONNECTION_STATE_READY,
	.controller_address = NULL,
	.state_cb = NULL,
	.keep_alive_check_attempts_left = KEEP_ALIVE_CHECK_ATTEMPTS,
	.connect_accept_attempts_left = HELLO_ACCEPT_ATTEMPTS,
	.connect_accept_timer = 0,
	.keep_alive_check_timer = 0
};

static int _try_connect(const char *ip, int port);
static void _disconnect();
static void _set_state(controller_connection_state_e state);
static void _receive_cb(message_t *message, void *data);
static void _reset_counters();
static gboolean _send_connect_accept();
static gboolean _connect_accept_timer_cb(gpointer data);
static gboolean _keep_alive_check_timer_cb(gpointer data);
static int _addr_cmp(const char *addr1, int port1, const char *addr2, int port2);

int controller_connection_manager_listen()
{
	s_info.message_factory = message_factory_create();
	if(!s_info.message_factory) {
		return -1;
	}
	message_manager_set_receive_message_cb(_receive_cb, NULL);
	return 0;
}

controller_connection_state_e controller_connection_manager_get_state()
{
	return s_info.state;
}

void controller_connection_manager_set_state_change_cb(connection_state_cb callback)
{
	s_info.state_cb = callback;
}

void controller_connection_manager_set_command_received_cb(command_received_cb callback)
{
	s_info.command_cb = callback;
}

void controller_connection_manager_handle_message(message_t *message)
{
	if(!s_info.message_factory) {
		_E("Message factory not initialized");
		return;
	}
	const char *msg_address;
	int msg_port;
	message_get_sender(message, &msg_address, &msg_port);
	int address_match = !_addr_cmp(s_info.controller_address, s_info.controller_port, msg_address, msg_port);

	switch(message_get_type(message)) {
	case MESSAGE_CONNECT:
		if(s_info.state == CONTROLLER_CONNECTION_STATE_READY) {
			if(_try_connect(msg_address, msg_port)) {
				_E("Received CONNECT, but cannot establish connection");
			} else {
				s_info.last_serial = message_get_serial(message);
				_I("Established connection with %s:%d", s_info.controller_address, s_info.controller_port);
			}
		} else {
			message_t *response = message_factory_create_message(s_info.message_factory, MESSAGE_CONNECT_REFUSED);
			if(!response) {
				_W("Failed to create CONNECT_REFUSED message");
				break;
			}
			message_set_receiver(response, msg_address, msg_port);
			message_manager_send_message(response);
			message_destroy(response);
		}
		break;
	case MESSAGE_KEEP_ALIVE:
		if(s_info.state == CONTROLLER_CONNECTION_STATE_RESERVED && address_match) {
			unsigned long long int serial = message_get_serial(message);
			if(serial > s_info.last_serial) {
				SAFE_SOURCE_REMOVE(s_info.connect_accept_timer);
				s_info.keep_alive_check_attempts_left = KEEP_ALIVE_CHECK_ATTEMPTS;
				message_ack_t response;
				message_ack_init_from_request(&response, message);
				message_set_receiver((message_t*)&response, s_info.controller_address, s_info.controller_port);
				message_manager_send_message((message_t*)&response);
				message_destroy((message_t*)&response);
				s_info.last_serial = serial;
			} else {
				_W("Received late KEEP_ALIVE (%d, when last is %d)", serial, s_info.last_serial);
			}
		} else {
			_W("Unexpectedly received KEEP_ALIVE from %s:%d (address_match == %d)", msg_address, msg_port, address_match);
		}
		break;
	case MESSAGE_COMMAND:
		if(s_info.state == CONTROLLER_CONNECTION_STATE_RESERVED && address_match) {
			const command_s *command = message_command_get_command((message_command_t *) message);
			if(!command) {
				_E("Failed to obtain command");
				break;
			}
			if(s_info.command_cb) {
				s_info.command_cb(*command);
			}
		} else {
			_W("Unexpectedly received COMMAND from %s:%d (address_match == %d)", msg_address, msg_port, address_match);
		}
		break;
	case MESSAGE_BYE:
		if(s_info.state == CONTROLLER_CONNECTION_STATE_RESERVED && address_match) {
			_disconnect();
		} else {
			_W("Unexpectedly received BYE from %s:%d (address_match == %d)", msg_address, msg_port, address_match);
		}
		break;
	default:
		_W("Received incorrect message");
	}
}

void controller_connection_manager_release()
{
	if(s_info.state == CONTROLLER_CONNECTION_STATE_RESERVED) {
		_disconnect();
	}
	message_factory_destroy(s_info.message_factory);
	message_manager_shutdown();
	s_info.message_factory = NULL;
}

static void _set_state(controller_connection_state_e state)
{
	if(state == s_info.state) {
		return;
	}

	controller_connection_state_e previous = s_info.state;
	s_info.state = state;
	_I("Connection state changed from %d to %d", previous, state);
	if(s_info.state_cb) {
		s_info.state_cb(previous, state);
	}
}

static void _receive_cb(message_t *message, void *data)
{
	controller_connection_manager_handle_message(message);
}

static int _try_connect(const char *ip, int port)
{
	if(s_info.state != CONTROLLER_CONNECTION_STATE_READY) {
		_E("Attempt to connect failed - already reserved by %s:%d", s_info.controller_address, s_info.controller_port);
		return -1;
	}

	s_info.controller_address = strdup(ip);
	if(!s_info.controller_address) {
		_E("Failed to save controller address");
		return -1;
	}

	s_info.controller_port = port;
	_set_state(CONTROLLER_CONNECTION_STATE_RESERVED);
	if(!_send_connect_accept()) {
		_E("Failed to send CONNECT_ACCEPT");
	}
	_reset_counters();
	s_info.connect_accept_timer = g_timeout_add(HELLO_ACCEPT_INTERVAL, _connect_accept_timer_cb, NULL);
	s_info.keep_alive_check_timer = g_timeout_add(KEEP_ALIVE_CHECK_INTERVAL, _keep_alive_check_timer_cb, NULL);
	return 0;
}

static void _disconnect()
{
	if(s_info.state == CONTROLLER_CONNECTION_STATE_READY) {
		_W("No connection already initiated");
		return;
	}

	SAFE_SOURCE_REMOVE(s_info.connect_accept_timer);

	SAFE_SOURCE_REMOVE(s_info.keep_alive_check_timer);

	free(s_info.controller_address);
	s_info.controller_port = 0;
	_set_state(CONTROLLER_CONNECTION_STATE_READY);
}

static gboolean _send_connect_accept()
{
	if(s_info.state != CONTROLLER_CONNECTION_STATE_RESERVED) {
		_E("Car is not reserved");
		return FALSE;
	}
	if(!--s_info.connect_accept_attempts_left) {
		_W("Connect accepted, but no KEEP ALIVE received - disconnecting started");
		_disconnect();
		return FALSE;
	}
	message_t *message = message_factory_create_message(s_info.message_factory, MESSAGE_CONNECT_ACCEPTED);
	message_set_receiver(message, s_info.controller_address, s_info. controller_port);
	message_manager_send_message(message);
	message_destroy(message);
	return TRUE;
}

static gboolean _connect_accept_timer_cb(gpointer data)
{
	return _send_connect_accept();
}

static gboolean _keep_alive_check_timer_cb(gpointer data)
{
	if(s_info.state != CONTROLLER_CONNECTION_STATE_RESERVED) {
		_E("Incorrect state of connection");
	}

	if(!s_info.keep_alive_check_attempts_left--) {
		_W("KEEP ALIVE timeout reached - disconnecting started");
		_disconnect();
		return FALSE;
	}
	return TRUE;
}

static void _reset_counters()
{
	s_info.keep_alive_check_attempts_left = KEEP_ALIVE_CHECK_ATTEMPTS;
	s_info.connect_accept_attempts_left = HELLO_ACCEPT_ATTEMPTS;
}

static int _addr_cmp(const char *addr1, int port1, const char *addr2, int port2)
{
	if(addr1 == NULL || addr2 == NULL) {
		return -1;
	}

	unsigned int address_length = strlen(addr2);
	return port1 != port2 || strlen(addr1) != address_length || strncmp(addr1, addr2, address_length);
}
