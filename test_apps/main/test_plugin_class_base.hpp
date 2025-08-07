/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#pragma once

#include <string>
#include "esp_lib_utils.h"

#if ESP_UTILS_CONF_PLUGIN_SUPPORT
class TestPluginBase {
public:
    virtual void run() = 0;
    virtual void stop() = 0;

    virtual ~TestPluginBase() = default;
};

using TestPluginRegistry = esp_utils::PluginRegistry<TestPluginBase>;

#endif /* ESP_UTILS_CONF_PLUGIN_SUPPORT */
