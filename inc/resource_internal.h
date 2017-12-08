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

#ifndef __POSITION_FINDER_RESOURCE_INTERNAL_H__
#define __POSITION_FINDER_RESOURCE_INTERNAL_H__

#include <peripheral_io.h>
#include <stdint.h>

#define PIN_MAX 40

typedef void (*resource_read_cb)(double value, void *data);

typedef struct _resource_read_cb_s {
	resource_read_cb cb;
	void *data;
	int pin_num;
} resource_read_s;

typedef void (*resource_changed_cb)(unsigned int value, void *data);

typedef struct _resource_changed_s {
	resource_changed_cb cb;
	void *data;
	int pin_num;
} resource_changed_s;

typedef struct _resource_s {
	int opened;
	peripheral_gpio_h sensor_h;
	void (*close) (int);
	/*FIXME*/
	resource_changed_s *resource_changed_info;
} resource_s;


extern resource_s *resource_get_info(int pin_num);
extern void resource_close_all(void);

#endif /* __POSITION_FINDER_RESOURCE_INTERNAL_H__ */
