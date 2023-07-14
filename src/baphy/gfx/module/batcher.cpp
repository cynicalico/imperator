#include "baphy/gfx/module/batcher.hpp"

#include "baphy/core/module/application.hpp"

namespace baphy {

void Batcher::tri(float x0, float y0, float x1, float y1, float x2, float y2, const RGB &color) {
  auto cv = color.vec4();
  std::initializer_list<float> es = {
      x0, y0, z_,  cv.r, cv.g, cv.b, cv.a,  0.0f, 0.0f, 0.0f,
      x1, y1, z_,  cv.r, cv.g, cv.b, cv.a,  0.0f, 0.0f, 0.0f,
      x2, y2, z_,  cv.r, cv.g, cv.b, cv.a,  0.0f, 0.0f, 0.0f,
  };

  if (color.a < 255) t_primitive_batcher_->add_tri(es);
  else               o_primitive_batcher_->add_tri(es);

  z_ += 1.0f;
}

void Batcher::e_initialize_(const baphy::EInitialize &e) {
  gfx = module_mgr->get<GfxContext>();

  o_primitive_batcher_ = std::make_unique<OpaquePrimitiveBatcher>(*module_mgr);
  t_primitive_batcher_ = std::make_unique<TransPrimitiveBatcher>(*module_mgr);

  EventBus::sub<EDraw>(module_name, {EPI<Application>::name}, [&](const auto &e) { e_draw_(e); });

  Module::e_initialize_(e);
}

void Batcher::e_shutdown_(const baphy::EShutdown &e) {
  Module::e_shutdown_(e);
}

void Batcher::e_draw_(const EDraw &e) {
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
