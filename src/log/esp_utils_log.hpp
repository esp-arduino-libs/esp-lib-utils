/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <algorithm>
#include <cstdio>
#include <type_traits>
#include <memory>
#include <utility>
#include "esp_utils_log.h"

namespace esp_utils {
namespace detail {

// Template for string NTTP parameters
template <size_t N>
struct FixedString {
    char data[N] {};

    constexpr FixedString(const char (&str)[N])
    {
        std::copy_n(str, N, data);
    }

    constexpr bool operator==(const FixedString &) const = default;
    constexpr bool operator<(const FixedString &rhs) const
    {
        for (size_t i = 0; i < N; ++i) {
            if (data[i] != rhs.data[i]) {
                return data[i] < rhs.data[i];
            }
        }
        return false;
    }

    constexpr const char *c_str() const
    {
        return data;
    }
    constexpr size_t size() const
    {
        return N;
    }
};

// Log trace RAII class
template<FixedString file, int line, FixedString func>
class log_trace_guard {
public:
    log_trace_guard()
    {
        const char *fname = esp_utils_log_extract_file_name(file.c_str());
        printf("[D][" ESP_UTILS_LOG_TAG "][%s:%04d](%s): Enter\n", fname ? fname : "??", line, func.c_str());
    }

    explicit log_trace_guard(const void *this_ptr)
        : _this_ptr(this_ptr)
    {
        const char *fname = esp_utils_log_extract_file_name(file.c_str());
        printf("[D][" ESP_UTILS_LOG_TAG "][%s:%04d](%s): (@%p) Enter\n", fname ? fname : "??", line, func.c_str(), _this_ptr);
    }

    ~log_trace_guard()
    {
        const char *fname = esp_utils_log_extract_file_name(file.c_str());
        if (_this_ptr) {
            printf("[D][" ESP_UTILS_LOG_TAG "][%s:%04d](%s): (@%p) Exit\n", fname ? fname : "??", line, func.c_str(), _this_ptr);
        } else {
            printf("[D][" ESP_UTILS_LOG_TAG "][%s:%04d](%s): Exit\n", fname ? fname : "??", line, func.c_str());
        }
    }

private:
    const void *_this_ptr = nullptr;
};

} // namespace detail
} // namespace esp_utils

#if ESP_UTILS_CONF_ENABLE_LOG_TRACE
// The following macros are deprecated, please use `ESP_UTILS_LOG_TRACE_GUARD()` instead
#   define ESP_UTILS_LOG_TRACE_ENTER_WITH_THIS() ESP_UTILS_LOGD("(@%p) Enter", this)
#   define ESP_UTILS_LOG_TRACE_EXIT_WITH_THIS()  ESP_UTILS_LOGD("(@%p) Exit", this)

#   define ESP_UTILS_MAKE_FS(str) []{ constexpr esp_utils::detail::FixedString<sizeof(str)> s(str); return s; }()
#   define ESP_UTILS_LOG_TRACE_GUARD() \
    [[maybe_unused]] auto _log_trace_guard_ = [] { \
        using namespace esp_utils::detail; \
        constexpr auto __file = ESP_UTILS_MAKE_FS(__FILE__); \
        constexpr auto __func = ESP_UTILS_MAKE_FS(__func__); \
        std::unique_ptr<log_trace_guard<__file, __LINE__, __func>> _g; \
        if constexpr (ESP_UTILS_CONF_LOG_LEVEL == ESP_UTILS_LOG_LEVEL_DEBUG) { \
            _g = std::make_unique<log_trace_guard<__file, __LINE__, __func>>(); \
        } \
        return _g; \
    }()
#   define ESP_UTILS_LOG_TRACE_GUARD_WITH_THIS() \
    [[maybe_unused]] auto _log_trace_guard_ = [this] { \
        using namespace esp_utils::detail; \
        constexpr auto __file = ESP_UTILS_MAKE_FS(__FILE__); \
        constexpr auto __func = ESP_UTILS_MAKE_FS(__func__); \
        std::unique_ptr<log_trace_guard<__file, __LINE__, __func>> _g; \
        if constexpr (ESP_UTILS_CONF_LOG_LEVEL == ESP_UTILS_LOG_LEVEL_DEBUG) { \
            _g = std::make_unique<log_trace_guard<__file, __LINE__, __func>>(this); \
        } \
        return _g; \
    }()
#else
#   define ESP_UTILS_LOG_TRACE_ENTER_WITH_THIS()
#   define ESP_UTILS_LOG_TRACE_EXIT_WITH_THIS()
#   define ESP_UTILS_LOG_TRACE_GUARD()
#   define ESP_UTILS_LOG_TRACE_GUARD_WITH_THIS()
#endif
