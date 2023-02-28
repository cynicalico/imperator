#include "myco/myco.hpp"

class Indev : public myco::Application {
public:
  std::shared_ptr<myco::Context2D> ctx{};

  void initialize() override {
    window->open({
      .title = "Indev",
      .size = {1280, 720},
      .flags = myco::WindowFlags::centered | myco::WindowFlags::vsync
    });
    ctx = std::make_shared<myco::Context2D>(*window);
  }

  void update(double dt) override {
    MYCO_LOG_INFO("dt: {}", 1 / dt);
  }

  void draw() override {
    ctx->clear(myco::rgb(0xff0000));
  }
};

MYCO_RUN(Indev)
