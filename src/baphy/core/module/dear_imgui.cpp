#include "baphy/core/module/dear_imgui.hpp"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace baphy {

void DearImgui::new_frame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void DearImgui::render() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DearImgui::e_initialize_(const baphy::EInitialize &e) {
  auto window = module_mgr->get<Window>();
  auto gfx = module_mgr->get<GfxContext>();

  IMGUI_CHECKVERSION();
  ctx_ = ImGui::CreateContext();
  ImGui::StyleColorsDark();
  io_ = &ImGui::GetIO();
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

  Module::e_initialize_(e);
}

void DearImgui::e_shutdown_(const baphy::EShutdown &e) {
  ImPlot::DestroyContext(implot_ctx_);
  implot_ctx_ = nullptr;

  (void)io_;
  ImGui::DestroyContext(ctx_);
  ctx_ = nullptr;

  Module::e_shutdown_(e);
}

} // namespace baphy
