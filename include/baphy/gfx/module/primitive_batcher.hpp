#ifndef BAPHY_GFX_MODULE_PRIMTIIVE_BATCHER_HPP
#define BAPHY_GFX_MODULE_PRIMTIIVE_BATCHER_HPP

#include "baphy/core/module_mgr.hpp"
#include "baphy/gfx/internal/opaque_primitive_batcher.hpp"
#include "baphy/gfx/internal/trans_primitive_batcher.hpp"

namespace baphy {

class PrimitiveBatcher : public Module<PrimitiveBatcher> {
public:
  std::shared_ptr<GfxContext> gfx{nullptr};

  PrimitiveBatcher() : Module<PrimitiveBatcher>({EPI<GfxContext>::name, EPI<ShaderMgr>::name}) {}
  ~PrimitiveBatcher() override = default;

  void tri(float x0, float y0,
           float x1, float y1,
           float x2, float y2,
           float rx, float ry, float angle,
           const RGB &color);
  void tri(float x0, float y0, float x1, float y1, float x2, float y2, float angle, const RGB &color);
  void tri(float x0, float y0, float x1, float y1, float x2, float y2, const RGB &color);

  void tri_equilateral(float cx, float cy, float radius, float rx, float ry, float angle, const RGB &color);
  void tri_equilateral(float cx, float cy, float radius, float angle, const RGB &color);
  void tri_equilateral(float cx, float cy, float radius, const RGB &color);

private:
  float z_{1.0f};

  std::unique_ptr<OpaquePrimitiveBatcher> o_primitive_batcher_{nullptr};
  std::unique_ptr<TransPrimitiveBatcher> t_primitive_batcher_{nullptr};

  void e_initialize_(const EInitialize &e) override;
  void e_shutdown_(const EShutdown &e) override;

  void e_draw_(const EDraw &e);
};

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::PrimitiveBatcher);

#endif//BAPHY_GFX_MODULE_PRIMTIIVE_BATCHER_HPP
