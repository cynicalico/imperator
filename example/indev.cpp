#include "imperator/imperator.h"

class Indev final : public imp::Application {
public:
    std::shared_ptr<imp::G2D> g2d;
    imp::Sound select_sound{};

    explicit Indev(imp::ModuleMgr &module_mgr) : Application(module_mgr) {
        g2d = module_mgr_.create<imp::G2D>();

        select_sound = audio->load_sound(IMPERATOR_PATH("example/data/sound/【Yokune Ruko♂】God-ish【UTAU cover】.mp3"));
    }

    void update(double dt) override {
        if (dt == 0) { return; }
        if (inputs->pressed("escape")) { window->set_should_close(true); }

        if (inputs->pressed("mb_left")) { audio->play_sound(select_sound); }

        if (inputs->pressed("mb_right")) {
            if (select_sound.buffer != 0) {
                audio->unload_sound(select_sound);
            } else {
                select_sound = audio->load_sound(IMPERATOR_PATH("example/data/sound/【Yokune Ruko♂】God-ish【UTAU cover】.mp3"));
            }
        }
    }

    void draw() override { g2d->clear(imp::rgb(0x000000)); }
};

int main(int, char *[]) {
    imp::mainloop<Indev>({
        imp::WindowOpenParams{
                              .title = "Indev",
                              .size = {1280, 720},
                              .mode = imp::WindowMode::windowed,
                              .flags = imp::WindowFlags::centered | imp::WindowFlags::resizable
        },
        imp::GfxParams{.backend_version = {3, 3}, .vsync = false}
    });
}
