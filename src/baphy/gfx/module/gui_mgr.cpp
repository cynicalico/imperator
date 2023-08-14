#include "baphy/gfx/module/gui_mgr.hpp"

namespace baphy {

void GuiNode::update(double dt, float lay_x, float lay_y) {
  in_bounds_.update(
      input->mouse_x() >= lay_x + x_ &&
      input->mouse_x() < lay_x + x_ + w_ &&
      input->mouse_y() >= lay_y + y_ &&
      input->mouse_y() < lay_y + y_ + h_
  );
  hovered = in_bounds_[0];
}

PrimitiveButton::PrimitiveButton(
    std::shared_ptr<InputMgr> input, std::shared_ptr<PrimitiveBatcher> primitives,
    Font &font, float font_size, const std::string &text, OnClickListener &&f
) : GuiElement(input, primitives, 0.0f, 0.0f, 2.0f, 2.0f), font_(font), font_size_(font_size), text_(text), f(std::forward<OnClickListener>(f)) {
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
  primitives->draw_rect(lay_x + x_, lay_y + y_, w_ - 1, h_ - 1, hovered ? hovered_border_color_ : border_color_);
  primitives->fill_rect(lay_x + x_ + 1, lay_y + y_ + 1, w_ - 2, h_ - 2, hovered ? hovered_bg_color_ : bg_color_);
  font_.draw(lay_x + x_ + 1, lay_y + y_ + 1, font_size_, text_, hovered ? hovered_fg_color_ : fg_color_);
}

void PrimitiveButton::update(double dt, float lay_x, float lay_y) {
  GuiNode::update(dt, lay_x, lay_y);

  if (hovered && input->pressed("mb_left"))
    f();
}

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
    draw_border_();

  for (auto &e: elements)
    e->draw(x_, y_);
}

void AbsoluteLayout::update(double dt, float lay_x, float lay_y) {
  Layout::update(dt, lay_x, lay_y);

  if (in_bounds_[0] || in_bounds_[1])
    for (auto &e: elements)
      e->update(dt, x_, y_);
}

void AbsoluteLayout::draw_border_() {
  primitives->draw_rect(x_ - 1, y_ - 1, w_ + 1, h_ + 1, border_color_);
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
  for (auto &p: layouts_)
    if (!p.second->parent)
      p.second->update(e.dt);
}

} // namespace baphy
