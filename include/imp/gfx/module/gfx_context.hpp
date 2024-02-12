#ifndef IMP_GFX_MODULE_CONTEXT_HPP
#define IMP_GFX_MODULE_CONTEXT_HPP

#include "imp/core/module/window.hpp"
#include "imp/core/module_mgr.hpp"
#include "imp/gfx/gl/enum_types.hpp"
#include "imp/util/module/debug_overlay.hpp"
#include "imp/util/platform.hpp"
#if defined(IMP_PLATFORM_WINDOWS)
#include "glad/wgl.h"
#endif
#include "glm/vec2.hpp"

namespace imp {
class GfxContext : public Module<GfxContext> {
public:
  std::shared_ptr<DebugOverlay> debug_overlay{nullptr};
  std::shared_ptr<Window> window{nullptr};

  explicit GfxContext(const std::weak_ptr<ModuleMgr>& module_mgr, WindowOpenParams initialize_params);

  GladGLContext gl;
  glm::ivec2 version{};

  bool is_vsync() const;
  void set_vsync(bool v);

  void enable(const Capability& capability);
  void disable(const Capability& capability);

  void blend_func(const BlendFunc& sfactor, const BlendFunc& dfactor);
  void blend_func_separate(const BlendFunc& sfactor_rgb, const BlendFunc& dfactor_rgb,
                           const BlendFunc& sfactor_alpha, const BlendFunc& dfactor_alpha);

  void depth_mask(bool enable);

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
