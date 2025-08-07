/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#pragma once

#include "test_plugin_class_base.hpp"
#if defined(ESP_UTILS_LOG_TAG)
#undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "TestPlugin1"
#include "esp_lib_utils.h"

#if ESP_UTILS_CONF_PLUGIN_SUPPORT
class TestPlugin1 : public TestPluginBase {
public:
    TestPlugin1(const std::string &name = "Default")
        : TestPluginBase()
        , name_(name)
    {
        ESP_UTILS_LOGI("TestPlugin1(%s) constructor", name_.c_str());
    }

    void run() override
    {
        ESP_UTILS_LOGI("TestPlugin1(%s) run", name_.c_str());
    }

    void stop() override
    {
        ESP_UTILS_LOGI("TestPlugin1(%s) stop", name_.c_str());
    }

private:
    std::string name_;
};

#endif /* ESP_UTILS_CONF_PLUGIN_SUPPORT */
