/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include "esp_utils_config_internal.h"
#include "check/esp_utils_check.h"
#include "log/esp_utils_log.h"
#if ESP_UTILS_MEM_GENERAL_ALLOC_TYPE == ESP_UTILS_MEM_GENERAL_ALLOC_TYPE_ESP
#include "esp_heap_caps.h"
#elif ESP_UTILS_MEM_GENERAL_ALLOC_TYPE == ESP_UTILS_MEM_GENERAL_ALLOC_TYPE_CUSTOM
#include ESP_UTILS_MEM_GENERAL_ALLOC_CUSTOM_INCLUDE
#endif

void *esp_utils_memory_malloc(size_t size)
{
    void *p = NULL;
#if ESP_UTILS_MEM_GENERAL_ALLOC_TYPE == ESP_UTILS_MEM_ALLOC_TYPE_MICROPYTHON
#elif ESP_UTILS_MEM_GENERAL_ALLOC_TYPE == ESP_UTILS_MEM_ALLOC_TYPE_ESP
    p = heap_caps_malloc(size, ESP_UTILS_MEM_GENERAL_ALLOC_ESP_CAPS);
#elif ESP_UTILS_MEM_GENERAL_ALLOC_TYPE == ESP_UTILS_MEM_ALLOC_TYPE_CUSTOM
    p = ESP_UTILS_MEM_GENERAL_ALLOC_CUSTOM_MALLOC(size);
#else
    p = malloc(size);
#endif // ESP_UTILS_MEM_GENERAL_ALLOC_TYPE
    ESP_UTILS_LOGD("Malloc @%p: %d", p, (int)size);
    return p;
}

void esp_utils_memory_free(void *p)
{
    ESP_UTILS_LOGD("Free @%p", p);
#if ESP_UTILS_MEM_GENERAL_ALLOC_TYPE == ESP_UTILS_MEM_ALLOC_TYPE_MICROPYTHON
#elif ESP_UTILS_MEM_GENERAL_ALLOC_TYPE == ESP_UTILS_MEM_ALLOC_TYPE_ESP
    heap_caps_free(p);
#elif ESP_UTILS_MEM_GENERAL_ALLOC_TYPE == ESP_UTILS_MEM_ALLOC_TYPE_CUSTOM
    ESP_UTILS_MEM_GENERAL_ALLOC_CUSTOM_FREE(p);
#else
    free(p);
#endif // ESP_UTILS_MEM_GENERAL_ALLOC_TYPE
}
