/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdio.h>
#include <string.h>
#include "esp_utils_conf_internal.h"
#if ESP_UTILS_CONF_LOG_IMPL_TYPE == ESP_UTILS_LOG_IMPL_STDLIB
#   include "impl/esp_utils_log_impl_std.h"
#elif ESP_UTILS_CONF_LOG_IMPL_TYPE == ESP_UTILS_LOG_IMPL_ESP
#   include "impl/esp_utils_log_impl_esp.h"
#else
#   error "Invalid log implementation"
#endif

#define ESP_UTILS_LOGD_IMPL(tag, format, ...) ESP_UTILS_LOGD_IMPL_FUNC(tag, "[%s:%04d](%s): " format, \
                                        esp_utils_log_extract_file_name(__FILE__), __LINE__, __func__,  ##__VA_ARGS__)
#define ESP_UTILS_LOGI_IMPL(tag, format, ...) ESP_UTILS_LOGI_IMPL_FUNC(tag, "[%s:%04d](%s): " format, \
                                        esp_utils_log_extract_file_name(__FILE__), __LINE__, __func__,  ##__VA_ARGS__)
#define ESP_UTILS_LOGW_IMPL(tag, format, ...) ESP_UTILS_LOGW_IMPL_FUNC(tag, "[%s:%04d](%s): " format, \
                                        esp_utils_log_extract_file_name(__FILE__), __LINE__, __func__,  ##__VA_ARGS__)
#define ESP_UTILS_LOGE_IMPL(tag, format, ...) ESP_UTILS_LOGE_IMPL_FUNC(tag, "[%s:%04d](%s): " format, \
                                        esp_utils_log_extract_file_name(__FILE__), __LINE__, __func__,  ##__VA_ARGS__)

#define ESP_UTILS_LOG_LEVEL_IMPL(tag, level, format, ...) do {                                                   \
        if (level >= ESP_UTILS_CONF_LOG_LEVEL) {                                                                 \
            if      (level == ESP_UTILS_LOG_LEVEL_DEBUG)   { ESP_UTILS_LOGD_IMPL(tag, format, ##__VA_ARGS__); }  \
            else if (level == ESP_UTILS_LOG_LEVEL_INFO)    { ESP_UTILS_LOGI_IMPL(tag, format, ##__VA_ARGS__); }  \
            else if (level == ESP_UTILS_LOG_LEVEL_WARNING) { ESP_UTILS_LOGW_IMPL(tag, format, ##__VA_ARGS__); }  \
            else if (level == ESP_UTILS_LOG_LEVEL_ERROR)   { ESP_UTILS_LOGE_IMPL(tag, format, ##__VA_ARGS__); }  \
            else { }                                                                                             \
        }                                                                                                        \
    } while(0)

/**
 * Macros to simplify logging calls with target tag
 */
#define ESP_UTILS_TAG_LOGD(tag, format, ...) ESP_UTILS_LOG_LEVEL_IMPL(tag, ESP_UTILS_LOG_LEVEL_DEBUG,   format, ##__VA_ARGS__)
#define ESP_UTILS_TAG_LOGI(tag, format, ...) ESP_UTILS_LOG_LEVEL_IMPL(tag, ESP_UTILS_LOG_LEVEL_INFO,    format, ##__VA_ARGS__)
#define ESP_UTILS_TAG_LOGW(tag, format, ...) ESP_UTILS_LOG_LEVEL_IMPL(tag, ESP_UTILS_LOG_LEVEL_WARNING, format, ##__VA_ARGS__)
#define ESP_UTILS_TAG_LOGE(tag, format, ...) ESP_UTILS_LOG_LEVEL_IMPL(tag, ESP_UTILS_LOG_LEVEL_ERROR,   format, ##__VA_ARGS__)

#if defined(ESP_UTILS_LOG_TAG)
#   define ESP_UTILS_LOG_LEVEL(level, format, ...) \
            ESP_UTILS_LOG_LEVEL_IMPL(ESP_UTILS_LOG_TAG, level, format, ##__VA_ARGS__);
#else
#   define ESP_UTILS_LOG_TAG_DEFAULT "Utils"
#   define ESP_UTILS_LOG_LEVEL(level, format, ...) \
            ESP_UTILS_LOG_LEVEL_IMPL(ESP_UTILS_LOG_TAG_DEFAULT, level, format, ##__VA_ARGS__);
#endif

/**
 * Macros to simplify logging calls with a fixed tag (`ESP_UTILS_LOG_TAG` or `ESP_UTILS_LOG_TAG_DEFAULT`)
 */
#define ESP_UTILS_LOGD(format, ...) ESP_UTILS_LOG_LEVEL(ESP_UTILS_LOG_LEVEL_DEBUG,   format, ##__VA_ARGS__)
#define ESP_UTILS_LOGI(format, ...) ESP_UTILS_LOG_LEVEL(ESP_UTILS_LOG_LEVEL_INFO,    format, ##__VA_ARGS__)
#define ESP_UTILS_LOGW(format, ...) ESP_UTILS_LOG_LEVEL(ESP_UTILS_LOG_LEVEL_WARNING, format, ##__VA_ARGS__)
#define ESP_UTILS_LOGE(format, ...) ESP_UTILS_LOG_LEVEL(ESP_UTILS_LOG_LEVEL_ERROR,   format, ##__VA_ARGS__)

/**
 * Micros to log trace of function calls
 */
#if ESP_UTILS_CONF_ENABLE_LOG_TRACE
#   define ESP_UTILS_LOG_TRACE_ENTER() ESP_UTILS_LOGD("Enter")
#   define ESP_UTILS_LOG_TRACE_EXIT()  ESP_UTILS_LOGD("Exit")
#else
#   define ESP_UTILS_LOG_TRACE_ENTER()
#   define ESP_UTILS_LOG_TRACE_EXIT()
#endif

#ifdef __cplusplus
extern "C" {
#endif

const char *esp_utils_log_extract_file_name(const char *file_path);

#ifdef __cplusplus
}
#endif
