/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdio.h>

#if !defined(ESP_UTILS_LOGD_IMPL_FUNC)
#   define ESP_UTILS_LOGD_IMPL_FUNC(TAG, format, ...) printf("[D][%s]" format "\n", TAG, ##__VA_ARGS__)
#endif
#if !defined(ESP_UTILS_LOGI_IMPL_FUNC)
#   define ESP_UTILS_LOGI_IMPL_FUNC(TAG, format, ...) printf("[I][%s]" format "\n", TAG, ##__VA_ARGS__)
#endif
#if !defined(ESP_UTILS_LOGW_IMPL_FUNC)
#   define ESP_UTILS_LOGW_IMPL_FUNC(TAG, format, ...) printf("[W][%s]" format "\n", TAG, ##__VA_ARGS__)
#endif
#if !defined(ESP_UTILS_LOGE_IMPL_FUNC)
#   define ESP_UTILS_LOGE_IMPL_FUNC(TAG, format, ...) printf("[E][%s]" format "\n", TAG, ##__VA_ARGS__)
#endif
