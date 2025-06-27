/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_log.h"

#define ESP_UTILS_IMPL_LOGD(TAG, format, ...) ESP_LOGD(TAG, "[%s:%04d](%s): " format, \
                                        esp_utils_log_extract_file_name(__FILE__), __LINE__, __func__,  ##__VA_ARGS__)
#define ESP_UTILS_IMPL_LOGI(TAG, format, ...) ESP_LOGI(TAG, "[%s:%04d](%s): " format, \
                                        esp_utils_log_extract_file_name(__FILE__), __LINE__, __func__,  ##__VA_ARGS__)
#define ESP_UTILS_IMPL_LOGW(TAG, format, ...) ESP_LOGW(TAG, "[%s:%04d](%s): " format, \
                                        esp_utils_log_extract_file_name(__FILE__), __LINE__, __func__,  ##__VA_ARGS__)
#define ESP_UTILS_IMPL_LOGE(TAG, format, ...) ESP_LOGE(TAG, "[%s:%04d](%s): " format, \
                                        esp_utils_log_extract_file_name(__FILE__), __LINE__, __func__,  ##__VA_ARGS__)
