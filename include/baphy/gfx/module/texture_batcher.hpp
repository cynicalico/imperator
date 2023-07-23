#ifndef BAPHY_GFX_MODULE_TEXTURE_BATCHER_HPP
#define BAPHY_GFX_MODULE_TEXTURE_BATCHER_HPP

#include "baphy/core/module_mgr.hpp"
#include "baphy/gfx/internal/gl/texture_unit.hpp"
#include "baphy/gfx/module/gfx_context.hpp"
#include "baphy/gfx/module/batcher.hpp"
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

namespace baphy {

class TextureBatcher;

class Texture {
public:
  std::shared_ptr<TextureBatcher> mgr{nullptr};

  GLuint id;
  GLuint width{0};
  GLuint height{0};
  bool fully_opaque{true};

  Texture(std::shared_ptr<TextureBatcher> mgr, std::unique_ptr<TextureUnit> &gl_obj);

  Texture(const Texture &) = delete;
  Texture &operator=(const Texture &) = delete;

  Texture(Texture &&other) noexcept;
  Texture &operator=(Texture &&other) noexcept;

  void draw(float x, float y);

private:

  std::unique_ptr<TextureUnit> gl_obj_{nullptr};
};

class TextureBatcher : public Module<TextureBatcher>, public std::enable_shared_from_this<TextureBatcher> {
  friend class Texture;

public:
  std::shared_ptr<GfxContext> gfx{nullptr};
  std::shared_ptr<Batcher> batcher{nullptr};

  TextureBatcher() : Module<TextureBatcher>({EPI<GfxContext>::name}) {}
  ~TextureBatcher() override = default;

  std::shared_ptr<Texture> load(const std::filesystem::path &path, bool retro = false);

private:
  std::vector<std::shared_ptr<Texture>> textures_{};

  void draw_texture_(
      GLuint id, bool fully_opaque,
      float x, float y, float w, float h,
      float tx, float ty, float tw, float th,
      float rx, float ry, float angle,
      const baphy::RGB &color = baphy::rgb("white")
  );

  void e_initialize_(const EInitialize &e) override;
  void e_shutdown_(const EShutdown &e) override;
};

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::TextureBatcher);

#endif//BAPHY_GFX_MODULE_TEXTURE_BATCHER_HPP
