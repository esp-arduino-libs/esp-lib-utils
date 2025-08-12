/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

/* Base */
#include "esp_utils_types.h"
#include "esp_utils_versions.h"
#include "esp_utils_conf_internal.h"

/* Check */
#include "check/esp_utils_check.h"

/* Log */
#include "log/esp_utils_log.h"

/* Memory */
#include "memory/esp_utils_mem.h"

#if defined(__cplusplus)

/* Log */
#include "log/esp_utils_log.hpp"

/* Thread */
#include "thread/esp_utils_thread.hpp"

/* More */
#include "more/esp_utils_value_guard.hpp"
#include "more/esp_utils_function_guard.hpp"
#if ESP_UTILS_CONF_PLUGIN_SUPPORT
#   include "more/esp_utils_plugin_registry.hpp"
#endif

#endif // defined(__cplusplus)
