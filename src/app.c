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

#include <stdio.h>
#include <unistd.h>
#include <glib.h>
#include <service_app.h>
#include "log.h"
#include "resource.h"

enum {
	DIR_STATE_S,
	DIR_STATE_F,
	DIR_STATE_B,
};

typedef struct app_data_s {
	unsigned int f_value;
	unsigned int r_value;
	unsigned int dir_state;
} app_data;

#define FRONT_PIN 21
#define REAR_PIN 4

static void service_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	return;
}

static void service_app_region_changed(app_event_info_h event_info, void *user_data)
{
	return;
}

static void service_app_low_battery(app_event_info_h event_info, void *user_data)
{
	return;
}

static void service_app_low_memory(app_event_info_h event_info, void *user_data)
{
	return;
}

static int __servo_motor_test(void)
{
	resource_set_motor_driver_L298N_speed(MOTOR_ID_1, 0);
	resource_set_motor_driver_L298N_speed(MOTOR_ID_2, 0);
	resource_set_servo_motor_value(0, 400);
	sleep(1);
	resource_set_servo_motor_value(0, 500);
	sleep(1);
	resource_set_servo_motor_value(0, 450);
	sleep(1);
	return 0;
}

static void ___________control_motor(app_data *ad)
{
	_D("control motor, state(%u), f_val(%u), r_val(%u)",
			ad->dir_state, ad->f_value, ad->r_value);


	switch (ad->dir_state) {
	case DIR_STATE_F:
		if (ad->f_value)  {
			if (ad->r_value) {
				ad->dir_state = DIR_STATE_S;
				resource_set_motor_driver_L298N_speed(MOTOR_ID_1, 0);
				resource_set_motor_driver_L298N_speed(MOTOR_ID_2, 0);
			} else {
				ad->dir_state = DIR_STATE_B;
				__servo_motor_test();
				resource_set_motor_driver_L298N_speed(MOTOR_ID_1, -2000);
				resource_set_motor_driver_L298N_speed(MOTOR_ID_2, -2000);
			}
		}
		break;
	case DIR_STATE_B:
		if (ad->r_value)  {
			if (ad->f_value) {
				ad->dir_state = DIR_STATE_S;
				resource_set_motor_driver_L298N_speed(MOTOR_ID_1, 0);
				resource_set_motor_driver_L298N_speed(MOTOR_ID_2, 0);
			} else {
				ad->dir_state = DIR_STATE_F;
				__servo_motor_test();
				resource_set_motor_driver_L298N_speed(MOTOR_ID_1, 2000);
				resource_set_motor_driver_L298N_speed(MOTOR_ID_2, 2000);
			}
		}
		break;
	case DIR_STATE_S:
		if (!ad->f_value)  {
			ad->dir_state = DIR_STATE_F;
			__servo_motor_test();
			resource_set_motor_driver_L298N_speed(MOTOR_ID_1, 2000);
			resource_set_motor_driver_L298N_speed(MOTOR_ID_2, 2000);
		} else if (!ad->r_value) {
			ad->dir_state = DIR_STATE_B;
			__servo_motor_test();
			resource_set_motor_driver_L298N_speed(MOTOR_ID_1, -2000);
			resource_set_motor_driver_L298N_speed(MOTOR_ID_2, -2000);
		}
		break;
	}

	return;
}

static void _front_ioa_sensor_changed_cb(unsigned int value, void *data)
{
	app_data *ad = data;

	_D("FRONT has obstacle!");

	ad->f_value = value;

	___________control_motor(ad);

	return;
}

static void _back_ioa_sensor_changed_cb(unsigned int value, void *data)
{
	app_data *ad = data;

	_D("BACK has obstacle!");

	ad->r_value = value;

	___________control_motor(ad);

	return;
}

static bool service_app_create(void *data)
{
	int ret = 0;

	/*
	 * if you want to use default configuration,
	 * Do not need to call resource_set_motor_driver_L298N_configuration(),
	 *
	*/
	ret = resource_set_motor_driver_L298N_configuration(MOTOR_ID_1, 19, 16, 5);
	if (ret) {
		_E("resource_set_motor_driver_L298N_configuration()");
		service_app_exit();
	}
	ret = resource_set_motor_driver_L298N_configuration(MOTOR_ID_2, 26, 20, 4);
	if (ret) {
		_E("resource_set_motor_driver_L298N_configuration()");
		service_app_exit();
	}

	return true;
}

static void service_app_control(app_control_h app_control, void *data)
{
	app_data *ad = data;
	int ret;

	/* set speed 0, to reduce delay of initializing motor driver */
	resource_set_motor_driver_L298N_speed(MOTOR_ID_1, 0);
	resource_set_motor_driver_L298N_speed(MOTOR_ID_2, 0);

	resource_read_infrared_obstacle_avoidance_sensor(FRONT_PIN, &ad->f_value);
	resource_read_infrared_obstacle_avoidance_sensor(REAR_PIN, &ad->r_value);

	resource_set_infrared_obstacle_avoidance_sensor_interrupted_cb(FRONT_PIN,
		_front_ioa_sensor_changed_cb, ad);
	resource_set_infrared_obstacle_avoidance_sensor_interrupted_cb(REAR_PIN,
		_back_ioa_sensor_changed_cb, ad);

	___________control_motor(ad);

	return;
}

static void service_app_terminate(void *data)
{
	app_data *ad = data;

	log_file_close();

	_D("Bye ~");

	return;
}

int main(int argc, char* argv[])
{
	app_data *ad = NULL;
	int ret = 0;
	service_app_lifecycle_callback_s event_callback;
	app_event_handler_h handlers[5] = {NULL, };

	log_type_set(LOG_TYPE_DLOG);

	ad = calloc(1, sizeof(app_data));
	retv_if(!ad, -1);

	event_callback.create = service_app_create;
	event_callback.terminate = service_app_terminate;
	event_callback.app_control = service_app_control;

	service_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY],
		APP_EVENT_LOW_BATTERY, service_app_low_battery, &ad);
	service_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY],
		APP_EVENT_LOW_MEMORY, service_app_low_memory, &ad);
	service_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED],
		APP_EVENT_LANGUAGE_CHANGED, service_app_lang_changed, &ad);
	service_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
		APP_EVENT_REGION_FORMAT_CHANGED, service_app_region_changed, &ad);

	ret = service_app_main(argc, argv, &event_callback, ad);
	if (ret)
		_E("failed to start app");

	return 0;
}

