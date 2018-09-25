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

#include "messages/message.h"

#include <string.h>

static int64_t current_serial;

void message_destroy(message_t *message)
{
	if (message->vtable.destroy)
		message->vtable.destroy(message);
}

int message_serialize(message_t *message, writer_t *writer)
{
	if (message->vtable.serialize)
		return message->vtable.serialize(message, writer);
	return -1;
}

int message_deserialize(message_t *message, reader_t *reader)
{
	if (message->vtable.deserialize)
		return message->vtable.deserialize(message, reader);
	return -1;
}

void message_set_receiver(message_t *message, const char *ip, int port)
{
	if (ip) strncpy(message->receiver_ip, ip, IP_ADDRESS_LEN);
	message->receiver_port = port;
}

void message_get_receiver(message_t *message, const char **ip, int *port)
{
	if (ip) *ip = message->receiver_ip;
	if (port) *port = message->receiver_port;
}

time_t message_get_timestamp(message_t *message)
{
	return message->timestamp;
}

void message_set_timestamp(message_t *message, time_t time)
{
	message->timestamp = time;
}

void message_get_sender(message_t *message, const char **ip, int *port)
{
	if (ip) *ip = message->sender_ip;
	if (port) *port = message->sender_port;
}

void message_set_sender(message_t *message, const char *ip, int port)
{
	if (ip) strncpy(message->sender_ip, ip, IP_ADDRESS_LEN);
	message->sender_port = port;
}

message_type_e message_get_type(message_t *message)
{
	return message->type;
}

void message_set_type(message_t *message, message_type_e type)
{
	message->type = type;
}

int64_t message_get_serial(message_t *message)
{
	return message->serial;
}

void message_base_init(message_t *msg)
{
	memset(msg, 0x0, sizeof(message_t));

	msg->vtable.serialize = message_base_serialize;
	msg->vtable.deserialize = message_base_deserialize;
	msg->vtable.destroy = message_base_destroy;
	msg->serial = current_serial++;
}

void message_base_destroy(message_t *msg)
{
}

int message_base_deserialize(message_t *msg, reader_t *reader)
{
	int err = 0;

	err |= reader_read_int64(reader, &msg->serial);
	err |= reader_read_int32(reader, &msg->type);
	err |= reader_read_int64(reader, &msg->timestamp);

	return err;
}

int message_base_serialize(message_t *msg, writer_t *writer)
{
	int err = 0;

	err |= writer_write_int64(writer, msg->serial);
	err |= writer_write_int32(writer, msg->type);
	err |= writer_write_int64(writer, msg->timestamp);

	return err;
}
