#pragma once

#define SPDLOG_HEADER_ONLY
#if !defined(SPDLOG_ACTIVE_LEVEL)
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#endif

#include "fmt/ranges.h"  // Allow logging ranges (vector, etc.)
#include "spdlog/spdlog.h"
#include "spdlog/sinks/ansicolor_sink.h"
#include <memory>

namespace myco {

std::shared_ptr<spdlog::logger> logger();

} // namespace myco

#define MYCO_LOG_TRACE(...)    SPDLOG_LOGGER_TRACE(myco::logger(), __VA_ARGS__)
#define MYCO_LOG_DEBUG(...)    SPDLOG_LOGGER_DEBUG(myco::logger(), __VA_ARGS__)
#define MYCO_LOG_INFO(...)     SPDLOG_LOGGER_INFO(myco::logger(), __VA_ARGS__)
#define MYCO_LOG_WARN(...)     SPDLOG_LOGGER_WARN(myco::logger(), __VA_ARGS__)
#define MYCO_LOG_ERROR(...)    SPDLOG_LOGGER_ERROR(myco::logger(), __VA_ARGS__)
#define MYCO_LOG_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(myco::logger(), __VA_ARGS__)
