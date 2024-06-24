#ifndef IMPERATOR_UTIL_LOG_H
#define IMPERATOR_UTIL_LOG_H

#if !defined(SPDLOG_ACTIVE_LEVEL)
#    if defined(NDEBUG)
#        define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#    else
#        define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#    endif
#endif

#include "fmt/chrono.h" // Allow logging chrono types
#include "fmt/ranges.h" // Allow logging ranges (vector, etc.)
#include "fmt/std.h"    // Allow formatting STL types
#include "fmt/xchar.h"  // Allow logging wchar
#include "spdlog/sinks/dist_sink.h"
#include "spdlog/spdlog.h"

#include <memory>

namespace imp {
std::shared_ptr<spdlog::sinks::dist_sink_mt> sinks();

std::shared_ptr<spdlog::logger> logger();
} // namespace imp

#define IMPERATOR_LOG_TRACE(...)    SPDLOG_LOGGER_TRACE(imp::logger(), __VA_ARGS__)
#define IMPERATOR_LOG_DEBUG(...)    SPDLOG_LOGGER_DEBUG(imp::logger(), __VA_ARGS__)
#define IMPERATOR_LOG_INFO(...)     SPDLOG_LOGGER_INFO(imp::logger(), __VA_ARGS__)
#define IMPERATOR_LOG_WARN(...)     SPDLOG_LOGGER_WARN(imp::logger(), __VA_ARGS__)
#define IMPERATOR_LOG_ERROR(...)    SPDLOG_LOGGER_ERROR(imp::logger(), __VA_ARGS__)
#define IMPERATOR_LOG_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(imp::logger(), __VA_ARGS__)

#endif //IMPERATOR_UTIL_LOG_H
