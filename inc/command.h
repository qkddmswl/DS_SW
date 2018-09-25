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

#ifndef COMMAND_H_
#define COMMAND_H_

/**
 * @brief Types of commands used in communication.
 */
typedef enum command_type {
	COMMAND_TYPE_NONE, /** Command doesn't carry any information */
	COMMAND_TYPE_DRIVE, /** Command carries information about steering included in data.steering. */
	COMMAND_TYPE_CAMERA /** Command carries information about camera position in data.camera_position. */
} command_type_e;

/**
 * @brief Structure with info about speed and direction that should be set.
 */
typedef struct __command {
	command_type_e type; /** Type of command. */
	union {
		struct {
			int speed; /** Speed to be set from range [-10000, 10000]. */
			int direction; /** Direction to be set from range [-10000, 10000]. */
		} steering;
		struct {
			int camera_elevation; /** Elevation of camera to be set from range [-10000, 10000]. */
			int camera_azimuth; /** Azimuth of camera to be set from range [-10000, 10000]. */
		} camera_position;
	} data;
} command_s;

#endif //COMMAND_H_
