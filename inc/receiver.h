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

typedef enum __receiver_type_e {
	RECEIVER_TYPE_UDP,
	RECEIVER_TYPE_BLUETOOTH,
} receiver_type_e;

int receiver_init(receiver_type_e type);
void receiver_fini(void);

#endif /* __CAR_APP_RECEIVER_H__ */
