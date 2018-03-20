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
#include "receiver.h"
#include "message.h"
#include "connection_manager.h"

#define ENABLE_MOTOR 1

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

static gboolean __message_dispatcher(gpointer user_data)
{
	message_s *msg = NULL;

	do {
		msg = message_pop_from_inqueue();
		if (msg) {
			switch (msg->cmd) {
			case MESSAGE_CMD_HELLO:
				/* TODO : say hello to sender */
				break;
			case MESSAGE_CMD_CALIBRATION:
				/* TODO : set calibration mode */
				break;
			case MESSAGE_CMD_DRIVE:
			/* TODO : driving car */
				__driving_motors(msg->servo, msg->speed);
				break;
			case MESSAGE_CMD_BYE:
				__driving_motors(0, 0);
				break;
			}
		}
		free(msg);
	} while (msg);

	return TRUE;
}

static void __recv_state_change(receiver_type_e type,
	receiver_state_e state, void* user_data)
{
	app_data *ad = user_data;
	ret_if(!ad);

	_D("receiver type[%d] state changed[%d]", type, state);

	if (state == RECEIVER_STATE_CONNECTED) {
		if (!ad->idle_h)
			ad->idle_h = g_idle_add(__message_dispatcher, ad);
	} else {
		if (ad->idle_h) {
			g_source_remove(ad->idle_h);
			ad->idle_h = 0;
		}
		__driving_motors(0, 0);
	}

	return;
}

static void __conn_state_changed_cb(connection_state_e state,
	const char *ip, void* user_data)
{
	app_data *ad = user_data;

	_D("connection state changed : %d", state);

	if (state == CONNECTION_STATE_CONNECTED) {
		receiver_start(RECEIVER_TYPE_UDP);

	} else {
		receiver_stop(RECEIVER_TYPE_UDP);

		if (ad->idle_h) {
			g_source_remove(ad->idle_h);
			ad->idle_h = 0;
		}

		__driving_motors(0, 0);
	}
	return;
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

	receiver_init(RECEIVER_TYPE_UDP);
	receiver_set_state_changed_cb(RECEIVER_TYPE_UDP, __recv_state_change, ad);

	connection_manager_init();
	connection_manager_set_state_changed_cb(__conn_state_changed_cb, ad);

	message_queue_new();

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


	connection_manager_fini();
	receiver_fini(RECEIVER_TYPE_UDP);

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
