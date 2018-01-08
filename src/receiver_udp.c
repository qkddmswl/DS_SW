/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

#include <stdlib.h>
#include <glib.h>
#include <gio/gio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "log.h"
#include "receiver_internal.h"

#define RECEIVER_UDP_PORT 57984
#define RECEIVER_UDP_WAIT_TIMEOUT 3

typedef enum __receiver_udp_state_e {
	RECEIVER_UDP_STATE_NONE,
	RECEIVER_UDP_STATE_READY,
	RECEIVER_UDP_STATE_CONNECTED,
} receiver_udp_state_e;

typedef struct __receiver_udp_h {
	guint io_watch_id;
	guint wait_timer_id;
	receiver_udp_state_e state;
	GSocket *socket;
} receiver_udp_h;

static receiver_udp_h *udp_handle = NULL;

static gchar *__socket_address_to_string(GSocketAddress *address)
{
	GInetAddress *inet_address = NULL;
	char *str, *res;
	int port;

	inet_address =
		g_inet_socket_address_get_address(G_INET_SOCKET_ADDRESS(address));

	str = g_inet_address_to_string(inet_address);
	port = g_inet_socket_address_get_port(G_INET_SOCKET_ADDRESS(address));

	res = g_strdup_printf("%s:%d", str, port);
	g_free(str);

	return res;
}

static int __receiver_udp_init(void)
{
	if (udp_handle) {
		_E("receiver udp is already initialized");
		return -1;
	}

	udp_handle = malloc(sizeof(receiver_udp_h));
	if (!udp_handle) {
		_E("failed to alloc receiver udp handle");
		return -1;
	}

	udp_handle->state = RECEIVER_UDP_STATE_NONE;
	udp_handle->io_watch_id = 0;
	udp_handle->wait_timer_id = 0;
	udp_handle->socket = NULL;

	return 0;
}

static int __receiver_udp_fini(void)
{
	if (udp_handle) {
		if (udp_handle->io_watch_id)
			g_source_remove(udp_handle->io_watch_id);

		if (udp_handle->wait_timer_id)
			g_source_remove(udp_handle->wait_timer_id);

		if (udp_handle->socket) {
			g_socket_close(udp_handle->socket, NULL);
			g_object_unref(udp_handle->socket);
		}

		free(udp_handle);
		udp_handle = NULL;
	}

	return 0;
}

/* Uses system call, because glib socket API doesn't support unset connect
 * Please carefully use this function, and after use this function,
 * DO NOT use g_socket_is_connected().
 */
static int __receiver_udp_unset_connection(void)
{
	struct sockaddr addr;
	int s_fd = 0;

	retvm_if(!udp_handle, -1, "handle is not created");
	retvm_if(!udp_handle->socket, -1, "socket is not created");

	s_fd = g_socket_get_fd(udp_handle->socket);
	bzero((char *)&addr, sizeof(addr));
	addr.sa_family = AF_UNSPEC;

	if (connect(s_fd, &addr, sizeof(addr))) {
		_E("failed to unset connect - %s\n", strerror(errno));
		/* re-create socket or not ??? */
		return -1;
	}
	udp_handle->state = RECEIVER_UDP_STATE_READY;
	_D("unset connection");

	return 0;
}

static int __receiver_udp_set_connection(GSocketAddress *sender_a)
{
	GError *error = NULL;

	retv_if(!sender_a, -1);
	retvm_if(!udp_handle, -1, "handle is not created");
	retvm_if(!udp_handle->socket, -1, "socket is not created");

	if (udp_handle->state != RECEIVER_UDP_STATE_READY) {
		_E("check state %d", udp_handle->state);
		return -1;
	}

	/* use connect() to specify sender address and reject other sender */
	if (!g_socket_connect(udp_handle->socket, sender_a, NULL, &error)) {
		_E("failed to connect - %s", error->message);
		g_error_free(error);
		return -1;
	}
	udp_handle->state = RECEIVER_UDP_STATE_CONNECTED;
	_D("set connection");

	return 0;
}

static gboolean __wait_time_out(gpointer user_data)
{
	__receiver_udp_unset_connection();

	return FALSE;
}

