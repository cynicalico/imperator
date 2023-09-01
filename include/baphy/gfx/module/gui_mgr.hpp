#ifndef BAPHY_GFX_MODULE_GUI_MGR_HPP
#define BAPHY_GFX_MODULE_GUI_MGR_HPP

#include "baphy/core/module/input_mgr.hpp"
#include "baphy/core/module_mgr.hpp"
#include "baphy/gfx/internal/gui/gui_node.hpp"
#include "baphy/gfx/internal/gui/gui_element.hpp"
#include "baphy/gfx/internal/gui/layout.hpp"
#include "baphy/gfx/module/font_mgr.hpp"
#include "baphy/gfx/module/primitive_batcher.hpp"
#include "baphy/gfx/module/surface_mgr.hpp"
#include "baphy/util/helpers.hpp"
#include <string>

namespace baphy {

class GuiMgr : public Module<GuiMgr> {
public:
  std::shared_ptr<InputMgr> input;
  std::shared_ptr<PrimitiveBatcher> primitives;

  GuiMgr() : Module<GuiMgr>({
    EPI<InputMgr>::name,
    EPI<FontMgr>::name,
    EPI<PrimitiveBatcher>::name,
    EPI<SurfaceMgr>::name
  }) {}

  template<typename T, typename... Args>
  requires std::derived_from<T, Layout>
  std::shared_ptr<T> create(Args &&...args);

  template<typename T, typename... Args>
  requires std::derived_from<T, GuiElement>
  std::shared_ptr<T> create(Args &&...args);

private:
  std::vector<std::shared_ptr<Layout>> layouts_{};
  std::vector<std::shared_ptr<GuiElement>> elements_{};

  void e_initialize_(const EInitialize &e) override;
  void e_shutdown_(const EShutdown &e) override;

  void e_update_(const EUpdate &e);
};

template<typename T, typename... Args>
requires std::derived_from<T, Layout>
std::shared_ptr<T> GuiMgr::create(Args &&... args) {
  auto l = std::make_shared<T>(input, primitives, std::forward<Args>(args)...);
  layouts_.emplace_back(l);
  return l;
}

template<typename T, typename... Args>
requires std::derived_from<T, GuiElement>
std::shared_ptr<T> GuiMgr::create(Args &&...args) {
  auto e = std::make_shared<T>(input, primitives, std::forward<Args>(args)...);
  elements_.emplace_back(e);
  return e;
}

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::GuiMgr);

#endif//BAPHY_GFX_MODULE_GUI_MGR_HPP
