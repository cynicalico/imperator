#ifndef IMP_GFX_MODULE_GFX_2D_HPP
#define IMP_GFX_MODULE_GFX_2D_HPP

#include "imp/core/module_mgr.hpp"

namespace imp {
class Gfx2D : public Module<Gfx2D> {
public:
  explicit Gfx2D(const std::weak_ptr<ModuleMgr>& module_mgr);

  void say_hello() {
    IMP_LOG_INFO("hi");
  }
};
} // namespace imp

IMP_PRAISE_HERMES(imp::Gfx2D);

#endif//IMP_GFX_MODULE_GFX_2D_HPP
