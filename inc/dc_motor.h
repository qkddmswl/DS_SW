/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
 *
 * Contact: Jeonghoon Park <jh1979.park@samsung.com>
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

#ifndef __CAR_APP_DC_MOTOR_H__
#define __CAR_APP_DC_MOTOR_H__

typedef enum {
	DC_MOTOR_ID_L,
	DC_MOTOR_ID_R,
} dc_motor_id_e;

int dc_motor_init(void);
int dc_motor_fini(void);
int dc_motor_speed_set(dc_motor_id_e id, int speed);

#endif /* __CAR_APP_DC_MOTOR_H__ */
