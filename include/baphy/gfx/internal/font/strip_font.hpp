#ifndef BAPHY_STRIP_FONT_HPP
#define BAPHY_STRIP_FONT_HPP

#include "baphy/gfx/internal/font/font.hpp"
#include "baphy/gfx/module/texture_batcher.hpp"
#include <filesystem>

namespace baphy {

class StripFont : public Font {
public:
  StripFont(std::shared_ptr<Texture> tex, const std::string &characters);

  void draw(float x, float y, float size, const std::string &text, const RGB &color) override;

  glm::vec2 bounds(float size, const std::string &text) override;

private:
  std::shared_ptr<Texture> tex_{nullptr};
  float char_h_;
  float space_w_{0};

  std::unordered_map<char, glm::vec4> bounds_{};
};

} // namespace baphy

#endif//BAPHY_STRIP_FONT_HPP
