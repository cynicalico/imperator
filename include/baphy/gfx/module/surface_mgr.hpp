#ifndef BAPHY_GFX_MODULE_SURFACE_MGR_HPP
#define BAPHY_GFX_MODULE_SURFACE_MGR_HPP

#include "baphy/core/module_mgr.hpp"
#include "baphy/gfx/internal/gl/framebuffer.hpp"
#include "baphy/gfx/internal/gl/static_buffer.hpp"
#include "baphy/gfx/internal/gl/vertex_array.hpp"
#include "baphy/gfx/module/batcher.hpp"
#include "baphy/gfx/module/gfx_context.hpp"
#include "baphy/gfx/module/texture_batcher.hpp"
#include "glm/glm.hpp"
#include <memory>

namespace baphy {

class SurfaceMgr;

class Surface {
public:
  std::shared_ptr<SurfaceMgr> mgr{nullptr};

  GladGLContext &gl;
  std::shared_ptr<Framebuffer> fbo{nullptr};

  Surface(std::shared_ptr<SurfaceMgr> mgr, GfxContext &gfx, TextureBatcher &textures, float w, float h);

  float w() const;
  float h() const;

  template<std::invocable T>
  void draw_on(T f);

  void draw(float x, float y);

private:
  float w_;
  float h_;

  glm::mat4 projection_{};
};

class SurfaceMgr : public Module<SurfaceMgr>, public std::enable_shared_from_this<SurfaceMgr> {
  friend class Surface;

public:
  std::shared_ptr<GfxContext> gfx{nullptr};
  std::shared_ptr<TextureBatcher> textures{nullptr};

  SurfaceMgr() : Module<SurfaceMgr>({EPI<GfxContext>::name, EPI<TextureBatcher>::name}) {}

  std::shared_ptr<Surface> create(float w, float h);

private:
  std::vector<std::shared_ptr<Surface>> surfaces_{};

  std::vector<GLuint> fbo_id_stack_{};
  std::vector<glm::mat4> projection_stack_{};
  std::vector<glm::ivec2> viewport_stack_{};

  glm::mat4 curr_projection_();

  void push_(glm::mat4 projection, GLuint fbo_id, GLsizei w, GLsizei h);
  void pop_();

  void e_initialize_(const EInitialize &e) override;
  void e_shutdown_(const EShutdown &e) override;
};

template<std::invocable T>
void Surface::draw_on(T f) {
  mgr->push_(projection_, fbo->id, w_, h_);
  f();
  mgr->pop_();
}

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::SurfaceMgr);

#endif//BAPHY_GFX_MODULE_SURFACE_MGR_HPP
