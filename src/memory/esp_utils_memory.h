/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"
#include "esp_utils_config_internal.h"

#ifdef __cplusplus

#include <memory>
#include <cstdlib>

extern "C" void *esp_utils_memory_malloc(size_t size);
extern "C" void esp_utils_memory_free(void *p);

namespace esp_utils {

template <typename T>
struct MemoryAllocator {
    using value_type = T;

    MemoryAllocator() = default;

    template <typename U>
    MemoryAllocator(const MemoryAllocator<U> &) {}

    T *allocate(std::size_t n)
    {
        if (n == 0) {
            return nullptr;
        }
        void *ptr = esp_utils_memory_malloc(n * sizeof(T));
#if CONFIG_COMPILER_CXX_EXCEPTIONS
        if (ptr == nullptr) {
            throw std::bad_alloc();
        }
#endif // CONFIG_COMPILER_CXX_EXCEPTIONS
        return static_cast<T *>(ptr);
    }

    void deallocate(T *p, std::size_t n)
    {
        esp_utils_memory_free(p);
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
    return std::allocate_shared<T, MemoryAllocator<T>>(MemoryAllocator<T>(), std::forward<Args>(args)...);
}

} // namespace esp_utils

#else

void *esp_utils_memory_malloc(size_t size);
void esp_utils_memory_free(void *p);

#endif // __cplusplus

/**
 * @brief Helper functions to allocate memory using the memory allocator
 *
 */
#undef malloc
#undef free
#undef calloc
#define malloc(size)    esp_utils_memory_malloc(size)
#define free(ptr)       esp_utils_memory_free(ptr)
#define calloc(n, size)                \
    ({                              \
        size_t _size = (size_t)n * size;  \
        void *p = malloc(_size);    \
        if (p != NULL) {         \
            memset(p, 0, _size); \
        }                           \
        p;                          \
    })
