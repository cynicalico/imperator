#ifndef IMP_GFX_MODULE_TEXTURE_MGR_HPP
#define IMP_GFX_MODULE_TEXTURE_MGR_HPP

#include "imp/core/module_mgr.hpp"
#include "imp/gfx/gl/tex_image.hpp"
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

namespace imp {

class Texture {
public:
  Texture(const std::string& name, TexImage& ti);

  std::string name() const;

  GLuint id() const;

  int w() const;
  int h() const;

  bool fully_opaque() const;

  bool flipped() const;

private:
  std::string name_;
  TexImage ti_;
};

class TextureMgr : public Module<TextureMgr> {
public:
  std::shared_ptr<GfxContext> ctx{nullptr};

  explicit TextureMgr(const std::weak_ptr<ModuleMgr>& module_mgr);

  std::shared_ptr<Texture> load(const std::string& name, const std::filesystem::path& path, bool retro = false);
  std::shared_ptr<Texture> load(const std::filesystem::path& path, bool retro = false);

private:
  std::unordered_map<std::string, std::shared_ptr<Texture>> textures_{};
};

} // namespace imp

IMP_PRAISE_HERMES(imp::TextureMgr);

#endif//IMP_GFX_MODULE_TEXTURE_MGR_HPP
