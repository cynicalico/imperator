#include "baphy/baphy.hpp"

class Indev : public baphy::Application {
public:
  std::string tag{};

  void initialize() override {}

  void update(double dt) override {
    if (input->pressed("1")) {
      timer->after(2.0, [&] {
        BAPHY_LOG_INFO("After 2s");
      });
      BAPHY_LOG_INFO("pressed 1");
    }

    if (input->pressed("2")) {
      tag = timer->every(std::vector{0.1, 0.25, 0.5}, [&] {
        BAPHY_LOG_INFO("Tick!");
      });
    }

    if (input->pressed("3")) {
      timer->until(1.0, 10, [&] {
        BAPHY_LOG_INFO("Hello!");
        return input->down("4");
      });
    }

    if (input->pressed("5"))
      timer->cancel(tag);

    if (input->pressed("6"))
      timer->cancel_all();

    if (input->pressed("7"))
      timer->toggle(tag);
  }

  void draw() override {
    gfx->clear(baphy::rgb(0x222244));
  }
};

BAPHY_RUN(Indev,
  .title = "Indev",
  .size = {800, 800},
  .monitor_num = 0,
  .flags = baphy::WindowFlags::centered | baphy::WindowFlags::vsync
)
