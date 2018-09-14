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

#define _GNU_SOURCE
#include "config.h"
#include "log.h"

#include <stdio.h>
#include <glib.h>
#include <stdlib.h>
#include <app_common.h>

#define CONFIG_FILENAME "config.ini"

static GKeyFile *gk = NULL;

static char *_config_file_path_get()
{
	char *ret = NULL;
	char *data = app_get_data_path();

	if (!data)
		return NULL;

	if (asprintf(&ret, "%s%s", data, CONFIG_FILENAME) == -1) {
		ret = NULL;
	}

	free(data);
	return ret;
}

int config_init()
{
	GError *error = NULL;

	if (gk) {
		_D("config_init already called.");
		return 0;
	}

	gk = g_key_file_new();
	if (!gk) {
		_E("g_key_file_new failed.");
		return -1;
	}

	char *path = _config_file_path_get();

	if (!g_key_file_load_from_file(gk, path, G_KEY_FILE_NONE, &error)) {
		if (error->code != G_FILE_ERROR_NOENT) {
			_E("g_key_file_load_from_file failed. %s", error->message);
			g_key_file_free(gk);
			g_error_free(error);
			free(path);
			return -1;
		}
		g_error_free(error);
	}

	free(path);
	return 0;
}

int config_save()
{
	GError *error = NULL;

	retv_if(!gk, -1);

	char *path = _config_file_path_get();

	if (!g_key_file_save_to_file(gk, path, &error)) {
		_E("g_key_file_save_to_file failed: %s", error->message);
		g_error_free(error);
		free(path);
		return -1;
	}
	free(path);
	return 0;
}

void config_shutdown()
{
	if (gk) g_key_file_free(gk);
	gk = NULL;
}

int config_get_string(const char *group, const char *key, char **out)
{
	GError *error = NULL;
	char *value;

	retv_if(!gk, -1);
	retv_if(!group, -1);
	retv_if(!key, -1);
	retv_if(!out, -1);

	value = g_key_file_get_string(gk, group, key, &error);
	if (error) {
		_E("g_key_file_get_string failed: %s", error->message);
		g_error_free(error);
		return -1;
	}
	*out = value;
	return 0;
}

void config_set_string(const char *group, const char *key, const char *value)
{
	ret_if(!gk);
	ret_if(!group);
	ret_if(!key);

	g_key_file_set_string(gk, group, key, value);
}

int config_get_int(const char *group, const char *key, int *out)
{
	GError *error = NULL;
	int value;

	retv_if(!gk, -1);
	retv_if(!group, -1);
	retv_if(!key, -1);
	retv_if(!out, -1);

	value = g_key_file_get_integer(gk, group, key, &error);
	if (error) {
		_E("g_key_file_get_integer failed: %s", error->message);
		g_error_free(error);
		return -1;
	}
	*out = value;
	return 0;
}

void config_set_int(const char *group, const char *key, int value)
{
	ret_if(!gk);
	ret_if(!group);
	ret_if(!key);

	g_key_file_set_integer(gk, group, key, value);
}

int config_get_double(const char *group, const char *key, double *out)
{
	GError *error = NULL;
	double value;

	retv_if(!gk, -1);
	retv_if(!group, -1);
	retv_if(!key, -1);
	retv_if(!out, -1);

	value = g_key_file_get_double(gk, group, key, &error);
	if (error) {
		_E("g_key_file_get_double failed: %s", error->message);
		g_error_free(error);
		return -1;
	}
	*out = value;
	return 0;
}

void config_set_double(const char *group, const char *key, double value)
{
	ret_if(!gk);
	ret_if(!group);
	ret_if(!key);

	g_key_file_set_double(gk, group, key, value);
}

int config_get_bool(const char *group, const char *key, bool *out)
{
	GError *error = NULL;
	bool value;

	retv_if(!gk, -1);
	retv_if(!group, -1);
	retv_if(!key, -1);
	retv_if(!out, -1);

	value = g_key_file_get_boolean(gk, group, key, &error);
	if (error) {
		_E("g_key_file_get_boolean failed: %s", error->message);
		g_error_free(error);
		return -1;
	}
	*out = value;
	return 0;
}

void config_set_bool(const char *group, const char *key, bool value)
{
	ret_if(!gk);
	ret_if(!group);
	ret_if(!key);

	g_key_file_set_boolean(gk, group, key, value);
}

int config_remove_key(const char *group, const char *key)
{
	retv_if(!gk, -1);
	retv_if(!group, -1);
	retv_if(!key, -1);

	return g_key_file_remove_key(gk, group, key, NULL) ? 0 : 1;
}

int config_remove_group(const char *group)
{
	retv_if(!gk, -1);
	retv_if(!group, -1);

	return g_key_file_remove_group(gk, group, NULL) ? 0 : 1;
}
