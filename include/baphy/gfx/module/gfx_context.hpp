#ifndef BAPHY_CORE_MODULE_GFX_CONTEXT_HPP
#define BAPHY_CORE_MODULE_GFX_CONTEXT_HPP

#include "baphy/core/module/window.hpp"
#include "baphy/core/module_mgr.hpp"
#include "baphy/gfx/color.hpp"
#include "baphy/gfx/gfx_enum_types.hpp"
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

  GladGLContext gl;
  glm::ivec2 version{0, 0};

  GfxContext() : Module<GfxContext>({EPI<Window>::name}) {}

  ~GfxContext() override = default;

  bool is_vsync() const;
  void set_vsync(bool v);

  void enable(Capability c);
  void disable(Capability c);

  void clear(const RGB &color, const ClearBit &bit = ClearBit::color | ClearBit::depth);

  void blend_func(BlendFunc s_factor, BlendFunc d_factor);
  void blend_func_separate(BlendFunc src_rgb, BlendFunc dst_rgb, BlendFunc src_alpha, BlendFunc dst_alpha);

  void depth_mask(bool enabled);

  void flush();

  void reset_viewport();

  glm::mat4 ortho_projection() const;

private:
  void initialize_platform_extensions_();
  bool platform_is_vsync_() const;
  void platform_set_vsync_(bool v);
#if defined(BAPHY_PLATFORM_WINDOWS)
  PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT{nullptr};
  PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT{nullptr};
#endif

  void e_initialize_(const EInitialize &e) override;
  void e_shutdown_(const EShutdown &e) override;

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

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::GfxContext);

#endif//BAPHY_CORE_MODULE_GFX_CONTEXT_HPP
