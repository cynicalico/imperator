#include "indev.hpp"

const auto CWD = std::filesystem::current_path();

class Indev : public imp::Application {
public:
  void initialize() override;
  void update(double dt) override;
  void draw() override;
};

void Indev::initialize() {
  debug_overlay->add_tab("Tab 2", [&] {
    ImGui::Text("wawa");
    if (ImGui::Button("Toggle vsync")) {
      gfx->set_vsync(!gfx->is_vsync());
    }
  });

  debug_overlay->add_tab("Tab 3", [&] {
    ImGui::Text("Stuff is happening");
  });
}

void Indev::update(double dt) {
  if (inputs->pressed("1")) {
    gfx->set_vsync(!gfx->is_vsync());
  }
}

void Indev::draw() {
  gfx->gl.ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  gfx->gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

int main(int, char*[]) {
  auto e = imp::Engine();
  e.run_application<Indev>(imp::InitializeParams{
    .title = "Indev",
    .size = {1280, 720},
    .flags = imp::WindowFlags::centered // | imp::WindowFlags::vsync
  });
}
