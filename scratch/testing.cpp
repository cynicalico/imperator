#include "myco/myco.h"

class Testing : public myco::Application {
    void initialize() override {

    }

    void update(double dt) override {

    }

    void draw() override {

    }
};

MYCO_RUN(Testing, (myco::WindowOpenParams{
        .title = "Testing",
        .size = {800, 600},
        .flags = myco::WindowFlags::centered
}))
