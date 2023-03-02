#include "myco/myco.hpp"

class Indev : public myco::Application {
public:
  void initialize() override {
    window->open({
      .title = "Indev",
      .size = {1280, 720},
      .flags = myco::WindowFlags::centered
    });
    ctx = std::make_shared<myco::Context2D>(*window);
    dear = std::make_unique<myco::Dear>(*window, *ctx);

    application_show_debug_overlay();
  }

  void update(double dt) override {

  }

  void draw() override {
    ctx->clear(myco::rgb(0xff0000));
  }
};

MYCO_RUN(Indev)
