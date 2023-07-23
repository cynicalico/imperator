#include "baphy/gfx/module/surface_mgr.hpp"

#include "baphy/util/io.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace baphy {

Surface::Surface(std::shared_ptr<SurfaceMgr> mgr, GfxContext &gfx, TextureBatcher &textures, float w, float h)
    : mgr(std::move(mgr)),
      gl(gfx.gl),
      fbo(FramebufferBuilder(gfx, textures, w, h)
          .texture("color", TexFormat::rgba32f)
          .renderbuffer(RBufFormat::d32f)
          .check_complete()),
      w_(w),
      h_(h),
      projection_(glm::ortho(0.0f, w, h, 0.0f, 0.0f, 1.0f)) {}

float Surface::w() const {
  return w_;
}

float Surface::h() const {
  return h_;
}

void Surface::draw(float x, float y) {
  fbo->get_texture("color")->draw(x, y);
}

std::shared_ptr<Surface> SurfaceMgr::create(float w, float h) {
  surfaces_.emplace_back(std::make_shared<Surface>(shared_from_this(), *gfx, *textures, w, h));
  return surfaces_.back();
}

glm::mat4 SurfaceMgr::curr_projection_() {
  return projection_stack_.back();
}

void SurfaceMgr::push_(glm::mat4 projection, GLuint fbo_id, GLsizei w, GLsizei h) {
  EventBus::send_nowait<EFlush>(curr_projection_());
  gfx->clear(baphy::rgba(0x00000000), ClearBit::depth);

  projection_stack_.emplace_back(projection);
  viewport_stack_.emplace_back(w, h);

  // Make the new framebuffer the current one
  fbo_id_stack_.emplace_back(fbo_id);
  gfx->gl.BindFramebuffer(GL_FRAMEBUFFER, fbo_id_stack_.back());
  BAPHY_LOG_TRACE("Active surface: {}", fbo_id_stack_.back());

  // Set up our viewport
  gfx->gl.Viewport(0, 0, w, h);
}

void SurfaceMgr::pop_() {
  EventBus::send_nowait<EFlush>(curr_projection_());
  gfx->clear(baphy::rgba(0x00000000), ClearBit::depth);

  projection_stack_.pop_back();
  viewport_stack_.pop_back();

  // Make the old framebuffer the current one
  fbo_id_stack_.pop_back();
  gfx->gl.BindFramebuffer(GL_FRAMEBUFFER, fbo_id_stack_.back());
  BAPHY_LOG_TRACE("Active surface: {}", fbo_id_stack_.back());

  // Restore the viewport
  auto viewport = viewport_stack_.back();
  gfx->gl.Viewport(0, 0, viewport.x, viewport.y);
}

void SurfaceMgr::e_initialize_(const EInitialize &e) {
  gfx = module_mgr->get<GfxContext>();
  textures = module_mgr->get<TextureBatcher>();

  fbo_id_stack_.emplace_back(0);
  projection_stack_.emplace_back(gfx->ortho_projection());
  viewport_stack_.emplace_back(gfx->window->w(), gfx->window->h());

  Module::e_initialize_(e);
}

void SurfaceMgr::e_shutdown_(const EShutdown &e) {
  Module::e_shutdown_(e);
}

} // namespace baphy
