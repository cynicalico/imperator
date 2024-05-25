#ifndef IMPERATOR_MODULE_GFX_2D_G2D_H
#define IMPERATOR_MODULE_GFX_2D_G2D_H

#include "imperator/core/color.h"
#include "imperator/module/module_mgr.h"
#include "imperator/gl/enum_types.h"
#include "imperator/module/gfx/gfx_context.h"

namespace imp {
class G2D : public Module<G2D> {
public:
  std::shared_ptr<GfxContext> ctx;

  G2D(ModuleMgr &module_mgr);

  void clear(const Color &c, const ClearBit &bits = ClearBit::color | ClearBit::depth);
};
} // namespace imp

IMPERATOR_DECLARE_MODULE(imp::G2D);

#endif//IMPERATOR_MODULE_GFX_2D_G2D_H
