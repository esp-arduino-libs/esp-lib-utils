/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_utils_config_internal.h"

#ifndef ESP_UTILS_LOG_TAG
#define ESP_UTILS_LOG_TAG "Utils"
#endif

#ifdef __cplusplus

#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <functional>
#include <mutex>

extern "C" const char *esp_utils_log_extract_file_name(const char *file_path);

namespace esp_utils {

/**
 * Class to handle logging
 *
 */
class Log {
public:
    // Singleton pattern: Get the unique instance of the class
    static Log &getInstance()
    {
        static Log instance;
        return instance;
    }

#if ESP_UTILS_ENABLE_LOG
    // Templates and conditional compilation: Filter logs by different levels
    template <int level>
    void print(const char *file, int line, const char *func, const char *format, ...)
    {
        // Logs below the global level will not be compiled
        if constexpr (level >= ESP_UTILS_LOG_GLOBAL_LEVEL) {
            // Mutex to avoid interleaved log messages
            _mutex.lock();
            // Use variadic arguments for formatted output
            va_list args;
            va_start(args, format);
            vsnprintf(_buffer, sizeof(_buffer), format, args);
            va_end(args);
            // Output log message
            printf(
                "[%s][%s][%s:%04d](%s): %s\n", ESP_UTILS_LOG_TAG, logLevelToString(level),
                esp_utils_log_extract_file_name(file), line, func, _buffer
            );
            _mutex.unlock();
        }
    }
#else
    // When logging is disabled, the `print` function has an empty implementation
    template <int level>
    void print(const char *, int, const char *, const char *, ...) const {}
#endif /* ESP_UTILS_ENABLE_LOG */

private:
    Log() = default;

#if ESP_UTILS_ENABLE_LOG
    // Convert log level to string
    static const char *logLevelToString(int level)
    {
        switch (level) {
        case ESP_UTILS_LOG_LEVEL_DEBUG:   return "DEBUG";
        case ESP_UTILS_LOG_LEVEL_INFO:    return " INFO";
        case ESP_UTILS_LOG_LEVEL_WARNING: return " WARN";
        case ESP_UTILS_LOG_LEVEL_ERROR:   return "ERROR";
        default: break;
        }
        return "UNKNOWN";
    }

    char _buffer[ESP_UTILS_LOG_BUFFER_SIZE];
    std::mutex _mutex;
#endif
};

} // namespace esp_utils

/**
 * Macros to simplify logging calls
 *
 */
