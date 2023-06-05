#ifndef BAPHY_CORE_MODULE_WINDOW_HPP
#define BAPHY_CORE_MODULE_WINDOW_HPP

#include "baphy/core/module/module.hpp"

namespace baphy {

class Window : public Module<Window> {
public:
  Window() : Module<Window>() {}

  ~Window() override = default;

private:
  void initialize_(const EInitialize &e) override;
  void shutdown_(const EShutdown &e) override;
};

} // namespace baphy

BAPHY_DECLARE_MODULE(baphy::Window);

#endif//BAPHY_CORE_MODULE_WINDOW_HPP
