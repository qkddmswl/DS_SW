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

#ifndef MESSAGE_MANAGER_H_
#define MESSAGE_MANAGER_H_

#include "messages/message.h"

typedef void (*receive_message_cb)(message_t *message, void *user_data);

/**
 * @brief Initializes message manager module.
 *
 * @return 0 on success, other value on error.
 */
int message_manager_init();

/**
 * @brief Send message using message manager conenction.
 *
 * @return 0 on success, other value on error.
 *
 * @note the @message_manager_init should be called beforehead.
 */
int message_manager_send_message(message_t *message);

/**
 * @brief Set message recieved callback.
 *
 * @param[in] callback user callback.
 * @param[in] user_data user data.
 */
void message_manager_set_receive_message_cb(receive_message_cb callback, void *user_data);

/**
 * @brief Shutdowns message manager module.
 *
 * @return 0 on success, other value on error.
 */
void message_manager_shutdown();

#endif /* MESSAGE_MANAGER_H_ */
