/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>
#include "esp_utils_time_profiler.hpp"

namespace esp_utils {

TimeProfiler &TimeProfiler::get_instance()
{
    static TimeProfiler inst;
    return inst;
}

TimeProfiler::TimeProfiler() : root_("root") {}

void TimeProfiler::set_format_options(const FormatOptions &options)
{
    std::lock_guard<std::mutex> lock(mutex_);
    format_ = options;
}

void TimeProfiler::enter_scope(const std::string &name)
{
    auto &local_stack = thread_local_stack();
    Node *parent = local_stack.empty() ? &root_ : local_stack.top();

    std::lock_guard<std::mutex> lock(mutex_);
    auto &child = parent->children[name];
    if (!child) {
        child = std::make_unique<Node>(name, parent);
    }
    local_stack.push(child.get());
    start_times()[local_stack.top()] = Clock::now();
}

void TimeProfiler::leave_scope()
{
    auto &local_stack = thread_local_stack();
    if (local_stack.empty()) {
        return;
    }
    Node *node = local_stack.top();
    local_stack.pop();

    auto end = Clock::now();
    auto start_it = start_times().find(node);
    if (start_it != start_times().end()) {
        double duration = to_unit(end - start_it->second);
        start_times().erase(start_it);
        std::lock_guard<std::mutex> lock(mutex_);
        node->total += duration;
        if (duration < node->min) {
            node->min = duration;
        }
        if (duration > node->max) {
            node->max = duration;
        }
        node->count++;
    }
}

void TimeProfiler::start_event(const std::string &name)
{
    std::lock_guard<std::mutex> lock(mutex_);
    event_stacks_[name].push_back(Clock::now());
}

void TimeProfiler::end_event(const std::string &name)
{
    TimePoint end = Clock::now();
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = event_stacks_.find(name);
    if (it == event_stacks_.end() || it->second.empty()) {
        return;
    }
    TimePoint start = it->second.back();
    it->second.pop_back();
    if (it->second.empty()) {
        event_stacks_.erase(it);
    }
    double duration = to_unit(end - start);

    Node *node = find_or_create_node(name);
    node->total += duration;
    if (duration < node->min) {
        node->min = duration;
    }
    if (duration > node->max) {
        node->max = duration;
    }
    node->count++;
}

void TimeProfiler::report()
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::cout << "\n=== Performance Tree Report ===\n";
    std::cout << "(Unit: " << unit_name() << ")\n";

    const double overall_total = sum_children_total(&root_);
    print_header();

    auto children = sorted_children(&root_);
    for (size_t i = 0; i < children.size(); ++i) {
        Node *child = children[i];
        const bool is_last = (i + 1 == children.size());
        print_node(child, "", is_last, /*parent_total=*/overall_total, /*overall_total=*/overall_total);
    }
    std::cout << "===============================\n";
}

void TimeProfiler::clear()
{
    std::lock_guard<std::mutex> lock(mutex_);
    root_.children.clear();
    event_stacks_.clear();
}

std::stack<TimeProfiler::Node *> &TimeProfiler::thread_local_stack()
{
    static thread_local std::stack<Node *> stack;
    return stack;
}

std::unordered_map<TimeProfiler::Node *, TimeProfiler::TimePoint> &TimeProfiler::start_times()
{
    static thread_local std::unordered_map<Node *, TimePoint> times;
    return times;
}

TimeProfiler::Node *TimeProfiler::find_or_create_node(const std::string &name)
{
    auto &child = root_.children[name];
    if (!child) {
        child = std::make_unique<Node>(name, &root_);
    }
    return child.get();
}

double TimeProfiler::to_unit(std::chrono::duration<double> d) const
{
    switch (format_.time_unit) {
    case FormatOptions::TimeUnit::Microseconds: return d.count() * 1000000.0;
    case FormatOptions::TimeUnit::Milliseconds: return d.count() * 1000.0;
    case FormatOptions::TimeUnit::Seconds: return d.count();
    }
    return d.count();
}

std::string TimeProfiler::unit_name() const
{
    switch (format_.time_unit) {
    case FormatOptions::TimeUnit::Microseconds: return "us";
    case FormatOptions::TimeUnit::Milliseconds: return "ms";
    case FormatOptions::TimeUnit::Seconds: return "s";
    }
    return "unknown";
}

