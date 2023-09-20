#ifndef BAPHY_GFX_MODULE_FONT_MGR_HPP
#define BAPHY_GFX_MODULE_FONT_MGR_HPP

#include "baphy/core/module_mgr.hpp"
#include "baphy/gfx/internal/font/font.hpp"
#include "baphy/gfx/module/texture_batcher.hpp"
#include <concepts>
#include <string>
#include <unordered_map>

namespace baphy {

class FontMgr : public Module<FontMgr> {
public:
  FontMgr() : Module<FontMgr>({EPI<TextureBatcher>::name}) {}

  template<typename T, typename... Args>
  requires std::derived_from<T, Font>
  std::shared_ptr<T> load(const std::string &name, Args &&...args);

  template<typename T>
  std::shared_ptr<T> get(const std::string &name);

private:
  std::unordered_map<std::string, std::shared_ptr<Font>> fonts_{};

  void e_initialize_(const EInitialize &e) override;
  void e_shutdown_(const EShutdown &e) override;
};

template<typename T, typename... Args>
requires std::derived_from<T, Font>
std::shared_ptr<T> FontMgr::load(const std::string &name, Args &&... args) {
  if (auto it = fonts_.find(name); it != fonts_.end())
    return std::dynamic_pointer_cast<T>(it->second);

  auto f = std::make_shared<T>(std::forward<Args>(args)...);
  fonts_[name] = f;
  return f;
}

template<typename T>
std::shared_ptr<T> FontMgr::get(const std::string &name) {
  return std::dynamic_pointer_cast<T>(fonts_[name]);
}

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::FontMgr);

#endif//BAPHY_GFX_MODULE_FONT_MGR_HPP
