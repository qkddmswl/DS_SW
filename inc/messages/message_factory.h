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

#ifndef MESSAGE_FACTORY_H
#define MESSAGE_FACTORY_H

#include "messages/message.h"

/**
 * @brief the factory handle.
 */
typedef struct _message_factory message_factory_t;

/**
 * @brief Creates new instance of message factory.
 *
 * @return new factory pointer or NULL on error.
 */
message_factory_t *message_factory_create();

/**
 * @brief Destroys factory instance.
 *
 * @param[in] factory factory pointer.
 */
void message_factory_destroy(message_factory_t *factory);

/**
 * @brief Returns message poiner.
 *
 * @param[in] factory factory pointer.
 * @param[in] type of message to be created.
 *
 * @note only ONE message instance returned by a factory is considered
 * valid at time. User should destroy previous message returned by
 * this function with @message_destroy function.
 *
 * @return message pointer or NULL on error.
 */
message_t *message_factory_create_message(message_factory_t *factory, message_type_e type);

#endif /* end of include guard: MESSAGE_FACTORY_H */
