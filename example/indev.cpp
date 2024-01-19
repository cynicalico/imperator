#include "indev.hpp"

const auto CWD = std::filesystem::current_path();

class Indev : public imp::Application {
public:
  void initialize() override;
  void update(double dt) override;
  void draw() override;
};

void Indev::initialize() {
  debug_overlay->set_flying_log_enabled(true);
  debug_overlay->set_console_binding("grave_accent");
}

void Indev::update(double dt) {
  if (inputs->pressed("escape")) {
    window->set_should_close(true);
  }
}

void Indev::draw() {
  gfx->clear(imp::rgb("black"));
}

int main(int, char*[]) {
  auto e = imp::Engine();
  e.run_application<Indev>(imp::WindowOpenParams{
    .title = "Indev",
    .size = {1280, 720},
    .mode = imp::WindowMode::windowed,
    .flags = imp::WindowFlags::centered | imp::WindowFlags::vsync
  });
}
