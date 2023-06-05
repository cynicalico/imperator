#include "baphy/core/engine.hpp"

namespace baphy {

Engine::Engine() {
  module_mgr_ = std::make_shared<ModuleMgr>();
}

} // namespace baphy
