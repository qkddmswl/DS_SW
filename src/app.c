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
#include "dc_motor.h"
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

static void ___________control_motor(app_data *ad)
{
	_D("control motor, state(%u), f_val(%u), r_val(%u)",
			ad->dir_state, ad->f_value, ad->r_value);


	switch (ad->dir_state) {
		case DIR_STATE_F:
			if (ad->f_value)  {
				if (ad->r_value) {
					ad->dir_state = DIR_STATE_S;
					dc_motor_speed_set(DC_MOTOR_ID_L, 0);
					dc_motor_speed_set(DC_MOTOR_ID_R, 0);
				} else {
					ad->dir_state = DIR_STATE_B;
					dc_motor_speed_set(DC_MOTOR_ID_L, -2000);
					dc_motor_speed_set(DC_MOTOR_ID_R, -2000);
				}
			}
			break;
		case DIR_STATE_B:
			if (ad->r_value)  {
				if (ad->f_value) {
					ad->dir_state = DIR_STATE_S;
					dc_motor_speed_set(DC_MOTOR_ID_L, 0);
					dc_motor_speed_set(DC_MOTOR_ID_R, 0);
				} else {
					ad->dir_state = DIR_STATE_F;
					dc_motor_speed_set(DC_MOTOR_ID_L, 2000);
					dc_motor_speed_set(DC_MOTOR_ID_R, 2000);
				}
			}
			break;
		case DIR_STATE_S:
			if (!ad->f_value)  {
				ad->dir_state = DIR_STATE_F;
				dc_motor_speed_set(DC_MOTOR_ID_L, 2000);
				dc_motor_speed_set(DC_MOTOR_ID_R, 2000);
			} else if (!ad->r_value) {
				ad->dir_state = DIR_STATE_B;
				dc_motor_speed_set(DC_MOTOR_ID_L, -2000);
				dc_motor_speed_set(DC_MOTOR_ID_R, -2000);
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

	ret = dc_motor_init();
	if (ret) {
		_E("failed init motor, terminating this application");
		service_app_exit();
	}

	return true;
}

static void service_app_control(app_control_h app_control, void *data)
{
	app_data *ad = data;
	int ret;

	resource_read_infrared_obstacle_avoidance_sensor(FRONT_PIN, &ad->f_value);
	resource_read_infrared_obstacle_avoidance_sensor(REAR_PIN, &ad->r_value);

	resource_set_infrared_obstacle_avoidance_sensor_interrupted_cb(FRONT_PIN, _front_ioa_sensor_changed_cb, ad);
	resource_set_infrared_obstacle_avoidance_sensor_interrupted_cb(REAR_PIN, _back_ioa_sensor_changed_cb, ad);

	___________control_motor(ad);

	return;
}

static void service_app_terminate(void *data)
{
	app_data *ad = data;

	dc_motor_fini();
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

