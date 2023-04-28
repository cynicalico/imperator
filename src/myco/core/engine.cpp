#include "myco/core/engine.h"

namespace myco {

Engine::Engine() {
    EventBus::sub<Shutdown>("myco::Engine", [&](const auto &){ received_shutdown_ = true; });
}

Engine::~Engine() {
    auto init_prio = EventBus::get_prio<Initialize>();
    for (const auto &name: init_prio | std::views::reverse)
        modules_.erase(name);

    MYCO_LOG_DEBUG("Engine destroyed");
}

} // namespace myco
