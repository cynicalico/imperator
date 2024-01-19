#ifndef IMP_GFX_MODULE_CONTEXT_HPP
#define IMP_GFX_MODULE_CONTEXT_HPP

#include "imp/core/module/window.hpp"
#include "imp/core/module_mgr.hpp"
#include "imp/util/module/debug_overlay.hpp"
#include "imp/util/platform.hpp"
#if defined(IMP_PLATFORM_WINDOWS)
#include "glad/wgl.h"
#endif
#include "glad/gl.h"
#include "glm/vec2.hpp"

namespace imp {
enum class ClearBit {
  color = GL_COLOR_BUFFER_BIT,
  depth = GL_DEPTH_BUFFER_BIT,
  stencil = GL_STENCIL_BUFFER_BIT
};
} // namespace imp

ENUM_ENABLE_BITOPS(imp::ClearBit);

namespace imp {
class GfxContext : public Module<GfxContext> {
public:
  std::shared_ptr<DebugOverlay> debug_overlay{nullptr};
  std::shared_ptr<Window> window{nullptr};

  GfxContext(WindowOpenParams initialize_params)
    : Module({EPI<DebugOverlay>::name, EPI<Window>::name}),
      initialize_params_(std::move(initialize_params)) {}

  GladGLContext gl;
  glm::ivec2 version{};

  bool is_vsync() const;
  void set_vsync(bool v);

  void clear(const Color& color, const ClearBit& mask = ClearBit::color | ClearBit::depth);

protected:
  void r_initialize_(const E_Initialize& p) override;
  void r_shutdown_(const E_Shutdown& p) override;

private:
  WindowOpenParams initialize_params_;

  static void GLAPIENTRY gl_message_callback_(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam
  );

#if defined(IMP_PLATFORM_WINDOWS)
  PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT{nullptr};
  PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT{nullptr};
#endif
  void initialize_platform_extensions_();

  bool platform_is_vsync_() const;
  void platform_set_vsync_(bool v);
};
} // namespace imp

IMP_PRAISE_HERMES(imp::GfxContext);

#endif//IMP_GFX_MODULE_CONTEXT_HPP
