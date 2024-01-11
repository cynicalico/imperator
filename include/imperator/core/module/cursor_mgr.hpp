#ifndef IMPERATOR_CORE_MODULE_CURSOR_MGR_HPP
#define IMPERATOR_CORE_MODULE_CURSOR_MGR_HPP

#define GLFW_INCLUDE_NONE
#include "imperator/core/module_mgr.hpp"
#include "imperator/core/module/window.hpp"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include <filesystem>
#include <memory>

namespace imp {
class CursorMgr;

class Cursor {
public:
  std::shared_ptr<CursorMgr> mgr{nullptr};

  explicit Cursor(std::shared_ptr<CursorMgr> mgr, GLFWcursor* glfw_cursor);
  ~Cursor();

  void set();

private:
  GLFWcursor* glfw_cursor_{nullptr};
};

class CursorMgr : public Module<CursorMgr>, public std::enable_shared_from_this<CursorMgr> {
  friend class Cursor;

public:
  std::shared_ptr<Window> window{nullptr};

  CursorMgr() : Module({EPI<Window>::name}) {}

  std::shared_ptr<Cursor> create(const std::filesystem::path& path, int xhot, int yhot);

protected:
  void r_initialize_(const E_Initialize& p) override;
  void r_shutdown_(const E_Shutdown& p) override;

private:
  GLFWcursor* curr_cursor_{nullptr};
  bool refresh_cursor_{false};
  std::vector<std::shared_ptr<Cursor>> cursors_{};
  std::unordered_map<ImGuiMouseCursor, GLFWcursor*> standard_cursors_{};

  ImGuiMouseCursor imgui_cursor_{ImGuiMouseCursor_Arrow};
  ImGuiMouseCursor imgui_cursor_last_{ImGuiMouseCursor_Arrow};
  bool imgui_hovered_{false};
  bool imgui_hovered_last_{false};

  bool is_imgui_hovered_();
  ImGuiMouseCursor get_imgui_cursor_();

  void set_(GLFWcursor* cursor);

  void r_end_frame_(const E_EndFrame& p);
};
} // namespace imp

IMP_PRAISE_HERMES(imp::CursorMgr);

#endif//IMPERATOR_CORE_MODULE_CURSOR_MGR_HPP
