#ifndef BAPHY_GUI_ELEMENT_HPP
#define BAPHY_GUI_ELEMENT_HPP

#include "gui_node.hpp"

namespace baphy {

class GuiElement : public GuiNode {
public:
  GuiElement(
      std::shared_ptr<InputMgr> input, std::shared_ptr<PrimitiveBatcher> primitives,
      float x, float y, float w, float h
  ) : GuiNode(input, primitives, x, y, w, h) {}
};

} // namespace baphy

#endif//BAPHY_GUI_ELEMENT_HPP
