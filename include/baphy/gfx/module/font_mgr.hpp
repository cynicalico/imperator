#ifndef BAPHY_GFX_MODULE_FONT_MGR_HPP
#define BAPHY_GFX_MODULE_FONT_MGR_HPP

#include "baphy/core/module_mgr.hpp"
#include "baphy/gfx/internal/font/cp437.hpp"
#include "baphy/gfx/internal/font/font.hpp"
#include "baphy/gfx/module/texture_batcher.hpp"
#include <string>
#include <unordered_map>

namespace baphy {

class FontMgr : public Module<FontMgr> {
public:
  FontMgr() : Module<FontMgr>({EPI<TextureBatcher>::name}) {}

  std::shared_ptr<Font> cp437(const std::string &name, std::shared_ptr<Texture> tex, int char_w, int char_h);

private:
  std::unordered_map<std::string, std::shared_ptr<Font>> fonts_{};

  void e_initialize_(const EInitialize &e) override;
  void e_shutdown_(const EShutdown &e) override;
};

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::FontMgr);

#endif//BAPHY_GFX_MODULE_FONT_MGR_HPP
