#ifndef IMP_UTIL_MODULE_DEBUG_OVERLAY_HPP
#define IMP_UTIL_MODULE_DEBUG_OVERLAY_HPP

#include "imp/core/module_mgr.hpp"
#include "imp/gfx/color.hpp"
#include "imp/util/ds/trie.hpp"
#include "argparse/argparse.hpp"
#include <deque>

namespace imp {
class InputMgr;
class GfxContext;

using ConsoleParseFunc = std::function<void(argparse::ArgumentParser&)>;
using ConsoleCallbackFunc = std::function<void(argparse::ArgumentParser&)>;

class DebugOverlay : public Module<DebugOverlay> {
public:
  std::weak_ptr<InputMgr> inputs;
  std::weak_ptr<GfxContext> ctx;

  explicit DebugOverlay(const std::weak_ptr<ModuleMgr>& module_mgr);

  // DebugOverlay needs to be initialized first to ensure it can be used
  // by anything that wants it, but also uses some modules to get information
  // This is a circular relationship, and shared pointers cannot free this automatically
  void lateinit_modules();

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

IMP_PRAISE_HERMES(imp::DebugOverlay);

#endif//IMP_UTIL_MODULE_DEBUG_OVERLAY_HPP
