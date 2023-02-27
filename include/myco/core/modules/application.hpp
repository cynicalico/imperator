#pragma once

#include "myco/core/modules/module.hpp"
#include "myco/core/modules/window.hpp"

namespace myco {

class Application : public Module<Application> {
public:
  std::shared_ptr<Window> window{nullptr};

  Application() : Module<Application>({
    ModuleInfo<Window>::name
  }) {}

  ~Application() override {
    MYCO_LOG_DEBUG("Application destroyed");
  }

protected:
  virtual void initialize();
  virtual void update(double dt);
  virtual void draw();

private:
  void initialize_(const Initialize &e) override;
};

} // namespace myco

DECLARE_MYCO_MODULE(myco::Application)
