#ifndef BAPHY_GFX_ENUM_TYPES_HPP
#define BAPHY_GFX_ENUM_TYPES_HPP

#include "baphy/util/enum_bitops.hpp"
#include "glad/gl.h"

namespace baphy {

enum class ClearBit {
  stencil = GL_STENCIL_BUFFER_BIT,
  color = GL_COLOR_BUFFER_BIT,
  depth = GL_DEPTH_BUFFER_BIT
};

} // namespace baphy

ENABLE_BITOPS(baphy::ClearBit);

#endif//BAPHY_GFX_ENUM_TYPES_HPP
