#include "baphy/util/module/debug_overlay.hpp"

#include "baphy/core/module/application.hpp"
#include "baphy/util/memusage.hpp"

namespace baphy {

void DebugOverlay::update_window_state_() {
  window_tab_.x = window->x();
  window_tab_.y = window->y();
  window_tab_.w = window->w();
  window_tab_.h = window->h();
  window_tab_.resizable = window->is_resizable();
  window_tab_.decorated = window->is_decorated();
  window_tab_.auto_iconify = window->is_auto_iconify();
  window_tab_.floating = window->is_floating();
  window_tab_.focus_on_show = window->is_focus_on_show();
}

void DebugOverlay::e_initialize_(const baphy::EInitialize &e) {
  EventBus::sub<EUpdate>(module_name, [&](const auto &e) { e_update_(e); });
  EventBus::sub<EDraw>(module_name, {EPI<Application>::name}, [&](const auto &e) { e_draw_(e); });

  dear = module_mgr->get<DearImgui>();
  gfx = module_mgr->get<GfxContext>();
  window = module_mgr->get<Window>();

  Module::e_initialize_(e);
}

void DebugOverlay::e_shutdown_(const baphy::EShutdown &e) {
  Module::e_shutdown_(e);
}

void DebugOverlay::e_update_(const EUpdate &e) {
  fps_ = e.fps;

  timestamps_.emplace_back(e.timestamp);
  dts_.emplace_back(e.dt);

  // Keep size >= 2 otherwise dt will give bad results on pauses longer than 1s
  while (timestamps_.size() > 2 && timestamps_.back() - timestamps_.front() > 1'000'000'000) {
    timestamps_.pop_front();
    dts_.pop_front();
  }

  update_window_state_();
}

void DebugOverlay::e_draw_(const EDraw &e) {
  ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, {0.0f, 0.0f});
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {2.0f, 2.0f});
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {2.0f, 2.0f});
  ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {2.0f, 2.0f});

  ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Always);
  dear->begin(module_name.c_str(), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize), [&]{
    dear->text("FPS: {:.2f}{}", fps_, gfx->is_vsync() ? " (vsync)" : "");
    dear->text("Mem: {:.2f} MB", memusage_mb());
  };

  ImGui::PopStyleVar(5);

  ImGui::SetNextWindowPos({static_cast<float>(window->w()), 0}, ImGuiCond_Always, {1, 0});
  dear->begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize), [&]{
    dear->tab_bar("tabs"), [&]{
      dear->tab_item("Window"), [&] {
        ImGui::PushItemWidth(50);

        if (ImGui::InputInt("##x", &window_tab_.x, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue))
          window->set_x(window_tab_.x);
        ImGui::SameLine();
        if (ImGui::InputInt("x/y", &window_tab_.y, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue))
          window->set_y(window_tab_.y);

        if (ImGui::InputInt("##w", &window_tab_.w, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue))
          window->set_w(window_tab_.w);
        ImGui::SameLine();
        if (ImGui::InputInt("w/h", &window_tab_.h, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue))
          window->set_h(window_tab_.h);

        ImGui::PopItemWidth();

        if (ImGui::Checkbox("Resizable", &window_tab_.resizable))
          window->set_resizable(window_tab_.resizable);

        if (ImGui::Checkbox("Decorated", &window_tab_.decorated))
          window->set_decorated(window_tab_.decorated);

        if (ImGui::Checkbox("Auto iconify", &window_tab_.auto_iconify))
          window->set_auto_iconify(window_tab_.auto_iconify);

        if (ImGui::Checkbox("Floating", &window_tab_.floating))
          window->set_floating(window_tab_.floating);

        if (ImGui::Checkbox("Focus on show", &window_tab_.focus_on_show))
          window->set_focus_on_show(window_tab_.focus_on_show);
      };
    };
  };
}

} // namespace baphy
