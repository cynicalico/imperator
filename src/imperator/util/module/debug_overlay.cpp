#include "imperator/util/module/debug_overlay.hpp"

#include "imperator/core/module/application.hpp"
#include "imperator/gfx/module/gfx_context.hpp"
#include "imperator/util/memusage.hpp"

namespace imp {
void DebugOverlay::set_flying_log_enabled(bool enabled) {
  flying_log_.enabled = enabled;
}

bool DebugOverlay::is_flying_log_enabled() const {
  return flying_log_.enabled;
}

void DebugOverlay::set_flying_log_limit(std::size_t max_lines) {
  flying_log_.max_lines = max_lines;
}

void DebugOverlay::set_flying_log_timeout(double timeout) {
  flying_log_.timeout = timeout;
}

void DebugOverlay::r_initialize_(const E_Initialize& p) {
  Hermes::sub<E_Draw>(module_name, {EPI<Application>::name}, [&](const auto& p) { r_draw_(p); });
  Hermes::sub<E_Update>(module_name, [&](const auto& p) { r_update_(p); });
  Hermes::sub<E_LogMsg>(module_name, [&](const auto& p) { r_log_msg_(p); });
  Hermes::sub<E_GlfwWindowSize>(module_name, [&](const auto& p) { r_glfw_window_size_(p); });

  Module::r_initialize_(p);
}

void DebugOverlay::r_shutdown_(const E_Shutdown& p) {
  Module::r_shutdown_(p);
}

void DebugOverlay::r_update_(const E_Update& p) {
  // Late-initialize so GfxContext can add a tab at some point if that is useful
  if (!gfx) {
    gfx = module_mgr->get<GfxContext>();
  }

  fps = p.fps;

  for (auto& [s, color, acc, fade_max]: flying_log_.lines) {
    acc -= p.dt;
    if (acc <= fade_max) {
      color.a = static_cast<int>(std::floor(255.0 * (acc / fade_max)));
    }
  }
  while (!flying_log_.lines.empty() && flying_log_.lines.back().acc <= 0.0) {
    flying_log_.lines.pop_back();
  }
  Hermes::poll<E_LogMsg>(module_name);
}

void DebugOverlay::r_draw_(const E_Draw& p) {
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {1, 1});
  ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, {0, 0});
  ImGui::SetNextWindowPos({1, 1});
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

  auto dl = ImGui::GetBackgroundDrawList();
  auto pos = glm::vec2{1, window_size_.y - 1};

  auto draw_line = [&](const std::string& s, const RGB& color) {
    auto text_size = ImGui::CalcTextSize(s.c_str());
    dl->AddRectFilled(
      {pos.x, pos.y - text_size.y - 2},
      {pos.x + text_size.x + 2, pos.y},
      ImGui::GetColorU32(rgba(0, 0, 0, color.a).vec4())
    );
    dl->AddText(
      {pos.x + 1, pos.y - text_size.y - 1},
      ImGui::GetColorU32(color.vec4()),
      s.c_str()
    );
    pos.y -= text_size.y + 2;
  };

  for (const auto& l: flying_log_.lines) {
    draw_line(l.s, l.color);
  }
}

void DebugOverlay::r_log_msg_(const E_LogMsg& p) {
  // TODO: Make these customizable
  static std::unordered_map<spdlog::level::level_enum, RGB> level_colors{
    {spdlog::level::trace, rgb(0x808080)},
    {spdlog::level::debug, rgb(0x8080ff)},
    {spdlog::level::info, rgb(0xffffff)},
    {spdlog::level::warn, rgb(0xffff80)},
    {spdlog::level::err, rgb(0xff8080)},
    {spdlog::level::critical, rgb(0xff0000)}
  };

  if (flying_log_.lines.size() == flying_log_.max_lines) {
    flying_log_.lines.pop_back();
  }
  flying_log_.lines.emplace_front(
    p.text,
    level_colors[p.level],
    flying_log_.timeout,
    flying_log_.timeout / 2.0
  );
}

void DebugOverlay::r_glfw_window_size_(const E_GlfwWindowSize& p) {
  window_size_.x = p.width;
  window_size_.y = p.height;
}
} // namespace imp
