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

#ifndef __CAR_INFO_H_
#define __CAR_INFO_H_

#include <stdbool.h>

/**
 * @brief The car info structure template.
 */
typedef struct car_info car_info_t;

/**
 * @brief Creates new instance of car_info_t.
 * @return new car_info_t object, or NULL on error.
 */
car_info_t *car_info_create();

/**
 * @brief Crates deep copy of given car_info_t object.
 * @return Copied car_info_t object, or NULL on error.
 */
car_info_t *car_info_copy(const car_info_t *car_info);

/**
 * @brief Releases car_info_t.
 * @param[in] car_info Car info struct.
 */
void car_info_destroy(car_info_t *car_info);

/**
 * @brief Checks if car_info_t structure core fields are set.
 * @param[in] car_info Car info struct.
 * @returns Returns true if object is valid, otherwise false.
 */
bool car_info_is_valid(car_info_t *car_info);

/**
 * @brief Gets car id.
 * @param[in] car_info Car info struct.
 * @param[out] car_id The car id.
 * @return Returns car id or NULL on error.
 * @remark This value is valid only during car_info life.
 */
const char *car_info_get_car_id(car_info_t *car_info);

/**
 * @brief Sets car id.
 * @param[in] car_info Car info struct.
 * @param[in] car_id The car id.
 * @return Returns 0 on success, -1 otherwise.
 */
int car_info_set_car_id(car_info_t *car_info, const char *car_id);

/**
 * @brief Gets car name.
 * @param[in] car_info Car info struct.
 * @param[out] car_id The car id.
 * @return Returns car name or NULL on error.
 * @remark This value is valid only during car_info life.
 */
const char *car_info_get_car_name(car_info_t *car_info);

/**
 * @brief Sets car name.
 * @param[in] car_info Car info struct.
 * @param[in] car_name The car name.
 * @return Returns 0 on success, -1 otherwise.
 */
int car_info_set_car_name(car_info_t *car_info, const char *car_name);

/**
 * @brief Gets car ip.
 * @param[in] car_info Car info struct.
 * @param[out] car_ip The car ip.
 * @return Returns car ip or NULL on error.
 * @remark This value is valid only during car_info life.
 */
const char *car_info_get_car_ip(car_info_t *car_info);

/**
 * @brief Sets car ip.
 * @param[in] car_info Car info struct.
 * @param[in] car_ip The car ip.
 * @return Returns 0 on success, -1 otherwise.
 * @remarks Param car_ip should be valid ip address.
 */
int car_info_set_car_ip(car_info_t *car_info, const char *car_ip);

/**
 * @brief Gets access point mac.
 * @param[in] car_info Car info struct.
 * @param[out] ap_mac The access point mac address.
 * @return Returns access po id or NULL on error.
 * @remark This value is valid only during car_info life.
 */
const char *car_info_get_ap_mac(car_info_t *car_info);

/**
 * @brief Sets access point mac.
 * @param[in] car_info Car info struct.
 * @param[in] ap_mac The access point mac address.
 * @return Returns 0 on success, -1 otherwise.
 * @remarks Param ap_max should be valid mac address.
 */
int car_info_set_car_ap_mac(car_info_t *car_info, const char *ap_mac);

/**
 * @brief Gets access point ssid.
 * @param[in] car_info Car info struct.
 * @param[out] ap_ssid The access point ssid.
 * @return Returns -1 if any error occurred, 0 otherwise.
 */
const char *car_info_get_ap_ssid(car_info_t *car_info);

/**
 * @brief Sets access point ssid.
 * @param[out] ap_ssid The access point ssid.
 * @param[in] car_id The car id.
 * @return Returns 0 on success, -1 otherwise.
 */
int car_info_set_ap_ssid(car_info_t *car_info, const char *ap_ssid);

#endif