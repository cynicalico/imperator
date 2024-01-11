#include "imperator/core/module/cursor_mgr.hpp"

#include "imperator/core/module/application.hpp"
#include "imperator/util/io.hpp"

namespace imp {
Cursor::Cursor(std::shared_ptr<CursorMgr> mgr, GLFWcursor* glfw_cursor)
  : mgr(std::move(mgr)), glfw_cursor_(glfw_cursor) {}

Cursor::~Cursor() {
  glfwDestroyCursor(glfw_cursor_);
  glfw_cursor_ = nullptr;
}

void Cursor::set() {
  mgr->set_(glfw_cursor_);
}

std::shared_ptr<Cursor> CursorMgr::create(const std::filesystem::path& path, int xhot, int yhot) {
  auto image_data = ImageData(path);
  auto glfw_image = image_data.glfw_image();
  auto glfw_cursor = glfwCreateCursor(&glfw_image, xhot, yhot);
  cursors_.emplace_back(std::make_shared<Cursor>(shared_from_this(), glfw_cursor));

  return cursors_.back();
}

void CursorMgr::r_initialize_(const E_Initialize& p) {
  window = module_mgr->get<Window>();

  standard_cursors_[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
  standard_cursors_[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
  standard_cursors_[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
  standard_cursors_[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
  standard_cursors_[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
  standard_cursors_[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
  standard_cursors_[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
  standard_cursors_[ImGuiMouseCursor_Hand] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
  standard_cursors_[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
  curr_cursor_ = standard_cursors_[ImGuiMouseCursor_Arrow];

  Hermes::sub<E_EndFrame>(module_name, {EPI<Application>::name}, IMP_MAKE_RECEIVER(E_EndFrame, r_end_frame_));

  Module::r_initialize_(p);
}

void CursorMgr::r_shutdown_(const E_Shutdown& p) {
  for (auto& p: standard_cursors_)
    glfwDestroyCursor(p.second);
  standard_cursors_.clear();

  Module::r_shutdown_(p);
}

bool CursorMgr::is_imgui_hovered_() {
  return ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow | ImGuiHoveredFlags_AllowWhenBlockedByPopup) ||
         ImGui::IsAnyItemHovered();
}

ImGuiMouseCursor CursorMgr::get_imgui_cursor_() {
  return ImGui::GetMouseCursor();
}

void CursorMgr::set_(GLFWcursor* cursor) {
  curr_cursor_ = cursor;
  refresh_cursor_ = true;
}

void CursorMgr::r_end_frame_(const E_EndFrame& p) {
  imgui_cursor_last_ = imgui_cursor_;
  imgui_hovered_last_ = imgui_hovered_;

  imgui_cursor_ = get_imgui_cursor_();
  imgui_hovered_ = is_imgui_hovered_();

  if ((imgui_hovered_ && !imgui_hovered_last_) ||
      imgui_cursor_ != ImGuiMouseCursor_Arrow ||
      (imgui_hovered_ && imgui_cursor_ != imgui_cursor_last_))
    glfwSetCursor(window->handle(), standard_cursors_[imgui_cursor_]);

  else if (refresh_cursor_ || (imgui_hovered_last_ && !imgui_hovered_)) {
    glfwSetCursor(window->handle(), curr_cursor_);

    if (refresh_cursor_)
      refresh_cursor_ = false;
  }
}
} // namespace imp
