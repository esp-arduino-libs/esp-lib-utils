/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace esp_utils {

struct ThreadConfig {
    void dump() const;

    const char  *name = nullptr;
    int       core_id = (CONFIG_PTHREAD_TASK_CORE_DEFAULT == -1) ? tskNO_AFFINITY : CONFIG_PTHREAD_TASK_CORE_DEFAULT;
    size_t   priority = CONFIG_PTHREAD_TASK_PRIO_DEFAULT;
    size_t stack_size = CONFIG_PTHREAD_TASK_STACK_SIZE_DEFAULT;
    bool stack_in_ext = false;
};

class thread_config_guard {
public:
    thread_config_guard(const ThreadConfig &config);
    ~thread_config_guard();
};

}; // namespace esp_utils
