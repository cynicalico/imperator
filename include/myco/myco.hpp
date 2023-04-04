#pragma once

#include "myco/core/modules/application.hpp"

#include "myco/core/engine.hpp"
#include "myco/core/scheduler.hpp"

#include "myco/gfx/context2d.hpp"

#include "myco/util/helpers.hpp"
#include "myco/util/io.hpp"
#include "myco/util/log.hpp"
#include "myco/util/platform.hpp"
#include "myco/util/rnd.hpp"
#include "myco/util/time.hpp"

#if defined(MYCO_INCLUDE_GL_WRAPPERS)
#include "myco/gfx/gl/shader.hpp"
#endif

#define MYCO_RUN(application, window_open_params) \
  int main(int, char *[]) {                       \
    auto e = std::make_shared<myco::Engine>();    \
    e->run<application>(window_open_params);      \
  }
