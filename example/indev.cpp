#include "indev.hpp"

const auto CWD = std::filesystem::current_path();

class Indev : public imp::Application {
public:
  imp::FrameCounter fps{};

  void initialize() override;

  void update(double dt) override;

  void draw() override;
};

void Indev::initialize() {}

void Indev::update(double dt) {
  fps.update();

  if (input->released("a")) {
    IMPERATOR_LOG_INFO("a released!");
  }

  if (input->down("b", imp::Mods::ctrl)) {
    IMPERATOR_LOG_INFO("ctrl+b held");
  }

  if (input->down("c", imp::Mods::ctrl, 0.5)) {
    IMPERATOR_LOG_INFO("Holding c");
  }

  if (input->down("left_control", 0.5, 2)) {
    IMPERATOR_LOG_INFO("Holding lctrl");
  }
}

void Indev::draw() {
  gfx->gl.ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  gfx->gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  dear->new_frame();

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {1, 1});
  ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, {0, 0});
  ImGui::SetNextWindowPos({0, 0});
  if (ImGui::Begin("FPS", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("%s", fmt::format("{:.2f} fps", fps.fps()).c_str());
    ImGui::Text("%s", fmt::format("{:.2f} MB", imp::memusage_mb()).c_str());
  }
  ImGui::End();
  ImGui::PopStyleVar(2);

  dear->render();
}

int main(int, char*[]) {
  auto e = imp::Engine();
  e.run_application<Indev>(imp::InitializeParams{
    .title = "Indev",
    .size = {1280, 720},
    .flags = imp::WindowFlags::centered // | imp::WindowFlags::vsync
  });
}
