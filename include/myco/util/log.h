#ifndef MYCO_UTIL_LOG_H
#define MYCO_UTIL_LOG_H

#if !defined(SPDLOG_ACTIVE_LEVEL)
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#endif

#include "fmt/ranges.h"  // Allow logging ranges (vector, etc.)
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <memory>

namespace myco {

std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink();
std::shared_ptr<spdlog::sinks::basic_file_sink_mt> file_sink();

std::shared_ptr<spdlog::logger> logger();

} // namespace myco

#define MYCO_LOG_TRACE(...)    SPDLOG_LOGGER_TRACE(myco::logger(), __VA_ARGS__)
#define MYCO_LOG_DEBUG(...)    SPDLOG_LOGGER_DEBUG(myco::logger(), __VA_ARGS__)
#define MYCO_LOG_INFO(...)     SPDLOG_LOGGER_INFO(myco::logger(), __VA_ARGS__)
#define MYCO_LOG_WARN(...)     SPDLOG_LOGGER_WARN(myco::logger(), __VA_ARGS__)
#define MYCO_LOG_ERROR(...)    SPDLOG_LOGGER_ERROR(myco::logger(), __VA_ARGS__)
#define MYCO_LOG_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(myco::logger(), __VA_ARGS__)

#endif//MYCO_UTIL_LOG_H
