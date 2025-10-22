/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#if defined(ESP_PLATFORM)
#   include "esp_pthread.h"
#endif
#include "check/esp_utils_check.h"
#include "log/esp_utils_log.hpp"
#include "esp_utils_thread.hpp"

namespace esp_utils {

void ThreadConfig::dump() const
{
    ESP_UTILS_LOGI(
        "\n{ThreadConfig}:\n"
        "\t-name(%s)\n"
        "\t-core_id(%d)\n"
        "\t-priority(%d)\n"
        "\t-stack_size(%d)\n"
#if !ESP_UTILS_THREAD_CONFIG_STACK_CAPS_INVALID
        "\t-stack_in_ext(%s)\n"
#endif
        , name
        , core_id
        , static_cast<int>(priority)
        , static_cast<int>(stack_size)
#if !ESP_UTILS_THREAD_CONFIG_STACK_CAPS_INVALID
        , stack_in_ext ? "true" : "false"
#endif
    );
}

thread_config_guard::thread_config_guard(const ThreadConfig &config)
{
    ESP_UTILS_LOG_TRACE_GUARD();

    ESP_UTILS_LOGD("Param: config(%p)", &config);
#if ESP_UTILS_CONF_LOG_LEVEL == ESP_UTILS_LOG_LEVEL_DEBUG
    config.dump();
#endif

#if defined(ESP_PLATFORM)
    auto new_cfg = esp_pthread_get_default_config();
    new_cfg.thread_name = config.name;
    new_cfg.stack_size = config.stack_size;
    new_cfg.prio = config.priority;
    new_cfg.inherit_cfg = false;
    new_cfg.pin_to_core = config.core_id;
#if !ESP_UTILS_THREAD_CONFIG_STACK_CAPS_INVALID
    new_cfg.stack_alloc_caps = (config.stack_in_ext ? MALLOC_CAP_SPIRAM : MALLOC_CAP_INTERNAL) | MALLOC_CAP_8BIT;
#endif

    auto err = esp_pthread_set_cfg(&new_cfg);
    ESP_UTILS_CHECK_FALSE_EXIT(err == ESP_OK, "Set thread config failed(%s)", esp_err_to_name(err));
#else
    (void)config;
    ESP_UTILS_LOGW("Not supported on non-ESP platforms");
#endif
}

thread_config_guard::~thread_config_guard()
{
    ESP_UTILS_LOG_TRACE_GUARD();

#if defined(ESP_PLATFORM)
    auto new_cfg = esp_pthread_get_default_config();
    auto err = esp_pthread_set_cfg(&new_cfg);
    ESP_UTILS_CHECK_FALSE_EXIT(err == ESP_OK, "Set thread config failed(%s)", esp_err_to_name(err));
#else
    ESP_UTILS_LOGW("Not supported on non-ESP platforms");
#endif
}

}; // namespace esp_utils
