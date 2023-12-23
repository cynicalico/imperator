#include "imperator/util/module/debug_overlay.hpp"

#include "imperator/core/module/application.hpp"
#include "imperator/gfx/module/gfx_context.hpp"
#include "imperator/util/memusage.hpp"

namespace imp {
void DebugOverlay::r_initialize_(const E_Initialize& p) {
  Hermes::sub<E_Draw>(module_name, {EPI<Application>::name}, [&](const auto& p) { r_draw_(p); });
  Hermes::sub<E_Update>(module_name, [&](const auto& p) { r_update_(p); });

  Module::r_initialize_(p);
}

void DebugOverlay::r_shutdown_(const E_Shutdown& p) {
  Module::r_shutdown_(p);
}

void DebugOverlay::r_update_(const E_Update& p) {
  fps = p.fps;
}

void DebugOverlay::r_draw_(const E_Draw& p) {
  // Late-initialize so GfxContext can add a tab at some point if that is useful
  if (!gfx) {
    gfx = module_mgr->get<GfxContext>();
  }

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {1, 1});
  ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, {0, 0});
  ImGui::SetNextWindowPos({0, 0});
  if (ImGui::Begin("FPS", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize)) {
    const auto vsync_str = gfx->is_vsync() ? " (vsync)" : "";
    ImGui::Text("%s", fmt::format("{:.2f} fps{}{}", fps, vsync_str, BUILD_TYPE).c_str());
    ImGui::Text("%s", fmt::format("{:.2f} MB", imp::memusage_mb()).c_str());
  }
  ImGui::End();
  ImGui::PopStyleVar(2);

  if (ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
    static std::string current_item{tabs.begin()->first};
    if (ImGui::BeginCombo("##combo", current_item.c_str())) {
      for (const auto& k: tabs | std::views::keys) {
        const bool is_selected = current_item == k;
        if (ImGui::Selectable(k.c_str(), is_selected)) {
          current_item = k;
        }
        if (is_selected) {
          ImGui::SetItemDefaultFocus();
        }
      }
      ImGui::EndCombo();
    }

    ImGui::Separator();
    tabs[current_item]();
  }
  ImGui::End();
}
} // namespace imp
