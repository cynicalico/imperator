#include "myco/util/log.hpp"

namespace myco {

std::shared_ptr<spdlog::logger> logger() {
  static auto logger = std::invoke([] {
    auto console_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
    console_sink->set_color_mode(spdlog::color_mode::always);

    auto l = std::make_shared<spdlog::logger>("", console_sink);
    l->set_level(spdlog::level::trace);

    return l;
  });

  return logger;
}

} // namespace myco
