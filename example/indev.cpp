#include "baphy/baphy.hpp"

class Indev : public baphy::Application {
public:
  void initialize() override {}

  void update(double dt) override {}

  void draw() override {
    gfx->clear(baphy::rgb(0x222244));
  }
};

int main(int, char *[]) {
  auto e = std::make_unique<baphy::Engine>();
  e->run_application<Indev>({
      .title = "Indev",
      .size = {500, 500},
      .monitor_num = 0,
      .flags = baphy::WindowFlags::centered
  });
}
