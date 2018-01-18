/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

#ifndef __CAR_APP_RECEIVER_INTERNAL_H__
#define __CAR_APP_RECEIVER_INTERNAL_H__

#include "receiver_type.h"

/* TODO */
typedef int (*receiver_init_func) (void *data);
typedef int (*receiver_fini_func) (void *data);
typedef int (*receiver_start_func) (void *data);
typedef int (*receiver_stop_func) (void *data);
typedef receiver_state_e (*receiver_get_state_func) (void *data);

typedef struct __receiver_module_h receiver_module_h;

int receiver_set_module_data(receiver_module_h *handle, void *module_data);
void *receiver_get_module_data(receiver_module_h *handle);

int receiver_set_module_init_function(
	receiver_module_h *handle, receiver_init_func func);

int receiver_set_module_fini_function(
	receiver_module_h *handle, receiver_fini_func func);

int receiver_set_module_start_function(
	receiver_module_h *handle, receiver_start_func func);

int receiver_set_module_stop_function(
	receiver_module_h *handle, receiver_stop_func func);

int receiver_set_module_get_state_function(
	receiver_module_h *handle, receiver_get_state_func func);

void receiver_module_state_changed(
	receiver_module_h *handle, receiver_state_e state);

#endif /* __CAR_APP_RECEIVER_INTERNAL_H__ */
