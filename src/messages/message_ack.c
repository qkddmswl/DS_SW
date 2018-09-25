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

#include "messages/message_ack.h"
#include "messages/macros.h"

static int _message_ack_serialze_vcall(message_t *msg, writer_t *writer)
{
	message_ack_t *ack_msg = container_of(msg, message_ack_t, base);
	return message_ack_serialize(ack_msg, writer);
}

static int _message_ack_deserialize_vcall(message_t *msg, reader_t *reader)
{
	message_ack_t *ack_msg = container_of(msg, message_ack_t, base);
	return message_ack_deserialize(ack_msg, reader);
}

static void _message_ack_destroy_vcall(message_t *msg)
{
	message_ack_t *ack_msg = container_of(msg, message_ack_t, base);
	message_ack_destroy(ack_msg);
}

void message_ack_init(message_ack_t *message)
{
	message_base_init(&message->base);

	message_set_type(&message->base, MESSAGE_ACK);

	message->base.vtable.serialize = _message_ack_serialze_vcall;
	message->base.vtable.deserialize = _message_ack_deserialize_vcall;
	message->base.vtable.destroy = _message_ack_destroy_vcall;

	message->ack_serial = -1; // ack_serial not set
}

void message_ack_init_from_request(message_ack_t *message, message_t *request)
{
	message_ack_init(message);

	message->ack_serial = message_get_serial(request);
}

void message_ack_destroy(message_ack_t *message)
{
	message_base_destroy(&message->base);
}

int64_t message_ack_get_ack_serial(message_ack_t *message)
{
	return message->ack_serial;
}

int message_ack_deserialize(message_ack_t *message, reader_t *reader)
{
	int err = 0;

	err |= message_base_deserialize(&message->base, reader);
	err |= reader_read_int64(reader, &message->ack_serial);

	return err;
}

int message_ack_serialize(message_ack_t *message, writer_t *writer)
{
	int err = 0;

	err |= message_base_serialize(&message->base, writer);
	err |= writer_write_int64(writer, message->ack_serial);

	return err;
}
