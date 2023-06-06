#include "baphy/baphy.hpp"

class Indev : public baphy::Application {
public:
  baphy::EMA fps{0.5};

  void initialize() override {}

  void update(double dt) override {
    if (dt != 0)
      fps.update(1.0 / dt);
  }

  void draw() override {
    gfx->clear(baphy::rgb(0xff0000));

    dear->new_frame();
    ImGui::Begin("test");
    ImGui::Text("%s", fmt::format("{:.2f} fps", fps.value()).c_str());
    ImGui::End();
    dear->render();
  }
};

int main(int, char *[]) {
  auto e = std::make_unique<baphy::Engine>();
  e->run_application<Indev>({
      .title = "Indev",
      .size = {1280, 960},
      .flags = baphy::WindowFlags::centered | baphy::WindowFlags::vsync
  });
}
