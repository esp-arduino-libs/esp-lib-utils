/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_log.h"

#if !defined(ESP_UTILS_LOGD_IMPL_FUNC)
#   define ESP_UTILS_LOGD_IMPL_FUNC(TAG, format, ...) ESP_LOGD(TAG, format, ##__VA_ARGS__)
#endif
#if !defined(ESP_UTILS_LOGI_IMPL_FUNC)
#   define ESP_UTILS_LOGI_IMPL_FUNC(TAG, format, ...) ESP_LOGI(TAG, format, ##__VA_ARGS__)
#endif
#if !defined(ESP_UTILS_LOGW_IMPL_FUNC)
#   define ESP_UTILS_LOGW_IMPL_FUNC(TAG, format, ...) ESP_LOGW(TAG, format, ##__VA_ARGS__)
#endif
#if !defined(ESP_UTILS_LOGE_IMPL_FUNC)
#   define ESP_UTILS_LOGE_IMPL_FUNC(TAG, format, ...) ESP_LOGE(TAG, format, ##__VA_ARGS__)
#endif
