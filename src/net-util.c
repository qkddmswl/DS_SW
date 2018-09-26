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

#include "net-util.h"
#include <wifi-manager.h>
#include <string.h>
#include <stdlib.h>
#include "log.h"

typedef struct net_util_ {
    wifi_manager_h wifi;
    connection_h conn;
    connection_type_e net_state;
    char *ap_mac;
    char *ap_ssid;
    char *ip_addr;
    wifi_connection_changed_cb cb;
    void *cb_user_data;
} net_util_t;

static net_util_t net_util;

static int _set_ap_mac();
static int _set_ap_ssid();
static int _set_ip_addr();
static void _connection_changed_cb(connection_type_e type, void *user_data);

int net_util_init()
{
    int ret = 0;
    ret = connection_create(&net_util.conn);
    if (ret != CONNECTION_ERROR_NONE) {
        _E("connection_create() failed, error: %s", get_error_message(ret));
        return -1;
    }

    ret = connection_get_type(net_util.conn, &net_util.net_state);
    if (ret != CONNECTION_ERROR_NONE) {
        _E("connection_get_type() failed, error: %s", get_error_message(ret));
        goto ERROR_1;
    }

    ret = wifi_manager_initialize(&net_util.wifi);
    if (ret != WIFI_MANAGER_ERROR_NONE) {
        _E("wifi_manager_initialize() failed, error: %s", get_error_message(ret));
        goto ERROR_1;
    }

    ret = connection_set_type_changed_cb(net_util.conn, _connection_changed_cb, NULL);
    if (ret != CONNECTION_ERROR_NONE) {
        _E("connection_set_type_changed_cb() failed, error: %s", get_error_message(ret));
        goto ERROR_2;
    }

    _set_ap_mac();
    _set_ap_ssid();
    _set_ip_addr();

    return 0;

    ERROR_2:
    ret = wifi_manager_deinitialize(&net_util.wifi);
    if (ret != WIFI_MANAGER_ERROR_NONE) {
        _E("wifi_manager_deinitialize() failed, error: %s", get_error_message(ret));
    }

    ERROR_1:
    ret = connection_destroy(&net_util.conn);
    if (ret != CONNECTION_ERROR_NONE) {
        _E("wifi_manager_deinitialize() failed, error: %s", get_error_message(ret));
    }
    return -1;
}

void net_util_fini()
{
    net_util.net_state = CONNECTION_TYPE_DISCONNECTED;

    int ret = 0;
    ret = connection_destroy(net_util.conn);
    if (ret != CONNECTION_ERROR_NONE) {
        _E("connection_set_type_changed_cb() failed, error: %s", get_error_message(ret));
    }

    ret = wifi_manager_deinitialize(&net_util.wifi);
    if (ret != WIFI_MANAGER_ERROR_NONE) {
        _E("wifi_manager_deinitialize() failed, error: %s", get_error_message(ret));
    }

    free(net_util.ap_mac);
    free(net_util.ap_ssid);
    free(net_util.ip_addr);
}

int net_util_set_wifi_connection_changed_cb(wifi_connection_changed_cb callback, void *user_data)
{
    if (net_util.cb)
    {
        _E("Callback already set");
        return -1;
    }

    net_util.cb = callback;
    net_util.cb_user_data = user_data;
    return 0;
}

int net_util_get_ap_mac(char **mac)
{
    retv_if(!mac, -1);

    if (!net_util.ap_mac && _set_ap_mac() != 0) {
        return -1;
    }

    *mac = strdup(net_util.ap_mac);
    return 0;
}

int net_util_get_ap_ssid(char **ssid)
{
    retv_if(!ssid, -1);

    if (!net_util.ap_ssid && _set_ap_ssid() != 0) {
        return -1;
    }

    *ssid = strdup(net_util.ap_ssid);
    return 0;
}

int net_util_get_ip_addr(char **ip)
{
    retv_if(!ip, -1);

    if (!net_util.ip_addr && _set_ip_addr() != 0) {
        return -1;
    }

    *ip = strdup(net_util.ip_addr);
    return 0;
}

static int _set_ap_mac()
{
    wifi_manager_ap_h ap_h = NULL;
    int ret = WIFI_MANAGER_ERROR_NONE;

    free(net_util.ap_mac);
    net_util.ap_mac = NULL;

    if (net_util.net_state != CONNECTION_TYPE_WIFI) {
        return -1;
    }

    ret = wifi_manager_get_connected_ap(net_util.wifi, &ap_h);
    if (ret != WIFI_MANAGER_ERROR_NONE) {
        _E("wifi_manager_get_connected_ap() failed, error: %s", get_error_message(ret));
        return -1;
    }

    int r_code = 0;
    ret = wifi_manager_ap_get_bssid(ap_h, &net_util.ap_mac);
    if (ret != WIFI_MANAGER_ERROR_NONE) {
        _E("wifi_manager_ap_get_bssid() failed, error: %s", get_error_message(ret));
        r_code = -1;
    }

    ret = wifi_manager_ap_destroy(ap_h);
    if (ret != WIFI_MANAGER_ERROR_NONE) {
        _E("wifi_manager_ap_destroy() failed, error: %s", get_error_message(ret));
    }
    return r_code;
}

static int _set_ap_ssid()
{
    wifi_manager_ap_h ap_h = NULL;
    int ssid_len;
    int ret = WIFI_MANAGER_ERROR_NONE;

    free(net_util.ap_ssid);
    net_util.ap_ssid = NULL;

    if (net_util.net_state != CONNECTION_TYPE_WIFI) {
        return -1;
    }

    ret = wifi_manager_get_connected_ap(net_util.wifi, &ap_h);
    if (ret != WIFI_MANAGER_ERROR_NONE) {
        _E("wifi_manager_get_connected_ap() failed, error: %s", get_error_message(ret));
        return -1;
    }

    int r_code = 0;
    ret = wifi_manager_ap_get_raw_ssid(ap_h, &net_util.ap_ssid, &ssid_len);
    if (ret != WIFI_MANAGER_ERROR_NONE) {
        _E("wifi_manager_ap_get_raw_ssid() failed, error: %s", get_error_message(ret));
        r_code = -1;
    }

    ret = wifi_manager_ap_destroy(ap_h);
    if (ret != WIFI_MANAGER_ERROR_NONE) {
        _E("wifi_manager_ap_destroy() failed, error: %s", get_error_message(ret));
    }
    return r_code;
}

static int _set_ip_addr()
{
    int ret = 0;

    free(net_util.ip_addr);
    net_util.ap_ssid = NULL;

    if (net_util.net_state == CONNECTION_TYPE_DISCONNECTED) {
        return -1;
    }

    ret = connection_get_ip_address(net_util.conn, CONNECTION_ADDRESS_FAMILY_IPV4, &net_util.ip_addr);
    if (CONNECTION_ERROR_NONE != ret) {
        _E("connection_get_ip_address() failed, error: %s", get_error_message(ret));
        return -1;
    }

    return 0;
}

static void _connection_changed_cb(connection_type_e type, void *user_data)
{
    net_util.net_state = type;
    _set_ap_mac();
    _set_ap_ssid();
    _set_ip_addr();

    if (type == CONNECTION_TYPE_WIFI && net_util.cb) {
        net_util.cb(net_util.ap_mac, net_util.ap_ssid, net_util.ip_addr, net_util.cb_user_data);
    }
}