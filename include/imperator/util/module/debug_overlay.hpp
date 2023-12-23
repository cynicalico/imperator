#ifndef IMPERATOR_UTIL_MODULE_DEBUG_OVERLAY_HPP
#define IMPERATOR_UTIL_MODULE_DEBUG_OVERLAY_HPP

/* This module depends on ImGui being initialized before the game loop starts.
 * It is not listed as a dependency in the constructor so other modules can add
 * controls the DebugOverlay if desired during their initialization (practically,
 * this module needs to be initialzed as the very first module)
 */

#include "imperator/core/module_mgr.hpp"

namespace imp {
class GfxContext;

class DebugOverlay : public Module<DebugOverlay> {
public:
  std::shared_ptr<GfxContext> gfx{nullptr};

  DebugOverlay() : Module() {}

  template<typename T>
  void add_tab(const std::string& name, T&& f);

protected:
  void r_initialize_(const E_Initialize& p) override;
  void r_shutdown_(const E_Shutdown& p) override;

private:
#if defined(NDEBUG)
  const char *BUILD_TYPE{" (release)"};
#else
  const char* BUILD_TYPE{" (debug)"};
#endif
  double fps{0};
  std::map<std::string, std::function<void()>> tabs{};

  void r_update_(const E_Update& p);
  void r_draw_(const E_Draw& p);
};

template<typename T>
void DebugOverlay::add_tab(const std::string& name, T&& f) {
  tabs.emplace(name, std::forward<T>(f));
}
} // namespace imp

IMPERATOR_PRAISE_HERMES(imp::DebugOverlay);

#endif//IMPERATOR_UTIL_MODULE_DEBUG_OVERLAY_HPP
