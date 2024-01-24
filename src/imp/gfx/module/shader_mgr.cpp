#include "imp/gfx/module/shader_mgr.hpp"

#include "imp/util/rnd.hpp"

namespace imp {
ShaderMgr::ShaderMgr(const std::weak_ptr<ModuleMgr>& module_mgr): Module(module_mgr) {
  ctx = module_mgr.lock()->get<GfxContext>();
}

std::shared_ptr<Shader> ShaderMgr::get(const std::string& name) {
  // TODO: Make this return optional if the name doesn't exist
  return shaders_[name];
}

std::shared_ptr<Shader> ShaderMgr::compile(const std::string& name, const ShaderSrc& src) {
  if (!shaders_.contains(name)) {
    shaders_[name] = std::make_shared<Shader>(*ctx, src);
  }
  return shaders_[name];
}

std::shared_ptr<Shader> ShaderMgr::compile(const ShaderSrc& src) {
  return compile(src.name.value_or(rnd::base58(11)), src);
}
} // namespace imp
