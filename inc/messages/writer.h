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

#ifndef WRITER_H
#define WRITER_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct _writer writer_t;

/**
 * @brief writer object
 */
struct _writer
{
	char *data;
	size_t length;
	size_t cap;
};

/**
 * @brief Initializes writer object
 *
 * @param[in] writer writer object.
 * @param[in] length the initial lenght of data buffer.
 *
 * @return 0 on success, other value on error
 */
int writer_init_sized(writer_t *writer, size_t length);

/**
 * @brief Shutdowns writer object
 *
 * @param[in] writer writer object.
 */
void writer_shutdown(writer_t *writer);

/**
 * @brief Resets writer object
 *
 * Resetting means that next call to write_write_* functions
 * will write content from the begining of the buffer, overwriting
 * any previously stored data.
 *
 * @param[in] writer writer object.
 * @param[in] the position in the buffer after which next
 *            write data will be placed.
 */
void writer_reset(writer_t *buf, size_t position);

/**
 * @brief Writes value into writer's buffer
 *
 * @param[in] writer writer object
 * @param[in] value value to write into the writer's buffer
 *
 * @return 0 on success, other value on error.
 */
int writer_write_int32(writer_t *writer, int32_t value);

/**
 * @brief Writes value into writer's buffer
 *
 * @param[in] writer writer object
 * @param[in] value value to write into the writer's buffer
 *
 * @return 0 on success, other value on error.
 */
int writer_write_int64(writer_t *writer, int64_t value);

/**
 * @brief Writes value into writer's buffer
 *
 * @param[in] writer writer object
 * @param[in] value value to write into the writer's buffer
 *
 * @return 0 on success, other value on error.
 */
int writer_write_bool(writer_t *writer, bool value);

/**
 * @brief Writes value into writer's buffer
 *
 * @param[in] writer writer object
 * @param[in] value value to write into the writer's buffer
 *
 * @return 0 on success, other value on error.
 */
int writer_write_char(writer_t *writer, char value);

/**
 * @brief Writes value into writer's buffer
 *
 * @param[in] writer writer object
 * @param[in] value value to write into the writer's buffer
 *
 * @return 0 on success, other value on error.
 */
int writer_write_string(writer_t *writer, const char *value);

#endif /* end of include guard: WRITER_H */
