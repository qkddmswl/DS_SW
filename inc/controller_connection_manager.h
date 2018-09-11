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

#ifndef INC_CONTROLLER_CONNECTION_MANAGER_H_
#define INC_CONTROLLER_CONNECTION_MANAGER_H_

#include "command.h"

/**
 * @brief Describes state of connection.
 */
typedef enum controller_connection_state {
	CONTROLLER_CONNECTION_STATE_READY, /** Listens and waits for controller. */
	CONTROLLER_CONNECTION_STATE_RESERVED /** Currently unavailable to connect. */
} controller_connection_state_e;

/**
 * @brief Called whenever state of connection changes.
 * @param[in] previous Previous state of connection.
 * @param[in] current Current state of connection.
 */
typedef void (*connection_state_cb)(controller_connection_state_e previous, controller_connection_state_e current);

/**
 * @brief Called whenever new message arrives.
 * @param[in] message Message tat arrived.
 */
typedef void (*message_received_cb)(command_s command);

/**
 * @brief Starts listening on the given port for messages.
 * @param[in] port Port on which application will be listening.
 * @return 0 on success, -1 otherwise.
 * @remarks This function allocates resources and that has to be freed with controller_connection_manager_release.
 */
int controller_connection_manager_listen(int port);

/**
 * @brief Gets currect connection state.
 * @return Connection state.
 */
controller_connection_state_e controller_connection_manager_get_state();

/**
 * @brief Sets callback function called whenever connection state changes.
 * @param[in] callback Callback function to be set.
 */
void controller_connection_set_manager_state_change_cb(connection_state_cb callback);

/**
 * @brief Sets callback function called whenever new message arrives.
 * @param[in] callback Callback function to be set.
 */
void controller_connection_manager_set_message_received_cb(message_received_cb callback);

/**
 * @brief Stops listening for messages and release resources connected with it.
 */
void controller_connection_manager_release();

#endif /* INC_CONTROLLER_CONNECTION_MANAGER_H_ */
