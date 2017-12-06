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

#include <stdlib.h>
#include <peripheral_io.h>
#include "log.h"
#include "pca9685.h"
#include "dc_motor.h"

/* connected GPIO pin numbers of raspberry pi 3 with IN pins of L298N */
#define MotorA_1 19
#define MotorA_2 16
#define MotorB_1 26
#define MotorB_2 20

/* connected channel numbers of PCA9685 with enable pins of L298N */
#define EnableA_CH 5
#define EnableB_CH 4

typedef enum {
	MOTOR_STATE_NONE,
	MOTOR_STATE_STOP,
	MOTOR_STATE_FORWARD,
	MOTOR_STATE_BACKWARD,
} motor_state_e;

peripheral_gpio_h motorA_1_h = NULL;
peripheral_gpio_h motorA_2_h = NULL;
peripheral_gpio_h motorB_1_h = NULL;
peripheral_gpio_h motorB_2_h = NULL;

static int MotorState[2] = {MOTOR_STATE_NONE, };

/* see Principle section in http://wiki.sunfounder.cc/index.php?title=Motor_Driver_Module-L298N */

static int dc_motor_stop(dc_motor_id_e id)
{
	int ret = PERIPHERAL_ERROR_NONE;
	int motor1_v = 0;
	int motor2_v = 0;
	peripheral_gpio_h motor_1_h = NULL;
	peripheral_gpio_h motor_2_h = NULL;
	int channel = 0;

	if (MotorState[id] == MOTOR_STATE_NONE) {
		_E("motor[%d] are not initialized - state(%d)", id, MotorState[id]);
		return -1;
	}

	if (MotorState[id] == MOTOR_STATE_STOP) {
		_D("motor[%d] is already stopped", id);
		return 0;
	}

	switch (id) {
	case DC_MOTOR_ID_L:
		channel = EnableA_CH;
		motor_1_h = motorA_1_h;
		motor_2_h = motorA_2_h;
		break;
	case DC_MOTOR_ID_R:
		channel = EnableB_CH;
		motor_1_h = motorB_1_h;
		motor_2_h = motorB_2_h;
		break;
	}

	switch (MotorState[id]) {
	case MOTOR_STATE_FORWARD:
		motor1_v = 0;
		motor2_v = 0;
		break;
	case MOTOR_STATE_BACKWARD:
		motor1_v = 1;
		motor2_v = 1;
		break;
	}

	/* Brake DC motor */
	ret = peripheral_gpio_write(motor_1_h, motor1_v);
	if (ret != PERIPHERAL_ERROR_NONE) {
		_E("Failed to set value[%d] Motor_1[%d]", motor1_v, id);
		return -1;
	}

	ret = peripheral_gpio_write(motor_2_h, motor2_v);
	if (ret != PERIPHERAL_ERROR_NONE) {
		_E("Failed to set value[%d] Motor_1[%d]", motor2_v, id);
		return -1;
	}

	/* set stop DC motor */
	pca9685_set_value_to_channel(channel, 0, 0);

	MotorState[id] = MOTOR_STATE_STOP;

	return 0;
}

static void pins_fini(void)
{
	MotorState[DC_MOTOR_ID_L] = MOTOR_STATE_NONE;
	MotorState[DC_MOTOR_ID_R] = MOTOR_STATE_NONE;

	pca9685_fini();

	if (motorA_1_h) {
		peripheral_gpio_close(motorA_1_h);
		motorA_1_h = NULL;
	}

	if (motorA_2_h) {
		peripheral_gpio_close(motorA_2_h);
		motorA_2_h = NULL;
	}


	if (motorB_1_h) {
		peripheral_gpio_close(motorB_1_h);
		motorB_1_h = NULL;
	}

	if (motorB_2_h) {
		peripheral_gpio_close(motorB_2_h);
		motorB_2_h = NULL;
	}

	return;
}

