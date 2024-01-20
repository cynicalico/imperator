#ifndef IMP_GFX_MODULE_GFX_2D_HPP
#define IMP_GFX_MODULE_GFX_2D_HPP

#include "imp/core/module_mgr.hpp"

namespace imp {
class Gfx2D : public Module<Gfx2D> {
public:
  Gfx2D() : Module() {}

  void say_hello() {
    IMP_LOG_INFO("hi");
  }

protected:
  void r_initialize_(const E_Initialize& p) override;
  void r_shutdown_(const E_Shutdown& p) override;
};
} // namespace imp

IMP_PRAISE_HERMES(imp::Gfx2D);

#endif//IMP_GFX_MODULE_GFX_2D_HPP
