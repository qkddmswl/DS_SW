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

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "messages/writer.h"
#include "messages/reader.h"

/**
 * @brief message types
 */
typedef enum message_type {
	MESSAGE_NONE,
	MESSAGE_CONNECT,          /** Connection request */
	MESSAGE_CONNECT_ACCEPTED, /** Connection accepted reply */
	MESSAGE_CONNECT_REFUSED,  /** Connection refused reply */
	MESSAGE_KEEP_ALIVE,       /** Keep alive request */
	MESSAGE_ACK,              /** Message delivery confirmation */
	MESSAGE_COMMAND,          /** Message with command data */
	MESSAGE_BYE               /** Connection end request */
} message_type_e;

#define IP_ADDRESS_LEN 15

typedef struct message message_t;

/**
 * @brief message data
 */
struct message {
	int64_t serial;
	int64_t timestamp;
	int32_t type;
	int sender_port;
	int receiver_port;
	char sender_ip[IP_ADDRESS_LEN+1];
	char receiver_ip[IP_ADDRESS_LEN+1];
	struct {
		int (*deserialize)(message_t *msg, reader_t *reader);
		int (*serialize)(message_t *msg, writer_t *writer);
		void (*destroy)(message_t *msg);
	} vtable;
};

/**
 * @brief Destroys message.
 *
 * @param[in] message message object.
 *
 * @note this function is implementing polymorphic behaviour of
 * message_t object. The the destroy is made via vtable->destroy pointer.
 */
void message_destroy(message_t *message);

/**
 * @brief Serializes message into writer's buffer.
 *
 * @param[in] message message object.
 * @param[in] writer writer object.
 *
 * @return 0 on success, other value on failure.
 *
 * @note this function is implementing polymorphic behaviour of
 * message_t object. The serialization is made via vtable->serialize pointer.
 */
int message_serialize(message_t *message, writer_t *writer);

/**
 * @brief Deserializes message from reader's buffer.
 *
 * @param[in] message message object.
 * @param[in] reader reader object.
 *
 * @return 0 on success, other value on failure.
 *
 * @note this function is implementing polymorphic behaviour of
 * message_t object. The deserialization is made via vtable->serialize pointer.
 */
int message_deserialize(message_t *message, reader_t *reader);

/**
 * @brief Get reciever of the message
 *
 * @param[in] message message object.
 * @param[out] ip address of reciever.
 * @param[out] port port of the reciever.
 *
 * @note if reciever is not set the @ip will be empty string.
 */
void message_get_receiver(message_t *messsage, const char **ip, int *port);

/**
 * @brief Set reciever of the message.
 *
 * @param[in] message message object.
 * @param[in] ip address of reciever.
 * @param[in] port port of the reciever.
 */
void message_set_receiver(message_t *messsage, const char *ip, int port);

/**
 * @brief Get sender of the message.
 *
 * @param[in] message message object.
 * @param[out] ip address of sender.
 * @param[out] port port of the sender.
 *
 * @note if sender is not set the @ip will be empty string.
 */
void message_get_sender(message_t *message, const char **ip, int *port);

/**
 * @brief Set sender of the message.
 *
 * @param[in] message message object.
 * @param[in] ip address of sender.
 * @param[in] port port of the sender.
 */
void message_set_sender(message_t *message, const char *ip, int port);

/**
 * @brief Get timestamp of the message.
 *
 * @param[in] message message object.
 *
 * @return Unix timestamp.
 */
time_t message_get_timestamp(message_t *messsage);

/**
 * @brief Set timestamp of the message.
 *
 * @param[in] message message object.
 * @param[in] time Unix timestamp.
 */
void message_set_timestamp(message_t *messsage, time_t time);

/**
 * @brief Get type of the message.
 *
 * @param[in] message message object.
 *
 * @return message type.
 */
message_type_e message_get_type(message_t *message);

/**
 * @brief Set type of the message.
 *
 * @param[in] message message object.
 * @param[in] type message type.
 */
void message_set_type(message_t *message, message_type_e type);

/**
 * @brief Get serial number of the message. Serial numbers
 * can be used to verify order of the messages.
 *
 * @param[in] message message object.
 *
 * @return message serial number.
 */
int64_t message_get_serial(message_t *message);

/**
 * @brief Initializes base message object. The main
 * reponsibilities of this functions are:
 * - zeroing memory.
 * - assigning serial number.
 *
 * @param[in] message message object.
 *
 * @note the purpose of this function is to be used in
 * derived classes.
 */
void message_base_init(message_t *msg);

/**
 * @brief Destroys base message object.
 *
 * @param[in] message message object.
 *
 * @note unlike @message_destroy, this function is performing
 * real destroy on message_t object.
 *
 * @note the purpose of this function is to be used in
 * derived classes.
 */
void message_base_destroy(message_t *msg);

/**
 * @brief Deserializes base message from reader's buffer.
 *
 * @param[in] message message object.
 * @param[in] reader reader object.
 *
 * @note unlike @message_deserialize, this function is performing
 * real deserialzation on message_t object.
 *
 * @note the purpose of this function is to be used in
 * derived classes.
 */
int message_base_deserialize(message_t *msg, reader_t *reader);

/**
 * @brief Serializes base message into writer's buffer.
 *
 * @param[in] message message object.
 * @param[in] writer writer object.
 *
 * @note unlike @message_serialize, this function is performing
 * real serialzation on message_t object.
 *
 * @note the purpose of this function is to be used in
 * derived classes.
 */
int message_base_serialize(message_t *msg, writer_t *writer);

#endif /* MESSAGE_H_ */
