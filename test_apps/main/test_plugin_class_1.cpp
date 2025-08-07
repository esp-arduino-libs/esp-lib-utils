/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#include "test_plugin_class_1.hpp"

#if ESP_UTILS_CONF_PLUGIN_SUPPORT
ESP_UTILS_REGISTER_PLUGIN(TestPluginBase, TestPlugin1, "Test_Plugin_1_2")
#endif /* ESP_UTILS_CONF_PLUGIN_SUPPORT */
