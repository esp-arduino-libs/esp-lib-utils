/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdio.h>

#define ESP_UTILS_IMPL_LOGD(TAG, format, ...) printf("[D][" TAG "][%s:%04d](%s): " format "\n", \
                                        esp_utils_log_extract_file_name(__FILE__), __LINE__, __func__,  ##__VA_ARGS__)
#define ESP_UTILS_IMPL_LOGI(TAG, format, ...) printf("[I][" TAG "][%s:%04d](%s): " format "\n", \
                                        esp_utils_log_extract_file_name(__FILE__), __LINE__, __func__,  ##__VA_ARGS__)
#define ESP_UTILS_IMPL_LOGW(TAG, format, ...) printf("[W][" TAG "][%s:%04d](%s): " format "\n", \
                                        esp_utils_log_extract_file_name(__FILE__), __LINE__, __func__,  ##__VA_ARGS__)
#define ESP_UTILS_IMPL_LOGE(TAG, format, ...) printf("[E][" TAG "][%s:%04d](%s): " format "\n", \
                                        esp_utils_log_extract_file_name(__FILE__), __LINE__, __func__,  ##__VA_ARGS__)
