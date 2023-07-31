#ifndef BAPHY_GFX_SPRITESHEET_HPP
#define BAPHY_GFX_SPRITESHEET_HPP

#include "baphy/gfx/module/texture_batcher.hpp"
#include <tuple>

namespace baphy {

class Spritesheet {
public:
  Spritesheet(std::shared_ptr<Texture> tex, const std::filesystem::path &path_to_spec);

  std::vector<std::string> sprite_names();

  void set_scale(float s);

  float w(const std::string &sprite_name);
  float h(const std::string &sprite_name);

  void draw(const std::string &sprite_name, float x, float y, float rx, float ry, float angle, const baphy::RGB &color = baphy::rgb("white"));
  void draw(const std::string &sprite_name, float x, float y, float angle, const baphy::RGB &color = baphy::rgb("white"));
  void draw(const std::string &sprite_name, float x, float y, const baphy::RGB &color = baphy::rgb("white"));

  void draw_scale(const std::string &sprite_name, float x, float y, float scale, float rx, float ry, float angle, const baphy::RGB &color = baphy::rgb("white"));
  void draw_scale(const std::string &sprite_name, float x, float y, float scale, float angle, const baphy::RGB &color = baphy::rgb("white"));
  void draw_scale(const std::string &sprite_name, float x, float y, float scale, const baphy::RGB &color = baphy::rgb("white"));

  void draw_wh(const std::string &sprite_name, float x, float y, float w, float h, float rx, float ry, float angle, const baphy::RGB &color = baphy::rgb("white"));
  void draw_wh(const std::string &sprite_name, float x, float y, float w, float h, float angle, const baphy::RGB &color = baphy::rgb("white"));
  void draw_wh(const std::string &sprite_name, float x, float y, float w, float h, const baphy::RGB &color = baphy::rgb("white"));

private:
  float scale_{1.0f};

  std::shared_ptr<Texture> tex_{nullptr};
  std::unordered_map<std::string, std::array<float, 4>> regions_{};
};

} // namespace baphy

#endif//BAPHY_GFX_SPRITESHEET_HPP
