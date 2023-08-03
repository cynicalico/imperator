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

class Node {
public:
  std::shared_ptr<InputMgr> input;
  std::shared_ptr<PrimitiveBatcher> primitives;

  Node(
      std::shared_ptr<InputMgr> input, std::shared_ptr<PrimitiveBatcher> primitives,
      float x, float y, float w, float h
  ) : input(input), primitives(primitives), x_(x), y_(y), w_(w), h_(h) {}

  float x() const { return x_; }
  float y() const { return y_; }
  float w() const { return w_; }
  float h() const { return h_; }

  virtual void draw(float lay_x, float lay_y) = 0;
  virtual void update(float lay_x, float lay_y, double dt);

protected:
  float x_{0}, y_{0}, w_{0}, h_{0};
  History<bool> in_bounds_{2, false};
};

class PrimitiveButton : public Node {
public:
  PrimitiveButton(
      std::shared_ptr<InputMgr> input, std::shared_ptr<PrimitiveBatcher> primitives,
      float x, float y, Font &font, float font_size, const std::string &text, OnClickListener &&f
  );

  void set_border_color(const RGB &color);
  void set_bg_color(const RGB &color);
  void set_fg_color(const RGB &color);

  void draw(float lay_x, float lay_y) override;
  void update(float lay_x, float lay_y, double dt) override;

private:
  Font &font_;
  float font_size_;
  std::string text_;
  OnClickListener f;

  RGB border_color_{baphy::rgb("white")};
  RGB bg_color_{baphy::rgb("black")};
  RGB fg_color_{baphy::rgb("white")};
};

class Layout {
public:
  std::shared_ptr<InputMgr> input;
  std::shared_ptr<PrimitiveBatcher> primitives;

  float w, h;
  glm::vec2 last_pos_{0, 0};
  History<bool> in_bounds_{2, false};
  std::unordered_map<std::string, std::shared_ptr<Node>> nodes{};

  Layout(
      std::shared_ptr<InputMgr> input, std::shared_ptr<PrimitiveBatcher> primitives,
      float w, float h
  ) : input(input), primitives(primitives), w(w), h(h) {}

  template<typename T, typename... Args>
  requires std::derived_from<T, Node>
  std::shared_ptr<T> create_node(float x, float y, Args &&... args);

  virtual void draw(float x, float y) = 0;
  virtual void update(double dt);
};

template<typename T, typename... Args>
requires std::derived_from<T, Node>
std::shared_ptr<T> Layout::create_node(float x, float y, Args &&... args) {
  auto n = std::make_shared<T>(input, primitives, x, y, std::forward<Args>(args)...);
  nodes[rnd::base58(11)] = n;
  return n;
}

class AbsoluteLayout : public Layout {
public:
  AbsoluteLayout(
      std::shared_ptr<InputMgr> input, std::shared_ptr<PrimitiveBatcher> primitives,
      float w, float h
  ) : Layout(input, primitives, w, h) {}

  void set_show_border(bool show);
  void set_border_color(const RGB &color);

  void draw(float x, float y) override;
  void update(double dt) override;

private:
  bool show_border{false};
  RGB border_color_{baphy::rgb("white")};

  void draw_border_(float x, float y);
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
  std::shared_ptr<T> create_layout(Args &&...args);

private:
  std::unordered_map<std::string, std::shared_ptr<Layout>> layouts_{};

  void e_initialize_(const EInitialize &e) override;
  void e_shutdown_(const EShutdown &e) override;

  void e_update_(const EUpdate &e);
};

template<typename T, typename... Args>
requires std::derived_from<T, Layout>
std::shared_ptr<T> GuiMgr::create_layout(Args &&... args) {
  auto l = std::make_shared<T>(input, primitives, std::forward<Args>(args)...);
  layouts_[rnd::base58(11)] = l;
  return l;
}

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::GuiMgr);

#endif//BAPHY_GFX_MODULE_GUI_MGR_HPP
