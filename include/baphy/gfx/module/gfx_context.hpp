#ifndef BAPHY_GFX_MODULE_CONTEXT_HPP
#define BAPHY_GFX_MODULE_CONTEXT_HPP

#include "baphy/core/module/window.hpp"
#include "baphy/core/module_mgr.hpp"
#include "baphy/util/platform.hpp"
#if defined(BAPHY_PLATFORM_WINDOWS)
#include "glad/wgl.h"
#elif defined(BAPHY_PLATFORM_LINUX)
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include "glad/glx.h"
#endif
#include "glad/gl.h"
#include "glm/vec2.hpp"

namespace baphy {
class GfxContext : public Module<GfxContext> {
public:
  std::shared_ptr<Window> window{nullptr};

  GfxContext() : Module({EPI<Window>::name}) {}

  GladGLContext gl;
  glm::ivec2 version{};

  bool is_vsync() const;
  void set_vsync(bool v);

protected:
  void r_initialize_(const E_Initialize& p) override;
  void r_shutdown_(const E_Shutdown& p) override;

private:
  static void GLAPIENTRY gl_message_callback_(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam
  );

#if defined(BAPHY_PLATFORM_WINDOWS)
  PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT{nullptr};
  PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT{nullptr};
#endif
  void initialize_platform_extensions_();

  bool platform_is_vsync_() const;
  void platform_set_vsync_(bool v);
};
} // namespace baphy

BAPHY_PRAISE_HERMES(baphy::GfxContext);

#endif//BAPHY_GFX_MODULE_CONTEXT_HPP
