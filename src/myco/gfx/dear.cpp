#include "myco/gfx/dear.hpp"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace myco {

Dear::Dear(const Window &window, const Context2D &ctx) {
  IMGUI_CHECKVERSION();
  imgui_ctx_ = ImGui::CreateContext();
  ImGui::StyleColorsDark();
  io_ = &ImGui::GetIO();
  io_->IniFilename = nullptr;

  ImGui_ImplGlfw_InitForOpenGL(window.glfw_handle, true);
  std::string glsl_version;
  if      (ctx.version == glm::ivec2{2, 0}) glsl_version = "#version 110";
  else if (ctx.version == glm::ivec2{2, 1}) glsl_version = "#version 120";
  else if (ctx.version == glm::ivec2{3, 0}) glsl_version = "#version 130";
  else if (ctx.version == glm::ivec2{3, 1}) glsl_version = "#version 140";
  else if (ctx.version == glm::ivec2{3, 2}) glsl_version = "#version 150";
  else
    glsl_version = fmt::format("#version {}{}0 core", ctx.version.x, ctx.version.y);
  ImGui_ImplOpenGL3_Init(glsl_version.c_str());

  MYCO_LOG_DEBUG("Initialized ImGui");

  implot_ctx_ = ImPlot::CreateContext();

  MYCO_LOG_DEBUG("Initialized ImPlot");
}

void Dear::new_frame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void Dear::render() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

} // namespace myco
