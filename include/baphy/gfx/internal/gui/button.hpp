#ifndef BAPHY_BUTTON_HPP
#define BAPHY_BUTTON_HPP

#include "gui_element.hpp"
#include <functional>

namespace baphy {

using OnClickListener = std::function<void(void)>;

class Button : public GuiElement {
public:
  Button(
      std::shared_ptr<InputMgr> input, std::shared_ptr<PrimitiveBatcher> primitives,
      float x, float y, float w, float h, OnClickListener &&f
  ) : GuiElement(input, primitives, x, y, w, h), f(std::forward<OnClickListener>(f)) {}

  void update(double dt, float lay_x, float lay_y, bool can_become_active) override;

protected:
  History<bool> clicked{2, false};
  OnClickListener f;
};

} // namespace baphy

#endif//BAPHY_BUTTON_HPP
