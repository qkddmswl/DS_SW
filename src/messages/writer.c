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

#include "messages/writer.h"

#include <endian.h>
#include <stdint.h>
#include <string.h>

int writer_init_sized(writer_t *writer, size_t length)
{
	writer->data = malloc(length);
	writer->length = 0;
	writer->cap = length;

	return writer->data ? 0 : -1;
}

static int _writer_resize_buffer(writer_t *writer, size_t new_size)
{
	writer->data = realloc(writer->data, new_size);
	writer->cap = new_size;

	return writer->data ? 0 : -1;
}

static int _writer_bytes_append(writer_t *writer, void *buf, size_t len)
{
	if (writer->length + len >= writer->cap)
		if (_writer_resize_buffer(writer, writer->cap + len))
			return -1;

	memcpy(&writer->data[writer->length], (char*)buf, len);
	writer->length += len;
	return 0;
}

void writer_shutdown(writer_t *writer)
{
	free(writer->data);
}

void writer_reset(writer_t *writer, size_t position)
{
	writer->length = position > writer->cap ? writer->cap : position;
}

int writer_write_int32(writer_t *writer, int32_t value)
{
	int32_t converted = htobe32(value);
	return _writer_bytes_append(writer, &converted, sizeof(converted));
}

int writer_write_int64(writer_t *writer, int64_t value)
{
	int64_t converted = htobe64(value);
	return _writer_bytes_append(writer, &converted, sizeof(converted));
}

int writer_write_bool(writer_t *writer, bool value)
{
	return writer_write_char(writer, value ? 1 : 0);
}

int writer_write_char(writer_t *writer, char value)
{
	return _writer_bytes_append(writer, &value, sizeof(value));
}

int writer_write_string(writer_t *writer, const char *value)
{
	size_t len = strlen(value);
	if (writer_write_int32(writer, len)) {
		return -1;
	}
	return _writer_bytes_append(writer, (void*)value, sizeof(char) * len);
}
