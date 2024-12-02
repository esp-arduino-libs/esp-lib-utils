[![Arduino Lint](https://github.com/esp-arduino-libs/esp-lib-utils/actions/workflows/arduino_lint.yml/badge.svg)](https://github.com/esp-arduino-libs/esp-lib-utils/actions/workflows/arduino_lint.yml) [![pre-commit](https://github.com/esp-arduino-libs/esp-lib-utils/actions/workflows/pre-commit.yml/badge.svg)](https://github.com/esp-arduino-libs/esp-lib-utils/actions/workflows/pre-commit.yml) [![Build Test Apps](https://github.com/esp-arduino-libs/esp-lib-utils/actions/workflows/build_test.yml/badge.svg)](https://github.com/esp-arduino-libs/esp-lib-utils/actions/workflows/build_test.yml) [![Version Consistency](https://github.com/esp-arduino-libs/esp-lib-utils/actions/workflows/check_lib_versions.yml/badge.svg)](https://github.com/esp-arduino-libs/esp-lib-utils/actions/workflows/check_lib_versions.yml)

**Latest Arduino Library Version**: [![GitHub Release](https://img.shields.io/github/v/release/esp-arduino-libs/esp-lib-utils)](https://github.com/esp-arduino-libs/esp-lib-utils/releases)

**Latest Espressif Component Version**: [![Espressif Release](https://components.espressif.com/components/espressif/esp-lib-utils/badge.svg)](https://components.espressif.com/components/espressif/esp-lib-utils)

# ESP Library Utils

esp-lib-utils is a library designed for ESP SoCs to provide utility functions, including `logging`, `memory management`, and `checking`.

## How to Use

```cpp
// Define the log tag for the current library, should be declared before `esp_utils_library.h`
#define ESP_UTILS_LOG_TAG "MyLibrary"
#include "esp_utils_library.h"

void test_log(void)
{
    ESP_UTILS_LOG_TRACE_ENTER();

    ESP_UTILS_LOGD("This is a debug message");
    ESP_UTILS_LOGI("This is an info message");
    ESP_UTILS_LOGW("This is a warning message");
    ESP_UTILS_LOGE("This is an error message");

    ESP_UTILS_LOG_TRACE_EXIT();
}

void test_memory(void)
{
    // Allocate memory in C style (`malloc/calloc` and `free` are re-defined by the library)
    int *c_ptr = (int *)malloc(sizeof(int));
    ESP_UTILS_CHECK_NULL_EXIT(c_ptr, "Failed to allocate memory");
    free(c_ptr);

    // Allocate memory in C++ style
    std::shared_ptr<int> cxx_ptr = nullptr;
    ESP_UTILS_CHECK_EXCEPTION_EXIT(
        (cxx_ptr = esp_utils::make_shared<int>()), "Failed to allocate memory"
    );
    cxx_ptr = nullptr;
}
```
