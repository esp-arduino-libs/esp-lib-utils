/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// *INDENT-OFF*

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////// Check Configurations /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Check functions related.
 *
 */
#ifndef ESP_UTILS_ENABLE_CHECK
    #ifdef CONFIG_ESP_UTILS_ENABLE_CHECK
        #define ESP_UTILS_ENABLE_CHECK   CONFIG_ESP_UTILS_ENABLE_CHECK
    #else
        #define ESP_UTILS_ENABLE_CHECK   (0)
    #endif
#endif
#ifndef ESP_UTILS_CHECK_WITH_ERROR_LOG
    #ifdef CONFIG_ESP_UTILS_CHECK_WITH_ERROR_LOG
        #define ESP_UTILS_CHECK_WITH_ERROR_LOG   CONFIG_ESP_UTILS_CHECK_WITH_ERROR_LOG
    #else
        #define ESP_UTILS_CHECK_WITH_ERROR_LOG   (0)
    #endif
#endif
#ifndef ESP_UTILS_CHECK_WITH_ASSERT
    #ifdef CONFIG_ESP_UTILS_CHECK_WITH_ASSERT
        #define ESP_UTILS_CHECK_WITH_ASSERT   CONFIG_ESP_UTILS_CHECK_WITH_ASSERT
    #else
        #define ESP_UTILS_CHECK_WITH_ASSERT   (0)
    #endif
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////// LOG Configurations //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ESP_UTILS_ENABLE_LOG
    #ifdef CONFIG_ESP_UTILS_ENABLE_LOG
        #define ESP_UTILS_ENABLE_LOG            CONFIG_ESP_UTILS_ENABLE_LOG
    #else
        #define ESP_UTILS_ENABLE_LOG            (0)
    #endif
#endif

#ifndef ESP_UTILS_LOG_BUFFER_SIZE
    #ifdef CONFIG_ESP_UTILS_LOG_BUFFER_SIZE
        #define ESP_UTILS_LOG_BUFFER_SIZE       CONFIG_ESP_UTILS_LOG_BUFFER_SIZE
    #else
        #define ESP_UTILS_LOG_BUFFER_SIZE       (256)
    #endif
#endif

#ifndef ESP_UTILS_LOG_GLOBAL_LEVEL
    #ifdef CONFIG_ESP_UTILS_LOG_GLOBAL_LEVEL
        #define ESP_UTILS_LOG_GLOBAL_LEVEL      CONFIG_ESP_UTILS_LOG_GLOBAL_LEVEL
    #else
        #define ESP_UTILS_LOG_GLOBAL_LEVEL      ESP_UTILS_LOG_LEVEL_INFO
    #endif
#endif

#ifndef ESP_UTILS_ENABLE_LOG_TRACE
    #ifdef CONFIG_ESP_UTILS_ENABLE_LOG_TRACE
        #define ESP_UTILS_ENABLE_LOG_TRACE      CONFIG_ESP_UTILS_ENABLE_LOG_TRACE
    #else
        #define ESP_UTILS_ENABLE_LOG_TRACE      0
    #endif
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////// Memory Configurations /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ESP_UTILS_MEM_GENERAL_ALLOC_TYPE
    #ifdef CONFIG_ESP_UTILS_MEM_GENERAL_ALLOC_TYPE
        #define ESP_UTILS_MEM_GENERAL_ALLOC_TYPE        CONFIG_ESP_UTILS_MEM_GENERAL_ALLOC_TYPE
    #else
        #define ESP_UTILS_MEM_GENERAL_ALLOC_TYPE        (ESP_UTILS_MEM_GENERAL_ALLOC_TYPE_STDLIB)
    #endif
#endif


#if ESP_UTILS_MEM_GENERAL_ALLOC_TYPE == ESP_UTILS_MEM_ALLOC_TYPE_ESP
    #ifndef ESP_UTILS_MEM_GENERAL_ALLOC_ESP_CAPS
        #ifdef CONFIG_ESP_UTILS_MEM_GENERAL_ALLOC_ESP_CAPS
            #define ESP_UTILS_MEM_GENERAL_ALLOC_ESP_CAPS    CONFIG_ESP_UTILS_MEM_GENERAL_ALLOC_ESP_CAPS
        #else
            #error "`ESP_UTILS_MEM_GENERAL_ALLOC_ESP_CAPS` must be defined when `ESP_UTILS_MEM_GENERAL_ALLOC_TYPE` is set to `ESP_UTILS_MEM_ALLOC_TYPE_ESP`"
        #endif
    #endif
#elif ESP_UTILS_MEM_GENERAL_ALLOC_TYPE == ESP_UTILS_MEM_ALLOC_TYPE_CUSTOM
    #ifndef ESP_UTILS_MEM_GENERAL_ALLOC_CUSTOM_INCLUDE
        #ifdef CONFIG_ESP_UTILS_MEM_GENERAL_ALLOC_CUSTOM_INCLUDE
            #define ESP_UTILS_MEM_GENERAL_ALLOC_CUSTOM_INCLUDE  CONFIG_ESP_UTILS_MEM_GENERAL_ALLOC_CUSTOM_INCLUDE
        #else
            #define ESP_UTILS_MEM_GENERAL_ALLOC_CUSTOM_INCLUDE "stdlib.h"
        #endif
    #endif

    #ifndef ESP_UTILS_MEM_GENERAL_ALLOC_CUSTOM_MALLOC
        #error "`ESP_UTILS_MEM_GENERAL_ALLOC_CUSTOM_MALLOC` must be defined when `ESP_UTILS_MEM_GENERAL_ALLOC_TYPE` is set to `ESP_UTILS_MEM_GENERAL_ALLOC_TYPE_CUSTOM`"
    #endif

    #ifndef ESP_UTILS_MEM_GENERAL_ALLOC_CUSTOM_FREE
        #error "`ESP_UTILS_MEM_GENERAL_ALLOC_CUSTOM_FREE` must be defined when `ESP_UTILS_MEM_GENERAL_ALLOC_TYPE` is set to `ESP_UTILS_MEM_GENERAL_ALLOC_TYPE_CUSTOM`"
    #endif
#endif