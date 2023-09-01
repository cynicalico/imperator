#ifndef BAPHY_GUI_NODE_HPP
#define BAPHY_GUI_NODE_HPP

#include "baphy/core/module/input_mgr.hpp"
#include "baphy/gfx/module/primitive_batcher.hpp"
#include "baphy/util/helpers.hpp"
#include <memory>

namespace baphy {

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

} // namespace baphy

#endif//BAPHY_GUI_NODE_HPP
