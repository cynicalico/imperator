#include "baphy/core/engine.hpp"

namespace baphy {

Engine::Engine() {
  module_mgr_ = std::make_shared<ModuleMgr>();

  EventBus::sub<EShutdownEngine>(EPI<Engine>::name, [&](const auto &) {
    received_shutdown_ = true;
  });
}

} // namespace baphy
