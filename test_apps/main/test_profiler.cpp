/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <thread>
#include <iostream>
#include <vector>
#include "unity.h"
#if defined(ESP_UTILS_LOG_TAG)
#undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "TestProfiler"
#include "esp_lib_utils.h"

using namespace esp_utils;

void do_test(size_t sleep_time)
{
    ESP_UTILS_TIME_PROFILER_SCOPE("do_test");
    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
}

void work_task()
{
    ESP_UTILS_TIME_PROFILER_SCOPE("work_task");
    do_test(12 + rand() % 8);
}

std::thread async_task(int index)
{
    std::string event_name = "cross_thread_event";
    TimeProfiler::get_instance().start_event(event_name);
    return std::thread([event_name, index] {
        ESP_UTILS_TIME_PROFILER_SCOPE("async_task_" + std::to_string(index));
        do_test(40);
        TimeProfiler::get_instance().end_event(event_name);
    });
}

TEST_CASE("Test profiler functions on cpp", "[utils][plugin][CPP]")
{
    auto &prof = TimeProfiler::get_instance();
    TimeProfiler::FormatOptions opt;
    opt.use_unicode = true;
    opt.use_color = true;          // 开启颜色高亮（>50%红色，>20%黄色，>5%青色）
    opt.sort_by = TimeProfiler::FormatOptions::SortBy::TotalDesc;
    opt.show_percentages = true;
    opt.name_width = 40;
    opt.calls_width = 6;
    opt.num_width = 10;
    opt.percent_width = 7;
    opt.precision = 2;
    opt.time_unit = TimeProfiler::FormatOptions::TimeUnit::Milliseconds;
    prof.set_format_options(opt);

    std::vector<std::thread> workers;
    workers.reserve(5);
    for (int i = 0; i < 5; ++i) {
        ESP_UTILS_TIME_PROFILER_SCOPE("main_iteration");
        work_task();
        workers.emplace_back(async_task(i));
    }
    for (auto &t : workers) {
        if (t.joinable()) {
            t.join();
        }
    }
    TimeProfiler::get_instance().report();

    TimeProfiler::get_instance().clear();
}
