#include "imperator/util/log.h"

#include "imperator/util/io.h"
#include "imperator/util/platform.h"
#include "imperator/util/time.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace imp {
// class MsgSink : public spdlog::sinks::base_sink<spdlog::details::null_mutex> {
// protected:
//   void sink_it_(const spdlog::details::log_msg& msg) override;
//
//   void flush_() override { /* do nothing */
//   }
// };
//
// void MsgSink::sink_it_(const spdlog::details::log_msg& msg) {
//   spdlog::memory_buf_t formatted;
//   formatter_->format(msg, formatted);
//
//   // Hermes::send<E_LogMsg>(fmt::to_string(formatted), msg.level);
// }
//
// std::shared_ptr<MsgSink> msg_sink() {
//   static auto sink = std::invoke([] {
//     auto s = std::make_shared<MsgSink>();
//     s->set_pattern("[%H:%M:%S %t %L] %v");
//
//     return s;
//   });
//
//   return sink;
// }

std::shared_ptr<spdlog::sinks::dist_sink_mt> sinks() {
    static auto s = std::make_shared<spdlog::sinks::dist_sink_mt>();
    return s;
}

std::shared_ptr<spdlog::logger> logger() {
    static auto logger = std::invoke(
            [] {
                auto s = sinks();

                auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                console_sink->set_color_mode(spdlog::color_mode::always);
                s->add_sink(console_sink);

                auto filename = fmt::format("{}.log", imp::timestamp());
                auto path = (DATA_FOLDER / "log" / filename).string();
                auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path);
                s->add_sink(file_sink);

                auto l = std::make_shared<spdlog::logger>("imperator", s);
                l->set_level(spdlog::level::trace);

#if defined(IMPERATOR_PLATFORM_WINDOWS)
                SetConsoleOutputCP(CP_UTF8);
#endif

                return l;
            }
    );

    return logger;
}
} // namespace imp
