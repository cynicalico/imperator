#ifndef BAPHY_GFX_INTERNAL_FONT_CP437_HPP
#define BAPHY_GFX_INTERNAL_FONT_CP437_HPP

#include "baphy/gfx/internal/font/font.hpp"
#include "baphy/gfx/module/texture_batcher.hpp"

namespace baphy {

class CP437 : public Font {
public:
  CP437(std::shared_ptr<Texture> tex, int char_w, int char_h);

  void draw(float x, float y, float size, const std::string &text) override;

  glm::vec2 bounds(float size, const std::string &text) override;

private:
  std::shared_ptr<Texture> tex_{nullptr};
  float char_w_{0};
  float char_h_{0};
};

} // namespace baphy

#endif//BAPHY_GFX_INTERNAL_FONT_CP437_HPP
