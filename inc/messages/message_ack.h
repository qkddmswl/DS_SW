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

#ifndef MESSAGE_ACK_H
#define MESSAGE_ACK_H

#include "messages/message.h"

/**
 * @brief Ack message data
 */
typedef struct message_ack {
	message_t base;       /** Base class */
	int64_t ack_serial;   /** The serial of message which reception is confirmed */
} message_ack_t;

/**
 * @brief Initializes message_ack_t object.
 *
 * @param[in] message ack message.
 */
void message_ack_init(message_ack_t *message);

/**
 * @brief initializes message_ack_t message form other message.
 * The serial number returned by @message_ack_get_ack_serial will
 * be set to @request's serial.
 *
 * @param[in] message ack message.
 * @param[in] request the message which reception will be confirmed by
 * message_ack_t.
 */
void message_ack_init_from_request(message_ack_t *message, message_t *request);

/**
 * @brief Gets serial number of confirmed message.
 *
 * @return the serial number.
 */
int64_t message_ack_get_ack_serial(message_ack_t *message);

/**
 * @brief Destroys message_ack_t object.
 *
 * @param[in] message ack message.
 */
void message_ack_destroy(message_ack_t *message);

/**
 * @brief Deserializes message_ack_t from reader's buffer.
 *
 * @param[in] message ack message.
 * @param[in] reader reader object.
 *
 * @return 0 on success, other value on failure.
 */
int message_ack_deserialize(message_ack_t *message, reader_t *reader);

/**
 * @brief Serializes message_ack_t into writer's buffer.
 *
 * @param[in] message message object.
 * @param[in] writer writer object.
 *
 * @return 0 on success, other value on failure.
 */
int message_ack_serialize(message_ack_t *message, writer_t *writer);

#endif /* end of include guard: MESSAGE_ACK_H */