void TimeProfiler::print_node(Node *node,
                              const std::string &prefix,
                              bool is_last,
                              double parent_total,
                              double overall_total) const
{
    const double avg = node->count > 0 ? node->total / static_cast<double>(node->count) : 0.0;
    const double minv = node->count > 0 ? node->min : 0.0;
    const double maxv = node->count > 0 ? node->max : 0.0;
    const double children_sum = sum_children_total(node);
    const double self_time = node->total - children_sum;
    const double pct_parent = parent_total > 0.0 ? (node->total * 100.0 / parent_total) : 0.0;
    const double pct_total = overall_total > 0.0 ? (node->total * 100.0 / overall_total) : 0.0;

    const char *branch_mid = format_.use_unicode ? "├─ " : "|- ";
    const char *branch_end = format_.use_unicode ? "└─ " : "`- ";
    const char *pad_mid = format_.use_unicode ? "│  " : "|  ";
    const char *pad_end = "   ";
    const std::string connector = std::string(is_last ? branch_end : branch_mid);
    const std::string next_prefix = prefix + (is_last ? pad_end : pad_mid);

    std::cout.setf(std::ios::fixed);
    std::cout << std::setprecision(format_.precision);
    const std::string display_prefix = prefix + connector;
    const char *color_reset = "\033[0m";
    const char *color_start = "";
    if (format_.use_color) {
        if (pct_total >= 50.0) {
            color_start = "\033[31m";    // red
        } else if (pct_total >= 20.0) {
            color_start = "\033[33m";    // yellow
        } else if (pct_total >= 5.0) {
            color_start = "\033[36m";    // cyan
        }
    }
    const bool colorize = format_.use_color && *color_start != '\0';
    const std::string name_colored = colorize ? (std::string(color_start) + node->name + color_reset) : node->name;
    int occupy = static_cast<int>(display_prefix.size() + node->name.size());
    int pad = format_.name_width - occupy;
    if (pad < 1) {
        pad = 1;
    }

    std::cout << display_prefix
              << name_colored
              << std::string(pad, ' ')
              << " | " << std::setw(format_.calls_width) << node->count
              << " | " << std::setw(format_.num_width) << node->total
              << " | " << std::setw(format_.num_width) << self_time
              << " | " << std::setw(format_.num_width) << avg
              << " | " << std::setw(format_.num_width) << minv
              << " | " << std::setw(format_.num_width) << maxv;

    if (format_.show_percentages) {
        std::ostringstream p1, p2;
        p1.setf(std::ios::fixed); p1 << std::setprecision(2) << pct_parent << '%';
        p2.setf(std::ios::fixed); p2 << std::setprecision(2) << pct_total  << '%';
        std::cout << " | " << std::setw(format_.percent_width) << p1.str()
                  << " | " << std::setw(format_.percent_width) << p2.str();
    }
    std::cout << "\n";

    auto children = sorted_children(node);
    for (size_t i = 0; i < children.size(); ++i) {
        Node *child = children[i];
        const bool child_is_last = (i + 1 == children.size());
        print_node(child, next_prefix, child_is_last, /*parent_total=*/node->total, overall_total);
    }
}

void TimeProfiler::print_header() const
{
    std::ostringstream oss;
    oss.setf(std::ios::fixed);
    oss << std::left << std::setw(format_.name_width) << "Name"
        << " | " << std::setw(format_.calls_width) << "calls"
        << " | " << std::setw(format_.num_width) << "total"
        << " | " << std::setw(format_.num_width) << "self"
        << " | " << std::setw(format_.num_width) << "avg"
        << " | " << std::setw(format_.num_width) << "min"
        << " | " << std::setw(format_.num_width) << "max";
    if (format_.show_percentages) {
        oss << " | " << std::setw(format_.percent_width) << "%parent"
            << " | " << std::setw(format_.percent_width) << "%total";
    }
    std::cout << oss.str() << "\n";
    std::cout << std::string(oss.str().size(), '-') << "\n";
}

std::string TimeProfiler::format_percent(double pct)
{
    std::ostringstream os;
    os.setf(std::ios::fixed);
    os << std::setprecision(2) << pct << '%';
    return os.str();
}

double TimeProfiler::sum_children_total(const Node *node) const
{
    double sum = 0.0;
    for (const auto &kv : node->children) {
        if (kv.second) {
            sum += kv.second->total;
        }
    }
    return sum;
}

std::vector<TimeProfiler::Node *> TimeProfiler::sorted_children(Node *node) const
{
    std::vector<Node *> result;
    result.reserve(node->children.size());
    for (auto &kv : node->children) {
        if (kv.second) {
            result.push_back(kv.second.get());
        }
    }
    switch (format_.sort_by) {
    case FormatOptions::SortBy::TotalDesc:
        std::sort(result.begin(), result.end(), [](const Node * a, const Node * b) {
            return a->total > b->total;
        });
        break;
    case FormatOptions::SortBy::NameAsc:
        std::sort(result.begin(), result.end(), [](const Node * a, const Node * b) {
            return a->name < b->name;
        });
        break;
    case FormatOptions::SortBy::None:
        break;
    }
    return result;
}

TimeProfilerScope::TimeProfilerScope(const std::string &name)
{
    TimeProfiler::get_instance().enter_scope(name);
}
TimeProfilerScope::~TimeProfilerScope()
{
    TimeProfiler::get_instance().leave_scope();
}

} // namespace esp_utils
