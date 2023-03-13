#include "myco/myco.hpp"
#include "myco/util/thread_pool.hpp"

class Indev : public myco::Application {
public:
  myco::HSV clear_color = myco::hsv(0, 1, 1);

  std::unique_ptr<myco::ThreadPool> pool;
  myco::JobRes<int> res;

  void initialize() override {
    application_show_debug_overlay();

    pool = std::make_unique<myco::ThreadPool>(2);
    res = pool->add_job([&](auto wait) -> int {
      int n = 0;
      n++; wait(1.0);
      n++; wait(1.0);
      n++; wait(1.0);
      return n + input->mouse.x;
    });
  }

  void update(double dt) override {
    if (input->pressed("escape"))
      window->set_should_close(true);

    if (res.avail())
      MYCO_LOG_INFO("Calc finished, n={}", res.get());

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
