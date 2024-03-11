#include "imperator/imperator.h"

class Foo : public imp::Module<Foo> {
public:
  explicit Foo(std::shared_ptr<imp::Engine> engine)
    : Module(std::move(engine)) {}
};

IMPERATOR_DECLARE_MODULE(Foo);

class Bar : public imp::Module<Bar> {
public:
  std::shared_ptr<Foo> foo;

  explicit Bar(std::shared_ptr<imp::Engine> engine)
    : Module(std::move(engine)) {
    foo = imp_engine->get_module<Foo>();
  }
};

IMPERATOR_DECLARE_MODULE(Bar);

class Qux : public imp::Module<Qux> {
public:
  std::shared_ptr<Foo> foo;

  explicit Qux(std::shared_ptr<imp::Engine> engine)
    : Module(std::move(engine)) {
    foo = imp_engine->get_module<Foo>();
  }
};

IMPERATOR_DECLARE_MODULE(Qux);

int main(int, char*[]) {
  const auto engine = imp::Engine::create();

  engine->create_module<Foo>();
  engine->create_module<Bar>();
  engine->create_module<Qux>();

  engine->send_event_nowait<imp::E_FreeEngine>();
}
