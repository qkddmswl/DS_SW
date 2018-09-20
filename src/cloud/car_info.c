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
#include <stdio.h>
#include <glib.h>
#include <string.h>
#include <regex.h>
#include <arpa/inet.h>
#include <ctype.h>
#include "cloud/car_info.h"
#include "log.h"

#define MAX_LENGTH 256
#define MAX_LENGTH_IP 16
#define MAX_LENGTH_MAC 18
#define MAX_LENGTH_SSID 33

#define SAFE_STR_CPY(src, obj, size)\
do {\
    if(obj)\
        src = strndup(obj, size);\
} while(0)

static int validate_ip_address(const char *ip_address);
static int validate_mac_address(const char *mac_address);

struct car_info
{
    char *id;
    char *name;
    char *ip;
    char *ap_mac;
    char *ap_ssid;
};

car_info_t *car_info_create()
{
    struct car_info *car_info = g_new0(struct car_info, 1);
    retvm_if(!car_info, NULL, "Could not allocate memory!");

    return car_info;
}

car_info_t *car_info_copy(const car_info_t *car_info)
{
    retv_if(!car_info, NULL);

    struct car_info *car_info_cpy = g_new0(struct car_info, 1);
    SAFE_STR_CPY(car_info_cpy->id, car_info->id, MAX_LENGTH);
    SAFE_STR_CPY(car_info_cpy->name, car_info->name, MAX_LENGTH);
    SAFE_STR_CPY(car_info_cpy->ip, car_info->ip, MAX_LENGTH_IP);
    SAFE_STR_CPY(car_info_cpy->ap_mac, car_info->ap_mac, MAX_LENGTH_MAC);
    SAFE_STR_CPY(car_info_cpy->ap_ssid, car_info->ap_ssid, MAX_LENGTH_SSID);

    return car_info_cpy;
}

void car_info_destroy(car_info_t *car_info)
{
    ret_if(!car_info);

    free(car_info->id);
    free(car_info->name);
    free(car_info->ip);
    free(car_info->ap_mac);
    free(car_info->ap_ssid);

    g_free(car_info);
}

bool car_info_is_valid(car_info_t *car_info)
{
    return (car_info->id && car_info->ip && car_info->ap_mac && car_info->ap_ssid);
}

const char *car_info_get_car_id(car_info_t *car_info)
{
    retv_if(!car_info, NULL);

    return car_info->id;
}

int car_info_set_car_id(car_info_t *car_info, const char *car_id)
{
    retv_if(!car_info, -1);
    retv_if(car_id == NULL, -1);
    retv_if(strlen(car_id) >= MAX_LENGTH, -1);

    if (!car_info->id)
        car_info->id = (char *)g_malloc(MAX_LENGTH * sizeof(char));

    snprintf(car_info->id, MAX_LENGTH, "%s", car_id);
    return 0;
}

const char *car_info_get_car_name(car_info_t *car_info)
{
    retv_if(!car_info, NULL);
    return car_info->name;
}

int car_info_set_car_name(car_info_t *car_info, const char *car_name)
{
    retv_if(!car_info, -1);
    retv_if(car_name == NULL, -1);
    retv_if(strlen(car_name) >= MAX_LENGTH, -1);

    if (!car_info->name)
        car_info->name = (char *)g_malloc(MAX_LENGTH * sizeof(char));

    snprintf(car_info->name, MAX_LENGTH, "%s", car_name);
    return 0;
}

const char *car_info_get_car_ip(car_info_t *car_info)
{
    retv_if(!car_info, NULL);
    return car_info->ip;
}

int car_info_set_car_ip(car_info_t *car_info, const char *car_ip)
{
    retv_if(!car_info, -1);
    retv_if(car_ip == NULL, -1);
    retv_if(strlen(car_ip) >= MAX_LENGTH_IP, -1);
    retv_if(validate_ip_address(car_ip) != 0, -1);

    if (!car_info->ip)
        car_info->ip = (char *)g_malloc(MAX_LENGTH_IP * sizeof(char));

    snprintf(car_info->ip, MAX_LENGTH_IP, "%s", car_ip);
    return 0;
}

const char *car_info_get_ap_mac(car_info_t *car_info)
{
    retv_if(!car_info, NULL);
    return car_info->ap_mac ? car_info->ap_mac : "NULL";
}

int car_info_set_car_ap_mac(car_info_t *car_info, const char *ap_mac)
{
    retv_if(!car_info, -1);
    retv_if(ap_mac == NULL, -1);
    retv_if(strlen(ap_mac) >= MAX_LENGTH_MAC, -1);
    retv_if(validate_mac_address(ap_mac) != 0, -1);

    if (!car_info->ap_mac)
        car_info->ap_mac = (char *)g_malloc(MAX_LENGTH_MAC * sizeof(char));

    snprintf(car_info->ap_mac, MAX_LENGTH_MAC, "%s", ap_mac);
    return 0;
}

const char *car_info_get_ap_ssid(car_info_t *car_info)
{
    retv_if(!car_info, NULL);
    return car_info->ap_ssid;
}

int car_info_set_ap_ssid(car_info_t *car_info, const char *ap_ssid)
{
    retv_if(!car_info, -1);
    retv_if(ap_ssid == NULL, -1);
    retv_if(strlen(ap_ssid) >= MAX_LENGTH_SSID, -1);

    if (!car_info->ap_ssid)
        car_info->ap_ssid = (char *)g_malloc(MAX_LENGTH_SSID * sizeof(char));

    snprintf(car_info->ap_ssid, MAX_LENGTH_SSID, "%s", ap_ssid);
    return 0;
}

static int validate_ip_address(const char *ip_address)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ip_address, &(sa.sin_addr));
    return result == 1 ? 0 : -1;
}

static int validate_mac_address(const char *mac_address)
{
    int i = 0;
    int segments = 1;

    while(*mac_address)
    {
        if (isxdigit(*mac_address))
        {
            i++;
            if (i > 2) return -1;
        }
        else if (*mac_address == ':')
        {
            if (i != 2) return -1;
            i = 0;
            segments++;
        }
        else {
            return -1;
        }
        mac_address++;
    }

    return (segments == 6 && i == 2) ? 0 : -1;
}
