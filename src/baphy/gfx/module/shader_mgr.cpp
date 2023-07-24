#include "baphy/gfx/module/shader_mgr.hpp"

#include "baphy/util/rnd.hpp"
// stb defines this, and so does range-v3, and they do it in a very different way,
// and it caused the most gnarly compile error I've ever seen
#ifdef True
#undef True
#endif
#include "range/v3/all.hpp"
#include <vector>

namespace baphy {

std::shared_ptr<Shader> ShaderMgr::compile(const std::string &name, const ShaderSrc &src) {
  if (!shaders_.contains(name))
    shaders_[name] = std::make_shared<Shader>(*gfx, src);

  return shaders_[name];
}

std::shared_ptr<Shader> ShaderMgr::compile(const ShaderSrc &src) {
  return compile(src.name.value_or(rnd::base58(11)), src);
}

void ShaderMgr::recompile(const std::string &name, const ShaderSrc &src) {
  recompile_cache_[name] = src;
}

std::shared_ptr<Shader> ShaderMgr::get(const std::string &name) {
  return shaders_[name];
}

std::vector<std::string> ShaderMgr::get_names() {
  return shaders_
      | ranges::views::keys
      | ranges::to_vector
      | ranges::actions::sort;
}

void ShaderMgr::e_initialize_(const EInitialize &e) {
  gfx = module_mgr->get<GfxContext>();

  EventBus::sub<EEndFrame>(module_name, [&](const auto &e) { e_end_frame_(e); });

  Module::e_initialize_(e);
}

void ShaderMgr::e_shutdown_(const EShutdown &e) {
  Module::e_shutdown_(e);
}

void ShaderMgr::e_end_frame_(const EEndFrame &e) {
  for (const auto &[name, src]: recompile_cache_)
    shaders_[name]->recompile(src);

  recompile_cache_.clear();
}

} // namespace baphy
