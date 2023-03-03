#include "myco/myco.hpp"

class Indev : public myco::Application {
public:
  void initialize() override {
    window->open({
      .title = "Indev",
      .size = {1280, 720},
      .flags = myco::WindowFlags::centered | myco::WindowFlags::vsync
    });
    ctx = std::make_shared<myco::Context2D>(*window);
    dear = std::make_unique<myco::Dear>(*window, *ctx);

    application_show_debug_overlay();
  }

  void update(double dt) override {
    if (input->pressed("1"))
      window->set_vsync(!window->vsync);
  }

  void draw() override {
    ctx->clear(myco::rgb(128, 128, 255 * myco::normalize(input->mouse.x, 0, window->w)));
  }
};

MYCO_RUN(Indev)
