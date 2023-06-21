#ifndef BAPHY_CORE_MODULE_DEAR_IMGUI_HPP
#define BAPHY_CORE_MODULE_DEAR_IMGUI_HPP

#include "baphy/core/internal/dear_wrapper.hpp"
#include "baphy/core/module/window.hpp"
#include "baphy/core/module_mgr.hpp"
#include "baphy/gfx/module/gfx_context.hpp"
#include "baphy/gfx/color.hpp"
#include "imgui.h"
#include "implot.h"
#include <memory>

namespace baphy {

class DearImgui : public Module<DearImgui> {
public:
  DearImgui() : Module<DearImgui>({EPI<GfxContext>::name, EPI<Window>::name}) {}

  ~DearImgui() override = default;

  void new_frame();

  void render();

  inline Begin begin(const char *title, bool *open = nullptr, ImGuiWindowFlags flags = 0) noexcept {
    return {title, open, flags};
  }

  inline Child child(const char* title, const ImVec2& size = glm::vec2(0, 0), bool border = false, ImGuiWindowFlags flags = 0) noexcept {
    return {title, size, border, flags};
  }

  inline Child child(ImGuiID id, const ImVec2& size = glm::vec2(0, 0), bool border = false, ImGuiWindowFlags flags = 0) noexcept {
    return {id, size, border, flags};
  }

  inline Group group() noexcept {
    return {};
  }

  inline Combo combo(const char* label, const char* preview_value, ImGuiComboFlags flags = 0) noexcept {
    return {label, preview_value, flags};
  }

  /*
  IMGUI_API void          PushStyleColor(ImGuiCol idx, ImU32 col);                        // modify a style color. always use this if you modify the style after NewFrame().
  IMGUI_API void          PushStyleColor(ImGuiCol idx, const ImVec4& col);
  IMGUI_API void          PopStyleColor(int count = 1);
  IMGUI_API void          PushStyleVar(ImGuiStyleVar idx, float val);                     // modify a style float variable. always use this if you modify the style after NewFrame().
  IMGUI_API void          PushStyleVar(ImGuiStyleVar idx, const ImVec2& val);             // modify a style ImVec2 variable. always use this if you modify the style after NewFrame().
  IMGUI_API void          PopStyleVar(int count = 1);
  IMGUI_API void          TextUnformatted(const char* text, const char* text_end = NULL); // raw text without formatting. Roughly equivalent to Text("%s", text) but: A) doesn't require null terminated string if 'text_end' is specified, B) it's faster, no memory copy is done, no buffer size limits, recommended for long chunks of text.
  */

  template<typename... Args>
  inline void text(fmt::string_view format, Args &&... args) {
    ImGui::Text("%s", fmt::vformat(format, fmt::make_format_args(args...)).c_str());
  }

  template<typename... Args>
  inline void text_colored(const RGB &color, fmt::string_view format, Args &&... args) {
    ImGui::TextColored(color.vec4(), "%s", fmt::vformat(format, fmt::make_format_args(args...)).c_str());
  }

  /*
  IMGUI_API void          TextDisabled(const char* fmt, ...)                              IM_FMTARGS(1); // shortcut for PushStyleColor(ImGuiCol_Text, style.Colors[ImGuiCol_TextDisabled]); Text(fmt, ...); PopStyleColor();
  IMGUI_API void          TextWrapped(const char* fmt, ...)                               IM_FMTARGS(1); // shortcut for PushTextWrapPos(0.0f); Text(fmt, ...); PopTextWrapPos();. Note that this won't work on an auto-resizing window if there's no other widgets to extend the window width, yoy may need to set a size using SetNextWindowSize().
  IMGUI_API void          LabelText(const char* label, const char* fmt, ...)              IM_FMTARGS(2); // display text+label aligned the same way as value+label widgets
  IMGUI_API void          BulletText(const char* fmt, ...)                                IM_FMTARGS(1); // shortcut for Bullet()+Text()
  IMGUI_API void          SeparatorText(const char* label);                               // currently: formatted text with an horizontal line
  */

  inline TreeNode tree_node(const char* label) noexcept {
    return {label};
  }

  template<typename... Args>
  inline TreeNode tree_node(const char* str_id, fmt::string_view format, Args &&... args) noexcept {
    return {str_id, fmt::vformat(format, fmt::make_format_args(args...)).c_str()};
  }

  template<typename... Args>
  inline TreeNode tree_node(const void* ptr_id, fmt::string_view format, Args &&... args) noexcept {
    return {ptr_id, fmt::vformat(format, fmt::make_format_args(args...)).c_str()};
  }

  inline TreeNodeEx tree_node_ex(const char* label, ImGuiTreeNodeFlags flags) noexcept {
    return {label, flags};
  }

  template<typename... Args>
  inline TreeNodeEx tree_node_ex(const char* str_id, ImGuiTreeNodeFlags flags, fmt::string_view format, Args &&... args) noexcept {
    return {str_id, flags, fmt::vformat(format, fmt::make_format_args(args...)).c_str()};
  }

  template<typename... Args>
  inline TreeNodeEx tree_node_ex(const void* ptr_id, ImGuiTreeNodeFlags flags, fmt::string_view format, Args &&... args) noexcept {
    return {ptr_id, flags, fmt::vformat(format, fmt::make_format_args(args...)).c_str()};
  }

  inline ListBox list_box(const char* label, const ImVec2& size = glm::vec2(0, 0)) noexcept {
    return {label, size};
  }

  inline MenuBar menu_bar() noexcept {
    return {};
  }

  inline MainMenuBar main_menu_bar() noexcept {
    return {};
  }

  inline Menu menu(const char* label, bool enabled = true) noexcept {
    return {label, enabled};
  }

  inline Tooltip tooltip() noexcept {
    return {};
  }

  inline CollapsingHeader collapsing_header(const char* label, ImGuiTreeNodeFlags flags = 0) noexcept {
    return {label, flags};
  }

  inline Popup popup(const char* str_id, ImGuiWindowFlags flags = 0) noexcept {
    return {str_id, flags};
  }

  inline PopupModal popup_modal(const char* name, bool* p_open = NULL, ImGuiWindowFlags flags = 0) noexcept {
    return {name, p_open, flags};
  }

  inline Table table(const char* str_id, int column, ImGuiTableFlags flags = 0, const ImVec2& outer_size = ImVec2(0.0f, 0.0f), float inner_width = 0.0f) noexcept {
    return {str_id, column, flags, outer_size, inner_width};
  }

  inline TabBar tab_bar(const char* name, ImGuiTabBarFlags flags = 0) noexcept {
    return {name, flags};
  }

  inline TabItem tab_item(const char* name, bool* open = nullptr, ImGuiTabItemFlags flags = 0) noexcept {
    return {name, open, flags};
  }

private:
  ImGuiContext *ctx_{nullptr};
  ImGuiIO *io_{nullptr};

  ImPlotContext *implot_ctx_{nullptr};

  void e_initialize_(const EInitialize &e) override;
  void e_shutdown_(const EShutdown &e) override;
};

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::DearImgui);

#endif//BAPHY_CORE_MODULE_DEAR_IMGUI_HPP
