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

#include <stdlib.h>
#include <glib.h>
#include "log.h"
#include "receiver_type.h"
#include "receiver_internal.h"
#include "receiver_udp.h"

typedef struct __receiver_h {
	receiver_type_e receiver_type;
	/* TODO */
} receiver_h;

struct __receiver_module_h {
	receiver_type_e receiver_type;
	void *module_data;
	receiver_init_func init;
	receiver_fini_func fini;
	receiver_start_func start;
	receiver_stop_func stop;
	receiver_get_state_func get_state;
	receiver_state_changed_cb state_change_cb;
	void *state_change_data;
};

static GHashTable *receiver_module_hash = NULL;

static void ___module_hash_destroy(gpointer data)
{
	receiver_module_h *module_h = data;

	module_h->fini(module_h);
	free(module_h);

	return;
}

int receiver_init(receiver_type_e type)
{
	int ret = 0;
	receiver_module_h *handle = NULL;

	if (!receiver_module_hash) {
		receiver_module_hash = g_hash_table_new_full(g_direct_hash,
			g_direct_equal, NULL, ___module_hash_destroy);

		if (!receiver_module_hash) {
			_E("failed to create hash table");
			return -1;
		}
	} else {
		handle = g_hash_table_lookup(receiver_module_hash,
			GUINT_TO_POINTER(type));

		if (handle) {
			_D("receiver [%d] type is already initialized", type);
			return 0;
		}
	}

	handle = calloc(1, sizeof(struct __receiver_module_h));
	if (!handle) {
		_D("failed to alloc handle memory");
		return -1;
	}

	handle->receiver_type = type;

	switch (type) {
	case RECEIVER_TYPE_UDP:
		/* TODO */
		ret = receiver_udp_module_register(handle);
		if (ret)
			goto ERROR;
		break;
	case RECEIVER_TYPE_BLUETOOTH:
		/* TODO : for bluetooth module */
		// ret = receiver_bluetooth_module_register(handle);
		// if (ret)
		//	goto ERROR;
		break;
	}

	if (handle->init) {
		ret = handle->init(handle);
		if (ret) {
			_E("failed to initialized type[%d]", type);
			goto ERROR;
		}
	} else {
		_W("receiver [%d] type is not implemented init func", type);
		goto ERROR;
	}

	g_hash_table_insert(receiver_module_hash, GUINT_TO_POINTER(type), handle);

	return 0;

ERROR:
	free(handle);
	return -1;
}

void receiver_fini(receiver_type_e type)
{
	receiver_module_h *handle = NULL;
	guint hash_size = 0;

	if (!receiver_module_hash)
		return;

	handle = g_hash_table_lookup(receiver_module_hash,
		GUINT_TO_POINTER(type));

	if (!handle) {
		_D("receiver [%d] type is not initialized", type);
		return;
	}

	if (!handle->fini)
		handle->fini(handle);

	g_hash_table_remove(receiver_module_hash, GUINT_TO_POINTER(type));

	hash_size = g_hash_table_size(receiver_module_hash);
	if (hash_size == 0) {
		g_hash_table_unref(receiver_module_hash);
		receiver_module_hash = NULL;
	}

	return;
}

int receiver_start(receiver_type_e type)
{
	receiver_module_h *handle = NULL;

	if (!receiver_module_hash) {
		_E("receiver is not initialized");
		return -1;
	}

	handle = g_hash_table_lookup(receiver_module_hash,
		GUINT_TO_POINTER(type));

	if (!handle) {
		_E("receiver [%d] type is not initialized", type);
		return -1;
	}

	if (!handle->start) {
		_D("receiver [%d] type is not implemented start func", type);
		return -1;
	}

	return handle->start(handle);
}

int receiver_stop(receiver_type_e type)
{
	receiver_module_h *handle = NULL;

	if (!receiver_module_hash) {
		_E("receiver is not initialized");
		return -1;
	}

	handle = g_hash_table_lookup(receiver_module_hash,
		GUINT_TO_POINTER(type));

	if (!handle) {
		_E("receiver [%d] type is not initialized", type);
		return -1;
	}

	if (!handle->stop) {
		_D("receiver [%d] type is not implemented stop func", type);
		return -1;
	}

	return handle->stop(handle);
}

receiver_state_e receiver_get_state(receiver_type_e type)
{
	receiver_module_h *handle = NULL;

	if (!receiver_module_hash) {
		_E("receiver is not initialized");
		return RECEIVER_STATE_NONE;
	}

	handle = g_hash_table_lookup(receiver_module_hash,
		GUINT_TO_POINTER(type));

	if (!handle) {
		_E("receiver [%d] type is not initialized", type);
		return RECEIVER_STATE_NONE;
	}

	if (!handle->get_state) {
		_D("receiver [%d] type is not implemented get_state func", type);
		return RECEIVER_STATE_NONE;
	}

	return handle->get_state(handle);
}

int receiver_set_state_changed_cb(receiver_type_e type,
	receiver_state_changed_cb callback, void *user_data)
{
	receiver_module_h *handle = NULL;

	if (!receiver_module_hash) {
		_E("receiver is not initialized");
		return -1;
	}

	handle = g_hash_table_lookup(receiver_module_hash,
		GUINT_TO_POINTER(type));

	if (!handle) {
		_E("receiver [%d] type is not initialized", type);
		return -1;
	}

	handle->state_change_cb = callback;

	if (callback)
		handle->state_change_data = user_data;
	else
		handle->state_change_data = NULL;

	return 0;
}

int receiver_set_module_data(receiver_module_h *handle, void *module_data)
{
	retv_if(!handle, -1);

	handle->module_data = module_data;

	return 0;
}

void *receiver_get_module_data(receiver_module_h *handle)
{
	retv_if(!handle, NULL);

	return handle->module_data;
}

int receiver_set_module_init_function(
	receiver_module_h *handle, receiver_init_func func)
{
	retv_if(!handle, -1);

	handle->init = func;

	return 0;
}

int receiver_set_module_fini_function(
	receiver_module_h *handle, receiver_fini_func func)
{
	retv_if(!handle, -1);

	handle->fini = func;

	return 0;
}

int receiver_set_module_start_function(
	receiver_module_h *handle, receiver_start_func func)
{
	retv_if(!handle, -1);

	handle->start = func;

	return 0;
}

int receiver_set_module_stop_function(
	receiver_module_h *handle, receiver_stop_func func)
{
	retv_if(!handle, -1);

	handle->stop = func;

	return 0;
}

int receiver_set_module_get_state_function(
	receiver_module_h *handle, receiver_get_state_func func)
{
	retv_if(!handle, -1);

	handle->get_state = func;

	return 0;
}

void receiver_module_state_changed(
	receiver_module_h *handle, receiver_state_e state)
{
	ret_if(!handle);

	if (handle->state_change_cb)
		handle->state_change_cb(handle->receiver_type,
			state, handle->state_change_data);

	return;
}
