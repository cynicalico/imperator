#include "myco/myco.hpp"
#include "myco/util/io.hpp"

void say_hello() {
  MYCO_LOG_INFO("Hello!");
}

class Indev : public myco::Application {
public:
  std::string every_tag;

  void initialize() override {
    window->open({
      .title = "Indev",
      .size = {500, 500},
      .flags = myco::WindowFlags::centered
    });
    ctx = std::make_shared<myco::Context2D>(*window);
    dear = std::make_unique<myco::Dear>(*window, *ctx);

    application_show_debug_overlay();

    every_tag = timer->every(1.0, say_hello);
  }

  void update(double dt) override {
    if (input->pressed("escape"))
      window->set_should_close(true);

    if (input->pressed("1"))
      timer->cancel(every_tag);
  }

  void draw() override {
    ctx->clear(myco::rgb("black"));
  }
};

MYCO_RUN(Indev)
