#include "baphy/core/module/application.hpp"
#include "baphy/core/engine.hpp"
#include "baphy/util/log.hpp"

class Indev : public baphy::Application {
public:
  void initialize() override {}

  void update(double dt) override {}

  void draw() override {}
};

int main(int, char *[]) {
  auto e = std::make_unique<baphy::Engine>();
  e->run_application<Indev>();
}
