#ifndef BAPHY_GFX_MODULE_SHADER_MGR_HPP
#define BAPHY_GFX_MODULE_SHADER_MGR_HPP

#include "baphy/core/module_mgr.hpp"
#include "baphy/gfx/internal/gl/shader.hpp"
#include "baphy/gfx/module/gfx_context.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace baphy {

class ShaderMgr : public Module<ShaderMgr> {
public:
  std::shared_ptr<GfxContext> gfx{nullptr};

  ShaderMgr() : Module<ShaderMgr>({EPI<GfxContext>::name}) {}
  ~ShaderMgr() override = default;

  std::vector<std::string> get_names();

  std::shared_ptr<Shader> get(const std::string &name);

  // Names must be unique, and a previously compiled shader will be returned if
  // a shader with that name has already been compiled (rather than recompiling)
  std::shared_ptr<Shader> compile(const std::string &name, const ShaderSrc &src);
  std::shared_ptr<Shader> compile(const ShaderSrc &src);

  void recompile(const std::string &name, const ShaderSrc &src);

private:
  std::unordered_map<std::string, std::shared_ptr<Shader>> shaders_{};
  std::unordered_map<std::string, ShaderSrc> recompile_cache_{};

  void e_initialize_(const EInitialize &e) override;
  void e_shutdown_(const EShutdown &e) override;

  void e_end_frame_(const EEndFrame &e);
};

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::ShaderMgr);

#endif//BAPHY_GFX_MODULE_SHADER_MGR_HPP
