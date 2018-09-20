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

#include "cloud/http_request.h"
#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include "log.h"

static size_t _response_write(void *ptr, size_t size, size_t nmemb, void *data);

int http_request_get(const char *url, char **response, long *response_code)
{
    retvm_if(!url, -1, "GET request URL is NULL!");
    retvm_if(!response, -1, "GET request response is null");

    CURL *curl = NULL;
    CURLcode res = CURLE_OK;

    curl = curl_easy_init();
    retvm_if(!curl, -1, "Failed to initialize curl!");

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _response_write);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)response);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        _E("curl_easy_perform() failed: %s", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        return -1;
    }

    long _response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &_response_code);
    if (_response_code) {
        *response_code = _response_code;
    }

    curl_easy_cleanup(curl);

    return 0;
}

int http_request_post(const char *url, const char *json, char **response, long *response_code)
{
    retvm_if(!url, -1, "POST request URL is NULL!");
    retvm_if(!json, -1, "POST request JSON message is NULL!");

    CURL *curl = curl_easy_init();
    retvm_if(!curl, -1, "Failed to initialize curl!");

    char *_response = NULL;
    struct curl_slist *headers = curl_slist_append(NULL, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _response_write);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&_response);

    CURLcode res = CURLE_OK;
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        _E("curl_easy_perform() failed: %s", curl_easy_strerror(res));
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return -1;
    }

    long _response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &_response_code);
    if (response_code) {
        *response_code = _response_code;
    }

    if (response) {
        *response = _response;
    }
    else {
        g_free(_response);
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return 0;
}

static size_t _response_write(void *ptr, size_t size, size_t nmemb, void *data)
{
    char **received = (char **)data;
    size_t real_size = size * nmemb;
    const char *response_msg = (const char *)ptr;

    if (received && real_size > 0) {
        if (*received) {
            char *temp = strndup(response_msg, real_size);
            size_t length = strlen(*received) + strlen(temp);
            char *new = g_malloc(length * sizeof(char));
            snprintf(new, length, "%s%s", *received, temp);
            g_free(temp);
            g_free(*received);
            *received = new;
        }
        else {
            *received = g_strndup((const char *)ptr, real_size);
        }
    }
    else {
        _E("Failed to get response, response size : %lu", real_size);
    }

    return real_size;
}