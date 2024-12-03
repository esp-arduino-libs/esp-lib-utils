/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_utils_conf_internal.h"

/* Library Version */
#define ESP_UTILS_VERSION_MAJOR 0
#define ESP_UTILS_VERSION_MINOR 1
#define ESP_UTILS_VERSION_PATCH 0

/* File `esp_utils_conf.h` */
#define ESP_UTILS_CONF_VERSION_MAJOR 0
#define ESP_UTILS_CONF_VERSION_MINOR 1
#define ESP_UTILS_CONF_VERSION_PATCH 0

// *INDENT-OFF*

/**
 * Check if the current configuration file version is compatible with the library version
 *
 */
/* File `esp_utils_conf.h` */
#ifndef ESP_UTILS_CONF_FILE_SKIP
    // Check if the current configuration file version is compatible with the library version
    #if ESP_UTILS_CONF_FILE_VERSION_MAJOR != ESP_UTILS_CONF_VERSION_MAJOR
        #error "The file `esp_utils_conf.h` version is not compatible. Please update it with the file from the library"
    #elif ESP_UTILS_CONF_FILE_VERSION_MINOR < ESP_UTILS_CONF_VERSION_MINOR
        #warning "The file `esp_utils_conf.h` version is outdated. Some new configurations are missing"
    #elif ESP_UTILS_CONF_FILE_VERSION_PATCH > ESP_UTILS_VERSION_PATCH
        #warning "The file `esp_utils_conf.h` version is newer than the library. Some new configurations are not supported"
    #endif /* ESP_UTILS_CONF_INCLUDE_INSIDE */
#endif /* ESP_UTILS_CONF_FILE_SKIP */

// *INDENT-OFF*
