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

#ifndef __CAR_INFO_SERIALIZER_H_
#define __CAR_INFO_SERIALIZER_H_

#include "car_info.h"

/**
 * @brief Serializes car_info_t struct into json string.
 * @param[in] car_info The struct with car data.
 * @return Json with car data.
 * @remarks Returned value should be freed.
 */
char *car_info_serializer_serialize(car_info_t *car_info);

/**
 * @brief Deserializes json string to array of car_info_t structs.
 * @param[in] json_data Json string with config.
 * @param[out] size Length of car_info_t array.
 * @return Dynamically allocated car_info_t array or NULL on error.
 * @remarks Returned array should be released with @free and its every element with @car_info_destroy.
 */
car_info_t **car_info_serializer_deserialize_array(const char *json_data, int *size);

#endif