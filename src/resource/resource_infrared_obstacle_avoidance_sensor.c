/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
 *
 * Contact: Jin Yoon <jinny.yoon@samsung.com>
 *          Geunsun Lee <gs86.lee@samsung.com>
 *          Eunyoung Lee <ey928.lee@samsung.com>
 *          Junkyu Han <junkyu.han@samsung.com>
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
#include "resource_internal.h"

void resource_close_infrared_obstacle_avoidance_sensor(int pin_num)
{
	if (!resource_get_info(pin_num)->opened) return;

	_I("Infrared Obstacle Avoidance Sensor is finishing...");
	if (resource_get_info(pin_num)->resource_changed_info) {
		free(resource_get_info(pin_num)->resource_changed_info);
		resource_get_info(pin_num)->resource_changed_info = NULL;
	}
	peripheral_gpio_unset_interrupted_cb(resource_get_info(pin_num)->sensor_h);
	peripheral_gpio_close(resource_get_info(pin_num)->sensor_h);
	resource_get_info(pin_num)->sensor_h = NULL;
	resource_get_info(pin_num)->opened = 0;

	return;
}

static int _init_pin(int pin_num)
{
	int ret = PERIPHERAL_ERROR_NONE;

	ret = peripheral_gpio_open(pin_num, &resource_get_info(pin_num)->sensor_h);
	retv_if(ret != PERIPHERAL_ERROR_NONE, -1);

	ret = peripheral_gpio_set_direction(resource_get_info(pin_num)->sensor_h, PERIPHERAL_GPIO_DIRECTION_IN);
	if (ret != PERIPHERAL_ERROR_NONE) {
		peripheral_gpio_close(resource_get_info(pin_num)->sensor_h);
		resource_get_info(pin_num)->sensor_h = NULL;
		return -1;
	}

	resource_get_info(pin_num)->opened = 1;
	resource_get_info(pin_num)->close = resource_close_infrared_obstacle_avoidance_sensor;

	return 0;
}

int resource_read_infrared_obstacle_avoidance_sensor(int pin_num, unsigned int *out_value)
{
	int ret = PERIPHERAL_ERROR_NONE;

	if (!resource_get_info(pin_num)->opened) {
		ret = _init_pin(pin_num);
		retv_if(ret < 0, -1);
	}

	ret = peripheral_gpio_read(resource_get_info(pin_num)->sensor_h, out_value);
	retv_if(ret != PERIPHERAL_ERROR_NONE, -1);

	*out_value = !*out_value;

	_I("Infrared Obstacle Avoidance Sensor Value : %d", *out_value);

	return 0;
}


static void __ioa_sensor_interrupt_cb(peripheral_gpio_h gpio, peripheral_error_e error, void *user_data)
{
	uint32_t value;
	resource_changed_s *resource_changed_info = (resource_changed_s *)user_data;

	/* Detected : 0, Non-detected : 1 */
	peripheral_gpio_read(gpio, &value);
	_D("interrupt value = %d", !value);

	resource_changed_info->cb(!value, resource_changed_info->data);

	return;
}

int resource_set_infrared_obstacle_avoidance_sensor_interrupted_cb(int pin_num, resource_changed_cb cb, void *data)
{
	int ret = PERIPHERAL_ERROR_NONE;

	if (resource_get_info(pin_num)->resource_changed_info == NULL) {
		resource_get_info(pin_num)->resource_changed_info = calloc(1, sizeof(resource_changed_s));
		retv_if(!resource_get_info(pin_num)->resource_changed_info, -1);
	} else {
		if (resource_get_info(pin_num)->sensor_h)
			peripheral_gpio_unset_interrupted_cb(resource_get_info(resource_get_info(pin_num)->resource_changed_info->pin_num)->sensor_h);
	}

	resource_get_info(pin_num)->resource_changed_info->cb = cb;
	resource_get_info(pin_num)->resource_changed_info->data = data;
	resource_get_info(pin_num)->resource_changed_info->pin_num = pin_num;

	if (!resource_get_info(pin_num)->opened) {
		ret = _init_pin(pin_num);
		goto_if(ret < 0, FREE_RESOURCE);
	}

	if (resource_get_info(pin_num)->sensor_h) {
		ret = peripheral_gpio_set_edge_mode(resource_get_info(pin_num)->sensor_h, PERIPHERAL_GPIO_EDGE_BOTH);
		goto_if(ret != PERIPHERAL_ERROR_NONE, FREE_RESOURCE);

		ret = peripheral_gpio_set_interrupted_cb(resource_get_info(pin_num)->sensor_h, __ioa_sensor_interrupt_cb, resource_get_info(pin_num)->resource_changed_info);
		goto_if(ret != PERIPHERAL_ERROR_NONE, FREE_RESOURCE);
	}

	return 0;

FREE_RESOURCE:
	if (resource_get_info(pin_num)->resource_changed_info) {
		free(resource_get_info(pin_num)->resource_changed_info);
		resource_get_info(pin_num)->resource_changed_info = NULL;
	}

	if (resource_get_info(pin_num)->sensor_h) {
		peripheral_gpio_unset_interrupted_cb(resource_get_info(pin_num)->sensor_h);
		peripheral_gpio_close(resource_get_info(pin_num)->sensor_h);
		resource_get_info(pin_num)->sensor_h = NULL;
		resource_get_info(pin_num)->opened = 0;
	}

	return -1;
}
