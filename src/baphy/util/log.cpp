#include "baphy/util/log.hpp"

#include "baphy/core/event_bus.hpp"
#include "baphy/util/io.hpp"
#include "baphy/util/time.hpp"

namespace baphy {

void MsgSink::sink_it_(const spdlog::details::log_msg &msg) {
  spdlog::memory_buf_t formatted;
  spdlog::sinks::base_sink<spdlog::details::null_mutex>::formatter_->format(msg, formatted);

  EventBus::send<ELogMsg>(fmt::to_string(formatted), msg.level);
}

std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink() {
  static auto sink = std::invoke([] {
    auto s = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    s->set_color_mode(spdlog::color_mode::always);

    return s;
  });

  return sink;
}

std::shared_ptr<spdlog::sinks::basic_file_sink_mt> file_sink() {
  static auto sink = std::invoke([] {
    auto filename = fmt::format("{}.log", baphy::timestamp());
    auto path = (DATA_FOLDER / "log" / filename).string();

    auto s = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path);

    return s;
  });

  return sink;
}

std::shared_ptr<MsgSink> msg_sink() {
  static auto sink = std::invoke([] {
    auto s = std::make_shared<MsgSink>();
    s->set_pattern("[%H:%M:%S %t %L] %v");

    return s;
  });

  return sink;
}

std::shared_ptr<spdlog::logger> logger() {
  static auto logger = std::invoke([] {
    auto l = std::make_shared<spdlog::logger>(
        "",
        spdlog::sinks_init_list{
            console_sink(),
            file_sink(),
            msg_sink()
        }
    );
    l->set_level(spdlog::level::trace);

    return l;
  });

  return logger;
}

} // namespace baphy
