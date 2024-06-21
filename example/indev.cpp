#include "imperator/imperator.h"
#include "imperator/module/gfx/2d/g2d.h"

class Indev final : public imp::Application {
public:
    std::shared_ptr<imp::G2D> g2d;

    explicit Indev(imp::ModuleMgr &module_mgr) : Application(module_mgr) {
        g2d = module_mgr_.create<imp::G2D>();
    }

    void update(double dt) override {
        if (dt == 0) { return; }
        if (inputs->pressed("escape")) { window->set_should_close(true); }

        if (inputs->pressed("mb_left")) {
            IMPERATOR_LOG_INFO("mouse pressed {} {}", inputs->mouse_x(), inputs->mouse_y());
        }
    }

    void draw() override {
        g2d->clear(imp::rgb(0x000000));
    }
};

int main(int, char *[]) {
    imp::mainloop<Indev>(
        {
            imp::WindowOpenParams{
                .title = "Indev",
                .size = {1280, 720},
                .mode = imp::WindowMode::windowed,
                .flags = imp::WindowFlags::centered | imp::WindowFlags::resizable
            },
            imp::GfxParams{
                .backend_version = {3, 3},
                .vsync = false
            }
        }
    );
}
