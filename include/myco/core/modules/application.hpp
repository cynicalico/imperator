#pragma once

#include "myco/core/modules/module.hpp"
#include "myco/core/modules/input_mgr.hpp"
#include "myco/core/modules/timer_mgr.hpp"
#include "myco/core/modules/window.hpp"

#include "myco/gfx/context2d.hpp"
#include "myco/gfx/dear.hpp"

#include "myco/util/time.hpp"

#include <map>
#include <utility>

namespace myco {

class Sticky {
public:
  std::string category;
  std::string label;
  std::function<std::string()> f;

  Sticky(std::string category, std::string label, std::function<std::string()> f)
      : category(std::move(category)), label(std::move(label)), f(std::move(f)) {}

  std::string string() const {
    return fmt::format("{}: {}", label, f());
  }
};

class Application : public Module<Application> {
public:
  std::shared_ptr<Window> window{nullptr};
  std::shared_ptr<InputMgr> input{nullptr};

  std::shared_ptr<TimerMgr> timer{nullptr};

  std::shared_ptr<Context2D> ctx{nullptr};
  std::unique_ptr<Dear> dear{nullptr};

  Application() : Module<Application>({
    ModuleInfo<Window>::name,
    ModuleInfo<InputMgr>::name
  }) {}

  ~Application() override = default;

protected:
  virtual void initialize();
  virtual void update(double dt);
  virtual void draw();

  void application_show_debug_overlay();
  void application_hide_debug_overlay();
  void application_toggle_debug_overlay();

  template<typename S, typename T> requires (!std::invocable<T>)
  void application_sticky(const std::string &label, const S &format, T &&v);

  template<typename T> requires (!std::invocable<T>)
  void application_sticky(const std::string &label, T &&v);

  void application_sticky(const std::string &label, std::function<std::string()> &&f);

private:
  std::map<std::string, std::vector<Sticky>> stickies_{};

  struct {
    bool active{false};
  } debug_overlay_{};

  void draw_stickies_();
  void draw_debug_overlay_();

  void initialize_(const Initialize &e) override;
  void start_(const StartApplication &e);

  void start_frame_();
  void end_frame_();

  void sticky_create_(const StickyCreate &e);
};

template<typename S, typename T> requires (!std::invocable<T>)
void Application::application_sticky(const std::string &label, const S &format, T &&v) {
  application_sticky(label, [&]() { return fmt::vformat(format, fmt::make_format_args(std::forward<T&>(v))); });
}

template<typename T> requires (!std::invocable<T>)
void Application::application_sticky(const std::string &label, T &&v) {
  application_sticky(label, [&]() { return fmt::format("{}", std::forward<T&>(v)); });
}

} // namespace myco

MYCO_DECLARE_MODULE(myco::Application);
