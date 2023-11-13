#ifndef BAPHY_UTIL_LOG_HPP
#define BAPHY_UTIL_LOG_HPP

#if !defined(SPDLOG_ACTIVE_LEVEL)
#if defined(NDEBUG)
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif
#endif

#include "fmt/ranges.h"  // Allow logging ranges (vector, etc.)
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <memory>

namespace baphy {

class MsgSink : public spdlog::sinks::base_sink<spdlog::details::null_mutex> {
protected:
  void sink_it_(const spdlog::details::log_msg& msg) override;
  void flush_() override { /* do nothing */ }
};

std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink();
std::shared_ptr<spdlog::sinks::basic_file_sink_mt> file_sink();
std::shared_ptr<MsgSink> msg_sink();

std::shared_ptr<spdlog::logger> logger();

} // namespace baphy

#define BAPHY_LOG_TRACE(...)    SPDLOG_LOGGER_TRACE(baphy::logger(), __VA_ARGS__)
#define BAPHY_LOG_DEBUG(...)    SPDLOG_LOGGER_DEBUG(baphy::logger(), __VA_ARGS__)
#define BAPHY_LOG_INFO(...)     SPDLOG_LOGGER_INFO(baphy::logger(), __VA_ARGS__)
#define BAPHY_LOG_WARN(...)     SPDLOG_LOGGER_WARN(baphy::logger(), __VA_ARGS__)
#define BAPHY_LOG_ERROR(...)    SPDLOG_LOGGER_ERROR(baphy::logger(), __VA_ARGS__)
#define BAPHY_LOG_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(baphy::logger(), __VA_ARGS__)

#endif//BAPHY_UTIL_LOG_HPP
