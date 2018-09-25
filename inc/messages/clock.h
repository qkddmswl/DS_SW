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

#ifndef _CLOCK_MONOTONIC_H
#define _CLOCK_MONOTONIC_H

#include <time.h>
#include <stdbool.h>

/**
 * @brief Check if platform supports all required clock types
 *
 * @return true if supported, false otherwise.
 */
bool clock_is_supported();

/**
 * @brief Gets current time using monotonic clock
 *
 * @note the func will abort if monotonic clock is not supported.
 * call @clock_is_supported beforehead to validate if platform
 * supports all clock types.
 *
 * @return: seconds since unspecified time point.
 */
float clock_monotonic_get();

/**
 * @brief Gets current time using realtime clock
 *
 * @note the func will abort if realtime clock is not supported.
 * call @clock_is_supported beforehead to validate if platform
 * supports all clock types.
 *
 * @return: number of seconds since Epoch
 */
time_t clock_realtime_get();

#endif
