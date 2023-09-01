#ifndef BAPHY_PRIMITIVE_BUTTON_HPP
#define BAPHY_PRIMITIVE_BUTTON_HPP

#include "baphy/gfx/internal/gui/button.hpp"
#include "baphy/gfx/internal/font/font.hpp"
#include "baphy/gfx/color.hpp"

namespace baphy {

class PrimitiveButton : public Button {
public:
  PrimitiveButton(
      std::shared_ptr<InputMgr> input, std::shared_ptr<PrimitiveBatcher> primitives,
      Font &font, float font_size, const std::string &text, OnClickListener &&f
  );

  void set_border_color(const RGB &color);
  void set_bg_color(const RGB &color);
  void set_fg_color(const RGB &color);

  void draw(float lay_x, float lay_y) override;
  void update(double dt, float lay_x, float lay_y, bool can_become_active) override;

private:
  Font &font_;
  float font_size_;
  std::string text_;

  RGB border_color_{baphy::rgb("red")};
  RGB bg_color_{baphy::rgb("black")};
  RGB fg_color_{baphy::rgb("red")};

  RGB hovered_border_color_{baphy::rgb("yellow")};
  RGB hovered_bg_color_{baphy::rgb("black")};
  RGB hovered_fg_color_{baphy::rgb("yellow")};

  RGB clicked_border_color_{baphy::rgb("lime")};
  RGB clicked_bg_color_{baphy::rgb("black")};
  RGB clicked_fg_color_{baphy::rgb("lime")};
};

} // namespace baphy

#endif//BAPHY_PRIMITIVE_BUTTON_HPP
