#include "baphy/baphy.hpp"

class Indev : public baphy::Application {
public:
  baphy::Ticker t{1.0};

  void initialize() override {}

  void update(double dt) override {
    if (t.tick())
      BAPHY_LOG_INFO("FPS: {:.2f}", 1 / dt);
  }

  void draw() override {}
};

int main(int, char *[]) {
  auto e = std::make_unique<baphy::Engine>();
  e->run_application<Indev>({
    .title = "Indev",
    .size = {1280, 960},
    .flags = baphy::WindowFlags::centered | baphy::WindowFlags::vsync
  });
}
