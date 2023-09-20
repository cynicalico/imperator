#ifndef BAPHY_GFX_INTERNAL_FONT_CP437_HPP
#define BAPHY_GFX_INTERNAL_FONT_CP437_HPP

#include "baphy/gfx/internal/font/font.hpp"
#include "baphy/gfx/module/texture_batcher.hpp"

namespace baphy {

class CP437Font : public Font {
public:
  CP437Font(std::shared_ptr<Texture> tex, int char_w, int char_h, int row_offset = 0);
  CP437Font(std::shared_ptr<Texture> tex, const std::filesystem::path &spec, int char_h, int row_offset = 0);

  void draw(float x, float y, float size, const std::string &text, const RGB &color) override;

  glm::vec2 bounds(float size, const std::string &text) override;

private:
  std::shared_ptr<Texture> tex_{nullptr};
  float char_w_{0};
  float char_h_{0};
  int row_offset_{0};

  bool use_bounds_{false};
  std::vector<glm::vec2> bounds_{};
};

} // namespace baphy

#endif//BAPHY_GFX_INTERNAL_FONT_CP437_HPP
