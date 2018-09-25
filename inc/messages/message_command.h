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

#ifndef MESSAGE_COMMAND_H
#define MESSAGE_COMMAND_H

#include "command.h"
#include "messages/message.h"

/**
 * @brief Command message data
 */
typedef struct message_command {
	message_t base;    /** Base class */
	command_s command; /** Command carried by message */
} message_command_t;

/**
 * @brief Initializes message_command_t object.
 *
 * @param[in] message ack message.
 */
void message_command_init(message_command_t *message);

/**
 * @brief Get command for mesage.
 *
 * @param[in] message ack message.
 *
 * @return command const pointer.
 */
const command_s *message_command_get_command(message_command_t *message);

/**
 * @brief Set command for mesage.
 *
 * @param[in] message ack message.
 * @param[in] command command to be set.
 */
void message_command_set_command(message_command_t *message, const command_s *cmd);

/**
 * @brief Destroys message_command_t object.
 *
 * @param[in] message ack message.
 */
void message_command_destroy(message_command_t *message);

/**
 * @brief Deserializes message_command_t from reader's buffer.
 *
 * @param[in] message ack message.
 * @param[in] reader reader object.
 *
 * @return 0 on success, other value on failure.
 */
int message_command_deserialize(message_command_t *message, reader_t *reader);

/**
 * @brief Serializes message_command_t into writer's buffer.
 *
 * @param[in] message message object.
 * @param[in] writer writer object.
 *
 * @return 0 on success, other value on failure.
 */
int message_command_serialize(message_command_t *message, writer_t *writer);

#endif /* end of include guard: MESSAGE_COMMAND_H */
