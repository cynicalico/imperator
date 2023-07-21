#ifndef BAPHY_GFX_MODULE_PRIMTIIVE_BATCHER_HPP
#define BAPHY_GFX_MODULE_PRIMTIIVE_BATCHER_HPP

#include "baphy/core/module_mgr.hpp"
#include "baphy/gfx/module/batcher.hpp"

namespace baphy {

class PrimitiveBatcher : public Module<PrimitiveBatcher> {
public:
  std::shared_ptr<GfxContext> gfx{nullptr};
  std::shared_ptr<Batcher> batcher{nullptr};

  PrimitiveBatcher() : Module<PrimitiveBatcher>({EPI<Batcher>::name}) {}
  ~PrimitiveBatcher() override = default;

  void rect(float x, float y, float w, float h, float rx, float ry, float angle, const RGB &color);
  void rect(float x, float y, float w, float h, float angle, const RGB &color);
  void rect(float x, float y, float w, float h, const RGB &color);

  void square(float x, float y, float side_l, float rx, float ry, float angle, const RGB &color);
  void square(float x, float y, float side_l, float angle, const RGB &color);
  void square(float x, float y, float side_l, const RGB &color);

  void tri(float x0, float y0, float x1, float y1, float x2, float y2, float rx, float ry, float angle, const RGB &color);
  void tri(float x0, float y0, float x1, float y1, float x2, float y2, float angle, const RGB &color);
  void tri(float x0, float y0, float x1, float y1, float x2, float y2, const RGB &color);

  void tri_equilateral(float cx, float cy, float radius, float rx, float ry, float angle, const RGB &color);
  void tri_equilateral(float cx, float cy, float radius, float angle, const RGB &color);
  void tri_equilateral(float cx, float cy, float radius, const RGB &color);

  void line(float x0, float y0, float x1, float y1, float rx, float ry, float angle, const RGB &color);
  void line(float x0, float y0, float x1, float y1, float angle, const RGB &color);
  void line(float x0, float y0, float x1, float y1, const RGB &color);

  void point(float x, float y, const RGB &color);

private:
  void e_initialize_(const EInitialize &e) override;
  void e_shutdown_(const EShutdown &e) override;
};

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::PrimitiveBatcher);

#endif//BAPHY_GFX_MODULE_PRIMTIIVE_BATCHER_HPP
