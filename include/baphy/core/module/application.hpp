#ifndef BAPHY_CORE_MODULE_APPLICATION_HPP
#define BAPHY_CORE_MODULE_APPLICATION_HPP

#include "baphy/core/module/module.hpp"
#include "baphy/core/module/window.hpp"

namespace baphy {

class Application : public Module<Application> {
public:
  std::shared_ptr<Window> window{nullptr};

  Application() : Module<Application>({
      ModuleInfo<Window>::name
  }) {}

  ~Application() override = default;

protected:
  virtual void initialize();
  virtual void update(double dt);
  virtual void draw();

private:
  void initialize_(const EInitialize &e) override;
  void shutdown_(const EShutdown &e) override;
};

} // namespace baphy

BAPHY_DECLARE_MODULE(baphy::Application);

#endif//BAPHY_CORE_MODULE_APPLICATION_HPP
