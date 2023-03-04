#include "myco/myco.hpp"

class Indev : public myco::Application {
public:
  void initialize() override {
    window->open({
      .title = "Indev",
      .size = {1280, 720},
      .flags = myco::WindowFlags::centered | myco::WindowFlags::vsync,
//      .win32_force_dark_mode = true
    });
    ctx = std::make_shared<myco::Context2D>(*window);
    dear = std::make_unique<myco::Dear>(*window, *ctx);

    application_show_debug_overlay();
  }

  void update(double dt) override {
    if (input->pressed("escape"))
      window->set_should_close(true);
  }

  void draw() override {
    ctx->clear(myco::rgb("black"));
  }
};

MYCO_RUN(Indev)
