/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_err.h"
#include "esp_utils_config_internal.h"
#include "log/esp_utils_log.h"

#if !ESP_UTILS_ENABLE_CHECK

#ifndef ESP_UTILS_CHECK_TAG
#define ESP_UTILS_CHECK_TAG(goto_tag)
#endif

#define ESP_UTILS_CHECK_NULL_RETURN(x, ...)       ((void)(x))
#define ESP_UTILS_CHECK_NULL_GOTO(x,  ...)        ((void)(x))
#define ESP_UTILS_CHECK_NULL_EXIT(x, ...)         ((void)(x))

#define ESP_UTILS_CHECK_FALSE_RETURN(x, ...)      ((void)(x))
#define ESP_UTILS_CHECK_FALSE_GOTO(x, ...)        ((void)(x))
#define ESP_UTILS_CHECK_FALSE_EXIT(x, ...)        ((void)(x))

#define ESP_UTILS_CHECK_ERROR_RETURN(x, ...)      ((void)(x))
#define ESP_UTILS_CHECK_ERROR_GOTO(x, ...)        ((void)(x))
#define ESP_UTILS_CHECK_ERROR_EXIT(x, ...)        ((void)(x))

#ifdef __cplusplus
#define ESP_UTILS_CHECK_EXCEPTION_RETURN(x, ret, fmt, ...)      ((void)(x))
#define ESP_UTILS_CHECK_EXCEPTION_GOTO(x, goto_tag, fmt, ...)   ((void)(x))
#define ESP_UTILS_CHECK_EXCEPTION_EXIT(x, fmt, ...)             ((void)(x))
#endif // __cplusplus

#else

#if ESP_UTILS_CHECK_WITH_ASSERT

#ifndef ESP_UTILS_CHECK_TAG
#define ESP_UTILS_CHECK_TAG(goto_tag)
#endif

#define ESP_UTILS_CHECK_NULL_RETURN(x, ...)       assert((x) != NULL)
#define ESP_UTILS_CHECK_NULL_GOTO(x,  ...)        assert((x) != NULL)
#define ESP_UTILS_CHECK_NULL_EXIT(x, ...)         assert((x) != NULL)

#define ESP_UTILS_CHECK_FALSE_RETURN(x, ...)      assert(x)
#define ESP_UTILS_CHECK_FALSE_GOTO(x, ...)        assert(x)
#define ESP_UTILS_CHECK_FALSE_EXIT(x, ...)        assert(x)

#define ESP_UTILS_CHECK_ERROR_RETURN(x, ...)      assert((x) != ESP_OK)
#define ESP_UTILS_CHECK_ERROR_GOTO(x, ...)        assert((x) != ESP_OK)
#define ESP_UTILS_CHECK_ERROR_EXIT(x, ...)        assert((x) != ESP_OK)

#ifdef __cplusplus
#define ESP_UTILS_CHECK_EXCEPTION_RETURN(x, ...) do {\
        try { \
            x; \
        } catch (const std::exception &e) { \
            assert(false); \
        } \
    } while (0)

#define ESP_UTILS_CHECK_EXCEPTION_GOTO(x, ...) do {\
        try { \
            x; \
        } catch (const std::exception &e) { \
            assert(false); \
        } \
    } while (0)

#define ESP_UTILS_CHECK_EXCEPTION_EXIT(x, ...) do {\
        try { \
            x; \
        } catch (const std::exception &e) { \
            assert(false); \
        } \
    } while (0)
#endif // __cplusplus

#else

#ifndef unlikely
#define unlikely(x)  (x)
#endif

#ifndef ESP_UTILS_CHECK_TAG
#define ESP_UTILS_CHECK_TAG(goto_tag)  goto_tag:
#endif

#if ESP_UTILS_CHECK_WITH_ERROR_LOG

/**
 * @brief Check if the pointer is NULL; if NULL, log an error and return the specified value.
 *
 * @param x Pointer to check
 * @param ret Value to return if the pointer is NULL
 * @param fmt Format string for the error message
 * @param ... Additional arguments for the format string
 */
