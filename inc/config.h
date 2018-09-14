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

#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

/**
 * @brief Initializes config module.
 *
 * @return 0 on success, other value on error.
 *
 * @note config module reads/writes configuration stored in
 * application's "data" directory in "config.ini" file.
 */
int config_init();

/**
 * @brief Flush config to disk.
 *
 * @return 0 on success, other value on error.
 */
int config_save();

/**
 * @brief Shutdowns config module.
 */
void config_shutdown();

/**
 * @brief Get string value
 *
 * @param[in] group configuration group to which @key belongs
 * @param[in] key configuration key name.
 * @param[out] out value stored in configuration for given group and key.
 *
 * @return 0 on success, other if value for given group with key do not exist.
 *
 * @note @config_init should be called before using this function
 * @note @out should be released with @free
 */
int config_get_string(const char *group, const char *key, char **out);

/**
 * @brief Sets string value
 *
 * @param[in] group configuration group to which @key belongs
 * @param[in] key configuration key name.
 * @param[in] value to be stored in configuration for given group and key.
 *
 * @note @config_init should be called before using this function
 */
void config_set_string(const char *group, const char *key, const char *value);

/**
 * @brief Get integer value
 *
 * @param[in] group configuration group to which @key belongs
 * @param[in] key configuration key name.
 * @param[out] out value stored in configuration for given group and key.
 *
 * @return 0 on success, other if value for given group with key do not exist.
 *
 * @note @config_init should be called before using this function
 */
int config_get_int(const char *group, const char *key, int *out);

/**
 * @brief Set integer value
 *
 * @param[in] group configuration group to which @key belongs
 * @param[in] key configuration key name.
 * @param[in] value to be stored in configuration for given group and key.
 *
 * @note @config_init should be called before using this function
 */
void config_set_int(const char *group, const char *key, int value);

/**
 * @brief Get double value
 *
 * @param[in] group configuration group to which @key belongs
 * @param[in] key configuration key name.
 * @param[out] out value stored in configuration for given group and key.
 *
 * @return 0 on success, other if value for given group with key do not exist.
 *
 * @note @config_init should be called before using this function
 */
int config_get_double(const char *group, const char *key, double *out);

/**
 * @brief Set double value
 *
 * @param[in] group configuration group to which @key belongs
 * @param[in] key configuration key name.
 * @param[in] value to be stored in configuration for given group and key.
 *
 * @note @config_init should be called before using this function
 */
void config_set_double(const char *group, const char *key, double value);

/**
 * @brief Get boolean value
 *
 * @param[in] group configuration group to which @key belongs
 * @param[in] key configuration key name.
 * @param[out] out value stored in configuration for given group and key.
 *
 * @return 0 on success, other if value for given group with key do not exist.
 *
 * @note @config_init should be called before using this function
 */
int config_get_bool(const char *group, const char *key, bool *out);

/**
 * @brief Set boolean value
 *
 * @param[in] group configuration group to which @key belongs
 * @param[in] key configuration key name.
 * @param[in] value to be stored in configuration for given group and key.
 *
 * @note @config_init should be called before using this function
 */
void config_set_bool(const char *group, const char *key, bool value);

/**
 * @brief Remove key from config.
 *
 * @param[in] group configuration group to which @key belongs
 * @param[in] key configuration key name.
 *
 * @note @config_init should be called before using this function
 */
int config_remove_key(const char *group, const char *key);

/**
 * @brief Remove group from config. Removes all keys assigned to this group.
 *
 * @param[in] group configuration group name.
 *
 * @note @config_init should be called before using this function
 */
int config_remove_group(const char *group);

#endif /* end of include guard: CONFIG_H */
