#include "baphy/core/module/application.hpp"

namespace baphy {

void Application::initialize() {}

void Application::update(double dt) {}

void Application::draw() {}

void Application::e_initialize_(const baphy::EInitialize &e) {
  EventBus::sub<EStartApplication>(module_name, [&](const auto &e) { e_start_application_(e); });
  EventBus::sub<EUpdate>(
      module_name, {
          EPI<AudioMgr>::name,
          EPI<InputMgr>::name,
          EPI<ThreadPool>::name,
          EPI<TimerMgr>::name,
          EPI<TweenMgr>::name,
          EPI<Window>::name,
      },
      [&](const auto &e) { e_update_(e); }
  );
  EventBus::sub<EStartFrame>(module_name, {EPI<Window>::name}, [&](const auto &e) { e_start_frame_(e); });
  EventBus::sub<EDraw>(module_name, [&](const auto &e) { e_draw_(e); });
  EventBus::sub<EEndFrame>(module_name, [&](const auto &e) { e_end_frame_(e); });
  EventBus::sub<EGlfwWindowSize>(module_name, [&](const auto &e) { e_glfw_window_size_(e); });

  audio = module_mgr->get<AudioMgr>();
  cursors = module_mgr->get<CursorMgr>();
  dear = module_mgr->get<DearImgui>();
  input = module_mgr->get<InputMgr>();
  window = module_mgr->get<Window>();
  fonts = module_mgr->get<FontMgr>();
  gfx = module_mgr->get<GfxContext>();
  primitives = module_mgr->get<PrimitiveBatcher>();
  surfaces = module_mgr->get<SurfaceMgr>();
  textures = module_mgr->get<TextureBatcher>();
  debug = module_mgr->get<DebugOverlay>();
  pool = module_mgr->get<ThreadPool>();
  timer = module_mgr->get<TimerMgr>();
  tween = module_mgr->get<TweenMgr>();

  window_surf_ = surfaces->create(window->w(), window->h());
  imgui_surf_ = surfaces->create(window->w(), window->h());

  Module::e_initialize_(e);
}

void Application::e_shutdown_(const baphy::EShutdown &e) {
  Module::e_shutdown_(e);
}

void Application::e_start_application_(const baphy::EStartApplication &e) {
  initialize();
}

void Application::e_update_(const EUpdate &e) {
  update(e.dt);
}

void Application::e_start_frame_(const EStartFrame &e) {
  dear->new_frame();
}

void Application::e_draw_(const EDraw &e) {
  gfx->clear(baphy::rgb(0x000000));

  window_surf_->draw_on([&] { draw(); });
  window_surf_->draw(0, 0);
}

void Application::e_end_frame_(const EEndFrame &e) {
  imgui_surf_->draw_on([&] {
    gfx->clear(baphy::rgba(0x00000000));
    dear->render();
  });
  imgui_surf_->draw(0, 0);

  EventBus::send_nowait<EFlush>(gfx->ortho_projection());

  if (frameinfo.first)
    frameinfo.first = false;
}

void Application::e_glfw_window_size_(const EGlfwWindowSize &e) {
  window_surf_ = surfaces->create(e.width, e.height);
  imgui_surf_ = surfaces->create(e.width, e.height);
}

} // namespace baphy
