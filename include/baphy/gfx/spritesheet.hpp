#ifndef BAPHY_GFX_SPRITESHEET_HPP
#define BAPHY_GFX_SPRITESHEET_HPP

#include "baphy/gfx/module/texture_batcher.hpp"
#include <tuple>

namespace baphy {

class Spritesheet {
public:
  Spritesheet(
      TextureBatcher &textures,
      const std::filesystem::path &path_to_texture,
      const std::filesystem::path &path_to_spec,
      bool retro = false
  );

  void draw(const std::string &sprite_name, float x, float y, float w, float h, const baphy::RGB &color = baphy::rgb("white"));
  void draw(const std::string &sprite_name, float x, float y, float scale, const baphy::RGB &color = baphy::rgb("white"));
  void draw(const std::string &sprite_name, float x, float y, const baphy::RGB &color = baphy::rgb("white"));

private:
  std::shared_ptr<Texture> tex_{nullptr};
  std::unordered_map<std::string, std::array<float, 4>> regions_{};
};

} // namespace baphy

#endif//BAPHY_GFX_SPRITESHEET_HPP
