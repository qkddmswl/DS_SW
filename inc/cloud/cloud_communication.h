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

/**
 * @brief Initializes cloud communication.
 * @return 0 on success, other value on failure.
*/
int cloud_communication_init();

/**
 * @brief Starts sending POST request with car data to the cloud.
 * @param[in] interval Interval in seconds between POST requests.
*/
void cloud_communication_start(int interval);

/**
 * @brief Stops sending POST request with car data to the cloud.
*/
void cloud_communication_stop();

/**
 * @brief Finalize all resources used by cloud communication.
*/
void cloud_communication_fini();