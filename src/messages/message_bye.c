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

#include "messages/message_bye.h"

void message_bye_init(message_bye_t *message)
{
	message_base_init(&message->base);
	message_set_type(&message->base, MESSAGE_BYE);
}

void message_bye_destroy(message_bye_t *message)
{
	message_base_destroy(&message->base);
}

int message_bye_deserialize(message_bye_t *message, reader_t *reader)
{
	return message_base_deserialize(&message->base, reader);
}

int message_bye_serialize(message_bye_t *message, writer_t *writer)
{
	return message_base_serialize(&message->base, writer);
}
