#include "baphy/gfx/internal/font/strip_font.hpp"

namespace baphy {

StripFont::StripFont(std::shared_ptr<Texture> tex, const std::string &characters) : tex_(std::move(tex)) {
  auto image = tex_->get_image();
  char_h_ = image.h();

  auto col_empty = [&](std::size_t c) {
    for (std::size_t r = 0; r < image.h(); ++r)
      if (image(r, c, 0) != 0 || image(r, c, 1) != 0 || image(r, c, 2) != 0 || image(r, c, 3) != 0)
        return false;
    return true;
  };

  std::size_t start_col = 0;
  for (const auto &c: characters) {
    glm::vec4 b = {start_col, 0, 0, image.h()};

    for (; b.x + b.z < image.w() && !col_empty(b.x + b.z); ++b.z);

    bounds_[c] = b;
    space_w_ += b.z;

    start_col += b.z + 1;
  }

  space_w_ /= characters.size();
  space_w_ /= 2.0f;
  space_w_ = std::floor(space_w_);

  BAPHY_LOG_INFO("space_w_ {}", space_w_);
}

void StripFont::draw(float x, float y, float size, const std::string &text, const baphy::RGB &color) {
  float ox = x;
  for (const auto &ch: text) {
    if (ch == '\n') {
      x = ox;
      y += char_h_ * size;
      continue;

    } else if (ch == ' ') {
      x += space_w_ * size;
      continue;
    }

    auto it = bounds_.find(ch);
    if (it == bounds_.end())
      continue;

    auto b = it->second;
    tex_->draw(x, y, b.z * size, b.w * size, b.x, b.y, b.z, b.w, color);
    x += (b.z + 1) * size;
  }
}

glm::vec2 StripFont::bounds(float size, const std::string &text) {
  // TODO
  return {0, 0};
}

} // namespace baphy
