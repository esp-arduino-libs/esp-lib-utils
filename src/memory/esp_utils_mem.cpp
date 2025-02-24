/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <new>
#include "log/esp_utils_log.h"
#include "esp_utils_mem.h"

void *operator new (std::size_t size)
{
    ESP_UTILS_LOG_TRACE_ENTER();

    // ESP_UTILS_LOGD("Param: size: %zu", size);
    void *ptr = esp_utils_mem_gen_malloc(size);
#if CONFIG_COMPILER_CXX_EXCEPTIONS
    if (!ptr) {
        throw std::bad_alloc();
    }
#endif

    ESP_UTILS_LOG_TRACE_EXIT();

    return ptr;
}

void *operator new[](std::size_t size)
{
    ESP_UTILS_LOG_TRACE_ENTER();

    // ESP_UTILS_LOGD("Param: size: %zu", size);
    void *ptr = esp_utils_mem_gen_malloc(size);
#if CONFIG_COMPILER_CXX_EXCEPTIONS
    if (!ptr) {
        throw std::bad_alloc();
    }
#endif

    ESP_UTILS_LOG_TRACE_EXIT();

    return ptr;
}

void operator delete (void *ptr) noexcept
{
    ESP_UTILS_LOG_TRACE_ENTER();

    // ESP_UTILS_LOGD("Param: ptr: %p", ptr);
    esp_utils_mem_gen_free(ptr);

    ESP_UTILS_LOG_TRACE_EXIT();
}

void operator delete[](void *ptr) noexcept
{
    ESP_UTILS_LOG_TRACE_ENTER();

    // ESP_UTILS_LOGD("Param: ptr: %p", ptr);
    esp_utils_mem_gen_free(ptr);

    ESP_UTILS_LOG_TRACE_EXIT();
}

void operator delete (void *ptr, std::size_t size) noexcept
{
    ESP_UTILS_LOG_TRACE_ENTER();

    // ESP_UTILS_LOGD("Param: ptr: %p, size: %zu", ptr, size);
    esp_utils_mem_gen_free(ptr);

    ESP_UTILS_LOG_TRACE_EXIT();
}

void operator delete[](void *ptr, std::size_t size) noexcept
{
    ESP_UTILS_LOG_TRACE_ENTER();

    //  ESP_UTILS_LOGD("Param: ptr: %p, size: %zu", ptr, size);
    esp_utils_mem_gen_free(ptr);

    ESP_UTILS_LOG_TRACE_EXIT();
}
