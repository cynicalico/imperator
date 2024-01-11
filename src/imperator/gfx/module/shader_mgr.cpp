#include "imperator/gfx/module/shader_mgr.hpp"

#include "imperator/util/rnd.hpp"

namespace imp {
std::shared_ptr<Shader> ShaderMgr::get(const std::string& name) {
  // TODO: Make this return optional if the name doesn't exist
  return shaders_[name];
}

std::shared_ptr<Shader> ShaderMgr::compile(const std::string& name, const ShaderSrc& src) {
  if (!shaders_.contains(name)) {
    shaders_[name] = std::make_shared<Shader>(*gfx, src);
  }
  return shaders_[name];
}

std::shared_ptr<Shader> ShaderMgr::compile(const ShaderSrc& src) {
  return compile(src.name.value_or(rnd::base58(11)), src);
}

void ShaderMgr::r_initialize_(const E_Initialize& p) {
  gfx = module_mgr->get<GfxContext>();

  Module::r_initialize_(p);
}

void ShaderMgr::r_shutdown_(const E_Shutdown& p) {
  Module::r_shutdown_(p);
}
} // namespace imp
