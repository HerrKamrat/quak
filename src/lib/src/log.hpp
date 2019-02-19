#pragma once

#include <iostream>

#include <fmt/color.h>
#include <fmt/format.h>
#include <fmt/ostream.h>

#include "platform.hpp"

enum class log_level { debug, info, warning, error };

void log(log_level level, const char* msg);

template <typename... Args>
static void log(log_level level, const char* format, const Args&... args) {
    std::string s = fmt::format(format, args...);
    //std::string l = fmt::format("[{}] {}\n", tag, s);
    log(level, s.c_str());
}

template <typename... Args>
static void debug(const char* format, const Args&... args) {
    log(log_level::debug, format, args...);
}


template <typename... Args>
static void info(const char* format, const Args&... args) {
    log(log_level::info, format, args...);
}

template <typename... Args>
static void warn(const char* format, const Args&... args) {
    log(log_level::warning, format, args...);
}

template <typename... Args>
static void error(const char* format, const Args&... args) {
    log(log_level::error, format, args...);
}
