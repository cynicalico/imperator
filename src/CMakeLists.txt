set(baphy_SRC_FILES ${baphy_SRC_FILES}
        src/baphy/core/module/application.cpp
        src/baphy/core/module/dear_imgui.cpp
        src/baphy/core/module/input_mgr.cpp
        src/baphy/core/module/window.cpp

        src/baphy/core/engine.cpp
        src/baphy/core/event_bus.cpp
        src/baphy/core/glfw_callbacks.cpp

        src/baphy/gfx/internal/gl/buffer.cpp
        src/baphy/gfx/internal/gl/framebuffer.cpp
        src/baphy/gfx/internal/gl/renderbuffer.cpp
        src/baphy/gfx/internal/gl/shader.cpp
        src/baphy/gfx/internal/gl/texture_unit.cpp
        src/baphy/gfx/internal/gl/vertex_array.cpp

        src/baphy/gfx/internal/opaque_primitive_batcher.cpp
        src/baphy/gfx/internal/trans_primitive_batcher.cpp

        src/baphy/gfx/module/gfx_context.cpp
        src/baphy/gfx/module/primitive_batcher.cpp
        src/baphy/gfx/module/shader_mgr.cpp
        src/baphy/gfx/module/surface_mgr.cpp

        src/baphy/gfx/color.cpp

        src/baphy/util/module/debug_overlay.cpp
        src/baphy/util/module/thread_pool.cpp
        src/baphy/util/module/timer_mgr.cpp
        src/baphy/util/module/tween_mgr.cpp

        src/baphy/util/noise/opensimplex.cpp
        src/baphy/util/noise/perlin.cpp

        src/baphy/util/averagers.cpp
        src/baphy/util/helpers.cpp
        src/baphy/util/log.cpp
        src/baphy/util/memusage.cpp
        src/baphy/util/platform.cpp
        src/baphy/util/rnd.cpp
        src/baphy/util/sops.cpp
        src/baphy/util/time.cpp)
