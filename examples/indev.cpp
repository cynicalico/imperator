#include "myco/myco.hpp"

class Indev : public myco::Application {
public:
  myco::HSV clear_color = myco::hsv(0, 1, 1);

  void initialize() override {
    application_sticky("cc", clear_color);
    application_sticky("mx", input->mouse.x);
    application_sticky("my", "{:10}", input->mouse.y);
  }

  void update(double dt) override {
    if (input->pressed("escape"))
      window->set_should_close(true);

    if (input->pressed("f3"))
      application_toggle_debug_overlay();

    clear_color.h = std::fmod(clear_color.h + (10 * dt), 360.0);
  }

  void draw() override {
    ctx->clear(clear_color.to_rgb());
  }
};

MYCO_RUN(Indev, (myco::WindowOpenParams{
    .title = "Indev",
    .size = {500, 500},
    .flags = myco::WindowFlags::centered
}))
