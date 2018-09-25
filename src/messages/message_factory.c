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

#include <stdlib.h>

#include "messages/message_factory.h"

#include "messages/message_ack.h"
#include "messages/message_bye.h"
#include "messages/message_command.h"
#include "messages/message_connect.h"
#include "messages/message_connect_accepted.h"
#include "messages/message_connect_refused.h"
#include "messages/message_keep_alive.h"

struct _message_factory {
	union {
		message_ack_t ack;
		message_bye_t bye;
		message_command_t command;
		message_connect_t connect;
		message_connect_accepted_t connect_accepted;
		message_connect_refused_t connect_refused;
		message_keep_alive_t keep_alive;
	} messages;
};

message_t *message_factory_create_message(message_factory_t *factory, message_type_e type)
{
	switch(type) {
	case MESSAGE_CONNECT:
		message_connect_init(&factory->messages.connect);
		return &factory->messages.connect.base;
	case MESSAGE_CONNECT_ACCEPTED:
		message_connect_accepted_init(&factory->messages.connect_accepted);
		return &factory->messages.connect_accepted.base;
	case MESSAGE_CONNECT_REFUSED:
		message_connect_refused_init(&factory->messages.connect_refused);
		return &factory->messages.connect_refused.base;
	case MESSAGE_KEEP_ALIVE:
		message_keep_alive_init(&factory->messages.keep_alive);
		return &factory->messages.keep_alive.base;
	case MESSAGE_ACK:
		message_ack_init(&factory->messages.ack);
		return &factory->messages.ack.base;
	case MESSAGE_COMMAND:
		message_command_init(&factory->messages.command);
		return &factory->messages.command.base;
	case MESSAGE_BYE:
		message_bye_init(&factory->messages.bye);
		return &factory->messages.bye.base;
	default:
		return NULL;
	}
}

message_factory_t *message_factory_create()
{
	return malloc(sizeof(message_factory_t));
}

void message_factory_destroy(message_factory_t *factory)
{
	free(factory);
}
