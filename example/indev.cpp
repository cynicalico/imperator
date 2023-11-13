#include "baphy/baphy.hpp"

class Indev : public baphy::Application {
public:
  void initialize() override;

  void update(double dt) override;

  void draw() override;
};

void Indev::initialize() {}

void Indev::update(double dt) {}

void Indev::draw() {}

int main(int, char*[]) {
  auto e = baphy::Engine();
  e.run_application<Indev>(baphy::InitializeParams{
    .title = "Indev"
  });
}
