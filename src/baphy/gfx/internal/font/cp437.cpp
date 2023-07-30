#include "baphy/gfx/internal/font/cp437.hpp"

namespace baphy {

CP437::CP437(std::shared_ptr<Texture> tex, int char_w, int char_h)
    : tex_(std::move(tex)), char_w_(static_cast<float>(char_w)), char_h_(static_cast<float>(char_h)) {}

void CP437::draw(float x, float y, float size, const std::string &text) {
  float ox = x;
  for (const auto &ch: text) {
    if (ch == '\n') {
      x = ox;
      y += char_h_ * size;
      continue;

    } else if (ch == ' ') {
      x += char_w_ * size;
      continue;
    }

    auto ch_x = char_w_ * (ch % 16);
    auto ch_y = char_h_ * ((ch / 16) - 2);
    tex_->draw(x, y, char_w_ * size, char_h_ * size, ch_x, ch_y, char_w_, char_h_);
    x += char_w_ * size;
  }
}

glm::vec2 CP437::bounds(float size, const std::string &text) {
  glm::vec2 b{};

  float x = 0;
  for (const auto &ch: text) {
    if (ch == '\n') {
      if (b.x < x)
        b.x = x;
      x = 0;
      b.y += char_h_ * size;
    } else
      x += char_w_ * size;
  }
  if (b.x < x)
    b.x = x;

  return b;
}

} // namespace baphy
