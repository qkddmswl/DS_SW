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

#ifndef READER_H
#define READER_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct _reader reader_t;

/**
 * @brief reader object
 */
struct _reader
{
	const char *data;
	size_t offset;
	size_t len;
};

/**
 * @brief Initialzies reader over buffer
 *
 * @param[in] reader reader object
 * @param[in] data buffer
 * @param[in] length length of the @data
 */
void reader_init_static(reader_t *reader, const char *data, size_t length);

/**
 * @brief Start reading buffer from the begining
 *
 * @param[in] reader reader object
 */
void reader_reset(reader_t *reader);

/** * @brief Reads 32-bit integer value from buffer
 *
 * @param[in] reader reader object
 * @param[out] value output value
 *
 * @return 0 on success, other value on failure.
 */
int reader_read_int32(reader_t *reader, int32_t *value);

/**
 * @brief Reads 64-bit integer value from buffer
 *
 * @param[in] reader reader object
 * @param[out] value output value
 *
 * @return 0 on success, other value on failure.
 */
int reader_read_int64(reader_t *reader, int64_t *value);

/**
 * @brief Reads character value from buffer
 *
 * @param[in] reader reader object
 * @param[out] value output value
 *
 * @return 0 on success, other value on failure.
 */
int reader_read_char(reader_t *reader, char *value);

/**
 * @brief Reads boolean value from buffer
 *
 * @param[in] reader reader object
 * @param[out] value output value
 *
 * @return 0 on success, other value on failure.
 */
int reader_read_bool(reader_t *reader, bool *value);

/**
 * @brief Reads string value from buffer
 *
 * @param[in] reader reader object
 * @param[out] value output value
 *
 * @return 0 on success, other value on failure.
 * @note value should be released with @free
 */
int reader_read_string(reader_t *reader, char **value);

#endif /* end of include guard: READER_H */
