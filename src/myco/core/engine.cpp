#include "myco/core/engine.hpp"

namespace myco {

Engine::Engine() {}

Engine::~Engine() {
  auto init_prio = Scheduler::get_prio<Initialize>();
  for (const auto &name: init_prio | std::views::reverse)
    modules_.erase(name);

  MYCO_LOG_DEBUG("Engine destroyed");
}

} // namespace myco
