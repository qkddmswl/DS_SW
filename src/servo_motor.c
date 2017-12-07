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

#include "log.h"
#include "pca9685.h"

int servo_motor_init(void)
{
	int ret = 0;
	ret = pca9685_init();
	if (ret) {
		_E("failed to init PCA9685");
		return -1;
	}

	return 0;
}

int servo_motor_fini(void)
{
	pca9685_fini();
	return 0;
}

int servo_motor_value_set(int motor_id, int value)
{
	int ret = 0;
	ret = pca9685_set_value_to_channel(motor_id, 0, value);
	return ret;
}
