#ifndef IMP_GFX_MODULE_SHADER_MGR_HPP
#define IMP_GFX_MODULE_SHADER_MGR_HPP

#include "imp/core/module_mgr.hpp"
#include "imp/gfx/gl/shader.hpp"
#include "imp/gfx/module/gfx_context.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace imp {
class ShaderMgr : public Module<ShaderMgr> {
public:
    std::shared_ptr<GfxContext> gfx{nullptr};

    ShaderMgr() : Module({EPI<GfxContext>::name}) {}

    std::shared_ptr<Shader> get(const std::string &name);

    // Names must be unique, and a previously compiled shader will be returned if
    // a shader with that name has already been compiled (rather than recompiling)
    std::shared_ptr<Shader> compile(const std::string &name, const ShaderSrc &src);
    std::shared_ptr<Shader> compile(const ShaderSrc &src);

protected:
    std::unordered_map<std::string, std::shared_ptr<Shader>> shaders_{};

    void r_initialize_(const E_Initialize& p) override;
    void r_shutdown_(const E_Shutdown& p) override;
};
} // namespace imp

IMP_PRAISE_HERMES(imp::ShaderMgr);

#endif//IMP_GFX_MODULE_SHADER_MGR_HPP