#define ESP_UTILS_CHECK_NULL_RETURN(x, ret, fmt, ...) do { \
            if (unlikely((x) == NULL)) {                          \
                ESP_UTILS_LOGE(fmt, ##__VA_ARGS__);        \
                return ret;                             \
            }                                           \
        } while(0)

/**
 * @brief Check if the pointer is NULL; if NULL, log an error and goto the specified label.
 *
 * @param x Pointer to check
 * @param goto_tag Label to jump to if the pointer is NULL
 * @param fmt Format string for the error message
 * @param ... Additional arguments for the format string
 */
#define ESP_UTILS_CHECK_NULL_GOTO(x, goto_tag, fmt, ...) do { \
            if (unlikely((x) == NULL)) {                             \
                ESP_UTILS_LOGE(fmt, ##__VA_ARGS__);           \
                goto goto_tag;                             \
            }                                              \
        } while(0)

/**
 * @brief Check if the pointer is NULL; if NULL, log an error and return without a value.
 *
 * @param x Pointer to check
 * @param fmt Format string for the error message
 * @param ... Additional arguments for the format string
 */
#define ESP_UTILS_CHECK_NULL_EXIT(x, fmt, ...) do { \
            if (unlikely((x) == NULL)) {                   \
                ESP_UTILS_LOGE(fmt, ##__VA_ARGS__); \
                return;                          \
            }                                    \
        } while(0)

/**
 * @brief Check if the value is false; if false, log an error and return the specified value.
 *
 * @param x Value to check
 * @param ret Value to return if the value is false
 * @param fmt Format string for the error message
 * @param ... Additional arguments for the format string
 */
#define ESP_UTILS_CHECK_FALSE_RETURN(x, ret, fmt, ...) do { \
            if (unlikely((x) == false)) {                          \
                ESP_UTILS_LOGE(fmt, ##__VA_ARGS__);         \
                return ret;                              \
            }                                            \
        } while(0)

/**
 * @brief Check if the value is false; if false, log an error and goto the specified label.
 *
 * @param x Value to check
 * @param goto_tag Label to jump to if the value is false
 * @param fmt Format string for the error message
 * @param ... Additional arguments for the format string
 */
#define ESP_UTILS_CHECK_FALSE_GOTO(x, goto_tag, fmt, ...) do { \
            if (unlikely((x) == false)) {                   \
                ESP_UTILS_LOGE(fmt, ##__VA_ARGS__);            \
                goto goto_tag;                              \
            }                                               \
        } while(0)

/**
 * @brief Check if the value is false; if false, log an error and return without a value.
 *
 * @param x Value to check
 * @param fmt Format string for the error message
 * @param ... Additional arguments for the format string
 */
#define ESP_UTILS_CHECK_FALSE_EXIT(x, fmt, ...) do { \
            if (unlikely((x) == false)) {                   \
                ESP_UTILS_LOGE(fmt, ##__VA_ARGS__);  \
                return;                           \
            }                                     \
        } while(0)

/**
 * @brief Check if the value is not `ESP_OK`; if not, log an error and return the specified value.
 *
 * @param x Value to check
 * @param ret Value to return if the value is false
 * @param fmt Format string for the error message
 * @param ... Additional arguments for the format string
 */
#define ESP_UTILS_CHECK_ERROR_RETURN(x, ret, fmt, ...) do { \
            esp_err_t err = (x);                        \
            if (unlikely(err != ESP_OK)) {                          \
                ESP_UTILS_LOGE(fmt " [%s]", ##__VA_ARGS__, esp_err_to_name(err)); \
                return ret;                              \
            }                                            \
        } while(0)

/**
 * @brief Check if the value is not `ESP_OK`; if not, log an error and goto the specified label.
 *
 * @param x Value to check
 * @param goto_tag Label to jump to if the value is false
 * @param fmt Format string for the error message
 * @param ... Additional arguments for the format string
 */
