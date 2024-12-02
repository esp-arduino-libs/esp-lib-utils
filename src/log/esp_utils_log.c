/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "esp_utils_config_internal.h"

#if ESP_UTILS_ENABLE_LOG
/**
 * @brief  Extract filename from file path
 *
 * @param  file_path File path
 * @return File name
 */
const char *esp_utils_log_extract_file_name(const char *file_path)
{
    const char *filename = strrchr(file_path, '/');
    if (!filename) {
        filename = strrchr(file_path, '\\');  // Windows path compatibility
    }

    return filename ? filename + 1 : file_path;
}
#endif // ESP_UTILS_ENABLE_LOG