#include "baphy/gfx/module/surface_mgr.hpp"

#include "baphy/util/io.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace baphy {

Surface::Surface(std::shared_ptr<SurfaceMgr> mgr, GfxContext &gfx, std::shared_ptr<Shader> &shader, float w, float h)
    : mgr(std::move(mgr)),
      gl(gfx.gl),
      fbo(FramebufferBuilder(gfx, w, h).texture("color", TexFormat::rgba32f).renderbuffer(RBufFormat::d32f).check_complete()),
      w_(w),
      h_(h),
      vao_(gfx),
      vbo_(gfx, BufTarget::array, BufUsage::static_draw, {}),
      shader_(shader),
      projection_(glm::ortho(0.0f, w, h, 0.0f, 0.0f, 1.0f))
{
  vao_.attrib(*shader_, vbo_, "in_pos:3f in_color:4f in_tex_coords:2f in_trans:3f");
}

float Surface::w() const {
  return w_;
}

float Surface::h() const {
  return h_;
}

void Surface::draw(float x, float y) {
  float z = mgr->curr_z_inc_();
  glm::mat4 proj = mgr->curr_projection_();
  GLsizei count = vbo_.size() / 12;

  mgr->add_draw_call_([&, x, y, z, proj, count](float z_max) {
    vbo_.write({
        x,      y,      z,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,  0.0f, 0.0f, 0.0f,
        x + w_, y,      z,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
        x + w_, y + h_, z,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
        x,      y,      z,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,  0.0f, 0.0f, 0.0f,
        x + w_, y + h_, z,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
        x,      y + h_, z,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f
    });

    shader_->use();
    shader_->uniform_mat4f("mvp", proj);
    shader_->uniform_1f("z_max", z_max);

    fbo->use_texture("color");
    vao_.draw_arrays(DrawMode::triangles, count);
  });
}

std::shared_ptr<Surface> SurfaceMgr::create(float w, float h) {
  surfaces_.emplace_back(std::make_shared<Surface>(shared_from_this(), *gfx, surface_shader_, w, h));
  return surfaces_.back();
}

float SurfaceMgr::curr_z_inc_() {
  EventBus::send_nowait<EPrimitivesReqAndIncZ>();
  return curr_z_resp_buf_;
}

float SurfaceMgr::curr_z_() {
  EventBus::send_nowait<EPrimitivesReqZ>();
  return curr_z_resp_buf_;
}

glm::mat4 SurfaceMgr::curr_projection_() {
  return projection_stack_.back();
}

void SurfaceMgr::add_draw_call_(SurfaceDrawCall draw_call) {
  surface_draw_calls_.back().emplace_back(draw_call);
}

void SurfaceMgr::push_(glm::mat4 projection, GLuint fbo_id, GLsizei w, GLsizei h) {
  // Process any framebuffer draw calls + geometry to draw on current target,
  // then clear the depth buffer so things can be layered on top later
  do_draw_calls_();
  EventBus::send_nowait<EFlush>(curr_projection_());
  gfx->clear(baphy::rgba(0x00000000), ClearBit::depth);

  projection_stack_.emplace_back(projection);
  viewport_stack_.emplace_back(w, h);
  surface_draw_calls_.emplace_back();

  // Make the new framebuffer the current one
  fbo_id_stack_.emplace_back(fbo_id);
  gfx->gl.BindFramebuffer(GL_FRAMEBUFFER, fbo_id_stack_.back());

  // Set up our viewport
  gfx->gl.Viewport(0, 0, w, h);
}

void SurfaceMgr::pop_() {
  // Process any framebuffer draw calls + geometry to draw on current target,
  // then clear the depth buffer so things can be layered on top later
  do_draw_calls_();
  EventBus::send_nowait<EFlush>(curr_projection_());
  gfx->clear(baphy::rgba(0x00000000), ClearBit::depth);

  projection_stack_.pop_back();
  viewport_stack_.pop_back();
  surface_draw_calls_.pop_back();

  // Make the old framebuffer the current one
  fbo_id_stack_.pop_back();
  gfx->gl.BindFramebuffer(GL_FRAMEBUFFER, fbo_id_stack_.back());

  // Restore the viewport
  auto viewport = viewport_stack_.back();
  gfx->gl.Viewport(0, 0, viewport.x, viewport.y);
}

void SurfaceMgr::e_initialize_(const EInitialize &e) {
  gfx = module_mgr->get<GfxContext>();
  shaders = module_mgr->get<ShaderMgr>();

  auto fbo_src = ShaderSrc::parse(DATA_FOLDER / "shader" / "fbos.glsl");
  if (fbo_src)
    surface_shader_ = shaders->compile(*fbo_src);

  fbo_id_stack_.emplace_back(0);
  projection_stack_.emplace_back(gfx->ortho_projection());
  viewport_stack_.emplace_back(gfx->window->w(), gfx->window->h());
  surface_draw_calls_.emplace_back();

  EventBus::sub<EPrimitivesRespZ>(module_name, [&](const auto &e) { e_primitives_resp_z_(e); });
  EventBus::sub<EFlushSurfaceDrawCalls>(module_name, [&](const auto &e) { e_flush_surface_draw_calls_(e); });

  Module::e_initialize_(e);
}

void SurfaceMgr::e_shutdown_(const EShutdown &e) {
  Module::e_shutdown_(e);
}

void SurfaceMgr::e_primitives_resp_z_(const EPrimitivesRespZ &e) {
  curr_z_resp_buf_ = e.z;
}

void SurfaceMgr::e_flush_surface_draw_calls_(const EFlushSurfaceDrawCalls &e) {
  do_draw_calls_();
}

void SurfaceMgr::do_draw_calls_() {
  gfx->blend_func_separate(BlendFunc::one, BlendFunc::one_minus_src_alpha, BlendFunc::one_minus_dst_alpha, BlendFunc::one);
  gfx->enable(Capability::blend);
  gfx->depth_mask(false);

  float z_max = curr_z_();
  for (const auto& draw_call: surface_draw_calls_.back())
    draw_call(z_max);
  surface_draw_calls_.back().clear();

  gfx->depth_mask(true);
  gfx->disable(Capability::blend);
}

} // namespace baphy
