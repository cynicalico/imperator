#ifndef IMP_GFX_MODULE_GFX_2D_HPP
#define IMP_GFX_MODULE_GFX_2D_HPP

#include "imp/core/module_mgr.hpp"
#include "imp/gfx/gl/enum_types.hpp"
#include "imp/gfx/module/batcher.hpp"
#include "imp/gfx/module/gfx_context.hpp"

namespace imp {
class Gfx2D : public Module<Gfx2D> {
public:
  std::shared_ptr<Batcher> batcher{nullptr};
  std::shared_ptr<GfxContext> ctx{nullptr};

  explicit Gfx2D(const std::weak_ptr<ModuleMgr>& module_mgr);

  void clear(const Color& color, const ClearBit& mask = ClearBit::color | ClearBit::depth);

private:
  static std::once_flag created_required_modules_;
};
} // namespace imp

IMP_PRAISE_HERMES(imp::Gfx2D);

#endif//IMP_GFX_MODULE_GFX_2D_HPP
