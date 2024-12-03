/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>
#include <stdlib.h>
#include "esp_utils_conf_internal.h"
#include "check/esp_utils_check.h"
#include "log/esp_utils_log.h"
#if ESP_UTILS_CONF_MEM_GEN_ALLOC_TYPE == ESP_UTILS_MEM_ALLOC_TYPE_ESP
#include "esp_heap_caps.h"
#elif ESP_UTILS_CONF_MEM_GEN_ALLOC_TYPE == ESP_UTILS_MEM_ALLOC_TYPE_CUSTOM
#include ESP_UTILS_CONF_MEM_GEN_ALLOC_CUSTOM_INCLUDE
#elif ESP_UTILS_CONF_MEM_GEN_ALLOC_TYPE == ESP_UTILS_MEM_ALLOC_TYPE_MICROPYTHON
#include <py/mpconfig.h>
#include <py/misc.h>
#include <py/gc.h>
#endif // ESP_UTILS_CONF_MEM_GEN_ALLOC_TYPE

void *esp_utils_mem_gen_malloc(size_t size)
{
    void *p = NULL;
#if ESP_UTILS_CONF_MEM_GEN_ALLOC_TYPE == ESP_UTILS_MEM_ALLOC_TYPE_STDLIB
    p = malloc(size);
#elif ESP_UTILS_CONF_MEM_GEN_ALLOC_TYPE == ESP_UTILS_MEM_ALLOC_TYPE_ESP
    p = heap_caps_aligned_alloc(ESP_UTILS_CONF_MEM_GEN_ALLOC_ESP_ALIGN, size, ESP_UTILS_CONF_MEM_GEN_ALLOC_ESP_CAPS);
#elif ESP_UTILS_CONF_MEM_GEN_ALLOC_TYPE == ESP_UTILS_MEM_ALLOC_TYPE_CUSTOM
    p = ESP_UTILS_CONF_MEM_GEN_ALLOC_CUSTOM_MALLOC(size);
#elif ESP_UTILS_CONF_MEM_GEN_ALLOC_TYPE == ESP_UTILS_MEM_ALLOC_TYPE_MICROPYTHON
#if MICROPY_MALLOC_USES_ALLOCATED_SIZE
    return gc_alloc(size, true);
#else
    return m_malloc(size);
#endif // MICROPY_MALLOC_USES_ALLOCATED_SIZE
#endif // ESP_UTILS_CONF_MEM_GEN_ALLOC_TYPE
    ESP_UTILS_LOGD("Malloc @%p: %d", p, (int)size);
    return p;
}

void esp_utils_mem_gen_free(void *p)
{
    ESP_UTILS_LOGD("Free @%p", p);
#if ESP_UTILS_CONF_MEM_GEN_ALLOC_TYPE == ESP_UTILS_MEM_ALLOC_TYPE_STDLIB
    free(p);
#elif ESP_UTILS_CONF_MEM_GEN_ALLOC_TYPE == ESP_UTILS_MEM_ALLOC_TYPE_ESP
    heap_caps_free(p);
#elif ESP_UTILS_CONF_MEM_GEN_ALLOC_TYPE == ESP_UTILS_MEM_ALLOC_TYPE_CUSTOM
    ESP_UTILS_CONF_MEM_GEN_ALLOC_CUSTOM_FREE(p);
#elif ESP_UTILS_CONF_MEM_GEN_ALLOC_TYPE == ESP_UTILS_MEM_ALLOC_TYPE_MICROPYTHON
#if MICROPY_MALLOC_USES_ALLOCATED_SIZE
    gc_free(p);
#else
    m_free(p);
#endif // MICROPY_MALLOC_USES_ALLOCATED_SIZE
#endif // ESP_UTILS_CONF_MEM_GEN_ALLOC_TYPE
}
