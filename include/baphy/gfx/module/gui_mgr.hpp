#ifndef BAPHY_GFX_MODULE_GUI_MGR_HPP
#define BAPHY_GFX_MODULE_GUI_MGR_HPP

#include "baphy/core/module/input_mgr.hpp"
#include "baphy/core/module_mgr.hpp"
#include "baphy/gfx/module/font_mgr.hpp"
#include "baphy/gfx/module/primitive_batcher.hpp"
#include "baphy/gfx/module/surface_mgr.hpp"
#include "baphy/util/helpers.hpp"
#include "baphy/util/rnd.hpp"
#include <string>

namespace baphy {

using OnClickListener = std::function<void(void)>;

class GuiNode {
public:
  GuiNode *parent{nullptr};
  bool active{false};

  std::shared_ptr<InputMgr> input;
  std::shared_ptr<PrimitiveBatcher> primitives;

  GuiNode(
      std::shared_ptr<InputMgr> input, std::shared_ptr<PrimitiveBatcher> primitives,
      float x, float y, float w, float h
  ) : input(input), primitives(primitives), x_(x), y_(y), w_(w), h_(h) {}

  void set_x(float x) { x_ = x; }
  void set_y(float y) { y_ = y; }
  void set_w(float w) { w_ = w; }
  void set_h(float h) { h_ = h; }

  float x() const { return x_; }
  float y() const { return y_; }
  float w() const { return w_; }
  float h() const { return h_; }

  virtual void draw(float lay_x, float lay_y) = 0;
  virtual void update(double dt, float lay_x, float lay_y, bool can_become_active);

protected:
  float x_{0}, y_{0}, w_{0}, h_{0};

  History<bool> in_bounds_{2, false};
  bool hovered{false};
};

class GuiElement : public GuiNode {
public:
  GuiElement(
      std::shared_ptr<InputMgr> input, std::shared_ptr<PrimitiveBatcher> primitives,
      float x, float y, float w, float h
  ) : GuiNode(input, primitives, x, y, w, h) {}
};

class Button : public GuiElement {
public:
  Button(
      std::shared_ptr<InputMgr> input, std::shared_ptr<PrimitiveBatcher> primitives,
      float x, float y, float w, float h, OnClickListener &&f
  ) : GuiElement(input, primitives, x, y, w, h), f(std::forward<OnClickListener>(f)) {}

  void update(double dt, float lay_x, float lay_y, bool can_become_active) override;

protected:
  History<bool> clicked{2, false};
  OnClickListener f;
};

class PrimitiveButton : public Button {
public:
  PrimitiveButton(
      std::shared_ptr<InputMgr> input, std::shared_ptr<PrimitiveBatcher> primitives,
      Font &font, float font_size, const std::string &text, OnClickListener &&f
  );

  void set_border_color(const RGB &color);
  void set_bg_color(const RGB &color);
  void set_fg_color(const RGB &color);

  void draw(float lay_x, float lay_y) override;
  void update(double dt, float lay_x, float lay_y, bool can_become_active) override;

private:
  Font &font_;
  float font_size_;
  std::string text_;

  RGB border_color_{baphy::rgb("red")};
  RGB bg_color_{baphy::rgb("black")};
  RGB fg_color_{baphy::rgb("red")};

  RGB hovered_border_color_{baphy::rgb("yellow")};
  RGB hovered_bg_color_{baphy::rgb("black")};
  RGB hovered_fg_color_{baphy::rgb("yellow")};

  RGB clicked_border_color_{baphy::rgb("lime")};
  RGB clicked_bg_color_{baphy::rgb("black")};
  RGB clicked_fg_color_{baphy::rgb("lime")};
};

class Layout : public GuiNode {
public:
  std::vector<std::shared_ptr<GuiNode>> elements{};

  Layout(
      std::shared_ptr<InputMgr> input, std::shared_ptr<PrimitiveBatcher> primitives,
      float x, float y, float w, float h
  ) : GuiNode(input, primitives, x, y, w, h) {}
};

class AbsoluteLayout : public Layout {
public:
  AbsoluteLayout(
      std::shared_ptr<InputMgr> input, std::shared_ptr<PrimitiveBatcher> primitives,
      float x, float y, float w, float h
  ) : Layout(input, primitives, x, y, w, h) {}

  AbsoluteLayout(
      std::shared_ptr<InputMgr> input, std::shared_ptr<PrimitiveBatcher> primitives,
      float w, float h
  ) : Layout(input, primitives, 0, 0, w, h) {}

  void add(std::shared_ptr<GuiNode> e, float x, float y);

  void set_show_border(bool show);
  void set_border_color(const RGB &color);

  void draw(float lay_x, float lay_y) override;
  void update(double dt, float lay_x, float lay_y, bool can_become_active) override;

private:
  bool show_border{false};
  RGB border_color_{baphy::rgb("white")};

  void draw_border_(float lay_x, float lay_y);
};

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
