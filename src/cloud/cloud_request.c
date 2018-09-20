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

#include "cloud/cloud_request.h"
#include "cloud/car_info_serializer.h"
#include "cloud/http_request.h"
#include <glib.h>
#include <gio/gio.h>
#include <string.h>
#include "log.h"

#define BASE_URL "https://son.tizen.online"
#define PATH_API_RACING "/api/racing"

typedef struct {
    char *ap_mac;
    cloud_request_car_list_data_cb cb;
    void *user_data;
} car_api_get_request_context_t;

typedef struct {
    car_info_t *car;
    cloud_request_car_post_finish_cb cb;
    void *user_data;
} car_api_post_request_context_t;

typedef struct {
    long response_code;
    char *response_msg;
} car_api_post_request_response_t;

typedef struct {
    long response_code;
    car_info_t **cars;
    int size;
} car_api_get_request_response_t;

GQuark g_spawn_error_quark();
static void car_api_post_task_thread_cb(GTask *task, gpointer source_object, gpointer task_data, GCancellable *cancellable);
static void car_api_post_task_ready_cb(GObject *source_object, GAsyncResult *res, gpointer user_data);
static void car_api_post_task_context_free(car_api_post_request_context_t *context);
static void car_api_post_request_response_free(car_api_post_request_response_t *response);

static void car_api_get_task_context_free(car_api_get_request_context_t *context);
static void car_api_get_request_response_free(car_api_get_request_response_t *response);
static void car_api_get_task_thread_cb(GTask *task, gpointer source_object, gpointer task_data, GCancellable *cancellable);
static void car_api_get_task_ready_cb(GObject *source_object, GAsyncResult *res, gpointer user_data);

#define G_ERROR_DOMAIN g_spawn_error_quark()

GCancellable *cloud_request_api_racing_get(const char *ap_mac, cloud_request_car_list_data_cb cb, void *user_data)
{
    GCancellable *cancellable = g_cancellable_new();

    GTask *task = g_task_new(NULL, cancellable, car_api_get_task_ready_cb, NULL);
    g_task_set_source_tag(task, cloud_request_api_racing_get);
    g_task_set_return_on_cancel(task, FALSE);

    car_api_get_request_context_t *context = g_new0(car_api_get_request_context_t, 1);
    context->ap_mac = strndup(ap_mac, strlen(ap_mac));
    context->cb = cb;
    context->user_data = user_data;

    g_task_set_task_data(task, context, (GDestroyNotify)car_api_get_task_context_free);
    g_task_run_in_thread(task, car_api_get_task_thread_cb);

    g_object_unref(task);

    return cancellable;
}

GCancellable *cloud_request_api_racing_post(const car_info_t *car_info, cloud_request_car_post_finish_cb cb, void *user_data)
{
    GCancellable *cancellable = g_cancellable_new();

    GTask *task = g_task_new(NULL, cancellable, car_api_post_task_ready_cb, NULL);
    g_task_set_source_tag(task, cloud_request_api_racing_post);
    g_task_set_return_on_cancel(task, FALSE);

    car_api_post_request_context_t *context = g_new0(car_api_post_request_context_t, 1);
    context->car = car_info_copy(car_info);
    context->cb = cb;
    context->user_data = user_data;

    g_task_set_task_data(task, context, (GDestroyNotify)car_api_post_task_context_free);
    g_task_run_in_thread(task, car_api_post_task_thread_cb);

    g_object_unref(task);

    return cancellable;
}

GQuark g_spawn_error_quark()
{
  return g_quark_from_static_string("cloud_request");
}

static void car_api_post_task_context_free(car_api_post_request_context_t *context)
{
    ret_if(!context);

    car_info_destroy(context->car);
    g_free(context);
}

static void car_api_post_request_response_free(car_api_post_request_response_t *response)
{
    ret_if(!response);

    g_free(response->response_msg);
    g_free(response);
}

