#ifndef BAPHY_GFX_MODULE_SURFACE_MGR_HPP
#define BAPHY_GFX_MODULE_SURFACE_MGR_HPP

#include "baphy/core/module_mgr.hpp"
#include "baphy/gfx/internal/gl/framebuffer.hpp"
#include "baphy/gfx/internal/gl/static_buffer.hpp"
#include "baphy/gfx/internal/gl/vertex_array.hpp"
#include "baphy/gfx/module/gfx_context.hpp"
#include "baphy/gfx/module/shader_mgr.hpp"
#include "glm/glm.hpp"
#include <memory>

namespace baphy {

class SurfaceMgr;

using SurfaceDrawCall = std::function<void(float)>;

class Surface {
public:
  std::shared_ptr<SurfaceMgr> mgr{nullptr};

  GladGLContext &gl;
  std::shared_ptr<Framebuffer> fbo{nullptr};

  Surface(std::shared_ptr<SurfaceMgr> mgr, GfxContext &gfx, std::shared_ptr<Shader> &shader, float w, float h);

  float w() const;
  float h() const;

  template<std::invocable T>
  void draw_on(T f);

  void draw(float x, float y);

private:
  float w_;
  float h_;

  VertexArray vao_;
  StaticBuffer<float> vbo_;

  std::shared_ptr<Shader> shader_;
  glm::mat4 projection_{};
};

class SurfaceMgr : public Module<SurfaceMgr>, public std::enable_shared_from_this<SurfaceMgr> {
  friend class Surface;

public:
  std::shared_ptr<GfxContext> gfx{nullptr};
  std::shared_ptr<ShaderMgr> shaders{nullptr};

  SurfaceMgr() : Module<SurfaceMgr>({EPI<GfxContext>::name, EPI<ShaderMgr>::name}) {}

  std::shared_ptr<Surface> create(float w, float h);

private:
  std::vector<std::shared_ptr<Surface>> surfaces_{};
  std::shared_ptr<Shader> surface_shader_{nullptr};

  std::vector<GLuint> fbo_id_stack_{};
  std::vector<glm::mat4> projection_stack_{};
  std::vector<glm::ivec2> viewport_stack_{};

  std::vector<std::vector<SurfaceDrawCall>> surface_draw_calls_{};
  void add_draw_call_(SurfaceDrawCall draw_call);
  void do_draw_calls_();

  float curr_z_resp_buf_{0};
  float curr_z_inc_();
  float curr_z_();

  glm::mat4 curr_projection_();

  void push_(glm::mat4 projection, GLuint fbo_id, GLsizei w, GLsizei h);
  void pop_();

  void e_initialize_(const EInitialize &e) override;
  void e_shutdown_(const EShutdown &e) override;

  void e_primitives_resp_z_(const EPrimitivesRespZ &e);
  void e_flush_surface_draw_calls_(const EFlushSurfaceDrawCalls &e);
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