static int pins_init(void)
{
	int ret = 0;

	if ((MotorState[DC_MOTOR_ID_L] > MOTOR_STATE_NONE) ||
		(MotorState[DC_MOTOR_ID_R] > MOTOR_STATE_NONE)) {
		_E("current state = %d, %d",
			MotorState[DC_MOTOR_ID_L], MotorState[DC_MOTOR_ID_R]);
		return -1;
	}

	ret = pca9685_init();
	if (ret) {
		_E("failed to init PCA9685");
		return -1;
	}

	ret = pca9685_set_frequency(60);
	if (ret) {
		_E("failed to set frequency to PCA9685");
		goto ERROR;
	}

	/* open pins for Motor A */
	ret = peripheral_gpio_open(MotorA_1, &motorA_1_h);
	if (ret == PERIPHERAL_ERROR_NONE)
		peripheral_gpio_set_direction(motorA_1_h,
			PERIPHERAL_GPIO_DIRECTION_OUT_INITIALLY_LOW);
	else {
		_E("failed to open MotorA_1 gpio pin(%d)", MotorA_1);
		goto ERROR;
	}

	ret = peripheral_gpio_open(MotorA_2, &motorA_2_h);
	if (ret == PERIPHERAL_ERROR_NONE)
		peripheral_gpio_set_direction(motorA_2_h,
			PERIPHERAL_GPIO_DIRECTION_OUT_INITIALLY_LOW);
	else {
		_E("failed to open MotorA_2 gpio pin(%d)", MotorA_2);
		goto ERROR;
	}

	MotorState[DC_MOTOR_ID_L] = MOTOR_STATE_STOP;

	/* open pins for Motor B */
	ret = peripheral_gpio_open(MotorB_1, &motorB_1_h);
	if (ret == PERIPHERAL_ERROR_NONE)
		peripheral_gpio_set_direction(motorB_1_h,
			PERIPHERAL_GPIO_DIRECTION_OUT_INITIALLY_LOW);
	else {
		_E("failed to open MotorB_1 gpio pin(%d)", MotorB_1);
		goto ERROR;
	}

	ret = peripheral_gpio_open(MotorB_2, &motorB_2_h);
	if (ret == PERIPHERAL_ERROR_NONE)
		peripheral_gpio_set_direction(motorB_2_h,
			PERIPHERAL_GPIO_DIRECTION_OUT_INITIALLY_LOW);
	else {
		_E("failed to open MotorB_2 gpio pin(%d)", MotorB_2);
		goto ERROR;
	}

	MotorState[DC_MOTOR_ID_R] = MOTOR_STATE_STOP;

	return 0;

ERROR:
	pca9685_fini();
	pins_fini();
	return -1;
}

int dc_motor_init(void)
{
	int ret = 0;

	ret = pins_init();

	return ret;
}

int dc_motor_fini(void)
{
	int ret = 0;

	pins_fini();

	return ret;
}

int dc_motor_speed_set(dc_motor_id_e id, int speed)
{
	int ret = 0;
	const int value_max = 4095;
	int value = 0;
	int channel = 0;
	peripheral_gpio_h motor_1_h = NULL;
	peripheral_gpio_h motor_2_h = NULL;
	int motor_v_1 = 0;
	int motor_v_2 = 0;
	int e_state = MOTOR_STATE_NONE;

	value = abs(speed);

	if (value > value_max) {
		value = value_max;
		_D("max speed is %d", value_max);
	}
	_D("set speed %d", value);

	if (speed == 0) {
		/* brake and stop */
		ret = dc_motor_stop(id);
		if (ret) {
			_E("failed to stop motor[%d]", id);
			return -1;
		}
		return 0; /* done */
	}

	switch (id) {
	case DC_MOTOR_ID_L:
		channel = EnableA_CH;
		motor_1_h = motorA_1_h;
		motor_2_h = motorA_2_h;
		break;
	case DC_MOTOR_ID_R:
		channel = EnableB_CH;
		motor_1_h = motorB_1_h;
		motor_2_h = motorB_2_h;
	break;
	}

	if (speed > 0)
		e_state = MOTOR_STATE_FORWARD; /* will be set forward */
	else
		e_state = MOTOR_STATE_BACKWARD; /* will be set backward */

	if (MotorState[id] == e_state)
		goto SET_SPEED;
	else {
		/* brake and stop */
		ret = dc_motor_stop(id);
		if (ret) {
			_E("failed to stop motor[%d]", id);
			return -1;
		}
	}

	switch (e_state) {
	case MOTOR_STATE_FORWARD:
		motor_v_1 = 1;
		motor_v_2 = 0;
		break;
	case MOTOR_STATE_BACKWARD:
		motor_v_1 = 0;
		motor_v_2 = 1;
		break;
	}
	ret = peripheral_gpio_write(motor_1_h, motor_v_1);
	if (ret != PERIPHERAL_ERROR_NONE) {
		_E("failed to set value[%d] Motor_1[%d]", motor_v_1, id);
		return -1;
	}

	ret = peripheral_gpio_write(motor_2_h, motor_v_2);
	if (ret != PERIPHERAL_ERROR_NONE) {
		_E("failed to set value[%d] Motor_2[%d]", motor_v_2, id);
		return -1;
	}

SET_SPEED:
	ret = pca9685_set_value_to_channel(channel, 0, value);
	if (ret) {
		_E("failed to set speed - %d", speed);
		return -1;
	}

	MotorState[id] = e_state;

	return 0;
}
