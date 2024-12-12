/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include "sdkconfig.h"
#include "esp_utils_conf_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

void *esp_utils_mem_gen_malloc(size_t size);
void esp_utils_mem_gen_free(void *p);
void *esp_utils_mem_gen_calloc(size_t n, size_t size);
bool esp_utils_mem_print_info(void);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#include <memory>

namespace esp_utils {

template <typename T>
struct GeneralMemoryAllocator {
    using value_type = T;

    GeneralMemoryAllocator() = default;

    template <typename U>
    GeneralMemoryAllocator(const GeneralMemoryAllocator<U> &) {}

    T *allocate(std::size_t n)
    {
        if (n == 0) {
            return nullptr;
        }
        void *ptr = esp_utils_mem_gen_malloc(n * sizeof(T));
        if (ptr == nullptr) {
#if CONFIG_COMPILER_CXX_EXCEPTIONS
            throw std::bad_alloc();
#else
            abort();
#endif // CONFIG_COMPILER_CXX_EXCEPTIONS
        }
        return static_cast<T *>(ptr);
    }

    void deallocate(T *p, std::size_t n)
    {
        esp_utils_mem_gen_free(p);
    }

    template <typename U, typename... Args>
    void construct(U *p, Args &&... args)
    {
        new (p) U(std::forward<Args>(args)...);
    }

    template <typename U>
    void destroy(U *p)
    {
        p->~U();
    }
};

/**
 * @brief Helper function to create a shared pointer using the memory allocator
 *
 */
template <typename T, typename... Args>
std::shared_ptr<T> make_shared(Args &&... args)
{
    return std::allocate_shared<T, GeneralMemoryAllocator<T>>(GeneralMemoryAllocator<T>(), std::forward<Args>(args)...);
}

} // namespace esp_utils

#endif // __cplusplus

/**
 * @brief Helper functions to allocate memory using the memory allocator
 *
 */
#undef malloc
#undef free
#undef calloc
#define malloc(size)    esp_utils_mem_gen_malloc(size)
#define free(p)         esp_utils_mem_gen_free(p)
#define calloc(n, size) esp_utils_mem_gen_calloc(n, size)
