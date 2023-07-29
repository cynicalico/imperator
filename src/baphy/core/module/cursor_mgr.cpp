#include "baphy/core/module/cursor_mgr.hpp"

#include "baphy/core/module/application.hpp"

namespace baphy {

bool CursorMgr::imgui_hovered() {
  return received_hovered_ || received_hovered_last_;
}

ImGuiMouseCursor CursorMgr::imgui_cursor() {
  if (imgui_cursor_last_ != ImGuiMouseCursor_None)
    return imgui_cursor_last_;
  return imgui_cursor_;
}

void CursorMgr::e_initialize_(const EInitialize &e) {
  EventBus::sub<EEndFrame>(module_name, {EPI<Application>::name}, [&](const auto &e) { e_end_frame_(e); });
  EventBus::sub<EImguiWindowHovered>(module_name, [&](const auto &e) { e_imgui_window_hovered_(e); });
  EventBus::sub<EImguiCursor>(module_name, [&](const auto &e) { e_imgui_cursor_(e); });

  Module::e_initialize_(e);
}

void CursorMgr::e_shutdown_(const EShutdown &e) {
  Module::e_shutdown_(e);
}

void CursorMgr::e_end_frame_(const EEndFrame &e) {
  received_hovered_last_ = received_hovered_;
  received_hovered_ = false;

  imgui_cursor_last_ = imgui_cursor_;
  imgui_cursor_ = ImGuiMouseCursor_None;
}

void CursorMgr::e_imgui_window_hovered_(const EImguiWindowHovered &e) {
  received_hovered_ = true;
}

void CursorMgr::e_imgui_cursor_(const EImguiCursor &e) {
  imgui_cursor_ = e.cursor;
}

} // namespace baphy
