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

#ifndef __HTTP_REQUEST_H_
#define __HTTP_REQUEST_H_

/**
 * @brief Generic HTTP GET.
 * @param[in] url Url of request.
 * @param[out] response Response from the request.
 * @param[out] response_code HTTP response code.
 * @return Returns 0 on success, -1 otherwise.
 * @remarks Response should be released with @free.
 */
int http_request_get(const char *url, char **response, long *response_code);

/**
 * @brief Generic HTTP POST.
 * @param[in] url URL of request.
 * @param[in] json Content of the response in json format.
 * @param[out] response Response from the request.
 * @param[out] response_code HTTP response code.
 * @return Returns 0 on success, -1 otherwise.
 * @remarks Response should be released with @free.
 */
int http_request_post(const char *url, const char *json, char **response, long *response_code);

#endif