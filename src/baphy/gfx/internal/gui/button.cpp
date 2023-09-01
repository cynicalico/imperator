#include "baphy/gfx/internal/gui/button.hpp"

namespace baphy {

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

} // namespace baphy
