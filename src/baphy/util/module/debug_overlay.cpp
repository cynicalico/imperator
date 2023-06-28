#include <utility>

#include "baphy/util/module/debug_overlay.hpp"

#include "baphy/core/module/application.hpp"
#include "baphy/util/memusage.hpp"
#include "baphy/util/sops.hpp"

namespace baphy {

void DebugOverlay::log_clear() {
  log_.buf.clear();
  log_.line_offsets.clear();
  log_.line_offsets.push_back(0);
  log_.line_levels.clear();
  log_.line_levels.push_back(spdlog::level::info);
}

void DebugOverlay::log_add(spdlog::level::level_enum level, const std::string &text) {
  int old_size = log_.buf.size();
  log_.buf.append(text.c_str());

  for (int new_size = log_.buf.size(); old_size < new_size; old_size++)
    if (log_.buf[old_size] == '\n') {
      log_.line_offsets.push_back(old_size + 1);
      log_.line_levels[log_.line_levels.size() - 1] = level;
      log_.line_levels.push_back(level);
    }
}

void DebugOverlay::log_draw(const char *title, bool *p_open) {
  dear->begin(title, p_open), [&] {
    dear->popup("Options"), [&] {
      ImGui::Checkbox("Auto-scroll", &log_.autoscroll);
      ImGui::Checkbox("Wrapping", &log_.wrapping);
      ImGui::Checkbox("Docked", &log_.docked);
    };

    if (ImGui::Button("Options"))
      ImGui::OpenPopup("Options");
    ImGui::SameLine();
    if (ImGui::Button("Clear"))
      log_clear();
    ImGui::SameLine();
    if (ImGui::Button("Copy"))
      ImGui::LogToClipboard();
    ImGui::SameLine();
    log_.filter.Draw("Filter", -100.0f);

    ImGui::Separator();

    dear->child("scrolling", {0, 0}, false, ImGuiWindowFlags_HorizontalScrollbar), [&] {
      ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

      if (log_.wrapping)
        ImGui::PushTextWrapPos(ImGui::GetWindowWidth());

      const char* buf_it = log_.buf.begin();
      const char* buf_end = log_.buf.end();

      if (log_.filter.IsActive()) {
        for (int line_no = 0; line_no < log_.line_offsets.Size; line_no++) {
          const char* line_start = buf_it + log_.line_offsets[line_no];
          const char* line_end = (line_no + 1 < log_.line_offsets.Size) ? (buf_it + log_.line_offsets[line_no + 1] - 1) : buf_end;
          if (log_.filter.PassFilter(line_start, line_end)) {
            ImGui::PushStyleColor(ImGuiCol_Text, log_.color_map[log_.line_levels[line_no]]);
            ImGui::TextUnformatted(line_start, line_end);
            ImGui::PopStyleColor();
          }
        }

      } else {
        // https://github.com/ocornut/imgui/issues/1447
        // Wrapping is just not compatible with list clipping currently, so we have to turn it off
        if (!log_.wrapping) {
          ImGuiListClipper clipper;
          clipper.Begin(log_.line_offsets.Size);
          while (clipper.Step()) {
            for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++) {
              const char *line_start = buf_it + log_.line_offsets[line_no];
              const char *line_end = (line_no + 1 < log_.line_offsets.Size) ? (buf_it + log_.line_offsets[line_no + 1] -
                                                                               1) : buf_end;
              ImGui::PushStyleColor(ImGuiCol_Text, log_.color_map[log_.line_levels[line_no]]);
              ImGui::TextUnformatted(line_start, line_end);
              ImGui::PopStyleColor();
            }
          }
          clipper.End();

        } else {
          for (int line_no = 0; line_no < log_.line_offsets.Size; line_no++) {
            const char* line_start = buf_it + log_.line_offsets[line_no];
            const char* line_end = (line_no + 1 < log_.line_offsets.Size) ? (buf_it + log_.line_offsets[line_no + 1] - 1) : buf_end;
            ImGui::PushStyleColor(ImGuiCol_Text, log_.color_map[log_.line_levels[line_no]]);
            ImGui::TextUnformatted(line_start, line_end);
            ImGui::PopStyleColor();
          }
        }
      }

      if (log_.wrapping)
        ImGui::PopTextWrapPos();

      ImGui::PopStyleVar();

      if (log_.autoscroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);
    };
  };
}

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

void DebugOverlay::update_gfx_state_() {
  gfx_tab_.vsync = gfx->is_vsync();
}

void DebugOverlay::e_initialize_(const baphy::EInitialize &e) {
  EventBus::sub<ELogMsg>(module_name, [&](const auto &e) { e_log_msg_(e); });
  EventBus::sub<EUpdate>(module_name, [&](const auto &e) { e_update_(e); });
  EventBus::sub<EDraw>(module_name, {EPI<Application>::name}, [&](const auto &e) { e_draw_(e); });

  dear = module_mgr->get<DearImgui>();
  gfx = module_mgr->get<GfxContext>();
  shader_mgr = module_mgr->get<ShaderMgr>();
  window = module_mgr->get<Window>();

  shader_editor_.te.SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
  shader_editor_.te.SetPalette(TextEditor::GetDarkPalette());

  log_clear();
  log_.color_map.insert({
      {spdlog::level::trace, IM_COL32(198, 208, 245, 255)},
      {spdlog::level::debug, IM_COL32(140, 170, 238, 255)},
      {spdlog::level::info, IM_COL32(166, 209, 137, 255)},
      {spdlog::level::warn, IM_COL32(229, 200, 144, 255)},
      {spdlog::level::err, IM_COL32(231, 130, 132, 255)},
      {spdlog::level::critical, IM_COL32(244, 184, 228, 255)}
  });

  Module::e_initialize_(e);
}

