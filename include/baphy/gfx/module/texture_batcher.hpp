#ifndef BAPHY_GFX_MODULE_TEXTURE_MGR_HPP
#define BAPHY_GFX_MODULE_TEXTURE_MGR_HPP

#include "baphy/core/module_mgr.hpp"
#include "baphy/gfx/module/gfx_context.hpp"
#include "baphy/gfx/module/shader_mgr.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace baphy {

class TextureBatcher;

class Texture {
public:
  std::shared_ptr<TextureBatcher> mgr{nullptr};

  Texture(std::shared_ptr<TextureBatcher> &mgr);

private:

};

class TextureBatcher : public Module<TextureBatcher>, public std::enable_shared_from_this<TextureBatcher> {
public:
  TextureBatcher() : Module<TextureBatcher>({EPI<GfxContext>::name, EPI<ShaderMgr>::name}) {}
};

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::TextureBatcher);

#endif//BAPHY_GFX_MODULE_TEXTURE_MGR_HPP
