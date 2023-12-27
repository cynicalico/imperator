#ifndef IMPERATOR_UTIL_MODULE_DEBUG_OVERLAY_HPP
#define IMPERATOR_UTIL_MODULE_DEBUG_OVERLAY_HPP

#include "imperator/core/module_mgr.hpp"
#include "imperator/gfx/color.hpp"
#include "imperator/util/ds/trie.hpp"
#include "argparse/argparse.hpp"
#include <deque>

namespace imp {
class InputMgr;
class GfxContext;

using ConsoleParseFunc = std::function<void(argparse::ArgumentParser&)>;
using ConsoleCallbackFunc = std::function<void(argparse::ArgumentParser&)>;

class DebugOverlay : public Module<DebugOverlay> {
public:
  std::shared_ptr<InputMgr> inputs{nullptr};
  std::shared_ptr<GfxContext> gfx{nullptr};

  /* ImGui is required for display
   * GfxContext is required for vsync status
   *
   * Required modules are not listed as dependencies because this module must be
   * initialized first so other modules can add controls if they desire.
   */
  DebugOverlay() = default;

  template<typename T>
  void add_tab(const std::string& name, T&& f);

  void set_flying_log_enabled(bool enabled);
  bool is_flying_log_enabled() const;
  void set_flying_log_limit(std::size_t max_lines);
  void set_flying_log_timeout(double timeout);

  void register_console_cmd(
    const std::string& name,
    const ConsoleParseFunc&& parser_setup,
    const ConsoleCallbackFunc&& callback
  );
  void set_console_binding(const std::string& binding);

protected:
  void r_initialize_(const E_Initialize& p) override;
  void r_shutdown_(const E_Shutdown& p) override;

private:
  const float WINDOW_EDGE_PADDING = 4.0f;

#if defined(NDEBUG)
  const char* BUILD_TYPE{" (release)"};
#else
  const char* BUILD_TYPE{" (debug)"};
#endif
  double fps{0};
  std::map<std::string, std::function<void()>> tabs{};

  struct FlyingLogLine {
    std::string s{};
    RGB color{};
    double acc{0};
    double fade_max{0};
  };
  struct {
    bool enabled{false};
    std::deque<FlyingLogLine> lines{};
    std::size_t max_lines{20};
    double timeout{5};
  } flying_log_{};

  struct {
    bool enabled{false};
    std::string binding{};

    std::string input{};
    std::string last_pre{};

    Trie<std::string> trie{};
    std::vector<std::pair<std::string, std::optional<std::string>>> matches{};

    std::unordered_map<std::string, ConsoleParseFunc> parsers{};
    std::unordered_map<std::string, ConsoleCallbackFunc> callbacks{};
  } console_{};

  void r_update_(const E_Update& p);
  void r_draw_(const E_Draw& p);
  void r_log_msg_(const E_LogMsg& p);

  glm::vec2 window_size_{0, 0};

  void r_glfw_window_size_(const E_GlfwWindowSize& p);
};

template<typename T>
void DebugOverlay::add_tab(const std::string& name, T&& f) {
  tabs.emplace(name, std::forward<T>(f));
}
} // namespace imp

IMPERATOR_PRAISE_HERMES(imp::DebugOverlay);

#endif//IMPERATOR_UTIL_MODULE_DEBUG_OVERLAY_HPP
