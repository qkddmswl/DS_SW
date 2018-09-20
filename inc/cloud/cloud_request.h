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

#ifndef __CLOUD_REQUEST_H_
#define __CLOUD_REQUEST_H_

#include <gio/gio.h>
#include "car_info.h"

/**
 * @brief Enum that indicates if HTTP request finished succesfully or not.
*/
typedef enum
{
    SUCCESS = 0,
    FAILURE
} request_result_e;

/**
 * @brief Called when data from HTTP /api/racing GET request was obtained.
 *
 * @param[in] result Result of request.
 * @param[in] car_info The array of car_info struct.
 * @param[in] size Length of the car_info array
 * @param[in] user_data User data passed in @cloud_request_api_racing_get function.
 */
typedef void(*cloud_request_car_list_data_cb)(request_result_e result, car_info_t **car_info, int size, void *user_data);

/**
 * @brief Called when data from HTTP /api/racing POST request was obtained.
 *
 * @param[in] result Result of request.
 * @param[in] user_data User data passed in @cloud_request_api_racing_post function.
 */
typedef void(*cloud_request_car_post_finish_cb)(request_result_e result, void *user_data);

/**
 * @brief Sends cloud request that obtains list of registered cars.
 *
 * @param[in] ap_mac Mac address of access point that device is connected to.
 * @param[in] callback Function that will be invoked, when request will be finished.
 *
 * @return Returns @GCancellable object that allows to cancel this request.
 * @remark To cancel task function g_cancellable_cancel should be called.
 */
GCancellable *cloud_request_api_racing_get(const char *ap_mac, cloud_request_car_list_data_cb callback, void *user_data);

/**
 * @brief Sends cloud request registering the car.
 *
 * @param[in] car_info The car_info object with car data.
 * @param[in] callback Function that will be invoked, when request will be finished.
 *
 * @return Returns @GCancellable object that allows to cancel this request.
 * @remark To cancel task function g_cancellable_cancel should be called.
 */
GCancellable *cloud_request_api_racing_post(const car_info_t *car_info, cloud_request_car_post_finish_cb callback, void *user_data);

#endif