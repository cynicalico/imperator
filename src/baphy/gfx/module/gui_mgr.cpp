#include "baphy/gfx/module/gui_mgr.hpp"

namespace baphy {

void Node::update(float lay_x, float lay_y, double dt) {
  in_bounds_.update(
      input->mouse_x() >= lay_x + x_ &&
      input->mouse_x() < lay_x + x_ + w_ &&
      input->mouse_y() >= lay_y + y_ &&
      input->mouse_y() < lay_y + y_ + h_
  );
}

PrimitiveButton::PrimitiveButton(
    std::shared_ptr<InputMgr> input, std::shared_ptr<PrimitiveBatcher> primitives,
    float x, float y, Font &font, float font_size, const std::string &text, OnClickListener &&f
) : Node(input, primitives, x, y, 2, 2), font_(font), font_size_(font_size), text_(text), f(std::forward<OnClickListener>(f)) {
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
  primitives->draw_rect(lay_x + x_, lay_y + y_, w_ - 1, h_ - 1, border_color_);
  primitives->fill_rect(lay_x + x_ + 1, lay_y + y_ + 1, w_ - 2, h_ - 2, bg_color_);
  font_.draw(lay_x + x_ + 1, lay_y + y_ + 1, font_size_, text_, fg_color_);
}

void PrimitiveButton::update(float lay_x, float lay_y, double dt) {
  Node::update(lay_x, lay_y, dt);

  if (in_bounds_[0] && input->pressed("mb_left"))
    f();
}

void Layout::update(double dt) {
  in_bounds_.update(
      input->mouse_x() >= last_pos_[0] &&
      input->mouse_x() < last_pos_[0] + w &&
      input->mouse_y() >= last_pos_[1] &&
      input->mouse_y() < last_pos_[1] + h
  );
}

void AbsoluteLayout::set_show_border(bool show) {
  show_border = show;
}

void AbsoluteLayout::set_border_color(const RGB &color) {
  border_color_ = color;
}

void AbsoluteLayout::draw(float x, float y) {
  if (show_border)
    draw_border_(x, y);

  for (auto &node: nodes)
    node.second->draw(x, y);

  last_pos_ = {x, y};
}

void AbsoluteLayout::update(double dt) {
  Layout::update(dt);

  if (in_bounds_[0] || in_bounds_[1])
    for (auto &node: nodes)
      node.second->update(last_pos_[0], last_pos_[1], dt);
}

void AbsoluteLayout::draw_border_(float x, float y) {
  primitives->draw_rect(x - 1, y - 1, w + 1, h + 1, border_color_);
}

void GuiMgr::e_initialize_(const EInitialize &e) {
  input = module_mgr->get<InputMgr>();
  primitives = module_mgr->get<PrimitiveBatcher>();

  EventBus::sub<EUpdate>(module_name, {EPI<InputMgr>::name}, [&](const auto &e) { e_update_(e); });

  Module::e_initialize_(e);
}

void GuiMgr::e_shutdown_(const EShutdown &e) {
  Module::e_shutdown_(e);
}

void GuiMgr::e_update_(const EUpdate &e) {
  for (auto &layout: layouts_)
    layout.second->update(e.dt);
}

} // namespace baphy
