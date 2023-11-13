#include "baphy/core/engine.hpp"

namespace baphy {

Engine::Engine() {
  module_mgr_ = std::make_shared<ModuleMgr>();

  Hermes::sub<E_ShutdownEngine>(EPI<Engine>::name, [&](const auto &) {
    received_shutdown_ = true;
  });
}

} // namespace baphy
