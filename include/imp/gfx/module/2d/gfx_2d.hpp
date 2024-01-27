#ifndef IMP_GFX_MODULE_GFX_2D_HPP
#define IMP_GFX_MODULE_GFX_2D_HPP

#include "imp/core/module_mgr.hpp"
#include "imp/gfx/gl/enum_types.hpp"
#include "imp/gfx/module/2d/batcher.hpp"
#include "imp/gfx/module/gfx_context.hpp"
#include "imp/gfx/module/texture_mgr.hpp"

namespace imp {
class Gfx2D : public Module<Gfx2D> {
public:
  std::shared_ptr<Batcher> batcher{nullptr};
  std::shared_ptr<GfxContext> ctx{nullptr};
  std::shared_ptr<TextureMgr> textures{nullptr};

  explicit Gfx2D(const std::weak_ptr<ModuleMgr>& module_mgr);

  void clear(const Color& color, const ClearBit& mask = ClearBit::color | ClearBit::depth);

  /* PRIMITIVES */
  void point(glm::vec2 xy, const Color& c);

  void line(glm::vec2 p0, glm::vec2 p1, glm::vec2 rcenter, float angle, const Color& c);
  void line(glm::vec2 p0, glm::vec2 p1, float angle, const Color& c);
  void line(glm::vec2 p0, glm::vec2 p1, const Color& c);

  void draw_tri(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 rcenter, float angle, const Color& c);
  void draw_tri(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, float angle, const Color& c);
  void draw_tri(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, const Color& c);

  void fill_tri(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 rcenter, float angle, const Color& c);
  void fill_tri(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, float angle, const Color& c);
  void fill_tri(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, const Color& c);

  void draw_rect(glm::vec2 xy, glm::vec2 size, glm::vec2 rcenter, float angle, const Color& c);
  void draw_rect(glm::vec2 xy, glm::vec2 size, float angle, const Color& c);
  void draw_rect(glm::vec2 xy, glm::vec2 size, const Color& c);

  void fill_rect(glm::vec2 xy, glm::vec2 size, glm::vec2 rcenter, float angle, const Color& c);
  void fill_rect(glm::vec2 xy, glm::vec2 size, float angle, const Color& c);
  void fill_rect(glm::vec2 xy, glm::vec2 size, const Color& c);

  /* TEXTURES */
  void draw_tex(const Texture& t, glm::vec2 xy, glm::vec2 rcenter, float angle, const Color& c = rgb("white"));
  void draw_tex(const Texture& t, glm::vec2 xy, float angle, const Color& c = rgb("white"));
  void draw_tex(const Texture& t, glm::vec2 xy, const Color& c = rgb("white"));

private:
  static std::once_flag created_required_modules_;
};
} // namespace imp

IMP_PRAISE_HERMES(imp::Gfx2D);

#endif//IMP_GFX_MODULE_GFX_2D_HPP
