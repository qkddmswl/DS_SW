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

#ifndef __CAR_APP_RECEIVER_TYPE_H__
#define __CAR_APP_RECEIVER_TYPE_H__

typedef enum __receiver_type_e {
	RECEIVER_TYPE_UDP = (1 << 0),
	RECEIVER_TYPE_BLUETOOTH = (1 << 1),
} receiver_type_e;

typedef enum __receiver_state_e {
	RECEIVER_STATE_NONE,
	RECEIVER_STATE_INIT,
	RECEIVER_STATE_READY,
	RECEIVER_STATE_CONNECTED,
} receiver_state_e;

typedef void(*receiver_state_changed_cb)
	(receiver_type_e type, receiver_state_e state, void* user_data);

#endif /* __CAR_APP_RECEIVER_TYPE_H__ */
