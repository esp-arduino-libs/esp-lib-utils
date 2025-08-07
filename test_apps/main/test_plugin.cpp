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
        ESP_UTILS_LOGI("TestPlugin3 constructor");
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

ESP_UTILS_REGISTER_PLUGIN(TestPluginBase, TestPlugin3, "Test_Plugin_3")

class TestPlugin4 : public TestPluginBase {
public:
    TestPlugin4(const TestPlugin4 &) = delete;
    TestPlugin4(TestPlugin4 &&) = delete;
    TestPlugin4 &operator=(const TestPlugin4 &) = delete;
    TestPlugin4 &operator=(TestPlugin4 &&) = delete;

    void run() override
    {
        ESP_UTILS_LOGI("TestPlugin4 run");
    }

    void stop() override
    {
        ESP_UTILS_LOGI("TestPlugin4 stop");
    }

    static std::shared_ptr<TestPlugin4> requestInstance()
    {
        if (!instance_) {
            ESP_UTILS_CHECK_EXCEPTION_RETURN(
                instance_ = std::shared_ptr<TestPlugin4>(new TestPlugin4()), nullptr,
                "Failed to create TestPlugin4 instance"
            );
        }
        return instance_;
    }

private:
    TestPlugin4()
        : TestPluginBase()
    {
        ESP_UTILS_LOGI("TestPlugin4 constructor");
    }

    inline static std::shared_ptr<TestPlugin4> instance_ = nullptr;
};

