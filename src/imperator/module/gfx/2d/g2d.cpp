#include "imperator/module/gfx/2d/g2d.h"

namespace imp {
G2D::G2D(ModuleMgr &module_mgr)
    : Module(module_mgr) { ctx = module_mgr_.get<GfxContext>(); }

void G2D::clear(const Color &c, const ClearBit &bits) {
    auto gl_color = c.gl_color();
    ctx->gl.ClearColor(gl_color.r, gl_color.g, gl_color.b, gl_color.a);
    ctx->gl.Clear(unwrap(bits));
}
} // namespace imp
