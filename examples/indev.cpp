#include "myco/myco.hpp"

struct Foo {
  int a;
  int b;
};

struct Bar {
  double a;
  double b;
};

int main(int, char *[]) {
  myco::Scheduler::sub<Foo>("A", [](const auto &e) {
    fmt::print("A Foo {} {}\n", e.a, e.b);
  });
  myco::Scheduler::sub<Foo>("C", {"B"}, [](const auto &e) {
    fmt::print("C Foo {} {}\n", e.a, e.b);
  });
  myco::Scheduler::sub<Foo>("B", {"A"}, [](const auto &e) {
    fmt::print("B Foo {} {}\n", e.a, e.b);
  });

  myco::Scheduler::sub<Bar>("A", {"B"}, [](const auto &e) {
    fmt::print("A Bar {} {}\n", e.a, e.b);
  });
  myco::Scheduler::sub<Bar>("B", {"C"}, [](const auto &e) {
    fmt::print("B Bar {} {}\n", e.a, e.b);
  });
  myco::Scheduler::sub<Bar>("C", [](const auto &e) {
    fmt::print("C Bar {} {}\n", e.a, e.b);
  });

  myco::Scheduler::send<Foo>(1, 2);
  myco::Scheduler::send<Bar>(3.0, 4.0);
}
