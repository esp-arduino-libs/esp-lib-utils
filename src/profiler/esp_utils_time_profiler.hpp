/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <chrono>
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <stack>
#include <vector>
#include <limits>

namespace esp_utils {

class TimeProfiler {
public:
    struct Node {
        std::string name;
        double total = 0.0;
        size_t count = 0;
        double min = std::numeric_limits<double>::infinity();
        double max = 0.0;
        std::unordered_map<std::string, std::unique_ptr<Node>> children;
        Node *parent = nullptr;

        Node(const std::string &n, Node *p = nullptr) : name(n), parent(p) {}
    };

    struct FormatOptions {
        enum class SortBy {
            TotalDesc,
            NameAsc,
            None
        };
        enum class TimeUnit {
            Microseconds,
            Milliseconds,
            Seconds
        };

        int name_width = 22;
        int calls_width = 6;
        int num_width = 10;
        int percent_width = 7;
        int precision = 2;
        bool use_unicode = true;
        bool show_percentages = true;
        bool use_color = false;
        SortBy sort_by = SortBy::TotalDesc;
        TimeUnit time_unit = TimeUnit::Milliseconds;
    };

    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    static TimeProfiler &get_instance();

    // ---------- 配置 ----------
    void set_format_options(const FormatOptions &options);

    // ---------- RAII Scope ----------
    void enter_scope(const std::string &name);

    void leave_scope();

    // ---------- Cross-thread Events ----------
    void start_event(const std::string &name);

    void end_event(const std::string &name);

    // ---------- Output ----------
    void report();

    void clear();

private:
    TimeProfiler();

    static std::stack<Node *> &thread_local_stack();

    static std::unordered_map<Node *, TimePoint> &start_times();

    Node *find_or_create_node(const std::string &name);

    double to_unit(std::chrono::duration<double> d) const;

    std::string unit_name() const;

    void print_node(Node *node,
                    const std::string &prefix,
                    bool is_last,
                    double parent_total,
                    double overall_total) const;

    void print_header() const;

    mutable std::mutex mutex_;
    Node root_;

    std::unordered_map<std::string, std::vector<TimePoint>> event_stacks_;
    FormatOptions format_{};

    static std::string format_percent(double pct);

    double sum_children_total(const Node *node) const;

    std::vector<Node *> sorted_children(Node *node) const;
};

class TimeProfilerScope {
public:
    explicit TimeProfilerScope(const std::string &name);
    ~TimeProfilerScope();
};

} // namespace esp_utils

#define ESP_UTILS_TIME_PROFILER_SCOPE(name) esp_utils::TimeProfilerScope _profile_scope_##__LINE__(name)

#define ESP_UTILS_TIME_PROFILER_START_EVENT(name) esp_utils::TimeProfiler::get_instance().start_event(name)

#define ESP_UTILS_TIME_PROFILER_END_EVENT(name) esp_utils::TimeProfiler::get_instance().end_event(name)
