#include "baphy/gfx/internal/font/cp437_font.hpp"

#include "baphy/util/sops.hpp"
#include <iostream>
#include <fstream>

namespace baphy {

CP437Font::CP437Font(std::shared_ptr<Texture> tex, int char_w, int char_h, int row_offset)
    : tex_(std::move(tex)),
    char_w_(static_cast<float>(char_w)),
    char_h_(static_cast<float>(char_h)),
    row_offset_(row_offset) {}

CP437Font::CP437Font(std::shared_ptr<Texture> tex, const std::filesystem::path &spec, int char_h, int row_offset)
    : tex_(std::move(tex)),
    char_h_(static_cast<float>(char_h)),
    row_offset_(row_offset)
{
  std::ifstream ifs(spec);
  if (ifs.is_open()) {
    bounds_.resize(256);

    auto i = row_offset_ * 16;
    auto x_acc = 0;

    std::string s;
    while (i < 256 && ifs >> s) {
      auto w = stoi(s);
      if (!w)
        continue;

      bounds_[i] = {x_acc, w};
      i++;

      x_acc += w;
      if (i % 16 == 0)
        x_acc = 0;
    }

    use_bounds_ = true;

  } else
    BAPHY_LOG_ERROR("Failed to load CP437 spec: '{}'", spec.string());
}

void CP437Font::draw(float x, float y, float size, const std::string &text, const RGB &color) {
  float ox = x;

  for (const auto &ch: text) {
    if (ch < 0) continue;

    if (ch == '\n') {
      x = ox;
      y += char_h_ * size;
      continue;

    } else if (ch == ' ') {
      x += (use_bounds_ ? bounds_[ch][1] : char_w_) * size;
      continue;
    }

    float ch_x, ch_w;
    if (use_bounds_) {
      ch_x = bounds_[ch][0];
      ch_w = bounds_[ch][1];
    } else {
      ch_x = char_w_ * (ch % 16);
      ch_w = char_w_;
    }
    float ch_y = char_h_ * ((ch / 16) - row_offset_);
    float ch_h = char_h_;

    tex_->draw(x, y, ch_w * size, ch_h * size, ch_x, ch_y, ch_w, ch_h, color);
    x += ch_w * size;
  }
}

glm::vec2 CP437Font::bounds(float size, const std::string &text) {
  glm::vec2 b{};

  float x = 0;
  for (const auto &ch: text) {
    if (ch == '\n') {
      if (b.x < x)
        b.x = x;
      x = 0;
      b.y += char_h_ * size;
    } else
      x += (use_bounds_ ? bounds_[ch][1] : char_w_) * size;
  }
  if (b.x < x)
    b.x = x;
  if (b.x > 0 || b.y > 0)
    b.y += char_h_ * size;

  return b;
}

} // namespace baphy
