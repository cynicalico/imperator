#include "imperator/imperator.h"

class Indev : public imp::Application {
public:
  explicit Indev(std::shared_ptr<imp::Engine> engine)
    : Application(std::move(engine)) {}
};

int main(int, char*[]) {
  imp::Engine::create()->run_application<Indev>(imp::WindowOpenParams{
    .title = "Indev",
    .size = {1280, 720},
    .flags = imp::WindowFlags::centered
  });
}
