#ifndef BAPHY_CORE_MODULE_APPLICATION_HPP
#define BAPHY_CORE_MODULE_APPLICATION_HPP

#include "baphy/core/module_mgr.hpp"

namespace baphy {

class Application : public Module<Application> {
public:
  Application() : Module({}) {}

  virtual void initialize() {}
  virtual void update(double dt) {}
  virtual void draw() {}

protected:
  void r_initialize_(const E_Initialize& e) override;
  void r_shutdown_(const E_Shutdown& e) override;
};

} // namespace baphy

BAPHY_PRAY_TO_HERMES(baphy::Application);

#endif//BAPHY_CORE_MODULE_APPLICATION_HPP
