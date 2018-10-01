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

#include "cloud/cloud_communication.h"
#include <glib.h>
#include <wifi-manager.h>
#include <stdlib.h>
#include "cloud/car_info.h"
#include "cloud/cloud_request.h"
#include "log.h"
#include "config.h"
#include "net-util.h"

typedef struct communication_data_ {
    gboolean is_initialized;
    gboolean is_running;
    car_info_t *car_info;
    guint source_id;
} communication_data_t;

static communication_data_t _communication;

static void post_response_cb(request_result_e result, void *user_data);
static gboolean post_timer_cb(gpointer data);
static void wifi_changed_cb(const char *ap_mac, const char *ap_ssid, char *ip_addr, void *user_data);

static int set_car_id();
static int set_car_ip();
static int set_car_name();
static int set_ap_mac();
static int set_ap_ssid();

int cloud_communication_init()
{
    retvm_if(_communication.is_initialized, -1, "Cloud communication is already initialized");
    _communication.car_info = car_info_create();

    if (set_car_id() != 0) {
        return -1;
    }
    if (set_car_ip() != 0) {
        return -1;
    }
    if (set_car_name() != 0) {
        return -1;
    }
    if (set_ap_mac() != 0) {
        return -1;
    }
    if (set_ap_ssid() != 0) {
        return -1;
    }

    net_util_set_wifi_connection_changed_cb(wifi_changed_cb, NULL);

    _communication.is_initialized = true;
    return 0;
}

void cloud_communication_start(int interval)
{
    retm_if(!_communication.is_initialized, "Cloud communication is not initialized");
    retm_if(_communication.is_running, "Cloud communication is already running");

    _communication.source_id = g_timeout_add_seconds(interval, post_timer_cb, _communication.car_info);
    _communication.is_running = TRUE;
}

void cloud_communication_stop()
{
    retm_if(!_communication.is_initialized, "Cloud communication is not initialized");
    retm_if(_communication.is_running, "Cloud communication is already stopped");

    g_source_remove(_communication.source_id);
    _communication.is_running = FALSE;
}

void cloud_communication_fini()
{
    retm_if(!_communication.is_initialized, "Cloud communication is already finalized");

    cloud_communication_stop();
    car_info_destroy(_communication.car_info);
}

static void post_response_cb(request_result_e result, void *user_data)
{
    if (result == SUCCESS) {
        _I("POST SUCCESS");
    }
    else {
        _I("POST FAILURE");
    }
}

static gboolean post_timer_cb(gpointer data)
{
    retv_if(!data, FALSE);
    car_info_t *car = (car_info_t *)data;
    cloud_request_api_racing_post(car, post_response_cb, NULL);
    return TRUE;
}

static int set_car_id()
{
    char *id = NULL;
    int ret = 0;
    ret = config_get_string("Car", "Id", &id);
    if (ret != 0) {
        _E("Getting car ID from config failed!");
        return -1;
    }

    car_info_set_car_id(_communication.car_info, id);
    g_free(id);
    return 0;
}

static int set_car_ip()
{
    char *ip;
    int ret = net_util_get_ip_addr(&ip);
    if (ret != 0) {
        return -1;
    }
    car_info_set_car_ip(_communication.car_info, ip);
    g_free(ip);
    return 0;
}

static int set_car_name()
{
    char *name;
    int ret = 0;

    ret = config_get_string("Car", "Name", &name);
    if (ret != 0) {
        _E("Getting car name from config failed!");
        return -1;
    }
    car_info_set_car_name(_communication.car_info, name);
    g_free(name);
    return 0;
}

static int set_ap_mac()
{
    char *mac;
    int ret = net_util_get_ap_mac(&mac);
    if (ret != 0) {
        return -1;
    }
    car_info_set_car_ap_mac(_communication.car_info, mac);
    g_free(mac);
    return 0;
}

static int set_ap_ssid()
{
    char *ssid;
    int ret = net_util_get_ap_ssid(&ssid);
    if (ret != 0) {
        return -1;
    }
    car_info_set_ap_ssid(_communication.car_info, ssid);
    g_free(ssid);
    return 0;
}

static void wifi_changed_cb(const char *ap_mac, const char *ap_ssid, char *ip_addr, void *user_data)
{
    car_info_set_car_ap_mac(_communication.car_info, ap_mac);
    car_info_set_ap_ssid(_communication.car_info, ap_ssid);
    car_info_set_car_ip(_communication.car_info, ip_addr);
}