static void __receiver_udp_update_wait_timer(void)
{
	if (udp_handle) {
		if (udp_handle->wait_timer_id) {
			g_source_remove(udp_handle->wait_timer_id);
			udp_handle->wait_timer_id = 0;
		}
		udp_handle->wait_timer_id =
			g_timeout_add_seconds(RECEIVER_UDP_WAIT_TIMEOUT,
				(GSourceFunc)__wait_time_out, NULL);
	}
	return;
}

static gboolean __read_socket(GIOChannel *channel,
		GIOCondition condition,
		gpointer data)
{
	gssize size = 0;
	receiver_udp_h *handle = (receiver_udp_h *)data;
	GError *error = NULL;

	char buf[1024] = {0, }; /* temp */

	retv_if(!handle, TRUE);
	retv_if(!handle->socket, TRUE);

	if (handle->state == RECEIVER_UDP_STATE_NONE) {
		_E("receiver udp is not ready yet");
		return TRUE;
	}

	if (handle->state == RECEIVER_UDP_STATE_READY) {
		char *s_addr = NULL;
		GSocketAddress *address = NULL;

		size = g_socket_receive_from(handle->socket, &address,
			buf, sizeof(buf), NULL, &error);

		if (size < 0) {
			_D("Error receiving from socket: %s", error->message);
			g_error_free(error);
		}

		s_addr = __socket_address_to_string(address);
		_D("received first data from [%s]", s_addr);


		if (!__receiver_udp_set_connection(address))
			__receiver_udp_update_wait_timer();
		else
			_E("failed to set connection with [%s]", s_addr);

		free(s_addr);
		g_object_unref(address);
	} else { /* state is RECEIVER_UDP_STATE_CONNECTED */
		size = g_socket_receive(handle->socket,
			buf, sizeof(buf), NULL, &error);

		if (size < 0) {
			_D("Error receiving from socket: %s", error->message);
			g_error_free(error);
		}
		_D("received data");
		__receiver_udp_update_wait_timer();
	}

	/* TODO : what should I do after receiveing some data? */

	return TRUE;
}

int receiver_udp_start(void)
{
	GError *error = NULL;
	GSocketAddress *address = NULL;
	GInetAddress *i_addr = NULL;
	int socket_fd = 0;
	GIOChannel *ch = NULL;
	int ret = 0;

	if (!udp_handle) {
		ret = __receiver_udp_init();
		if (ret)
			return -1;
	}

	if (udp_handle->state >= RECEIVER_UDP_STATE_READY) {
		_W("receiver udp is already started");
		return 0;
	}

	udp_handle->socket = g_socket_new(G_SOCKET_FAMILY_IPV4,
		G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, &error);

	if (udp_handle->socket == NULL) {
		_E("failed to get new socket - %s", error->message);
		goto ERROR;
	}

	/* set non-blocking mode */
	g_socket_set_blocking(udp_handle->socket, FALSE);

	i_addr = g_inet_address_new_any(G_SOCKET_FAMILY_IPV4);
	if (!i_addr) {
		_E("failed to get inet any address");
		goto ERROR;
	}
	address = g_inet_socket_address_new(i_addr, RECEIVER_UDP_PORT);
	g_object_unref(i_addr);
	i_addr = NULL;

	if (!address) {
		_E("failed to get socket address");
		goto ERROR;
	}

	if (!g_socket_bind(udp_handle->socket, address, TRUE, &error)) {
		_E("Can't bind socket: %s\n", error->message);
		goto ERROR;
	}
	g_object_unref(address);
	address = NULL;

	socket_fd = g_socket_get_fd(udp_handle->socket);
	ch = g_io_channel_unix_new(socket_fd);
	udp_handle->io_watch_id =
		g_io_add_watch(ch, G_IO_IN, __read_socket, udp_handle);
	g_io_channel_unref(ch);
	ch = NULL;

	udp_handle->state = RECEIVER_UDP_STATE_READY;
	_D("receiver udp started");

	return 0;

ERROR:
	if (error)
		g_error_free(error);

	if (address)
		g_object_unref(address);

	if (i_addr)
		g_object_unref(i_addr);

	__receiver_udp_fini();

	return -1;
}

int receiver_udp_stop(void)
{
	__receiver_udp_fini();
	_D("receiver udp stopped");

	return 0;
}
