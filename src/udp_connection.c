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

#include "udp_connection.h"
#include <gio/gio.h>
#include <stdlib.h>
#include "log.h"
#define MESSAGE_IN_BUF_SIZE 512

struct udp_connection {
	GSocket *socket;
	udp_receive_cb receive_cb;
	GIOChannel *channel;
	guint watch_id;
	GError *error;
};

static gboolean _channel_ready_cb(GIOChannel *source, GIOCondition cond, gpointer data);
static void _connection_release_resources(udp_connection_t *connection);

udp_connection_t *udp_connection_create(int port)
{
	udp_connection_t *connection = (udp_connection_t*) calloc(sizeof(udp_connection_t), 1);
	if(connection == NULL) {
		_E("Failed to calloc allocate memory");
		return NULL;
	}

	connection->socket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, &(connection->error));
	if(!connection->socket) {
		_E("Failed to create socket");
		_connection_release_resources(connection);
		return NULL;
	}
	_D("Socket created");

	GInetAddress *inet_addr = g_inet_address_new_any(G_SOCKET_FAMILY_IPV4);
	if(!inet_addr) {
		_E("Failed to obtain inet any address");
		_connection_release_resources(connection);
		return NULL;
	}
	gchar *address_str = g_inet_address_to_string(inet_addr);
	_D("Obtained address: %s", address_str);

	GInetSocketAddress *socket_addr = (GInetSocketAddress*) g_inet_socket_address_new(inet_addr, port);
	g_object_unref(inet_addr);
	if(!socket_addr) {
		_E("Failed to get socket address");
		g_free(address_str);
		_connection_release_resources(connection);
		return NULL;
	}

	gboolean res = g_socket_bind(connection->socket, (GSocketAddress*) socket_addr, TRUE, &(connection->error));
	g_object_unref(socket_addr);
	if(!res) {
		_E("Failed to bind socket: %s", connection->error->message);
		g_free(address_str);
		_connection_release_resources(connection);
		return NULL;
	}
	_D("Bound socket to %s:%d", address_str, port);
	g_socket_set_blocking(connection->socket, FALSE);

	int socket_fd = g_socket_get_fd(connection->socket);
	connection->channel = g_io_channel_unix_new(socket_fd);
	connection->watch_id = g_io_add_watch(connection->channel, G_IO_IN, _channel_ready_cb, connection);
	g_io_channel_unref(connection->channel);

	g_free(address_str);
	return connection;
}

int udp_connection_send(udp_connection_t *connection, const char *data, unsigned short int size, const char *address, int port)
{
	GIOCondition cond = g_socket_condition_check(connection->socket, G_IO_OUT);
	if(cond != G_IO_OUT) {
		_E("Failed to send data - socket is not G_IO_OUT");
		return -1;
	}

	GInetSocketAddress *receiver_address = (GInetSocketAddress*) g_inet_socket_address_new_from_string(address, port);
	if(!receiver_address) {
		_E("Failed to obtain socket address from %s:%d", address, port);
		return -1;
	}

	GError *error = NULL;
	gssize wr_size = g_socket_send_to(connection->socket, (GSocketAddress*) receiver_address, data, size, NULL, &error);
	if(wr_size != size) {
		_E("Error sending data to %s:%d - sent only %d", address, port, wr_size);
		g_object_unref(receiver_address);
		g_error_free(error);
		return -1;
	}
	g_object_unref(receiver_address);
	_D("Sent %d bytes", size);
	return 0;
}

void udp_connection_set_receive_cb(udp_connection_t *connection, udp_receive_cb callback)
{
	connection->receive_cb = callback;
}

void udp_connection_destroy(udp_connection_t *connection)
{
	_connection_release_resources(connection);
}

static gboolean _channel_ready_cb(GIOChannel *source, GIOCondition cond, gpointer data)
{
	udp_connection_t *connection = (udp_connection_t*) data;
	GError *error = NULL;
	GInetSocketAddress *socket_address = NULL;
	gchar buffer[MESSAGE_IN_BUF_SIZE];

	gssize size = g_socket_receive_from(connection->socket, (GSocketAddress**) &socket_address, buffer, MESSAGE_IN_BUF_SIZE, NULL, &error);
	if(size < 0) {
		_E("Cannot read data from socket");
		g_error_free(error);
		return FALSE;
	}

	if(size == MESSAGE_IN_BUF_SIZE) {
		_W("Packet dropped due to its size");
		g_error_free(error);
		return TRUE;
	}

	if(size == 0) {
		_W("No data to read");
		return TRUE;
	}

	GInetAddress *address= g_inet_socket_address_get_address(socket_address);
	if(!address) {
		_E("Failed to obtain the address of received message");
		return TRUE;
	}

	gchar *address_str = g_inet_address_to_string(address);
	if(!address_str) {
		_E("Failed to obtain the address in text of received message");
		g_object_unref(address);
		return TRUE;
	}

	int port = g_inet_socket_address_get_port(socket_address);
	if(connection->receive_cb) {
		connection->receive_cb(buffer, size, address_str, port);
	}

	g_free(address_str);
	g_object_unref(socket_address);

	return TRUE;
}

static void _connection_release_resources(udp_connection_t *connection)
{
	if(!connection) {
		return;
	}

	if(connection->error) {
		g_error_free(connection->error);
	}

	if(connection->socket) {
		g_socket_close(connection->socket, NULL);
	}

	free(connection);
}
