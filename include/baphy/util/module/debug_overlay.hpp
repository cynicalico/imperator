#ifndef BAPHY_UTIL_MODULE_DEBUG_OVERLAY_HPP
#define BAPHY_UTIL_MODULE_DEBUG_OVERLAY_HPP

#include "baphy/core/module/dear_imgui.hpp"
#include "baphy/core/module/input_mgr.hpp"
#include "baphy/core/module/window.hpp"
#include "baphy/core/module_mgr.hpp"
#include "baphy/gfx/module/gfx_context.hpp"
#include "baphy/gfx/module/shader_mgr.hpp"
#include "TextEditor.h"
#include <deque>
#include <memory>

namespace baphy {

class DebugOverlay : public Module<DebugOverlay> {
public:
  std::shared_ptr<DearImgui> dear{nullptr};
  std::shared_ptr<InputMgr> input{nullptr};
  std::shared_ptr<GfxContext> gfx{nullptr};
  std::shared_ptr<ShaderMgr> shader_mgr{nullptr};
  std::shared_ptr<Window> window{nullptr};

  DebugOverlay() : Module<DebugOverlay>({
      EPI<DearImgui>::name,
      EPI<InputMgr>::name,
      EPI<GfxContext>::name,
      EPI<ShaderMgr>::name,
      EPI<Window>::name,
  }) {}

  ~DebugOverlay() override = default;

  void log_clear();
  void log_add(spdlog::level::level_enum level, const std::string &text);

  void log_draw(const char *title, bool *p_open = nullptr);

  void set_cmd_key(const std::string &binding);
  void set_cmd_callback(const std::string &prefix, std::function<void(std::string)> callback);

private:
  double fps_{};
  std::deque<std::uint64_t> timestamps_{};
  std::deque<double> dts_{};

  struct {
    bool show{false};
    std::string binding{};

    std::array<char, 512> buf{};
    std::unordered_map<std::string, std::function<void(std::string)>> callbacks{};
  } cmd_{};

  struct {
    bool show_fps{true};
    bool show_mem{false};
    bool show_latency{false};
  } general_{};

  struct {
    bool show{false};
    bool start_collapsed{true};

    ImGuiTextBuffer buf;
    ImGuiTextFilter filter;
    ImVector<int> line_offsets;
    ImVector<spdlog::level::level_enum> line_levels;
    bool autoscroll{true};
    bool wrapping{false};
    bool docked{true};

    std::unordered_map<spdlog::level::level_enum, ImU32> color_map{};
  } log_{};

  struct {
    bool show{false};
    bool start_collapsed{true};
  } controls_{};

  struct {
    bool show{false};
    bool start_collapsed{true};

    TextEditor te{};
  } shader_editor_{};

  struct {
    int x{};
    int y{};
    int w{};
    int h{};
    bool resizable{false};
    bool decorated{false};
    bool auto_iconify{false};
    bool floating{false};
    bool focus_on_show{false};
  } window_tab_{};

  struct {
    bool vsync{false};
  } gfx_tab_{};

  void update_window_state_();
  void update_gfx_state_();

  void e_initialize_(const EInitialize &e) override;
  void e_shutdown_(const EShutdown &e) override;

  void e_log_msg_(const ELogMsg &e);
  void e_update_(const EUpdate &e);
  void e_draw_(const EDraw &e);
};

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::DebugOverlay);

#endif//BAPHY_UTIL_MODULE_DEBUG_OVERLAY_HPP