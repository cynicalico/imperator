#include "myco/util/log.hpp"

#include "myco/core/modules/application.hpp"
#include "myco/core/engine.hpp"

namespace myco {

void Application::initialize() {}
void Application::update(double dt) {}
void Application::draw() {}

void Application::application_show_debug_overlay() {
  debug_overlay_.active = true;
}

void Application::application_hide_debug_overlay() {
  debug_overlay_.active = false;
}

void Application::application_toggle_debug_overlay() {
  if (debug_overlay_.active)
    application_hide_debug_overlay();
  else
    application_show_debug_overlay();
}

void Application::draw_debug_overlay_() {
  ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Always);
  if (ImGui::Begin("Overlay_Perf", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("%s", fmt::format("FPS: {:.2f}{}", debug_overlay_.fps.fps(), window->vsync ? " (vsync)" : "").c_str());
  }
  ImGui::End();
}

void Application::initialize_(const Initialize &e) {
  Module<Application>::initialize_(e);

  Scheduler::sub<Update>(name, [&](const auto &e){ update(e.dt); });

  Scheduler::sub<StartFrame>(name, [&](const auto &){ start_frame_(); });
  Scheduler::sub<Draw>(name, [&](const auto &){ draw(); });
  Scheduler::sub<EndFrame>(name, [&](const auto &){ end_frame_(); });

  window = engine->get_module<Window>();

  initialize();
}

void Application::start_frame_() {
  if (dear)
    dear->new_frame();
}

void Application::end_frame_() {
  if (debug_overlay_.active)
    draw_debug_overlay_();

  if (dear)
    dear->render();

  debug_overlay_.fps.update();
}

} // namespace myco