#define ESP_UTILS_CHECK_ERROR_GOTO(x, goto_tag, fmt, ...) do { \
            if (unlikely((x) != ESP_OK)) {                   \
                ESP_UTILS_LOGE(fmt, ##__VA_ARGS__);            \
                goto goto_tag;                              \
            }                                               \
        } while(0)

/**
 * @brief Check if the value is not `ESP_OK`; if not, log an error and return without a value.
 *
 * @param x Value to check
 * @param fmt Format string for the error message
 * @param ... Additional arguments for the format string
 */
#define ESP_UTILS_CHECK_ERROR_EXIT(x, fmt, ...) do { \
            if (unlikely((x) != ESP_OK)) {                   \
                ESP_UTILS_LOGE(fmt, ##__VA_ARGS__);  \
                return;                           \
            }                                     \
        } while(0)

#ifdef __cplusplus
/**
 * The `try {} catch {}` block is only available in C++ and `CONFIG_COMPILER_CXX_EXCEPTIONS = 1`
 *
 */
#if CONFIG_COMPILER_CXX_EXCEPTIONS

#define ESP_UTILS_CHECK_EXCEPTION_RETURN(x, ret, fmt, ...) do {\
        try { \
            x; \
        } catch (const std::exception &e) { \
            ESP_UTILS_LOGE("Exception caught: %s", e.what()); \
            ESP_UTILS_LOGE(fmt, ##__VA_ARGS__); \
            return ret; \
        } \
    } while (0)

#define ESP_UTILS_CHECK_EXCEPTION_GOTO(x, goto_tag, fmt, ...) do {\
        try { \
            x; \
        } catch (const std::exception &e) { \
            ESP_UTILS_LOGE("Exception caught: %s", e.what()); \
            ESP_UTILS_LOGE(fmt, ##__VA_ARGS__); \
            goto goto_tag; \
        } \
    } while (0)

#define ESP_UTILS_CHECK_EXCEPTION_EXIT(x, fmt, ...) do {\
        try { \
            x; \
        } catch (const std::exception &e) { \
            ESP_UTILS_LOGE("Exception caught: %s", e.what()); \
            ESP_UTILS_LOGE(fmt, ##__VA_ARGS__); \
            return; \
        } \
    } while (0)

#else

#define ESP_UTILS_CHECK_EXCEPTION_RETURN(x, ret, fmt, ...)      (x)
#define ESP_UTILS_CHECK_EXCEPTION_GOTO(x, goto_tag, fmt, ...)   (x)
#define ESP_UTILS_CHECK_EXCEPTION_EXIT(x, fmt, ...)             (x)

#endif // CONFIG_COMPILER_CXX_EXCEPTIONS
#endif // __cplusplus

#else

/**
 * @brief Check if the pointer is NULL; if NULL, log an error and return the specified value.
 *
 * @param x Pointer to check
 * @param ret Value to return if the pointer is NULL
 * @param fmt Format string for the error message
 * @param ... Additional arguments for the format string
 */
#define ESP_UTILS_CHECK_NULL_RETURN(x, ret, fmt, ...) do { \
            if (unlikely((x) == NULL)) {                          \
                return ret;                             \
            }                                           \
        } while(0)

/**
 * @brief Check if the pointer is NULL; if NULL, log an error and goto the specified label.
 *
 * @param x Pointer to check
 * @param goto_tag Label to jump to if the pointer is NULL
 * @param fmt Format string for the error message
 * @param ... Additional arguments for the format string
 */
#define ESP_UTILS_CHECK_NULL_GOTO(x, goto_tag, fmt, ...) do { \
            if (unlikely((x) == NULL)) {                             \
                goto goto_tag;                             \
            }                                              \
        } while(0)

/**
 * @brief Check if the pointer is NULL; if NULL, log an error and return without a value.
 *
 * @param x Pointer to check
 * @param fmt Format string for the error message
 * @param ... Additional arguments for the format string
 */
