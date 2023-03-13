#include "myco/myco.hpp"
#include "myco/util/thread_pool.hpp"

using namespace std::chrono_literals;

class Indev : public myco::Application {
public:
  myco::HSV clear_color = myco::hsv("red");

  std::unique_ptr<myco::ThreadPool> pool;
  myco::JobRes<int> res;

  void initialize() override {
    window->open({
        .title = "Indev",
        .size = {500, 500},
        .flags = myco::WindowFlags::centered | myco::WindowFlags::vsync
    });
    ctx = std::make_shared<myco::Context2D>(*window);
    dear = std::make_unique<myco::Dear>(*window, *ctx);

    application_show_debug_overlay();

    pool = std::make_unique<myco::ThreadPool>(4);
    res = pool->add_job([&](auto wait) -> int {
      int n = 0;
      wait(1.0);
      n++;
      wait(1.0);
      n++;
      wait(1.0);
      n++;
      return n;
    });
  }

  void update(double dt) override {
    if (input->pressed("escape"))
      window->set_should_close(true);

    if (res.avail())
      MYCO_LOG_INFO("Calc finished, n={}", res.get());
  }

  void draw() override {
    ctx->clear(clear_color.to_rgb());
  }
};

MYCO_RUN(Indev)