TEST_CASE("Test plugin functions on cpp", "[utils][plugin][CPP]")
{
    // 1. Show unified data storage
    std::cout << "\n1. All plugin information in unified storage:" << std::endl;
    TestPluginRegistry::forEach([](const auto & plugin) {
        std::cout << plugin.dump() << std::endl;
    });

    // 2. Simplified iterator usage
    std::cout << "\n2. Using standard iterator:" << std::endl;
    auto plugins = TestPluginRegistry::getAllPluginInfo();
    for (const auto &plugin : plugins) {
        std::cout << plugin.dump() << std::endl;
    }

    // 3. On-demand instantiation
    std::cout << "\n3. Creating instances on demand:" << std::endl;
    auto test_plugin_1_2 = TestPluginRegistry::get("Test_Plugin_1_2"); // First match
    TEST_ASSERT_NOT_NULL_MESSAGE(test_plugin_1_2, "Failed to get Test_Plugin_1_2");
    test_plugin_1_2->run();
    test_plugin_1_2->stop();
    auto test_plugin_3 = TestPluginRegistry::get("Test_Plugin_3");
    TEST_ASSERT_NOT_NULL_MESSAGE(test_plugin_3, "Failed to get Test_Plugin_3");
    test_plugin_3->run();
    test_plugin_3->stop();
    auto test_plugin_4 = TestPluginRegistry::get("Test_Plugin_4");
    TEST_ASSERT_NULL_MESSAGE(test_plugin_4, "Test_Plugin_4 should not be instantiated");

    // 4. Get all plugins with same name
    std::cout << "\n4. Getting all \"Test_Plugin_1_2\" instances:" << std::endl;
    auto all_test_plugin_1_2 = TestPluginRegistry::getAll("Test_Plugin_1_2");
    for (size_t i = 0; i < all_test_plugin_1_2.size(); ++i) {
        all_test_plugin_1_2[i]->run();
        all_test_plugin_1_2[i]->stop();
    }

    // 5. Statistics information
    std::cout << "\n5. Simplified statistics:" << std::endl;
    std::cout << "Total plugins: " << TestPluginRegistry::getPluginCount() << std::endl;

    auto name_counts = TestPluginRegistry::getNameCounts();
    for (const auto& [name, count] : name_counts) {
        std::cout << "  " << name << ": " << count << " variants" << std::endl;
    }

    // 6. Direct access to internal data structure
    std::cout << "\n6. Check status after instantiation:" << std::endl;
    TestPluginRegistry::forEach([](const auto & plugin) {
        std::cout << "Plugin: " << plugin.name
                  << " | Instantiated: " << (plugin.hasInstance() ? "Yes" : "No")
                  << std::endl;
    });

    // 7. Singleton support
    std::cout << "\n7. Register singleton instance:" << std::endl;
    ESP_UTILS_REGISTER_PLUGIN_WITH_INSTANCE(TestPluginBase, TestPlugin4, "Test_Plugin_4", TestPlugin4::requestInstance());
    auto retrieved_singleton = TestPluginRegistry::get("Test_Plugin_4");
    TEST_ASSERT_NOT_NULL_MESSAGE(retrieved_singleton, "Failed to get Test_Plugin_4");
    retrieved_singleton->run();
    retrieved_singleton->stop();

    // 8. Find plugin by predicate
    std::cout << "\n8. Find plugin by predicate:" << std::endl;
    auto found_plugin_success = TestPluginRegistry::findIf([](const auto & plugin) {
        return plugin.name == "Test_Plugin_4";
    });
    TEST_ASSERT_TRUE_MESSAGE(found_plugin_success != TestPluginRegistry::end(), "Failed to find plugin");
    std::cout << "Found plugin: " << found_plugin_success->name << " with type: " << found_plugin_success->type_name << std::endl;
    auto found_plugin_failed = TestPluginRegistry::findIf([](const auto & plugin) {
        return plugin.name == "Test_Plugin_5";
    });
    TEST_ASSERT_TRUE_MESSAGE(found_plugin_failed == TestPluginRegistry::end(), "Found plugin that should not exist");
    std::cout << "Not found plugin: Test_Plugin_5 (as expected)" << std::endl;

    // 9. Test plugin removal interfaces
    std::cout << "\n9. Test plugin removal interfaces:" << std::endl;
    // Show current plugin count
    std::cout << "Current plugin count: " << TestPluginRegistry::getPluginCount() << std::endl;
    // Test remove by name (first match only)
    bool removed = TestPluginRegistry::removePlugin("Test_Plugin_1_2");
    std::cout << "Removed Test_Plugin_1_2 (first match): " << (removed ? "Success" : "Failed") << std::endl;
    // Test remove all plugins with same name
    size_t removed_count = TestPluginRegistry::removeAllPlugins("Test_Plugin_1_2");
    std::cout << "Removed all Test_Plugin_1_2: " << removed_count << " plugins" << std::endl;
    // Test remove by type
    bool removed_by_type = TestPluginRegistry::removePluginByType<TestPlugin3>();
    std::cout << "Removed TestPlugin3 by type: " << (removed_by_type ? "Success" : "Failed") << std::endl;
    // Test remove specific plugin by name and type
    bool removed_specific = TestPluginRegistry::removeSpecificPlugin<TestPlugin4>("Test_Plugin_4");
    std::cout << "Removed specific TestPlugin4 with name Test_Plugin_4: " << (removed_specific ? "Success" : "Failed") << std::endl;
    // Show remaining plugin count
    std::cout << "Remaining plugin count: " << TestPluginRegistry::getPluginCount() << std::endl;
    // List remaining plugins
    std::cout << "Remaining plugins:" << std::endl;
    TestPluginRegistry::forEach([](const auto & plugin) {
        std::cout << "  - " << plugin.name << " (" << plugin.type_name << ")" << std::endl;
    });
    // Test removeIf with predicate
    size_t removed_by_predicate = TestPluginRegistry::removeIf([](const auto & plugin) {
        return plugin.name.find("Test_Plugin") != std::string::npos;
    });
    std::cout << "Removed plugins with 'Test_Plugin' in name: " << removed_by_predicate << " plugins" << std::endl;
    // Clear all plugins
    std::cout << "Clearing all plugins..." << std::endl;
    size_t cleared_count = TestPluginRegistry::clearAllPlugins();
    std::cout << "Cleared " << cleared_count << " plugins" << std::endl;
    // Final count after clearAllPlugins
    TEST_ASSERT_TRUE_MESSAGE(TestPluginRegistry::getPluginCount() == 0, "Plugin count should be 0 after clearAllPlugins");
}

#endif /* ESP_UTILS_CONF_PLUGIN_SUPPORT */
