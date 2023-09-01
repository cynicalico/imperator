#ifndef BAPHY_LAYOUT_HPP
#define BAPHY_LAYOUT_HPP

#include "gui_node.hpp"
#include <vector>

namespace baphy {

class Layout : public GuiNode {
public:
  std::vector<std::shared_ptr<GuiNode>> elements{};

  Layout(
      std::shared_ptr<InputMgr> input, std::shared_ptr<PrimitiveBatcher> primitives,
      float x, float y, float w, float h
  ) : GuiNode(input, primitives, x, y, w, h) {}

  void update(double dt, float lay_x, float lay_y, bool can_become_active) override;
};

} // namespace baphy

#endif//BAPHY_LAYOUT_HPP
