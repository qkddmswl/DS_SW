/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

#include "messages/clock.h"

float clock_monotonic_get()
{
	struct timespec ret = {0,};

	if (clock_gettime(CLOCK_MONOTONIC, &ret) != 0) {
		abort();
	}

	return ret.tv_sec + (float)ret.tv_nsec / 1000000000.0f;
}

time_t clock_realtime_get()
{
	struct timespec ret = {0,};

	if (clock_gettime(CLOCK_REALTIME, &ret) != 0) {
		abort();
	}

	return ret.tv_sec;
}

bool clock_is_supported()
{
	struct timespec ret = {0,};

	if (clock_gettime(CLOCK_MONOTONIC, &ret) != 0) {
		return false;
	}
	if (clock_gettime(CLOCK_REALTIME, &ret) != 0) {
		return false;
	}
	return true;
}

