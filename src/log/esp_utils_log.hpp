/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <algorithm>
#include <cstdio>
#include <type_traits>
#include <utility>
#include <string>
#include <source_location>
#include "esp_utils_log.h"

namespace esp_utils {
namespace detail {

std::string parseFunctionName(const char *func_name);

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
template <FixedString TAG>
class log_trace_guard {
public:
    log_trace_guard(const void *this_ptr = nullptr, const std::source_location &loc = std::source_location::current())
        : _this_ptr(this_ptr)
    {
        _line = static_cast<int>(loc.line());
        _func_name = parseFunctionName(loc.function_name());
        if (_func_name.empty()) {
            _func_name = "???";
        }
        _file_name = std::string(esp_utils_log_extract_file_name(loc.file_name()));
        if (_file_name.empty()) {
            _file_name = "???";
        }

        if (_this_ptr) {
            ESP_UTILS_LOGD_IMPL_FUNC(
                TAG.c_str(), "[%s:%04d](%s): (@%p) Enter", _file_name.c_str(), _line, _func_name.c_str(), _this_ptr
            );
        } else {
            ESP_UTILS_LOGD_IMPL_FUNC(
                TAG.c_str(), "[%s:%04d](%s): Enter", _file_name.c_str(), _line, _func_name.c_str()
            );
        }
    }

    ~log_trace_guard()
    {
        if (_this_ptr) {
            ESP_UTILS_LOGD_IMPL_FUNC(
                TAG.c_str(), "[%s:%04d](%s): (@%p) Exit", _file_name.c_str(), _line, _func_name.c_str(), _this_ptr
            );
        } else {
            ESP_UTILS_LOGD_IMPL_FUNC(
                TAG.c_str(), "[%s:%04d](%s): Exit", _file_name.c_str(), _line, _func_name.c_str()
            );
        }
    }

private:
    int _line = 0;
    std::string _func_name;
    std::string _file_name;
    const void *_this_ptr = nullptr;
};

} // namespace detail
} // namespace esp_utils

#if ESP_UTILS_CONF_ENABLE_LOG_TRACE
// The following macros are deprecated, please use `ESP_UTILS_LOG_TRACE_GUARD()` instead
#   define ESP_UTILS_LOG_TRACE_ENTER_WITH_THIS() ESP_UTILS_LOGD("(@%p) Enter", this)
#   define ESP_UTILS_LOG_TRACE_EXIT_WITH_THIS()  ESP_UTILS_LOGD("(@%p) Exit", this)

#   define ESP_UTILS_LOG_MAKE_FS(str) []{ constexpr esp_utils::detail::FixedString<sizeof(str)> s(str); return s; }()
#   define ESP_UTILS_LOG_TRACE_GUARD()           esp_utils::detail::log_trace_guard<ESP_UTILS_LOG_MAKE_FS(ESP_UTILS_LOG_TAG)> _log_trace_guard_{}
#   define ESP_UTILS_LOG_TRACE_GUARD_WITH_THIS() esp_utils::detail::log_trace_guard<ESP_UTILS_LOG_MAKE_FS(ESP_UTILS_LOG_TAG)> _log_trace_guard_{this}
#else
#   define ESP_UTILS_LOG_TRACE_ENTER_WITH_THIS()
#   define ESP_UTILS_LOG_TRACE_EXIT_WITH_THIS()
#   define ESP_UTILS_LOG_TRACE_GUARD()
#   define ESP_UTILS_LOG_TRACE_GUARD_WITH_THIS()
#endif
