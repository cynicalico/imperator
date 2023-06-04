#include "baphy/core/module/module.hpp"
#include "baphy/core/event_bus.hpp"
#include "baphy/util/log.hpp"

class Foo : public baphy::Module<Foo> {
public:
  Foo() : baphy::Module<Foo>() {}

  void qux() const {
    BAPHY_LOG_INFO("uwu");
  }
};
BAPHY_DECLARE_MODULE(Foo);

class Bar : public baphy::Module<Bar> {
public:
  Bar() : baphy::Module<Bar>({baphy::ModuleInfo<Foo>::name}) {}

private:
  void initialize_(const baphy::EInitialize &e) override {
    baphy::Module<Bar>::initialize_(e);

    module_mgr->get<Foo>()->qux();
  }
};
BAPHY_DECLARE_MODULE(Bar);

int main(int, char *[]) {
  auto mm = std::make_shared<baphy::ModuleMgr>();
  mm->create<Foo>();
  mm->create<Bar>();

  baphy::EventBus::send_nowait<baphy::EInitialize>(mm);
  baphy::EventBus::send_nowait<baphy::EShutdown>();
}