static void car_api_post_task_thread_cb(GTask *task, gpointer source_object, gpointer task_data, GCancellable *cancellable)
{
    car_api_post_request_context_t *context = (car_api_post_request_context_t *)task_data;

    if (g_task_return_error_if_cancelled(task)) {
        return;
    }

    car_api_post_request_response_t *response = g_new0(car_api_post_request_response_t, 1);

    char *json = car_info_serializer_serialize(context->car);
    int retval = http_request_post(BASE_URL""PATH_API_RACING, json, &(response->response_msg), &(response->response_code));
    g_free(json);

    if (retval != 0) {
        GError *err = g_error_new(G_ERROR_DOMAIN, retval, "http_request_post failed!");
        g_task_return_error(task, err);
    }

    g_task_return_pointer(task, response, (GDestroyNotify)car_api_post_request_response_free);
}

static void car_api_post_task_ready_cb(GObject *source_object, GAsyncResult *res, gpointer user_data)
{
    GTask *task = G_TASK(res);
    GError *error = NULL;

    //If no error occurred g_task_propagate_pointer transfers ownership, so later response have to be freed.
    car_api_post_request_response_t *response = g_task_propagate_pointer(task, &error);
    if (error != NULL) {
        _E("POST async task failed with msg: %s", error->message);
        g_error_free(error);
        return;
    }
    car_api_post_request_context_t *context = g_task_get_task_data(task);

    request_result_e result = (response->response_code == 200 && (strcmp(response->response_msg, "Success") == 0)) ?
        SUCCESS :
        FAILURE;

    if (context->cb) {
        context->cb(result, context->user_data);
    }

    car_api_post_request_response_free(response);
}

static void car_api_get_task_context_free(car_api_get_request_context_t *context)
{
    ret_if(!context);

    g_free(context->ap_mac);
    g_free(context);
}

static void car_api_get_request_response_free(car_api_get_request_response_t *response)
{
    ret_if(!response);
    ret_if(response->size <= 0);

    for (int i = 0; i < response->size; i++)
    {
        car_info_destroy(response->cars[i]);
    }
    g_free(response->cars);
    g_free(response);
}

static void car_api_get_task_thread_cb(GTask *task, gpointer source_object, gpointer task_data, GCancellable *cancellable)
{
    car_api_get_request_context_t *context = (car_api_get_request_context_t *)task_data;

    if (g_task_return_error_if_cancelled(task)) {
        return;
    }

    car_api_get_request_response_t *response = g_new0(car_api_get_request_response_t, 1);
    char *response_json = NULL;

    GString *url = g_string_new(BASE_URL""PATH_API_RACING"?apMac=");
    g_string_append(url, context->ap_mac);

    int retval = http_request_get(url->str, &response_json, &(response->response_code));
    g_string_free(url, TRUE);

    if (retval != 0) {
        GError *err = g_error_new(G_ERROR_DOMAIN, retval, "http_request_get failed!");
        g_task_return_error(task, err);
    }
    else {
        response->cars = car_info_serializer_deserialize_array(response_json, &(response->size));
    }

    g_free(response_json);
    g_task_return_pointer(task, response, (GDestroyNotify)car_api_get_request_response_free);
}

static void car_api_get_task_ready_cb(GObject *source_object, GAsyncResult *res, gpointer user_data)
{
    GTask *task = G_TASK(res);
    GError *error = NULL;

    //If no error occurred g_task_propagate_pointer transfers ownership, so later response have to be freed.
    car_api_get_request_response_t *response = g_task_propagate_pointer(task, &error);
    if (error != NULL) {
        _E("GET async task failed with msg: %s", error->message);
        g_error_free(error);
        return;
    }
    car_api_get_request_context_t *context = g_task_get_task_data(task);

    request_result_e result = (response->response_code == 200) ? SUCCESS : FAILURE;

    if (context->cb) {
        context->cb(result, response->cars, response->size, context->user_data);
    }

    car_api_get_request_response_free(response);
}