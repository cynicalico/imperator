#ifndef BAPHY_GFX_SURFACE_HPP
#define BAPHY_GFX_SURFACE_HPP

#include "baphy/core/module_mgr.hpp"
#include "baphy/gfx/internal/gl/framebuffer.hpp"
#include "baphy/gfx/internal/gl/static_buffer.hpp"
#include "baphy/gfx/internal/gl/vertex_array.hpp"
#include "baphy/gfx/module/gfx_context.hpp"
#include "baphy/gfx/module/shader_mgr.hpp"
#include "glm/glm.hpp"
#include <concepts>

namespace baphy {

class Surface {
public:
  std::shared_ptr<GfxContext> gfx{nullptr};
  std::shared_ptr<ShaderMgr> shaders{nullptr};

  GladGLContext &gl;
  std::shared_ptr<Framebuffer> fbo{nullptr};

  Surface(std::shared_ptr<GfxContext> &gfx, std::shared_ptr<ShaderMgr> &shaders, float w, float h);

  template<std::invocable T>
  void draw_on(T f) {
    fbo->bind();
    f();
    gfx->flush_draw_calls(projection_);
    fbo->unbind();
    gfx->reset_viewport();
  }

  void draw(float x, float y);

private:
  float w_;
  float h_;

  VertexArray vao_;
  StaticBuffer<float> vbo_;

  std::shared_ptr<Shader> shader_;
  glm::mat4 projection_{};
};

} // namespace baphy

#endif//BAPHY_GFX_SURFACE_HPP
