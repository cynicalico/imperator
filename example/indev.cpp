#include "indev.hpp"

const auto CWD = std::filesystem::current_path();

class Indev : public imp::Application {
public:
  std::map<std::string, std::function<void()>> tabs{};

  imp::FrameCounter fps{};

  void initialize() override;
  void update(double dt) override;
  void draw() override;
};

void Indev::initialize() {
  tabs.emplace("Tab 2", [&] {
    ImGui::Text("wawa");
    if (ImGui::Button("Toggle vsync")) {
      gfx->set_vsync(!gfx->is_vsync());
    }
  });

  tabs.emplace("Tab 3", [&] {
    ImGui::Text("Stuff is happening");
  });
}

void Indev::update(double dt) {
  fps.update();

  if (input->pressed("1")) {
    gfx->set_vsync(!gfx->is_vsync());
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

  if (ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
    static std::string current_item{tabs.begin()->first};
    if (ImGui::BeginCombo("##combo", current_item.c_str())) {
      for (const auto& k: tabs | std::views::keys) {
        const bool is_selected = current_item == k;
        if (ImGui::Selectable(k.c_str(), is_selected)) {
          current_item = k;
        }
        if (is_selected) {
          ImGui::SetItemDefaultFocus();
        }
      }
      ImGui::EndCombo();
    }

    ImGui::Separator();
    tabs[current_item]();
  }
  ImGui::End();

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
