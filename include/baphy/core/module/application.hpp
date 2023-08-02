#ifndef BAPHY_CORE_MODULE_APPLICATION_HPP
#define BAPHY_CORE_MODULE_APPLICATION_HPP

#include "baphy/core/module/audio_mgr.hpp"
#include "baphy/core/module/cursor_mgr.hpp"
#include "baphy/core/module/dear_imgui.hpp"
#include "baphy/core/module/input_mgr.hpp"
#include "baphy/core/module/window.hpp"
#include "baphy/core/module_mgr.hpp"
#include "baphy/gfx/module/font_mgr.hpp"
#include "baphy/gfx/module/gfx_context.hpp"
#include "baphy/gfx/module/gui_mgr.hpp"
#include "baphy/gfx/module/primitive_batcher.hpp"
#include "baphy/gfx/module/surface_mgr.hpp"
#include "baphy/gfx/module/texture_batcher.hpp"
#include "baphy/util/module/debug_overlay.hpp"
#include "baphy/util/module/thread_pool.hpp"
#include "baphy/util/module/timer_mgr.hpp"
#include "baphy/util/module/tween_mgr.hpp"

namespace baphy {

class Application : public Module<Application> {
public:
  std::shared_ptr<AudioMgr> audio{nullptr};
  std::shared_ptr<CursorMgr> cursors{nullptr};
  std::shared_ptr<DearImgui> dear{nullptr};
  std::shared_ptr<InputMgr> input{nullptr};
  std::shared_ptr<Window> window{nullptr};
  std::shared_ptr<FontMgr> fonts{nullptr};
  std::shared_ptr<GfxContext> gfx{nullptr};
  std::shared_ptr<GuiMgr> gui{nullptr};
  std::shared_ptr<PrimitiveBatcher> primitives{nullptr};
  std::shared_ptr<SurfaceMgr> surfaces{nullptr};
  std::shared_ptr<TextureBatcher> textures{nullptr};
  std::shared_ptr<DebugOverlay> debug{nullptr};
  std::shared_ptr<ThreadPool> pool{nullptr};
  std::shared_ptr<TimerMgr> timer{nullptr};
  std::shared_ptr<TweenMgr> tween{nullptr};

  struct {
    bool first{true};
  } frameinfo{};

  Application() : Module<Application>({
      EPI<AudioMgr>::name,
      EPI<CursorMgr>::name,
      EPI<DearImgui>::name,
      EPI<InputMgr>::name,
      EPI<Window>::name,
      EPI<FontMgr>::name,
      EPI<GfxContext>::name,
      EPI<GuiMgr>::name,
      EPI<PrimitiveBatcher>::name,
      EPI<SurfaceMgr>::name,
      EPI<TextureBatcher>::name,
      EPI<DebugOverlay>::name,
      EPI<ThreadPool>::name,
      EPI<TimerMgr>::name,
      EPI<TweenMgr>::name,
  }) {}

  ~Application() override = default;

protected:
  virtual void initialize();
  virtual void update(double dt);
  virtual void draw();

private:
  std::shared_ptr<Surface> window_surf_{nullptr};
  std::shared_ptr<Surface> imgui_surf_{nullptr};

  void e_initialize_(const EInitialize &e) override;
  void e_shutdown_(const EShutdown &e) override;

  void e_start_application_(const EStartApplication &e);
  void e_update_(const EUpdate &e);
  void e_start_frame_(const EStartFrame &e);
  void e_draw_(const EDraw &e);
  void e_end_frame_(const EEndFrame &e);

  void e_glfw_window_size_(const EGlfwWindowSize &e);
};

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::Application);

#endif//BAPHY_CORE_MODULE_APPLICATION_HPP
