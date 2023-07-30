#include "baphy/baphy.hpp"

const auto HERE = std::filesystem::path(__FILE__).parent_path();

class Indev : public baphy::Application {
public:
  std::shared_ptr<baphy::Texture> tex{nullptr};
  std::shared_ptr<baphy::Font> font{nullptr};
  std::shared_ptr<baphy::Sound> s{nullptr};

  void initialize() override {
    tex = textures->load(HERE / "res" / "img" / "phantasm_10x10.png", true);
    font = fonts->cp437(baphy::rnd::base58(11), tex, 10, 10, 2);

    audio->open_device();
    audio->open_context();
    audio->make_current();

    s = audio->load(HERE / "res" / "sound" / "retro_fl.wav");
  }

  void update(double dt) override {
    if (input->pressed("escape")) window->set_should_close(true);

    if (input->pressed("mb_left"))
      s->play();
  }

  void draw() override {
    gfx->clear(baphy::rgb("black"));

//    dear->begin("audio devices"), [&] {
//      static std::vector<std::string> devices{};
//      if (ImGui::Button("query"))
//        devices = audio->available_devices();
//
//      static std::string current_item;
//      dear->combo("##combo", current_item.c_str()), [&] {
//        for (const auto & device : devices) {
//          bool is_selected = (current_item == device);
//          if (ImGui::Selectable(device.c_str(), is_selected))
//            current_item = device;
//          if (is_selected)
//            ImGui::SetItemDefaultFocus();
//        }
//      };
//    };

    auto s = std::string(R"(
Exsuls experimentum in azureus cubiculum!
Purpose, dogma and an apostolic underworld.
To the chilled pickles add leek, rhubarb, coffee and sour blood oranges.
God, taste me lubber, ye sunny jack!
Dozens of collision courses will be lost in courages like hypnosis in minerals.
)").substr(1);

    auto bounds = font->bounds(1, s);
    font->draw(std::floor(window->w() / 2) - std::floor(bounds.x / 2),
               std::floor(window->h() / 2) - std::floor(bounds.y / 2),
               1, s);
  }
};

BAPHY_RUN(Indev,
    .title = "Indev",
    .size = {1280, 720},
    .flags = baphy::WindowFlags::centered | baphy::WindowFlags::vsync,
    .debug_overlay_options_path = HERE / "debug_overlay_options.json"
)
