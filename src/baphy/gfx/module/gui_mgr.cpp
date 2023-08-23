#include "baphy/gfx/module/gui_mgr.hpp"

namespace baphy {

void GuiNode::update(double dt, float lay_x, float lay_y, bool can_become_active) {
  in_bounds_.update(
      can_become_active &&
      input->mouse_x() >= lay_x + x_ &&
      input->mouse_x() < lay_x + x_ + w_ &&
      input->mouse_y() >= lay_y + y_ &&
      input->mouse_y() < lay_y + y_ + h_
  );
  hovered = in_bounds_[0];
}

void Button::update(double dt, float lay_x, float lay_y, bool can_become_active) {
  GuiNode::update(dt, lay_x, lay_y, can_become_active);

  if (clicked[0]) {
    if (input->released("mb_left")) {
      // Currently clicked and now released
      clicked.update(false);
      active = false;

      // Prevent action from firing
      if (!hovered)
        clicked.update(false);

    } else
      clicked.update(true);

  } else if (can_become_active && !clicked[0] && hovered && input->pressed("mb_left")) {
    // Not clicked, hovered over this element, and input was pressed
    clicked.update(true);
    active = true;

  } else
    clicked.update(false);

  if (!clicked[0] && clicked[1])
    f();
}

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

  bool any_elements_are_active = false;
  bool prev_elem_is_active{false};
  std::size_t active_elem_idx = elements.size();

  if (active || in_bounds_[0] || in_bounds_[1])
    for (const auto &[i, e]: enumerate(elements | std::views::reverse)) {
      e->update(dt, lay_x + x_, lay_y + y_, can_become_active && !prev_elem_is_active);

      if (!prev_elem_is_active && e->active) {
        prev_elem_is_active = true;
        active_elem_idx = elements.size() - 1 - i;
      }

      if (e->active)
        any_elements_are_active = true;
    }

  if (active_elem_idx != elements.size())
    range_move_to_back(elements, active_elem_idx);

  active = any_elements_are_active;
}

void AbsoluteLayout::draw_border_(float lay_x, float lay_y) {
  primitives->draw_rect(lay_x + x_ - 1, lay_y + y_ - 1, w_ + 1, h_ + 1, border_color_);
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
  bool prev_layout_is_active{false};
  std::size_t active_layout_idx = layouts_.size();

  for (const auto &[i, l]: enumerate(layouts_ | std::views::reverse)) {
    if (!l->parent) {
      l->update(e.dt, 0.0, 0.0, !prev_layout_is_active);

      if (!prev_layout_is_active && l->active) {
        prev_layout_is_active = true;
        active_layout_idx = layouts_.size() - 1 - i;
      }
    }
  }

  if (active_layout_idx != layouts_.size())
    range_move_to_back(layouts_, active_layout_idx);
}

} // namespace baphy
