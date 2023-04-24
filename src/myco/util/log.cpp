#include "myco/util/log.h"

#include "myco/util/io.h"
#include "myco/util/time.h"

namespace myco {

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
        auto filename = fmt::format("{}.log", myco::timestamp());
        auto path = (DATA / "log" / filename).string();

        auto s = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path);

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
                        file_sink()
                }
        );
        l->set_level(spdlog::level::trace);

        return l;
    });

    return logger;
}

} // namespace myco
