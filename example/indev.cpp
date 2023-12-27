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

  debug_overlay->register_console_cmd(
    "foo",
    [](argparse::ArgumentParser& p) {
      p.add_argument("i").scan<'i', int>();
    },
    [](argparse::ArgumentParser& p) {
      IMPERATOR_LOG_INFO("Foo! i * 2: {}", p.get<int>("i") * 2);
    }
  );

  debug_overlay->register_console_cmd(
    "bar",
    [](argparse::ArgumentParser& p) {
      p.add_argument("i").scan<'i', int>();
      p.add_argument("j").scan<'i', int>();
    },
    [](argparse::ArgumentParser& p) {
      IMPERATOR_LOG_INFO("Bar! i * j: {}", p.get<int>("i") * p.get<int>("j"));
    }
  );

  debug_overlay->set_console_binding("grave_accent");
}

void Indev::update(double dt) {}

void Indev::draw() {
  gfx->gl.ClearColor(0.1f, 0.1f, 0.1f, 1.0f);
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
