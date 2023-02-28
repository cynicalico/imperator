#pragma once

#include "myco/core/modules/application.hpp"
#include "myco/core/modules/module.hpp"
#include "myco/core/modules/window.hpp"

#include "myco/core/engine.hpp"
#include "myco/core/scheduler.hpp"

#include "myco/gfx/context2d.hpp"

#include "myco/util/helpers.hpp"
#include "myco/util/log.hpp"
#include "myco/util/platform.hpp"
#include "myco/util/rnd.hpp"
#include "myco/util/time.hpp"

#define MYCO_RUN(application)                   \
  int main(int, char *[]) {                     \
    auto e = std::make_shared<myco::Engine>();  \
    e->run<application>();                      \
  }
