#include "imp/gfx/module/texture_mgr.hpp"

#include "imp/util/rnd.hpp"

namespace imp {
Texture::Texture(const std::string& name, TexImage& ti) : name_(name), ti_(std::move(ti)) {}

std::string Texture::name() const {
  return name_;
}

GLuint Texture::id() const {
  return ti_.id;
}

int Texture::w() const {
  return ti_.w;
}

int Texture::h() const {
  return ti_.h;
}

bool Texture::fully_opaque() const {
  return ti_.fully_opaque;
}

bool Texture::flipped() const {
  return ti_.flipped;
}

TextureMgr::TextureMgr(const std::weak_ptr<ModuleMgr>& module_mgr) : Module(module_mgr) {
  ctx = module_mgr.lock()->get<GfxContext>();
}

std::shared_ptr<Texture> TextureMgr::load(const std::string& name, const std::filesystem::path& path, bool retro) {
  auto it = textures_.find(name);
  if (it == textures_.end()) {
    auto tex_image = TexImage(*ctx, path, retro);
    it = textures_.emplace_hint(it, name, std::make_shared<Texture>(name, tex_image));
  }
  return it->second;
}

std::shared_ptr<Texture> TextureMgr::load(const std::filesystem::path& path, bool retro) {
  return load(rnd::base58(11), path, retro);
}
} // namespace imp
