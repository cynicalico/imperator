#include "baphy/gfx/surface.hpp"

#include "baphy/util/io.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace baphy {

Surface::Surface(std::shared_ptr<GfxContext> &gfx, std::shared_ptr<ShaderMgr> &shaders, float w, float h)
    : gfx(gfx),
      shaders(shaders),
      gl(gfx->gl),
      fbo(FramebufferBuilder(*gfx, w, h)
              .texture("color", TexFormat::rgba32f)
              .renderbuffer(RBufFormat::d32f)
              .check_complete()),
      w_(w),
      h_(h),
      vao_(*gfx),
      vbo_(*gfx, BufTarget::array, BufUsage::static_draw, {}),
      projection_(glm::ortho(0.0f, w, h, 0.0f, 0.0f, 1.0f)) {

  auto fbo_src = ShaderSrc::parse(DATA_FOLDER / "shader" / "fbos.glsl");
  if (fbo_src)
    shader_ = shaders->compile(*fbo_src);

  vao_.attrib(*shader_, vbo_, "in_pos:3f in_color:4f in_tex_coords:2f in_trans:3f");
}

void Surface::draw(float x, float y) {
  vbo_.write({
      x,      y,      1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,  0.0f, 0.0f, 0.0f,
      x + w_, y,      1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
      x + w_, y + h_, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
      x,      y,      1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,  0.0f, 0.0f, 0.0f,
      x + w_, y + h_, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
      x,      y + h_, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f
  });

  shader_->use();
  shader_->uniform_mat4f("mvp", gfx->ortho_projection());
  shader_->uniform_1f("z_max", 2.0f);

  fbo->use_texture("color");
  vao_.draw_arrays(DrawMode::triangles, vbo_.size() / 12);
}

} // namespace baphy
