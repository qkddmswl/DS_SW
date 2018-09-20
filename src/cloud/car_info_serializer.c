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

#include "cloud/car_info_serializer.h"
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <json-glib/json-glib.h>
#include "log.h"
#include "cloud/car_info.h"
#include "string.h"

#define JSON_SCHEMA_CAR_ID "carId"
#define JSON_SCHEMA_RESPONSE_CAR_ID "id"
#define JSON_SCHEMA_CAR_NAME "carName"
#define JSON_SCHEMA_CAR_IP "carIp"
#define JSON_SCHEMA_AP_MAC "apMac"
#define JSON_SCHEMA_AP_SSID "apSsid"

static JsonNode *parse_string(JsonParser *parser, const char *config_json);
static void car_info_array_iterate_cb(JsonArray *array, guint index, JsonNode *element, gpointer user_data);

char *car_info_serializer_serialize(car_info_t *car_info)
{
    JsonGenerator *generator = json_generator_new();
    JsonBuilder *builder = json_builder_new();
    json_builder_begin_object(builder);

    json_builder_set_member_name(builder, JSON_SCHEMA_CAR_ID);
    json_builder_add_string_value(builder, car_info_get_car_id(car_info));

    json_builder_set_member_name(builder, JSON_SCHEMA_CAR_NAME);
    json_builder_add_string_value(builder, car_info_get_car_name(car_info));

    json_builder_set_member_name(builder, JSON_SCHEMA_CAR_IP);
    json_builder_add_string_value(builder, car_info_get_car_ip(car_info));

    json_builder_set_member_name(builder, JSON_SCHEMA_AP_MAC);
    json_builder_add_string_value(builder, car_info_get_ap_mac(car_info));

    json_builder_set_member_name(builder, JSON_SCHEMA_AP_SSID);
    json_builder_add_string_value(builder, car_info_get_ap_ssid(car_info));

    json_builder_end_object(builder);

    JsonNode *root = json_builder_get_root(builder);
    json_generator_set_root(generator, root);

    char *json_data = json_generator_to_data(generator, NULL);

    g_object_unref(builder);
    g_object_unref(generator);
    return json_data;
}

car_info_t **car_info_serializer_deserialize_array(const char *json_data, int *size)
{
    JsonParser *parser = json_parser_new();

    JsonNode *root = parse_string(parser, json_data);
    JsonArray *array = json_node_get_array(root);
    if (!array)
    {
        _E("Json is invalid!");
        g_object_unref(parser);
        return NULL;
    }

    *size = json_array_get_length(array);

    if (*size == 0) {
        return NULL;
    }

    car_info_t **car_info_array = g_malloc(*size * sizeof(car_info_t *));

    json_array_foreach_element(array, car_info_array_iterate_cb, car_info_array);

    g_object_unref(parser);
    return car_info_array;
}

static JsonNode *parse_string(JsonParser *parser, const char *json)
{
    GError *err = NULL;

    if (!json_parser_load_from_data(parser, json, -1, &err))
    {
        _E("Function \"json_parser_load_from_data()\" failed with message: %s", err->message);
        g_error_free(err);
        return NULL;
    }

    return json_parser_get_root(parser);
}

static void car_info_array_iterate_cb(JsonArray *array, guint index, JsonNode *element, gpointer user_data)
{
    car_info_t **car_info_array = (car_info_t **)user_data;

    JsonObject *entry = json_node_get_object(element);

    car_info_array[index] = car_info_create();

    if (json_object_has_member(entry, JSON_SCHEMA_RESPONSE_CAR_ID) &&
        car_info_set_car_id(car_info_array[index], json_object_get_string_member(entry, JSON_SCHEMA_RESPONSE_CAR_ID)) != 0)
    {
        _E("Couldn't set car id!");
    }

    if (json_object_has_member(entry, JSON_SCHEMA_CAR_NAME) &&
        car_info_set_car_name(car_info_array[index], json_object_get_string_member(entry, JSON_SCHEMA_CAR_NAME)) != 0)
    {
        _E("Couldn't set car name!");
    }

    if (json_object_has_member(entry, JSON_SCHEMA_CAR_IP) &&
        car_info_set_car_ip(car_info_array[index], json_object_get_string_member(entry, JSON_SCHEMA_CAR_IP)) != 0)
    {
        _E("Couldn't set car ip!");
    }

    if (json_object_has_member(entry, JSON_SCHEMA_AP_MAC) &&
        car_info_set_car_ap_mac(car_info_array[index], json_object_get_string_member(entry, JSON_SCHEMA_AP_MAC)) != 0)
    {
        _E("Couldn't set access point MAC address!");
    }

    if (json_object_has_member(entry, JSON_SCHEMA_AP_SSID) &&
        car_info_set_ap_ssid(car_info_array[index], json_object_get_string_member(entry, JSON_SCHEMA_AP_SSID)) != 0)
    {
        _E("Couldn't set access point SSID!");
    }
}
