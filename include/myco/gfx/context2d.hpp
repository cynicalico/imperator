#pragma once

#include "myco/core/modules/window.hpp"
#include "myco/util/enum_bitops.hpp"
#include "myco/gfx/color.hpp"
#include "glad/gl.h"

namespace myco {

enum class ClearBit {
  stencil = GL_STENCIL_BUFFER_BIT,
  color = GL_COLOR_BUFFER_BIT,
  depth = GL_DEPTH_BUFFER_BIT
};

} // namespace myco

ENABLE_BITOPS(myco::ClearBit);

namespace myco {

class Context2D : std::enable_shared_from_this<Context2D> {
public:
  std::unique_ptr<GladGLContext> gl{nullptr};
  glm::ivec2 version{0, 0};

  explicit Context2D(const Window &window);

  void clear(const RGB &color, const ClearBit &bit = ClearBit::color | ClearBit::depth);

private:
  static void GLAPIENTRY gl_message_callback_(
      GLenum source,
      GLenum type,
      GLuint id,
      GLenum severity,
      GLsizei length,
      const GLchar *message,
      const void *userParam
  );
};

} // namespace myco
