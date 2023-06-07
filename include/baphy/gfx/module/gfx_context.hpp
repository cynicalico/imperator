#ifndef BAPHY_CORE_MODULE_GFX_CONTEXT_HPP
#define BAPHY_CORE_MODULE_GFX_CONTEXT_HPP

#include "baphy/core/module/window.hpp"
#include "baphy/core/module.hpp"
#include "baphy/gfx/color.hpp"
#include "baphy/gfx/enum_types.hpp"
#include "baphy/util/platform.hpp"
#if defined(BAPHY_PLATFORM_WINDOWS)
#include "glad/wgl.h"
#endif
#include "glad/gl.h"
#include "glm/vec2.hpp"

namespace baphy {

class GfxContext : public Module<GfxContext> {
public:
  std::unique_ptr<GladGLContext> gl{nullptr};
  glm::ivec2 version{0, 0};

  GfxContext() : Module<GfxContext>({EPI<Window>::name}) {}

  ~GfxContext() override = default;

  bool is_vsync() const;
  void set_vsync(bool v);

  void clear(const RGB &color, const ClearBit &bit = ClearBit::color | ClearBit::depth);

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
