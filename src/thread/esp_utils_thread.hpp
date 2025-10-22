/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#if defined(ESP_PLATFORM)
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define ESP_UTILS_THREAD_CONFIG_STACK_CAPS_INVALID  (ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 3, 0))
#endif

namespace esp_utils {

#if defined(ESP_PLATFORM)
constexpr const char *THREAD_CONFIG_NAME_DEFAULT        = CONFIG_PTHREAD_TASK_NAME_DEFAULT;
constexpr int         THREAD_CONFIG_CORE_DEFAULT        = ((CONFIG_PTHREAD_TASK_CORE_DEFAULT == -1) ? tskNO_AFFINITY : CONFIG_PTHREAD_TASK_CORE_DEFAULT);
constexpr size_t      THREAD_CONFIG_PRIO_DEFAULT        = CONFIG_PTHREAD_TASK_PRIO_DEFAULT;
constexpr size_t      THREAD_CONFIG_STACK_SIZE_DEFAULT  = CONFIG_PTHREAD_TASK_STACK_SIZE_DEFAULT;
#else
constexpr const char *THREAD_CONFIG_NAME_DEFAULT        = "pthread";
constexpr int         THREAD_CONFIG_CORE_DEFAULT        = 0;
constexpr size_t      THREAD_CONFIG_PRIO_DEFAULT        = 0;
constexpr size_t      THREAD_CONFIG_STACK_SIZE_DEFAULT  = 0;
#endif

struct ThreadConfig {
    void dump() const;

    const char  *name = nullptr;
    int       core_id = THREAD_CONFIG_CORE_DEFAULT;
    size_t   priority = THREAD_CONFIG_PRIO_DEFAULT;
    size_t stack_size = THREAD_CONFIG_STACK_SIZE_DEFAULT;
#if !ESP_UTILS_THREAD_CONFIG_STACK_CAPS_INVALID
    bool stack_in_ext = false;
#endif
};

class thread_config_guard {
public:
    thread_config_guard(const ThreadConfig &config);
    ~thread_config_guard();
};

}; // namespace esp_utils
