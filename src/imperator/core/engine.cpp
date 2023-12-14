#include "imperator/core/engine.hpp"

#include "imperator/util/io.hpp"

namespace imp {
Engine::Engine() {
  module_mgr_ = std::make_shared<ModuleMgr>();

  Hermes::sub<E_ShutdownEngine>(EPI<Engine>::name, [&](const auto&) {
    received_shutdown_ = true;
  });

  Hermes::sub<E_GlfwSetWindowSize>(EPI<Engine>::name, [&](const auto& p) { r_glfw_set_window_size_(p); });
  Hermes::sub<E_GlfwSetWindowPos>(EPI<Engine>::name, [&](const auto& p) { r_glfw_set_window_pos_(p); });
  Hermes::sub<E_GlfwSetWindowTitle>(EPI<Engine>::name, [&](const auto& p) { r_glfw_set_window_title_(p); });
  Hermes::sub<E_GlfwSetWindowIcon>(EPI<Engine>::name, [&](const auto& p) { r_glfw_set_window_icon_(p); });
}

void Engine::r_glfw_set_window_size_(const E_GlfwSetWindowSize& p) {
  glfwSetWindowSize(p.window, p.width, p.height);
}

void Engine::r_glfw_set_window_pos_(const E_GlfwSetWindowPos& p) {
  glfwSetWindowPos(p.window, p.x, p.y);
}

void Engine::r_glfw_set_window_title_(const E_GlfwSetWindowTitle& p) {
  glfwSetWindowTitle(p.window, p.title.c_str());
}

void Engine::r_glfw_set_window_icon_(const E_GlfwSetWindowIcon& p) {
  std::vector<ImageData> images{};
  for (const auto &path: p.paths)
    images.emplace_back(path, 4);

  std::vector<GLFWimage> glfw_images{};
  for (auto &i: images)
    glfw_images.emplace_back(i.glfw_image());

  glfwSetWindowIcon(p.window, glfw_images.size(), &glfw_images[0]);
}

} // namespace imp
