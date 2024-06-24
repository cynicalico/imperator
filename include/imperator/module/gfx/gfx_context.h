#ifndef IMPERATOR_MODULE_GFX_GFX_CONTEXT_H
#define IMPERATOR_MODULE_GFX_GFX_CONTEXT_H

#include "glad/gl.h"
#include "imperator/module/module_mgr.h"
#include "imperator/module/window.h"
#include "imperator/util/platform.h"

namespace imp {
struct GfxParams {
    glm::ivec2 backend_version{4, 3};
    bool vsync{true};
};

class GfxContext : public Module<GfxContext> {
public:
    GladGLContext gl;
    std::shared_ptr<Window> window{nullptr};

    explicit GfxContext(ModuleMgr &module_mgr, GfxParams params);

    bool is_vsync() const;

    void set_vsync(bool v);

private:
    bool use_platform_extensions_{false};

    void initialize_platform_extensions_();

    bool platform_is_vsync_() const;

    void platform_set_vsync_(bool v);

    static void GLAPIENTRY gl_message_callback_(
            GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message,
            const void *userParam
    );
};
} // namespace imp

IMPERATOR_DECLARE_MODULE(imp::GfxContext);

#endif //IMPERATOR_MODULE_GFX_GFX_CONTEXT_H
