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
    }

    void draw() override {
        g2d->clear(imp::rgb(0x000000));
    }
};

int main(int, char *[]) {
    auto window_open_params = imp::WindowOpenParams{
        .title = "Indev",
        .size = {1280, 720},
        .mode = imp::WindowMode::windowed,
        .flags = imp::WindowFlags::centered | imp::WindowFlags::resizable
    };
    auto gfx_params = imp::GfxParams{
        .backend_version = {3, 3},
        .vsync = false
    };
    imp::mainloop<Indev>({window_open_params, gfx_params});
}