void DebugOverlay::e_shutdown_(const baphy::EShutdown &e) {
  Module::e_shutdown_(e);
}

void DebugOverlay::e_log_msg_(const ELogMsg &e) {
  if (!received_shutdown_)
    log_add(e.level, e.text);
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
  update_gfx_state_();
}

void DebugOverlay::e_draw_(const EDraw &e) {
  EventBus::poll<ELogMsg>(module_name);

  ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, {0.0f, 0.0f});
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {2.0f, 2.0f});
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {2.0f, 2.0f});
  ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {2.0f, 2.0f});

  ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Always);
  dear->begin(module_name.c_str(), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize), [&] {
    if (ImGui::BeginPopupContextWindow()) {
      ImGui::Checkbox("Log", &log_.show);
      ImGui::Checkbox("Controls", &controls_.show);
      ImGui::Checkbox("Shader Editor", &shader_editor_.show);
      ImGui::EndPopup();
    }

    dear->text("FPS: {:.2f}{}", fps_, gfx->is_vsync() ? " (vsync)" : "");
    dear->text("Mem: {:.2f} MB", memusage_mb());

    auto dts_v = std::vector<double>{dts_.begin(), dts_.end()};
    auto [dt_min, dt_max] = std::minmax_element(dts_v.begin(), dts_v.end());

    ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, {0, 0});
    ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
    ImPlot::PushStyleVar(ImPlotStyleVar_PlotBorderSize, 1);
    ImPlot::PushStyleColor(ImPlotCol_Line, {0.0, 1.0, 0.0, 1.0});
    ImPlot::PushStyleColor(ImPlotCol_Fill, {0.0, 1.0, 0.0, 1.0});
    ImPlot::PushStyleColor(ImPlotCol_PlotBorder, {1.0, 1.0, 1.0, 1.0});

    auto latency_w = static_cast<float>(floor(window->w() / 8.0));
    auto latency_h = 40.0f;
    if (ImPlot::BeginPlot("##FPS_History", {latency_w, latency_h}, ImPlotFlags_CanvasOnly | ImPlotFlags_NoInputs)) {
      ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
      ImPlot::SetupAxisLimits(ImAxis_X1, 0, dts_v.size(), ImPlotCond_Always);
      ImPlot::SetupAxisLimits(ImAxis_Y1, 0, *dt_max + (*dt_max * 0.1), ImPlotCond_Always);
      ImPlot::PlotShaded("##History_Fill", &dts_v[0], dts_v.size(), -INFINITY);
      ImPlot::PlotLine("##History", &dts_v[0], dts_v.size());
      ImPlot::EndPlot();
    }

    ImPlot::PopStyleColor(3);
    ImPlot::PopStyleVar(3);
  };

  ImGui::PopStyleVar(5);

  if (controls_.show) {
    ImGui::SetNextWindowCollapsed(true, ImGuiCond_Once);
    ImGui::SetNextWindowPos({static_cast<float>(window->w()), 0}, ImGuiCond_Always, {1, 0});
    dear->begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize), [&] {
      dear->tab_bar("Control tabs"), [&] {
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

        dear->tab_item("Gfx"), [&] {
          if (ImGui::Checkbox("vsync", &gfx_tab_.vsync))
            gfx->set_vsync(gfx_tab_.vsync);
        };
      };
    };
  }

  if (shader_editor_.show) {
    dear->begin("Shader Editor"), [&] {
      const auto items = shader_mgr->get_names();
      static int item_current_idx = -1;

      if (ImGui::Button("Compile")) {
        auto src_o = baphy::ShaderSrc::parse(shader_editor_.te.GetText());
        if (src_o)
          shader_mgr->recompile(items[item_current_idx], *src_o);
      }

      ImGui::SameLine();

      std::string combo_preview_value;
      if (item_current_idx != -1)
        combo_preview_value = items[item_current_idx];
      dear->combo("Name", combo_preview_value.c_str()), [&] {
        for (const auto &[i, v]: baphy::enumerate(items)) {
          const bool is_selected = (item_current_idx == i);
          if (ImGui::Selectable(v.c_str(), is_selected)) {
            item_current_idx = i;
            shader_editor_.te.SetText(shader_mgr->get(v)->src());
          }

          if (is_selected)
            ImGui::SetItemDefaultFocus();
        }
      };

      shader_editor_.te.Render("##text edit");
    };
  }

  if (log_.show) {
    ImGui::SetNextWindowCollapsed(true, ImGuiCond_Once);
    ImGui::SetNextWindowSize({static_cast<float>(window->w() / 2.0), static_cast<float>(window->h() / 4.0)}, ImGuiCond_Once);
    if (log_.docked)
      ImGui::SetNextWindowPos({0, static_cast<float>(window->h())}, ImGuiCond_Always, {0, 1});
    log_draw("Log");
  }
}

} // namespace baphy
