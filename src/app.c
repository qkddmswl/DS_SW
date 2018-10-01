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
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <glib.h>
#include <service_app.h>
#include "log.h"
#include "resource.h"
#include "net-util.h"
#include "config.h"
#include "cloud/cloud_communication.h"
#include "messages/message_manager.h"
#include "controller_connection_manager.h"
#include "command.h"

#define ENABLE_MOTOR 1

#define CONFIG_GRP_CAR "Car"
#define CONFIG_KEY_ID "Id"
#define CONFIG_KEY_NAME "Name"
#define CLOUD_REQUESTS_FREQUENCY 15

enum {
	DIR_STATE_S,
	DIR_STATE_F,
	DIR_STATE_B,
};

typedef struct app_data_s {
	unsigned int f_value;
	unsigned int r_value;
	unsigned int dir_state;
	guint idle_h;
} app_data;

static void _initialize_components(app_data *ad);
static void _initialize_config();

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
	_E("low battery! exit app");
	service_app_exit();

	return;
}

static void service_app_low_memory(app_event_info_h event_info, void *user_data)
{
	return;
}

static inline double __map_round(double val)
{
	return floor(val + 0.5);
}

static int __map_range_val(int d_max, int d_min, int v_max, int v_min, int val)
{
	int rval = 0;
	double slope = 0;
	slope = 1.0 * (d_max - d_min) / (v_max - v_min);

	rval = d_min + __map_round(slope * (val - v_min));

	return rval;
}

static int ___map_speed_val(int speed)
{
	static const int motor_max = 4095;
	static const int motor_min = -4095;
	static const int speed_max = 1000;
	static const int speed_min = -1000;

	return __map_range_val(motor_max, motor_min,
		speed_max, speed_min, speed);
}

static int ___map_servo_val(int servo)
{
	static const int motor_max = 500;
	static const int motor_min = 400;
	static const int servo_max = 1000;
	static const int servo_min = -1000;

	return __map_range_val(motor_max, motor_min,
		servo_max, servo_min, servo);
}

static int __driving_motors(int servo, int speed)
{
	int val_speed;
	int val_servo;

	val_servo = ___map_servo_val(servo);
	val_speed = ___map_speed_val(speed);

	_D("control motor - servo[%4d : %4d], speed[%4d : %4d]",
		servo, val_servo, speed, val_speed);
#if ENABLE_MOTOR
	resource_set_servo_motor_value(0, val_servo);
	resource_set_motor_driver_L298N_speed(MOTOR_ID_1, val_speed);
	resource_set_motor_driver_L298N_speed(MOTOR_ID_2, val_speed);
#endif

	return 0;
}

static void __camera(int azimuth, int elevation)
{
	//TODO: Camera steering
}

static void __command_received_cb(command_s command) {
	switch(command.type) {
	case COMMAND_TYPE_DRIVE:
		__driving_motors(command.data.steering.direction, command.data.steering.speed);
		break;
	case COMMAND_TYPE_CAMERA:
		__camera(command.data.camera_position.camera_azimuth, command.data.camera_position.camera_elevation);
		break;
	case COMMAND_TYPE_DRIVE_AND_CAMERA:
		__driving_motors(command.data.steering_and_camera.direction, command.data.steering_and_camera.speed);
		__camera(command.data.steering_and_camera.camera_azimuth, command.data.steering_and_camera.camera_elevation);
		break;
	case COMMAND_TYPE_NONE:
		break;
	default:
		_E("Unknown command type");
		break;
	}
}

static void _initialize_config()
{
	config_init();

	char *id = NULL;
	char *name = NULL;
	gboolean modified = false;
	if (config_get_string(CONFIG_GRP_CAR, CONFIG_KEY_ID, &id) != 0) {
		char *uuid = g_uuid_string_random();
		config_set_string(CONFIG_GRP_CAR, CONFIG_KEY_ID, uuid);
		g_free(uuid);
		modified = true;
	}
	if (config_get_string(CONFIG_GRP_CAR, CONFIG_KEY_NAME, &id) != 0) {
		config_set_string(CONFIG_GRP_CAR, CONFIG_KEY_NAME, "Passerati");
		modified = true;
	}
	if (modified == true) {
		config_save();
	}
	free(id);
	free(name);
}

static void _initialize_components(app_data *ad)
{
	net_util_init();
	_initialize_config();
	cloud_communication_init();
	message_manager_init();
	controller_connection_manager_listen();
}

static bool service_app_create(void *data)
{
	int ret = 0;
	app_data *ad = data;

	/*
	 * if you want to use default configuration,
	 * Do not need to call resource_set_motor_driver_L298N_configuration(),
	 *
	*/
#if ENABLE_MOTOR
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
#endif

	_initialize_components(ad);
	cloud_communication_start(CLOUD_REQUESTS_FREQUENCY);

	controller_connection_manager_set_command_received_cb(__command_received_cb);

	return true;
}

static void service_app_control(app_control_h app_control, void *data)
{

#if ENABLE_MOTOR
	/* set speed 0, to reduce delay of initializing motor driver */
	resource_set_motor_driver_L298N_speed(MOTOR_ID_1, 0);
	resource_set_motor_driver_L298N_speed(MOTOR_ID_2, 0);
	resource_set_servo_motor_value(0, 450);
#endif

	return;
}

static void service_app_terminate(void *data)
{
	app_data *ad = data;

	if (ad->idle_h)
		g_source_remove(ad->idle_h);


	controller_connection_manager_release();
	message_manager_shutdown();

	cloud_communication_stop();
	cloud_communication_fini();
	config_shutdown();
	net_util_fini();

	resource_close_all();
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
