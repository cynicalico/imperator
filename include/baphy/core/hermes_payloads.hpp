#ifndef BAPHY_CORE_HERMES_PAYLOADS_HPP
#define BAPHY_CORE_HERMES_PAYLOADS_HPP

#include "hermes_payloads_types.hpp"
#include "spdlog/common.h"
#include <string>

// This gets defined when we include spdlog/common.hpp, but the
// intent is for it to get defined in the log.hpp header (or by
// the user)
#undef SPDLOG_ACTIVE_LEVEL

namespace baphy {

/* EVENTS */

struct E_Initialize {
  const InitializeParams params;
};

struct E_ShutdownEngine {};

struct E_Shutdown {};

struct E_LogMsg {
  std::string text;
  spdlog::level::level_enum level;
};

struct E_Update {
  double dt;
};

/* MESSAGES */

} // namespace baphy

#endif//BAPHY_CORE_HERMES_PAYLOADS_HPP
