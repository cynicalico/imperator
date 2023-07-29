#ifndef BAPHY_CORE_MODULE_CURSOR_MGR_HPP
#define BAPHY_CORE_MODULE_CURSOR_MGR_HPP

#define GLFW_INCLUDE_NONE
#include "baphy/core/module_mgr.hpp"
#include "baphy/core/module/window.hpp"
#include "GLFW/glfw3.h"
#include <filesystem>

namespace baphy {

class CursorMgr;

class Cursor {
public:
  std::shared_ptr<CursorMgr> mgr{nullptr};

  explicit Cursor(std::shared_ptr<CursorMgr> mgr, GLFWcursor *glfw_cursor);
  ~Cursor();

  void set();

private:
  GLFWcursor *glfw_cursor_{nullptr};
};

class CursorMgr : public Module<CursorMgr>, public std::enable_shared_from_this<CursorMgr> {
  friend class Cursor;

public:
  std::shared_ptr<Window> window{nullptr};

  CursorMgr() : Module<CursorMgr>({EPI<Window>::name}) {}
  ~CursorMgr() override = default;

  std::shared_ptr<Cursor> create(const std::filesystem::path &path, int xhot, int yhot);

private:
  GLFWcursor *curr_cursor_{nullptr};
  bool refresh_cursor_{false};
  std::vector<std::shared_ptr<Cursor>> cursors_{};
  std::unordered_map<ImGuiMouseCursor, GLFWcursor *> standard_cursors_{};

  ImGuiMouseCursor imgui_cursor_{ImGuiMouseCursor_Arrow};
  ImGuiMouseCursor imgui_cursor_last_{ImGuiMouseCursor_Arrow};
  bool imgui_hovered_{false};
  bool imgui_hovered_last_{false};

  bool is_imgui_hovered_();
  ImGuiMouseCursor get_imgui_cursor_();

  void set_(GLFWcursor *cursor);

  void e_initialize_(const EInitialize &e) override;
  void e_shutdown_(const EShutdown &e) override;

  void e_end_frame_(const EEndFrame &e);
};

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::CursorMgr);

#endif//BAPHY_CORE_MODULE_CURSOR_MGR_HPP
