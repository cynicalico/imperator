#include "imp/gfx/module/gfx_2d.hpp"

namespace imp {
std::once_flag Gfx2D::created_required_modules_;

Gfx2D::Gfx2D(const std::weak_ptr<ModuleMgr>& module_mgr): Module(module_mgr) {
  std::call_once(created_required_modules_, [&] {
    module_mgr.lock()->create<Batcher>();
  });

  batcher = module_mgr.lock()->get<Batcher>();
  ctx = module_mgr.lock()->get<GfxContext>();
}

void Gfx2D::clear(const Color& color, const ClearBit& mask) {
  const auto gl_color = color.gl_color();
  ctx->gl.ClearColor(gl_color.r, gl_color.g, gl_color.b, gl_color.a);
  ctx->gl.Clear(unwrap(mask));
}
} // namespace imp
