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

#include "messages/message_manager.h"
#include "messages/message_factory.h"
#include "udp_connection.h"
#include "messages/reader.h"
#include "messages/writer.h"
#include "messages/clock.h"

#define DEFAULT_PORT 4004

struct _message_mgr {
	writer_t writer;
	reader_t reader;
	udp_connection_t *conn;
	message_factory_t *factory;
	receive_message_cb cb;
	void *user_data;
};

static struct _message_mgr mgr;

static void msg_mgr_udp_receive_cb(const char *data, unsigned int size, const char *address, int port)
{
	int32_t message_type;
	message_t *message;

	if (!mgr.cb)
		return;

	reader_init_static(&mgr.reader, data, size);

	if (reader_read_int32(&mgr.reader, &message_type)) {
		return;
	}

	message = message_factory_create_message(mgr.factory, message_type);
	if (!message) {
		return;
	}

	if (message_deserialize(message, &mgr.reader)) {
		message_destroy(message);
		return;
	}

	message_set_sender(message, address, port);

	if (mgr.cb) mgr.cb(message, mgr.user_data);

	message_destroy(message);
}

int message_manager_init()
{
	if (mgr.conn) {
		return 0;
	}

	mgr.conn = udp_connection_create(DEFAULT_PORT); //TODO load from config
	if (!mgr.conn) {
		return -1;
	}

	mgr.factory = message_factory_create();
	if (!mgr.factory) {
		message_manager_shutdown();
		return -1;
	}

	udp_connection_set_receive_cb(mgr.conn, msg_mgr_udp_receive_cb);
	writer_init_sized(&mgr.writer, 256);

	return 0;
}

int message_manager_send_message(message_t *message)
{
	const char *address;
	int port;
	int32_t type;

	if (!mgr.conn)
		return -1;

	writer_reset(&mgr.writer, 0);

	type = message_get_type(message);

	if (writer_write_int32(&mgr.writer, type)) {
		return -1;
	}

	message_get_receiver(message, &address, &port);
	message_set_timestamp(message, clock_realtime_get());

	if (message_serialize(message, &mgr.writer))
		return -1;

	int err = udp_connection_send(mgr.conn,
			mgr.writer.data,
			mgr.writer.length,
			address,
			port);

	if (err)
		return -1;

	return 0;
}

void message_manager_set_receive_message_cb(receive_message_cb callback, void *user_data)
{
	if (!mgr.conn)
		return;

	mgr.cb = callback;
	mgr.user_data = user_data;
}

void message_manager_shutdown()
{
	if (!mgr.conn)
		return;

	writer_shutdown(&mgr.writer);
	message_factory_destroy(mgr.factory);
	udp_connection_destroy(mgr.conn);
	mgr.conn = NULL;
}
