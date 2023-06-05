#ifndef BAPHY_CORE_MSGS_TYPES_HPP
#define BAPHY_CORE_MSGS_TYPES_HPP

#include "baphy/util/enum_bitops.hpp"
#include "glm/vec2.hpp"
#include <string>

namespace baphy {

enum class WindowFlags {
  none       = 1 << 0,
  vsync      = 1 << 1,
  resizable  = 1 << 2,
  hidden     = 1 << 3,
  centered   = 1 << 4,
  fullscreen = 1 << 5,
  borderless = 1 << 6,
//  decorated  = 1 << 7,
};

struct WindowOpenParams {
  std::string title{"Baphy Window"};

  glm::ivec2 size{0, 0};
  glm::ivec2 pos{0, 0};

  int monitor_num{0};

  WindowFlags flags{WindowFlags::none};

  glm::ivec2 backend_version{3, 3};

  bool win32_force_light_mode{false};
  bool win32_force_dark_mode{false};
};

} // namespace baphy

ENABLE_BITOPS(baphy::WindowFlags);

#endif//BAPHY_CORE_MSGS_TYPES_HPP