#define ESP_UTILS_CHECK_NULL_EXIT(x, fmt, ...) do { \
            if (unlikely((x) == NULL)) {                   \
                return;                          \
            }                                    \
        } while(0)

/**
 * @brief Check if the value is false; if false, log an error and return the specified value.
 *
 * @param x Value to check
 * @param ret Value to return if the value is false
 * @param fmt Format string for the error message
 * @param ... Additional arguments for the format string
 */
#define ESP_UTILS_CHECK_FALSE_RETURN(x, ret, fmt, ...) do { \
            if (unlikely((x) == false)) {                          \
                return ret;                              \
            }                                            \
        } while(0)

/**
 * @brief Check if the value is false; if false, log an error and goto the specified label.
 *
 * @param x Value to check
 * @param goto_tag Label to jump to if the value is false
 * @param fmt Format string for the error message
 * @param ... Additional arguments for the format string
 */
#define ESP_UTILS_CHECK_FALSE_GOTO(x, goto_tag, fmt, ...) do { \
            if (unlikely((x) == false)) {                   \
                goto goto_tag;                              \
            }                                               \
        } while(0)

/**
 * @brief Check if the value is false; if false, log an error and return without a value.
 *
 * @param x Value to check
 * @param fmt Format string for the error message
 * @param ... Additional arguments for the format string
 */
#define ESP_UTILS_CHECK_FALSE_EXIT(x, fmt, ...) do { \
            if (unlikely((x) == false)) {                   \
                return;                           \
            }                                     \
        } while(0)

/**
 * @brief Check if the value is not `ESP_OK`; if not, log an error and return the specified value.
 *
 * @param x Value to check
 * @param ret Value to return if the value is false
 * @param fmt Format string for the error message
 * @param ... Additional arguments for the format string
 */
#define ESP_UTILS_CHECK_ERROR_RETURN(x, ret, fmt, ...) do { \
            esp_err_t err = (x);                        \
            if (unlikely(err != ESP_OK)) {                          \
                return ret;                              \
            }                                            \
        } while(0)

/**
 * @brief Check if the value is not `ESP_OK`; if not, log an error and goto the specified label.
 *
 * @param x Value to check
 * @param goto_tag Label to jump to if the value is false
 * @param fmt Format string for the error message
 * @param ... Additional arguments for the format string
 */
#define ESP_UTILS_CHECK_ERROR_GOTO(x, goto_tag, fmt, ...) do { \
            if (unlikely((x) != ESP_OK)) {                   \
                goto goto_tag;                              \
            }                                               \
        } while(0)

/**
 * @brief Check if the value is not `ESP_OK`; if not, log an error and return without a value.
 *
 * @param x Value to check
 * @param fmt Format string for the error message
 * @param ... Additional arguments for the format string
 */
#define ESP_UTILS_CHECK_ERROR_EXIT(x, fmt, ...) do { \
            if (unlikely((x) != ESP_OK)) {                   \
                return;                           \
            }                                     \
        } while(0)

#ifdef __cplusplus
#define ESP_UTILS_CHECK_EXCEPTION_RETURN(x, ret, fmt, ...) do {\
        try { \
            x; \
        } catch (const std::exception &e) { \
            return ret; \
        } \
    } while (0)

#define ESP_UTILS_CHECK_EXCEPTION_GOTO(x, goto_tag, fmt, ...) do {\
        try { \
            x; \
        } catch (const std::exception &e) { \
            goto goto_tag; \
        } \
    } while (0)

#define ESP_UTILS_CHECK_EXCEPTION_EXIT(x, fmt, ...) do {\
        try { \
            x; \
        } catch (const std::exception &e) { \
            return; \
        } \
    } while (0)
#endif // __cplusplus

#endif // ESP_UTILS_CHECK_WITH_ERROR_LOG
#endif // ESP_UTILS_CHECK_WITH_ASSERT
#endif // ESP_UTILS_ENABLE_CHECK