#define ESP_UTILS_LOGD(format, ...) \
    esp_utils::Log::getInstance().print<ESP_UTILS_LOG_LEVEL_DEBUG>(__FILE__, __LINE__, __func__, format, ##__VA_ARGS__)
#define ESP_UTILS_LOGI(format, ...) \
    esp_utils::Log::getInstance().print<ESP_UTILS_LOG_LEVEL_INFO>(__FILE__, __LINE__, __func__, format, ##__VA_ARGS__)
#define ESP_UTILS_LOGW(format, ...) \
    esp_utils::Log::getInstance().print<ESP_UTILS_LOG_LEVEL_WARNING>(__FILE__, __LINE__, __func__, format, ##__VA_ARGS__)
#define ESP_UTILS_LOGE(format, ...) \
    esp_utils::Log::getInstance().print<ESP_UTILS_LOG_LEVEL_ERROR>(__FILE__, __LINE__, __func__, format, ##__VA_ARGS__)

/**
 * Micros to log trace of function calls
 *
 */
#if ESP_UTILS_ENABLE_LOG_TRACE
#define ESP_UTILS_LOG_TRACE_ENTER_WITH_THIS() ESP_UTILS_LOGD("(@%p) Enter", this)
#define ESP_UTILS_LOG_TRACE_EXIT_WITH_THIS()  ESP_UTILS_LOGD("(@%p) Exit", this)
#else
#define ESP_UTILS_LOG_TRACE_ENTER_WITH_THIS()
#define ESP_UTILS_LOG_TRACE_EXIT_WITH_THIS()
#endif

#else

#include <stdio.h>
#include <string.h>

const char *esp_utils_log_extract_file_name(const char *file_path);

#define ESP_UTILS_IMPL_LOGD(format, ...) printf("[" ESP_UTILS_LOG_TAG "][DEBUG][%s:%04d](%s): " format "\n", \
                                         esp_utils_log_extract_file_name(__FILE__), __LINE__, __func__,  ##__VA_ARGS__)
#define ESP_UTILS_IMPL_LOGI(format, ...) printf("[" ESP_UTILS_LOG_TAG "][ INFO][%s:%04d](%s): " format "\n", \
                                         esp_utils_log_extract_file_name(__FILE__), __LINE__, __func__,  ##__VA_ARGS__)
#define ESP_UTILS_IMPL_LOGW(format, ...) printf("[" ESP_UTILS_LOG_TAG "][ WARN][%s:%04d](%s): " format "\n", \
                                         esp_utils_log_extract_file_name(__FILE__), __LINE__, __func__,  ##__VA_ARGS__)
#define ESP_UTILS_IMPL_LOGE(format, ...) printf("[" ESP_UTILS_LOG_TAG "][ERROR][%s:%04d](%s): " format "\n", \
                                         esp_utils_log_extract_file_name(__FILE__), __LINE__, __func__,  ##__VA_ARGS__)

#define ESP_UTILS_LOG_LEVEL(level, format, ...) do {                                                    \
        if      (level == ESP_UTILS_LOG_LEVEL_DEBUG)   { ESP_UTILS_IMPL_LOGD(format, ##__VA_ARGS__); }  \
        else if (level == ESP_UTILS_LOG_LEVEL_INFO)    { ESP_UTILS_IMPL_LOGI(format, ##__VA_ARGS__); }  \
        else if (level == ESP_UTILS_LOG_LEVEL_WARNING) { ESP_UTILS_IMPL_LOGW(format, ##__VA_ARGS__); }  \
        else if (level == ESP_UTILS_LOG_LEVEL_ERROR)   { ESP_UTILS_IMPL_LOGE(format, ##__VA_ARGS__); }  \
        else { }                                                                                        \
    } while(0)

#define ESP_UTILS_LOG_LEVEL_LOCAL(level, format, ...) do {                                          \
        if (level >= ESP_UTILS_LOG_GLOBAL_LEVEL) ESP_UTILS_LOG_LEVEL(level, format, ##__VA_ARGS__); \
    } while(0)

/**
 * Macros to simplify logging calls
 *
 */
#if ESP_UTILS_ENABLE_LOG
#define ESP_UTILS_LOGD(format, ...) ESP_UTILS_LOG_LEVEL_LOCAL(ESP_UTILS_LOG_LEVEL_DEBUG,   format, ##__VA_ARGS__)
#define ESP_UTILS_LOGI(format, ...) ESP_UTILS_LOG_LEVEL_LOCAL(ESP_UTILS_LOG_LEVEL_INFO,    format, ##__VA_ARGS__)
#define ESP_UTILS_LOGW(format, ...) ESP_UTILS_LOG_LEVEL_LOCAL(ESP_UTILS_LOG_LEVEL_WARNING, format, ##__VA_ARGS__)
#define ESP_UTILS_LOGE(format, ...) ESP_UTILS_LOG_LEVEL_LOCAL(ESP_UTILS_LOG_LEVEL_ERROR,   format, ##__VA_ARGS__)
#else
#define ESP_UTILS_LOGD(...)
#define ESP_UTILS_LOGI(...)
#define ESP_UTILS_LOGW(...)
#define ESP_UTILS_LOGE(...)
#endif // ESP_UTILS_ENABLE_LOG

#endif // __cplusplus

/**
 * Micros to log trace of function calls
 *
 */
#if ESP_UTILS_ENABLE_LOG_TRACE
#define ESP_UTILS_LOG_TRACE_ENTER() ESP_UTILS_LOGD("Enter")
#define ESP_UTILS_LOG_TRACE_EXIT()  ESP_UTILS_LOGD("Exit")
#else
#define ESP_UTILS_LOG_TRACE_ENTER()
#define ESP_UTILS_LOG_TRACE_EXIT()
#endif

/**
 * Macros to replace ESP-IDF logging functions
 *
 */
#undef ESP_LOGV
#undef ESP_LOGD
#undef ESP_LOGI
#undef ESP_LOGW
#undef ESP_LOGE
#define ESP_LOGV(TAG, ...) { (void)TAG; ESP_UTILS_LOGD(__VA_ARGS__); }
#define ESP_LOGD(TAG, ...) { (void)TAG; ESP_UTILS_LOGD(__VA_ARGS__); }
#define ESP_LOGI(TAG, ...) { (void)TAG; ESP_UTILS_LOGI(__VA_ARGS__); }
#define ESP_LOGW(TAG, ...) { (void)TAG; ESP_UTILS_LOGW(__VA_ARGS__); }
#define ESP_LOGE(TAG, ...) { (void)TAG; ESP_UTILS_LOGE(__VA_ARGS__); }
