#pragma once

#include "myco/core/modules/module.hpp"

namespace myco {

class Window : public Module<Window> {
public:
  int x{0}, y{0};

  Window() : Module<Window>() {}
  ~Window() override {
    MYCO_LOG_DEBUG("Window destroyed");
  }

private:
  void initialize_(const Initialize &e) override;
};

} // namespace myco

DECLARE_MYCO_MODULE(myco::Window)
