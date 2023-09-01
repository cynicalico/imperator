#include "baphy/gfx/internal/gui/gui_node.hpp"

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

} // namespace baphy
