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

#ifndef MESSAGE_BYE_H
#define MESSAGE_BYE_H

#include "messages/message.h"

/**
 * @brief Connect message data
 */
typedef struct message_bye {
	message_t base; /** Base class */
} message_bye_t;

/**
 * @brief Initializes message_bye_t object.
 *
 * @param[in] message ack message.
 */
void message_bye_init(message_bye_t *message);

/**
 * @brief Destroys message_bye_t object.
 *
 * @param[in] message ack message.
 */
void message_bye_destroy(message_bye_t *message);

/**
 * @brief Deserializes message_bye_t from reader's buffer.
 *
 * @param[in] message ack message.
 * @param[in] reader reader object.
 *
 * @return 0 on success, other value on failure.
 */
int message_bye_deserialize(message_bye_t *message, reader_t *reader);

/**
 * @brief Serializes message_bye_t into writer's buffer.
 *
 * @param[in] message message object.
 * @param[in] writer writer object.
 *
 * @return 0 on success, other value on failure.
 */
int message_bye_serialize(message_bye_t *message, writer_t *writer);

#endif /* end of include guard: MESSAGE_BYE_H */
