#include "baphy/gfx/gui/layout/absolute_layout.hpp"

namespace baphy {

void AbsoluteLayout::add(std::shared_ptr<GuiNode> e, float x, float y) {
  e->set_x(x);
  e->set_y(y);
  e->parent = this;

  elements.emplace_back(e);
}

void AbsoluteLayout::set_show_border(bool show) {
  show_border = show;
}

void AbsoluteLayout::set_border_color(const RGB &color) {
  border_color_ = color;
}

void AbsoluteLayout::draw(float lay_x, float lay_y) {
  if (show_border)
    draw_border_(lay_x, lay_y);

  for (auto &e: elements)
    e->draw(lay_x + x_, lay_y + y_);
}

void AbsoluteLayout::update(double dt, float lay_x, float lay_y, bool can_become_active) {
  Layout::update(dt, lay_x, lay_y, can_become_active);
}

void AbsoluteLayout::draw_border_(float lay_x, float lay_y) {
  primitives->draw_rect(lay_x + x_ - 1, lay_y + y_ - 1, w_ + 1, h_ + 1, border_color_);
}

} // namespace baphy
