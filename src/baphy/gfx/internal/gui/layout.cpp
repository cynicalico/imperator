#include "baphy/gfx/internal/gui/layout.hpp"

namespace baphy {

void Layout::update(double dt, float lay_x, float lay_y, bool can_become_active) {
  GuiNode::update(dt, lay_x, lay_y, can_become_active);

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

} // namespace baphy
