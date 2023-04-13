#pragma once

#include "myco/core/modules/window.hpp"
#include "myco/gfx/context2d.hpp"
#include "imgui.h"
#include "implot.h"
#include <memory>

namespace myco {

class Dear {
public:
  Dear(const Window &window, const Context2D &ctx);

  void new_frame();

  void render();

private:
  ImGuiContext *imgui_ctx_{nullptr};
  ImPlotContext *implot_ctx_{nullptr};

  ImGuiIO *io_{nullptr};
};

} // namespace myco
