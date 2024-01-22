#include "imp/core/module/cursor_mgr.hpp"

#include "imp/core/module/application.hpp"
#include "imp/util/io.hpp"

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

CursorMgr::CursorMgr(const std::weak_ptr<ModuleMgr>& module_mgr): Module(module_mgr) {
  window = module_mgr.lock()->get<Window>();

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

  IMP_HERMES_SUB(E_EndFrame, module_name, r_end_frame_, Application);
}

CursorMgr::~CursorMgr() {
  for (auto& p: standard_cursors_)
    glfwDestroyCursor(p.second);
  standard_cursors_.clear();
}

std::shared_ptr<Cursor> CursorMgr::create(const std::filesystem::path& path, int xhot, int yhot) {
  auto image_data = ImageData(path);
  auto glfw_image = image_data.glfw_image();
  auto glfw_cursor = glfwCreateCursor(&glfw_image, xhot, yhot);
  cursors_.emplace_back(std::make_shared<Cursor>(shared_from_this(), glfw_cursor));

  return cursors_.back();
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
