#ifndef BAPHY_CORE_MSGS_HPP
#define BAPHY_CORE_MSGS_HPP

#include <memory>

namespace baphy {

class ModuleMgr;

struct EInitialize {
  const std::shared_ptr<ModuleMgr> &module_mgr;
};

struct EShutdown {};

} // namespace baphy

#endif //BAPHY_CORE_MSGS_HPP
