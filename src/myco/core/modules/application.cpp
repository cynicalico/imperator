#include "myco/util/log.hpp"

#include "myco/core/modules/application.hpp"
#include "myco/core/engine.hpp"
#include "myco/util/memusage.hpp"

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

void Application::application_sticky(const std::string &label, std::function<std::string()> &&f) {
  Scheduler::send_nowait<StickyCreate>(std::nullopt, label, std::forward<std::function<std::string()>>(f));
}

void Application::draw_stickies_() {
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
  ImGui::SetNextWindowSize({static_cast<float>(window->w), static_cast<float>(window->h)}, ImGuiCond_Always);
  ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Always);
  if (ImGui::Begin("Stickies", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs)) {
    auto dl = ImGui::GetWindowDrawList();

    glm::vec2 pos{window->w, 0};

    for (const auto &[category, ss]: stickies_) {
      if (category != "__none__")
        pos.y += ImGui::GetTextLineHeight();

      for (const auto &s : ss) {
        auto text = s.string();
        auto size = ImGui::CalcTextSize(text.c_str());

        dl->AddRectFilled(
            {pos.x - size.x - 2, pos.y - 1},
            {pos.x, pos.y + size.y + 2},
            ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_WindowBg))
        );
        dl->AddText(
            {pos.x - size.x - 1, pos.y},
            ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_Text)),
            text.c_str()
        );

        pos.y += size.y + 1;
      }
    }
  }
  ImGui::End();
  ImGui::PopStyleVar(3);
}

void Application::draw_debug_overlay_() {
  ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0.0f, 0.0f));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2.0f, 2.0f));
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 2.0f));
  ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(2.0f, 2.0f));
  ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Always);
  if (ImGui::Begin("Overlay_Perf", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("%s", fmt::format("FPS: {:.2f}{}", engine->frame_counter.fps(), window->vsync ? " (vsync)" : "").c_str());

    auto dts = engine->frame_counter.dts_vec();
    if (!dts.empty()) {
      auto [dt_min, dt_max] = std::minmax_element(dts.begin(), dts.end());
      double mid = (*dt_max + *dt_min) / 2.0;

      ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, {0, 0});
      ImPlot::PushStyleColor(ImPlotCol_Line, myco::rgb(0x00ff00).vec4());
      ImPlot::PushStyleColor(ImPlotCol_Fill, myco::rgba(0x00ff0080).vec4());
      if (ImPlot::BeginPlot("FPS_History", {100, 35},
                            ImPlotFlags_CanvasOnly | ImPlotFlags_NoFrame | ImPlotFlags_NoInputs)) {
        ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, dts.size(), ImPlotCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, mid + 1.5 * (*dt_max - mid), ImPlotCond_Always);
        ImPlot::PlotShaded("History", &dts[0], dts.size());
        ImPlot::PlotLine("History", &dts[0], dts.size());
        ImPlot::EndPlot();
      }
      ImPlot::PopStyleColor(2);
      ImPlot::PopStyleVar();
    }

    ImGui::Text("%s", fmt::format("Mem: {:.2f} MB", memusage_mb()).c_str());
  }
  ImGui::End();
  ImGui::PopStyleVar(5);
}

void Application::initialize_(const Initialize &e) {
  Module<Application>::initialize_(e);

  Scheduler::sub<StartApplication>(name, [&](const auto &p){ start_(p); });

  Scheduler::sub<Update>(
      name,
      {
          ModuleInfo<InputMgr>::name,
          ModuleInfo<TimerMgr>::name,
          ModuleInfo<Window>::name,
      },
      [&](const auto &e){ update(e.dt); }
  );

  Scheduler::sub<StartFrame>(name, [&](const auto &){ start_frame_(); });
  Scheduler::sub<Draw>(name, [&](const auto &){ draw(); });
  Scheduler::sub<EndFrame>(name, [&](const auto &){ end_frame_(); });

  stickies_["__none__"] = {};
  Scheduler::sub<StickyCreate>(name, [&](const auto &p) { sticky_create_(p); });

  window = engine->get_module<Window>();
  input = engine->get_module<InputMgr>();
  timer = engine->get_module<TimerMgr>();
}

void Application::start_(const StartApplication &e) {
  window->open(e.window_open_params);
  ctx = std::make_shared<myco::Context2D>(*window);
  dear = std::make_unique<myco::Dear>(*window, *ctx);

  initialize();
}

void Application::start_frame_() {
  if (dear)
    dear->new_frame();
}

void Application::end_frame_() {
  draw_stickies_();

  if (debug_overlay_.active)
    draw_debug_overlay_();

  if (dear)
    dear->render();
}

void Application::sticky_create_(const StickyCreate &e) {
  std::string category = "__none__";
  if (e.category.has_value())
    category = e.category.value();

  stickies_[category].emplace_back(category, e.label, e.f);
}

} // namespace myco
