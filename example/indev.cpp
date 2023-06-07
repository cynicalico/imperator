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
    dear->begin("info"), [&]{
      dear->text("FPS: {:.2f}", fps.value());
      dear->tab_bar("tab_bar"), [&]{
        dear->tab_item("t1"), [&]{
          dear->text_colored(baphy::rgba(0xff000088), "tab a");
        };
        dear->tab_item("t2"), [&]{
          dear->text("tab b");
        };
        dear->tab_item("t3"), [&]{
          dear->text("tab c");
        };
        dear->tab_item("t4"), [&]{
          dear->text("tab d");
        };
      };
    };
    dear->render();
  }
};

int main(int, char *[]) {
  auto e = std::make_unique<baphy::Engine>();
  e->run_application<Indev>({
      .title = "Indev",
      .size = {1280, 960},
      .flags = baphy::WindowFlags::centered
  });
}
