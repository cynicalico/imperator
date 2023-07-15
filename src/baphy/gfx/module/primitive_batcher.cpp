#include "baphy/gfx/module/primitive_batcher.hpp"

#include "baphy/core/module/application.hpp"

namespace baphy {

void PrimitiveBatcher::tri(
    float x0, float y0,
    float x1, float y1,
    float x2, float y2,
    float rx, float ry, float angle,
    const RGB &color
) {
  auto cv = color.vec4();
  std::initializer_list<float> es = {
      x0, y0, z_,  cv.r, cv.g, cv.b, cv.a,  rx, ry, -glm::radians(angle),
      x1, y1, z_,  cv.r, cv.g, cv.b, cv.a,  rx, ry, -glm::radians(angle),
      x2, y2, z_,  cv.r, cv.g, cv.b, cv.a,  rx, ry, -glm::radians(angle),
  };

  if (color.a < 255) t_primitive_batcher_->add_tri(es);
  else               o_primitive_batcher_->add_tri(es);

  z_ += 1.0f;
}

void PrimitiveBatcher::tri(float x0, float y0, float x1, float y1, float x2, float y2, float angle, const RGB &color) {
  tri(x0, y0, x1, y1, x2, y2, (x0 + x1 + x2) / 3, (y0 + y1 + y2) / 3, angle, color);
}

void PrimitiveBatcher::tri(float x0, float y0, float x1, float y1, float x2, float y2, const RGB &color) {
  tri(x0, y0, x1, y1, x2, y2, 0, 0, 0, color);
}

void PrimitiveBatcher::tri_equilateral(float cx, float cy, float radius, float rx, float ry, float angle, const RGB &color) {
  float x0 = cx + radius * std::cos(0);
  float y0 = cy + radius * std::sin(0);
  float x1 = cx + radius * std::cos(2.0944);
  float y1 = cy + radius * std::sin(2.0944);
  float x2 = cx + radius * std::cos(4.18879);
  float y2 = cy + radius * std::sin(4.18879);
  tri(x0, y0, x1, y1, x2, y2, rx, ry, angle, color);
}

void PrimitiveBatcher::tri_equilateral(float cx, float cy, float radius, float angle, const RGB &color) {
  tri_equilateral(cx, cy, radius, cx, cy, angle, color);
}

void PrimitiveBatcher::tri_equilateral(float cx, float cy, float radius, const RGB &color) {
  tri_equilateral(cx, cy, radius, 0, 0, 0, color);
}

void PrimitiveBatcher::e_initialize_(const baphy::EInitialize &e) {
  gfx = module_mgr->get<GfxContext>();

  o_primitive_batcher_ = std::make_unique<OpaquePrimitiveBatcher>(*module_mgr);
  t_primitive_batcher_ = std::make_unique<TransPrimitiveBatcher>(*module_mgr);

  EventBus::sub<EDraw>(module_name, {EPI<Application>::name}, [&](const auto &e) { e_draw_(e); });

  Module::e_initialize_(e);
}

void PrimitiveBatcher::e_shutdown_(const baphy::EShutdown &e) {
  Module::e_shutdown_(e);
}

void PrimitiveBatcher::e_draw_(const EDraw &e) {
  o_primitive_batcher_->draw(gfx->ortho_projection(), z_);

  gfx->blend_func_separate(BlendFunc::one, BlendFunc::one_minus_src_alpha, BlendFunc::one_minus_dst_alpha, BlendFunc::one);
  gfx->enable(Capability::blend);
  gfx->depth_mask(false);

  t_primitive_batcher_->draw(gfx->ortho_projection(), z_);

  gfx->depth_mask(true);
  gfx->disable(Capability::blend);

  z_ = 1.0f;
}

} // namespace baphy
