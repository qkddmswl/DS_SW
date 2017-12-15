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

#ifndef __RESOURCE_MOTOR_DRIVER_L298N_H__
#define __RESOURCE_MOTOR_DRIVER_L298N_H__

/* connected GPIO pin numbers of raspberry pi 3 with IN pins of L298N */
#define DEFAULT_MOTOR1_PIN1 26
#define DEFAULT_MOTOR1_PIN2 20

#define DEFAULT_MOTOR2_PIN1 19
#define DEFAULT_MOTOR2_PIN2 16

#define DEFAULT_MOTOR3_PIN1 6
#define DEFAULT_MOTOR3_PIN2 12

#define DEFAULT_MOTOR4_PIN1 22
#define DEFAULT_MOTOR4_PIN2 23

/* connected channel numbers of PCA9685 with enable pins of L298N */
#define DEFAULT_MOTOR1_EN_CH 1
#define DEFAULT_MOTOR2_EN_CH 2
#define DEFAULT_MOTOR3_EN_CH 3
#define DEFAULT_MOTOR4_EN_CH 4

typedef enum {
	MOTOR_ID_1,
	MOTOR_ID_2,
	MOTOR_ID_3,
	MOTOR_ID_4,
	MOTOR_ID_MAX
} motor_id_e;

int resource_set_motor_driver_L298N_speed(motor_id_e id, int speed);

#endif /* __RESOURCE_MOTOR_DRIVER_L298N_H__ */
