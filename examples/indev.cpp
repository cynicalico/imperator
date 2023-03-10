#include "myco/myco.hpp"
#include "myco/util/thread_pool.hpp"

class Indev : public myco::Application {
public:
  myco::HSV clear_color = myco::hsv("red");
  std::unique_ptr<myco::ThreadPool> pool;

  void initialize() override {
    window->open({
        .title = "Indev",
        .size = {500, 500},
        .flags = myco::WindowFlags::centered
    });
    ctx = std::make_shared<myco::Context2D>(*window);
    dear = std::make_unique<myco::Dear>(*window, *ctx);

    application_show_debug_overlay();

    pool = std::make_unique<myco::ThreadPool>(4);
  }

  void update(double dt) override {
    if (input->pressed("escape"))
      window->set_should_close(true);

    if (input->pressed("1"))
      pool->add_job([&](auto wait){
        MYCO_LOG_INFO("Tick!");
        wait(1);
        MYCO_LOG_INFO("Tock!");
        wait(1);
        MYCO_LOG_INFO("Tick!");
        wait(1);
        MYCO_LOG_INFO("Tock!");
      });
  }

  void draw() override {
    ctx->clear(clear_color.to_rgb());
  }
};

MYCO_RUN(Indev)
