#include "myco/myco.hpp"
#include "myco/util/io.hpp"

class Indev : public myco::Application {
public:
  myco::HSV clear_color = myco::hsv("red");

  void initialize() override {
    window->open({
      .title = "Indev",
      .size = {500, 500},
      .flags = myco::WindowFlags::centered
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
    ctx->clear(clear_color.to_rgb());
  }
};

MYCO_RUN(Indev)
