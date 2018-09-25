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

#include "messages/reader.h"

#include <endian.h>
#include <stdint.h>
#include <string.h>

void reader_init_static(reader_t *reader, const char *data, size_t length)
{
	reader->data = data;
	reader->offset = 0;
	reader->len = length;
}

void reader_reset(reader_t *reader)
{
	reader->offset = 0;
}

int reader_read_int32(reader_t *reader, int32_t *value)
{
	if (reader->offset + sizeof(*value) > reader->len)
		return -1;

	*value = be32toh(*(uint32_t*)&reader->data[reader->offset]);

	reader->offset += sizeof(*value);
	return 0;
}

int reader_read_int64(reader_t *reader, int64_t *value)
{
	if (reader->offset + sizeof(*value) > reader->len)
		return -1;

	*value = be64toh(*(int64_t*)&reader->data[reader->offset]);

	reader->offset += sizeof(*value);
	return 0;
}

int reader_read_bool(reader_t *reader, bool *value)
{
	char val;

	if (reader_read_char(reader, &val)) {
		return -1;
	}

	*value = val ? true : false;
	return 0;
}

int reader_read_char(reader_t *reader, char *value)
{
	if (reader->offset + sizeof(*value) > reader->len)
		return -1;

	*value = *(char*)&reader->data[reader->offset];

	reader->offset += sizeof(*value);
	return 0;
}

int reader_read_string(reader_t *reader, char **value)
{
	int32_t len;
	char *ret;

	if (reader_read_int32(reader, &len)) {
		return -1;
	}

	if (len < 0)
		return -1;

	if (len == 0) {
		*value = strdup("");
		return 0;
	}

	if (reader->offset + len > reader->len)
		return -1;

	ret = (char*)malloc(sizeof(char) * (len + 1));
	if (!ret) {
		return -1;
	}

	memcpy(ret, &reader->data[reader->offset], len);
	reader->offset += len;

	ret[len] = '\0';
	*value = ret;
	return 0;
}
