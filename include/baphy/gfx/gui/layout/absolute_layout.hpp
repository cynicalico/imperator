#ifndef BAPHY_ABSOLUTE_LAYOUT_HPP
#define BAPHY_ABSOLUTE_LAYOUT_HPP

#include "baphy/gfx/internal/gui/layout.hpp"
#include "baphy/gfx/color.hpp"

namespace baphy {

class AbsoluteLayout : public Layout {
public:
  AbsoluteLayout(
      std::shared_ptr<InputMgr> input, std::shared_ptr<PrimitiveBatcher> primitives,
      float x, float y, float w, float h
  ) : Layout(input, primitives, x, y, w, h) {}

  AbsoluteLayout(
      std::shared_ptr<InputMgr> input, std::shared_ptr<PrimitiveBatcher> primitives,
      float w, float h
  ) : Layout(input, primitives, 0, 0, w, h) {}

  void add(std::shared_ptr<GuiNode> e, float x, float y);

  void set_show_border(bool show);
  void set_border_color(const RGB &color);

  void draw(float lay_x, float lay_y) override;
  void update(double dt, float lay_x, float lay_y, bool can_become_active) override;

private:
  bool show_border{false};
  RGB border_color_{baphy::rgb("white")};

  void draw_border_(float lay_x, float lay_y);
};

} // namespace baphy

#endif//BAPHY_ABSOLUTE_LAYOUT_HPP
