#include "baphy/gfx/gui/element/primitive_button.hpp"

namespace baphy {

PrimitiveButton::PrimitiveButton(
    std::shared_ptr<InputMgr> input, std::shared_ptr<PrimitiveBatcher> primitives,
    Font &font, float font_size, const std::string &text, OnClickListener &&f
) : Button(input, primitives, 0.0f, 0.0f, 2.0f, 2.0f, std::forward<OnClickListener>(f)), font_(font), font_size_(font_size), text_(text) {
  auto text_bounds = font_.bounds(font_size_, text_);
  w_ += text_bounds.x;
  h_ += text_bounds.y;
}

void PrimitiveButton::set_border_color(const RGB &color) {
  border_color_ = color;
}

void PrimitiveButton::set_bg_color(const RGB &color) {
  bg_color_ = color;
}

void PrimitiveButton::set_fg_color(const RGB &color) {
  fg_color_ = color;
}

void PrimitiveButton::draw(float lay_x, float lay_y) {
  baphy::RGB border = border_color_;
  baphy::RGB bg = bg_color_;
  baphy::RGB fg = fg_color_;

  if (hovered) {
    if (clicked[0]) {
      border = clicked_border_color_;
      bg = clicked_bg_color_;
      fg = clicked_fg_color_;

    } else {
      border = hovered_border_color_;
      bg = hovered_bg_color_;
      fg = hovered_fg_color_;
    }
  }

  primitives->draw_rect(lay_x + x_, lay_y + y_, w_ - 1, h_ - 1, border);
  primitives->fill_rect(lay_x + x_ + 1, lay_y + y_ + 1, w_ - 2, h_ - 2, bg);
  font_.draw(lay_x + x_ + 1, lay_y + y_ + 1, font_size_, text_, fg);
}

void PrimitiveButton::update(double dt, float lay_x, float lay_y, bool can_become_active) {
  Button::update(dt, lay_x, lay_y, can_become_active);
}

} // namespace baphy
