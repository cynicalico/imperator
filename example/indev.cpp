#include "baphy/baphy.hpp"
#include "baphy/util/noise/opensimplex.hpp"

const auto HERE = std::filesystem::path(__FILE__).parent_path();

class Indev : public baphy::Application {
public:
  double ox{0};
  double oy{0};
  double z{0};

  double x_dt{0.001};
  double y_dt{0.001};
  double z_dt{0.001};

  int s{8};
  int octaves{8};
  double persistence{0.7};

  void initialize() override {}

  void update(double dt) override {
    if (input->pressed("escape")) window->set_should_close(true);

    z += dt * z_dt;
    ox += dt * x_dt;
    oy += dt * y_dt;
  }

  void draw() override {
    gfx->clear(baphy::rgb("black"));

    dear->begin("Noise parameters", nullptr, ImGuiWindowFlags_AlwaysAutoResize), [&] {
      ImGui::InputDouble("x_dt", &x_dt, 0.0, 0.0, "%.6f", ImGuiInputTextFlags_EnterReturnsTrue);
      ImGui::InputDouble("y_dt", &y_dt, 0.0, 0.0, "%.6f", ImGuiInputTextFlags_EnterReturnsTrue);
      ImGui::InputDouble("z_dt", &z_dt, 0.0, 0.0, "%.6f", ImGuiInputTextFlags_EnterReturnsTrue);
      ImGui::InputInt("s", &s);
      ImGui::InputInt("octaves", &octaves);
      ImGui::InputDouble("persistence", &persistence);
    };

    for (std::size_t x = 0; x < window->w(); x += s)
      for (std::size_t y = 0; y < window->h(); y += s) {
        double v = baphy::opensimplex::octave3d_improve_xy(
            ox + ((x + (s / 2.0)) / (double)window->w()),
            oy + ((y + (s / 2.0)) / (double)window->h()),
            z,
            octaves, persistence
        );
        primitives->square(x, y, s, baphy::rgb(v * 255, v * 255, v * 255));
      }
  }
};

BAPHY_RUN(Indev,
    .title = "Indev",
    .size = {1280, 960},
    .monitor_num = 0,
    .flags = baphy::WindowFlags::centered | baphy::WindowFlags::vsync,
    .debug_overlay_options_path = HERE / "debug_overlay_options.json"
)
