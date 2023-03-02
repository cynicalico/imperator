#pragma once

#include "myco/core/modules/module.hpp"
#include "myco/core/modules/window.hpp"

#include "myco/gfx/context2d.hpp"
#include "myco/gfx/dear.hpp"

#include "myco/util/time.hpp"

namespace myco {

class Application : public Module<Application> {
public:
  std::shared_ptr<Window> window{nullptr};

  std::shared_ptr<Context2D> ctx{nullptr};
  std::unique_ptr<Dear> dear{nullptr};

  Application() : Module<Application>({
    ModuleInfo<Window>::name
  }) {}

  ~Application() override {
    MYCO_LOG_DEBUG("Application destroyed");
  }

protected:
  virtual void initialize();
  virtual void update(double dt);
  virtual void draw();

  void application_show_debug_overlay();
  void application_hide_debug_overlay();
  void application_toggle_debug_overlay();

private:
  struct {
    bool active{false};
    FrameCounter fps{};
  } debug_overlay_{};

  void draw_debug_overlay_();

  void initialize_(const Initialize &e) override;

  void start_frame_();
  void end_frame_();
};

} // namespace myco

DECLARE_MYCO_MODULE(myco::Application);
