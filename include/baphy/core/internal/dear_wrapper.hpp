/* The code in this file is basically just https://github.com/kfsone/imguiwrap/blob/main/src/imguiwrap.dear.h
 * That repo is not frequently updated and I wanted to add wrappers that were specific to this library
 *
 * I may look into a way to automate this code creation... I'd pull req the original repo if that happened
 */

#ifndef BAPHY_DEAR_WRAPPER_HPP
#define BAPHY_DEAR_WRAPPER_HPP

#include "imgui.h"

namespace baphy {

template<typename Base, bool ForceDtor = false>
struct ScopeWrapper {
  static constexpr bool force_dtor = ForceDtor;

protected:
  const bool should_begin_;

public:
  explicit constexpr ScopeWrapper(bool should_begin) noexcept
      : should_begin_{should_begin} {}

  ~ScopeWrapper() noexcept {
    if constexpr (!force_dtor) {
      if (!should_begin_)
        return;
    }
    Base::dtor();
  }

  ScopeWrapper(const ScopeWrapper&) = delete;
  ScopeWrapper& operator=(const ScopeWrapper&) = delete;

  template<typename Body>
  constexpr bool operator, (Body body) const noexcept {
    if (should_begin_)
      body();
    return should_begin_;
  }

  explicit constexpr operator bool() const noexcept { return should_begin_; }
};

struct Begin : public ScopeWrapper<Begin, true> {
  Begin(const char *title, bool *open, ImGuiWindowFlags flags) noexcept
      : ScopeWrapper(ImGui::Begin(title, open, flags)) {}
  static void dtor() noexcept { ImGui::End(); }
};

struct Child : public ScopeWrapper<Child, true> {
  Child(const char* title, const ImVec2& size, bool border, ImGuiWindowFlags flags) noexcept
      : ScopeWrapper(ImGui::BeginChild(title, size, border, flags)) {}
  Child(ImGuiID id, const ImVec2& size, bool border, ImGuiWindowFlags flags) noexcept
      : ScopeWrapper(ImGui::BeginChild(id, size, border, flags)) {}
  static void dtor() noexcept { ImGui::EndChild(); }
};

struct Group : public ScopeWrapper<Group, true> {
  Group() noexcept : ScopeWrapper(true) { ImGui::BeginGroup(); }
  static void dtor() noexcept { ImGui::EndGroup(); }
};

struct Combo : public ScopeWrapper<Combo> {
  Combo(const char* label, const char* preview_value, ImGuiComboFlags flags) noexcept
      : ScopeWrapper<Combo>(ImGui::BeginCombo(label, preview_value, flags)) {}
  static void dtor() noexcept { ImGui::EndCombo(); }
};

struct TreeNode : public ScopeWrapper<TreeNode> {
  TreeNode(const char* label) noexcept
      : ScopeWrapper(ImGui::TreeNode(label)) {}
  TreeNode(const char* str_id, const char* label) noexcept
      : ScopeWrapper(ImGui::TreeNode(str_id, "%s", label)) {}
  TreeNode(const void* ptr_id, const char* label) noexcept
      : ScopeWrapper(ImGui::TreeNode(ptr_id, "%s", label)) {}
  static void dtor() noexcept { ImGui::TreePop(); }
};

struct TreeNodeEx : public ScopeWrapper<TreeNodeEx> {
  TreeNodeEx(const char* label, ImGuiTreeNodeFlags flags) noexcept
      : ScopeWrapper(ImGui::TreeNodeEx(label, flags)) {}
  TreeNodeEx(const char* str_id, ImGuiTreeNodeFlags flags, const char* label) noexcept
      : ScopeWrapper(ImGui::TreeNodeEx(str_id, flags, "%s", label)) {}
  TreeNodeEx(const void* ptr_id, ImGuiTreeNodeFlags flags, const char* label) noexcept
      : ScopeWrapper(ImGui::TreeNodeEx(ptr_id, flags, "%s", label)) {}
  static void dtor() noexcept { ImGui::TreePop(); }
};

struct ListBox : public ScopeWrapper<ListBox> {
  ListBox(const char* label, const ImVec2& size) noexcept
      : ScopeWrapper(ImGui::BeginListBox(label, size)) {}
  static void dtor() noexcept { ImGui::EndListBox(); }
};

struct MenuBar : public ScopeWrapper<MenuBar> {
  MenuBar() noexcept : ScopeWrapper(ImGui::BeginMenuBar()) {}
  static void dtor() noexcept { ImGui::EndMenuBar(); }
};

struct MainMenuBar : public ScopeWrapper<MainMenuBar> {
  MainMenuBar() noexcept : ScopeWrapper(ImGui::BeginMainMenuBar()) {}
  static void dtor() noexcept { ImGui::EndMainMenuBar(); }
};

struct Menu : public ScopeWrapper<Menu> {
  Menu(const char* label, bool enabled = true) noexcept
      : ScopeWrapper(ImGui::BeginMenu(label, enabled)) {}
  static void dtor() noexcept { ImGui::EndMenu(); }
};

struct Tooltip : public ScopeWrapper<Tooltip> {
  Tooltip() noexcept : ScopeWrapper(ImGui::BeginTooltip()) {}
  static void dtor() noexcept { ImGui::EndTooltip(); }
};

struct CollapsingHeader : public ScopeWrapper<CollapsingHeader> {
  CollapsingHeader(const char* label, ImGuiTreeNodeFlags flags = 0) noexcept
      : ScopeWrapper(ImGui::CollapsingHeader(label, flags)) {}
  inline static void dtor() noexcept {}
};

struct Popup : public ScopeWrapper<Popup> {
  Popup(const char* str_id, ImGuiWindowFlags flags = 0) noexcept
      : ScopeWrapper<Popup>(ImGui::BeginPopup(str_id, flags)) {}
  static void dtor() noexcept { ImGui::EndPopup(); }
};

struct PopupModal : public ScopeWrapper<PopupModal> {
  PopupModal(const char* name, bool* p_open = NULL, ImGuiWindowFlags flags = 0) noexcept
      : ScopeWrapper<PopupModal>(ImGui::BeginPopupModal(name, p_open, flags)) {}
  static void dtor() noexcept { ImGui::EndPopup(); }
};

struct Table : public ScopeWrapper<Table> {
  Table(const char* str_id, int column, ImGuiTableFlags flags = 0, const ImVec2& outer_size = ImVec2(0.0f, 0.0f), float inner_width = 0.0f) noexcept
      : ScopeWrapper<Table>(ImGui::BeginTable(str_id, column, flags, outer_size, inner_width)) {}
  static void dtor() noexcept { ImGui::EndTable(); }
};

struct TabBar : public ScopeWrapper<TabBar> {
  TabBar(const char* name, ImGuiTabBarFlags flags) noexcept
      : ScopeWrapper(ImGui::BeginTabBar(name, flags)) {}
  static void dtor() noexcept { ImGui::EndTabBar(); }
};

struct TabItem : public ScopeWrapper<TabItem> {
  TabItem(const char* name, bool* open, ImGuiTabItemFlags flags) noexcept
      : ScopeWrapper(ImGui::BeginTabItem(name, open, flags)) {}
  static void dtor() noexcept { ImGui::EndTabItem(); }
};

struct ChildFrame : public ScopeWrapper<ChildFrame, true> {
  ChildFrame(ImGuiID id, const ImVec2& size, ImGuiWindowFlags flags = 0) noexcept
      : ScopeWrapper(ImGui::BeginChild(id, size, flags)) {}
  static void dtor() noexcept { ImGui::EndChildFrame(); }
};

} // namespace baphy

#endif//BAPHY_DEAR_WRAPPER_HPP
