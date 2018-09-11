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

#ifndef INC_UDP_CONNECTION_H_
#define INC_UDP_CONNECTION_H_

/**
 * @brief Called whenever new UDP data arrives.
 * @param[in] data Pointer to data that arrived.
 * @param[in] size Size of data in bytes.
 * @param[in] address Address of source of data.
 * @param[in] port Port of data source.
 */
typedef void (*udp_receive_cb)(const char *data, unsigned int size, const char *address, int port);

/**
 * @brief Structure of data about udp_connection.
 */
typedef struct udp_connection udp_connection_t;

/**
 * @brief Creates UDP connection object.
 * @param[in] port Local port on which creation should be stablished.
 * @return new udp_connection object on success, NULL otherwise.
 * @remarks Function allocates resources that have to be freed with udp_communication_destroy.
 */
udp_connection_t *udp_connection_create(int port);

/**
 * @brief Sets the receiver of sent data.
 * @param[in] connection UDP connection object.
 * @param[in] address IP address of receiver.
 * @param[in] port Port of receiver to send data on.
 * @return 0 on success, -1 otherwise.
 */
int udp_connection_set_receiver(udp_connection_t *connection, const char *address, int port);

/**
 * @brief Sends data to set receiver.
 * @param[in] connection UDP connection object.
 * @param[in] data Data to be sent.
 * @param[in] size Size in bytes of data pointed by data pointer.
 * @return 0 on success, -1 otherwise.
 */
int udp_connection_send(udp_connection_t *connection, const char *data, unsigned int size);

/**
 * @brief Sets callback for receiving data.
 * @param[in] connection UDP connection object.
 * @param[in] callback Callback to be set or NULL to unregister.
 * @remarks Function is passed pointer to data and its size.
 */
void udp_connection_set_receive_cb(udp_connection_t *connection, udp_receive_cb callback);

/**
 * @brief Stops UDP communication and releases all resources allocated by udp_communication_create.
 * @param[in] connection UDP connection object.
 */
void udp_connection_destroy(udp_connection_t *connection);

#endif /* INC_UDP_CONNECTION_H_ */
