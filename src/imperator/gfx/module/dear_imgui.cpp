#include "imperator/gfx/module/dear_imgui.hpp"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace imp {
void DearImgui::new_frame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void DearImgui::render() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DearImgui::r_initialize_(const E_Initialize& p) {
  auto window = module_mgr->get<Window>();
  auto gfx = module_mgr->get<GfxContext>();

  IMGUI_CHECKVERSION();
  ctx_ = ImGui::CreateContext();
  ImGui::StyleColorsClassic();
  setup_style_();
  io_ = &ImGui::GetIO();
  // io_->ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
  io_->IniFilename = nullptr;

  ImGui_ImplGlfw_InitForOpenGL(window->handle(), true);
  std::string glsl_version;
  if      (gfx->version == glm::ivec2{2, 0}) glsl_version = "#version 110";
  else if (gfx->version == glm::ivec2{2, 1}) glsl_version = "#version 120";
  else if (gfx->version == glm::ivec2{3, 0}) glsl_version = "#version 130";
  else if (gfx->version == glm::ivec2{3, 1}) glsl_version = "#version 140";
  else if (gfx->version == glm::ivec2{3, 2}) glsl_version = "#version 150";
  else
    glsl_version = fmt::format("#version {}{}0 core", gfx->version.x, gfx->version.y);
  ImGui_ImplOpenGL3_Init(glsl_version.c_str());

  implot_ctx_ = ImPlot::CreateContext();

  Module::r_initialize_(p);
}

void DearImgui::r_shutdown_(const E_Shutdown& p) {
  ImPlot::DestroyContext(implot_ctx_);
  implot_ctx_ = nullptr;

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();

  (void)io_;
  ImGui::DestroyContext(ctx_);
  ctx_ = nullptr;

  Module::r_shutdown_(p);
}

void DearImgui::setup_style_() {
  ImGuiStyle& style = ImGui::GetStyle();

  // Main
  style.WindowPadding = {4, 4};
  style.FramePadding = {2, 2};
  style.ItemSpacing = {2, 2};
  style.ItemInnerSpacing = {2, 2};
  style.TouchExtraPadding = {0, 0};
  style.IndentSpacing = 22;
  style.ScrollbarSize = 12;
  style.GrabMinSize = 12;

  // Borders
  style.WindowBorderSize = 1;
  style.ChildBorderSize = 1;
  style.PopupBorderSize = 1;
  style.FrameBorderSize = 0;
  style.TabBorderSize = 0;
  style.TabBarBorderSize = 1;

  // Rounding
  style.WindowRounding = 2;
  style.ChildRounding = 0;
  style.FrameRounding = 0;
  style.PopupRounding = 0;
  style.ScrollbarRounding = 0;
  style.GrabRounding = 0;
  style.TabRounding = 4;

  // Tables
  style.CellPadding = {2, 2};
  style.TableAngledHeadersAngle = 35;

  // Widgets
  style.WindowTitleAlign = {0, 0.5};
  style.WindowMenuButtonPosition = ImGuiDir_Left;
  style.ColorButtonPosition = ImGuiDir_Right;
  style.ButtonTextAlign = {0.5, 0.5};
  style.SelectableTextAlign = {0, 0};
  style.SeparatorTextBorderSize = 2;
  style.SeparatorTextAlign = {0, 0.5};
  style.SeparatorTextPadding = {12, 2};
  style.LogSliderDeadzone = 4;

  // Docking
  style.DockingSeparatorSize = 2;

  // Tooltips
  style.HoverFlagsForTooltipMouse = ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_Stationary;
  style.HoverFlagsForTooltipNav = ImGuiHoveredFlags_DelayNormal | ImGuiHoveredFlags_NoSharedDelay;

  // Misc
  style.DisplaySafeAreaPadding = {3, 3};
}
} // namespace imp
