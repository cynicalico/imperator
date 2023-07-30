#include "baphy/gfx/module/font_mgr.hpp"

namespace baphy {

std::shared_ptr<Font> FontMgr::cp437(const std::string &name, std::shared_ptr<Texture> tex, int char_w, int char_h, int row_offset) {
  if (auto it = fonts_.find(name); it != fonts_.end())
    return it->second;

  fonts_[name] = std::make_shared<CP437>(tex, char_w, char_h, row_offset);
  return fonts_[name];
}

void FontMgr::e_initialize_(const EInitialize &e) {
  Module::e_initialize_(e);
}

void FontMgr::e_shutdown_(const EShutdown &e) {
  Module::e_shutdown_(e);
}

} // namespace baphy
