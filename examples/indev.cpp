#include "myco/myco.hpp"

class Indev : public myco::Application {
  void initialize() override {
    MYCO_LOG_INFO("Indev module");
  }

  void update(double dt) override {

  }

  void draw() override {

  }
};

MYCO_RUN(Indev)
