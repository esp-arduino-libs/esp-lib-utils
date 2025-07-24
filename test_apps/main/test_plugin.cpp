/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <iostream>
#include "unity.h"
#if defined(ESP_UTILS_LOG_TAG)
#undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "TestPlugin"
#include "esp_lib_utils.h"
#include "test_plugin_class_base.hpp"

#if ESP_UTILS_CONF_PLUGIN_SUPPORT
class TestPlugin3 : public TestPluginBase {
public:
    TestPlugin3()
        : TestPluginBase()
    {
    }

    void run() override
    {
        ESP_UTILS_LOGI("TestPlugin3 run");
    }

    void stop() override
    {
        ESP_UTILS_LOGI("TestPlugin3 stop");
    }
};

ESP_UTILS_REGISTER_PLUGIN(TestPluginBase, TestPlugin3, "Test Plugin 3")

TEST_CASE("Test plugin functions on cpp", "[utils][plugin][CPP]")
{
    ESP_UTILS_LOGI("=== Registered Plugin Names ===");
    auto names = TestPluginPluginRegistry::listRegisteredNames();
    for (const auto &name : names) {
        ESP_UTILS_LOGI("Plugin name: %s", name.c_str());
    }

    ESP_UTILS_LOGI("=== Detailed Registration Info (Real Type Names) ===");
    auto info = TestPluginPluginRegistry::getRegistrationInfo();
    for (const auto& [name, real_type] : info) {
        ESP_UTILS_LOGI("Plugin name: %s -> Real type: %s", name.c_str(), real_type.c_str());
    }
    ESP_UTILS_LOGI("=== Testing Plugin Execution ===");

    auto test1_plugin = std::dynamic_pointer_cast<TestPluginBase>(TestPluginPluginRegistry::get("Test Plugin 1"));
    TEST_ASSERT_NOT_NULL_MESSAGE(test1_plugin, "Failed to get test1 plugin");
    test1_plugin->run();
    test1_plugin->stop();

    auto test2_plugin = std::dynamic_pointer_cast<TestPluginBase>(TestPluginPluginRegistry::get("Test Plugin 2"));
    TEST_ASSERT_NOT_NULL_MESSAGE(test2_plugin, "Failed to get test2 plugin");
    test2_plugin->run();
    test2_plugin->stop();

    auto test3_plugin_from_name = std::dynamic_pointer_cast<TestPluginBase>(TestPluginPluginRegistry::get("Test Plugin 3"));
    TEST_ASSERT_NOT_NULL_MESSAGE(test3_plugin_from_name, "Failed to get test3 plugin");
    auto test3_plugin_from_type = TestPluginPluginRegistry::get<TestPlugin3>();
    TEST_ASSERT_NOT_NULL_MESSAGE(test3_plugin_from_type, "Failed to get test3 plugin");
    TEST_ASSERT_EQUAL_PTR(test3_plugin_from_name.get(), test3_plugin_from_type.get());
    test3_plugin_from_name->run();
    test3_plugin_from_type->stop();

    auto test_plugin_none = std::dynamic_pointer_cast<TestPluginBase>(TestPluginPluginRegistry::get("Test Plugin None"));
    TEST_ASSERT_NULL_MESSAGE(test_plugin_none, "No such plugin, but it appears in the registry");
}
#endif /* ESP_UTILS_CONF_PLUGIN_SUPPORT */
