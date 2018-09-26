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

#ifndef __NET_UTIL_H_
#define __NET_UTIL_H_

#include <net_connection.h>

/**
 * @brief Called whenever WiFi connection has changed.
 * @param[in] ap_mac The MAC address of access point.
 * @param[in] ap_ssid The SSID of the access point.
 * @param[in] ip_addr Device IP address.
 * @param[in] user_data The user defined data.
 */
typedef void (*wifi_connection_changed_cb)(const char *ap_mac, const char *ap_ssid, char *ip_addr, void *user_data);

/**
 * @brief Initializes network utils.
*/
int net_util_init();

/**
 * @brief Finalizes network utils.
*/
void net_util_fini();

/**
 * @brief Sets WiFi connection changed callback.
 * @param[in] callback Callback to be set.
 * @param[in] user_data User defined data.
*/
int net_util_set_wifi_connection_changed_cb(wifi_connection_changed_cb callback, void *user_data);

/**
 * @brief Gets access point MAC address.
 * @param[out] mac The MAC address.
*/
int net_util_get_ap_mac(char **mac);

/**
 * @brief Gets access point SSID.
 * @param[out] The SSID.
*/
int net_util_get_ap_ssid(char **ssid);

/**
 * @brief Gets device IP address.
 * @param[out] The IP address
*/
int net_util_get_ip_addr(char **ip);

#endif