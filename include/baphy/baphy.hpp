#ifndef BAPHY_BAPHY_HPP
#define BAPHY_BAPHY_HPP

#include "baphy/core/module/application.hpp"

#include "baphy/core/engine.hpp"
#include "baphy/core/event_bus.hpp"
#include "baphy/core/module_mgr.hpp"
#include "baphy/core/msgs.hpp"
#include "baphy/core/msgs_types.hpp"
#include "baphy/core/prio_list.hpp"

#include "baphy/gfx/color.hpp"
#include "baphy/gfx/gfx_enum_types.hpp"

#include "baphy/util/averagers.hpp"
#include "baphy/util/enum_bitops.hpp"
#include "baphy/util/helpers.hpp"
#include "baphy/util/io.hpp"
#include "baphy/util/log.hpp"
#include "baphy/util/memusage.hpp"
#include "baphy/util/platform.hpp"
#include "baphy/util/rnd.hpp"
#include "baphy/util/sops.hpp"
#include "baphy/util/time.hpp"
#include "baphy/util/type_id.hpp"

#ifndef BAPHY_NO_MACRO
#define BAPHY_RUN(a, ...)                                        \
  int main(int, char *[]) {                                      \
    auto e = std::make_unique<baphy::Engine>();                  \
    e->run_application<a>(baphy::WindowOpenParams{__VA_ARGS__}); \
  }
#endif//BAPHY_NO_MACRO

#endif//BAPHY_BAPHY_HPP
