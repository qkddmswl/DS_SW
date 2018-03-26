/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

#ifndef __CAR_APP_RECEIVER_H__
#define __CAR_APP_RECEIVER_H__

#include "receiver_type.h"

int receiver_init(receiver_type_e type);
void receiver_fini(receiver_type_e type);
int receiver_start(receiver_type_e type);
int receiver_stop(receiver_type_e type);
receiver_state_e receiver_get_state(receiver_type_e type);
int receiver_set_state_changed_cb(receiver_type_e type,
	receiver_state_changed_cb callback, void *user_data);

#endif /* __CAR_APP_RECEIVER_H__ */
