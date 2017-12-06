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

typedef struct app_data_s {
	guint timer_id;
} app_data;

static void service_app_control(app_control_h app_control, void *data)
{
	return;
}

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

static inline int __get_r_val(int val)
{
	if (val > 0)
		return 0 - val;
	else
		return ABS(val);
}

static gboolean __control_dcmotor_cb(gpointer user_data)
{
	static int value = -2000;
	int value2 = 0;

	value = __get_r_val(value);

	dc_motor_speed_set(DC_MOTOR_ID_L, 0);
	dc_motor_speed_set(DC_MOTOR_ID_R, 0);

	sleep(1);

	dc_motor_speed_set(DC_MOTOR_ID_L, value);
	dc_motor_speed_set(DC_MOTOR_ID_R, value);

	sleep(5);
	if (value > 0)
		value2 = value + 1000;
	else
		value2 = value - 1000;

	dc_motor_speed_set(DC_MOTOR_ID_L, value2);
	dc_motor_speed_set(DC_MOTOR_ID_R, value2);

	return G_SOURCE_CONTINUE;
}

static bool service_app_create(void *data)
{
	app_data *ad = data;
	int ret = 0;
	ret = dc_motor_init();
	if (ret) {
		_E("failed init motor, terminating this application");
		service_app_exit();
	}

	ad->timer_id = g_timeout_add_seconds(10, __control_dcmotor_cb, ad);

	return true;
}

static void service_app_terminate(void *data)
{
	app_data *ad = data;

	if (ad->timer_id) {
		g_source_remove(ad->timer_id);
		ad->timer_id = 0;
	}

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

