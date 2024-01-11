#include "imp/gfx/module/dear_imgui.hpp"

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
  if (gfx->version == glm::ivec2{2, 0}) glsl_version = "#version 110";
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
  style.FrameBorderSize = 1;
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

  auto& colors = ImGui::GetStyle().Colors;
  colors[ImGuiCol_WindowBg] = ImVec4{0.1f, 0.1f, 0.13f, 1.0f};
  colors[ImGuiCol_MenuBarBg] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

  // Border
  colors[ImGuiCol_Border] = ImVec4{0.44f, 0.37f, 0.61f, 0.29f};
  colors[ImGuiCol_BorderShadow] = ImVec4{0.0f, 0.0f, 0.0f, 0.24f};

  // Text
  colors[ImGuiCol_Text] = ImVec4{1.0f, 1.0f, 1.0f, 1.0f};
  colors[ImGuiCol_TextDisabled] = ImVec4{0.5f, 0.5f, 0.5f, 1.0f};

  // Headers
  colors[ImGuiCol_Header] = ImVec4{0.13f, 0.13f, 0.17, 1.0f};
  colors[ImGuiCol_HeaderHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
  colors[ImGuiCol_HeaderActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

  // Buttons
  colors[ImGuiCol_Button] = ImVec4{0.13f, 0.13f, 0.17, 1.0f};
  colors[ImGuiCol_ButtonHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
  colors[ImGuiCol_ButtonActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
  colors[ImGuiCol_CheckMark] = ImVec4{0.74f, 0.58f, 0.98f, 1.0f};

  // Popups
  colors[ImGuiCol_PopupBg] = ImVec4{0.1f, 0.1f, 0.13f, 0.92f};

  // Slider
  colors[ImGuiCol_SliderGrab] = ImVec4{0.44f, 0.37f, 0.61f, 0.54f};
  colors[ImGuiCol_SliderGrabActive] = ImVec4{0.74f, 0.58f, 0.98f, 0.54f};

  // Frame BG
  colors[ImGuiCol_FrameBg] = ImVec4{0.13f, 0.13, 0.17, 1.0f};
  colors[ImGuiCol_FrameBgHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
  colors[ImGuiCol_FrameBgActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

  // Tabs
  colors[ImGuiCol_Tab] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
  colors[ImGuiCol_TabHovered] = ImVec4{0.24, 0.24f, 0.32f, 1.0f};
  colors[ImGuiCol_TabActive] = ImVec4{0.2f, 0.22f, 0.27f, 1.0f};
  colors[ImGuiCol_TabUnfocused] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
  colors[ImGuiCol_TabUnfocusedActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

  // Title
  colors[ImGuiCol_TitleBg] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
  colors[ImGuiCol_TitleBgActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
  colors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

  // Scrollbar
  colors[ImGuiCol_ScrollbarBg] = ImVec4{0.1f, 0.1f, 0.13f, 1.0f};
  colors[ImGuiCol_ScrollbarGrab] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
  colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
  colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{0.24f, 0.24f, 0.32f, 1.0f};

  // Seperator
  colors[ImGuiCol_Separator] = ImVec4{0.44f, 0.37f, 0.61f, 1.0f};
  colors[ImGuiCol_SeparatorHovered] = ImVec4{0.74f, 0.58f, 0.98f, 1.0f};
  colors[ImGuiCol_SeparatorActive] = ImVec4{0.84f, 0.58f, 1.0f, 1.0f};

  // Resize Grip
  colors[ImGuiCol_ResizeGrip] = ImVec4{0.44f, 0.37f, 0.61f, 0.29f};
  colors[ImGuiCol_ResizeGripHovered] = ImVec4{0.74f, 0.58f, 0.98f, 0.29f};
  colors[ImGuiCol_ResizeGripActive] = ImVec4{0.84f, 0.58f, 1.0f, 0.29f};

  // Docking
  colors[ImGuiCol_DockingPreview] = ImVec4{0.44f, 0.37f, 0.61f, 1.0f};
}
} // namespace imp
