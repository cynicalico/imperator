#ifndef BAPHY_GFX_INTERNAL_FONT_FONT_HPP
#define BAPHY_GFX_INTERNAL_FONT_FONT_HPP

#include "glm/vec2.hpp"
#include <string>

namespace baphy {

class Font {
public:
  virtual void draw(float x, float y, float size, const std::string &text) = 0;

  virtual glm::vec2 bounds(float size, const std::string &text) = 0;
};

} // namespace baphy

#endif//BAPHY_GFX_INTERNAL_FONT_FONT_HPP
