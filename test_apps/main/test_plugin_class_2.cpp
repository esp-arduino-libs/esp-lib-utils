/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#include "test_plugin_class_base.hpp"
#if defined(ESP_UTILS_LOG_TAG)
#undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "TestPlugin2"
#include "esp_lib_utils.h"

#if ESP_UTILS_CONF_PLUGIN_SUPPORT
class TestPlugin2 : public TestPluginBase {
public:
    TestPlugin2()
        : TestPluginBase()
    {
        ESP_UTILS_LOGI("TestPlugin2 constructor");
    }

    void run() override
    {
        ESP_UTILS_LOGI("TestPlugin2 run");
    }

    void stop() override
    {
        ESP_UTILS_LOGI("TestPlugin2 stop");
    }
};

ESP_UTILS_REGISTER_PLUGIN(TestPluginBase, TestPlugin2, "Test_Plugin_1_2")
#endif /* ESP_UTILS_CONF_PLUGIN_SUPPORT */
