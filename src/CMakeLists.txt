set(imp_SRC_FILES ${imp_SRC_FILES}
        core/module/application.cpp
        core/module/cursor_mgr.cpp
        core/module/glfw_callbacks.cpp
        core/module/input_mgr.cpp
        core/module/window.cpp
        core/engine.cpp
        core/hermes.cpp
        core/prio_list.cpp

        gfx/gl/buffer.cpp
        gfx/gl/renderbuffer.cpp
        gfx/gl/shader.cpp
        gfx/gl/tex_image.cpp
        gfx/gl/vertex_array.cpp
        gfx/module/dear_imgui.cpp
        gfx/module/gfx_context.cpp
        gfx/module/shader_mgr.cpp
        gfx/color.cpp

        util/module/debug_overlay.cpp
        util/module/timer_mgr.cpp
        util/averagers.cpp
        util/helpers.cpp
        util/io.cpp
        util/log.cpp
        util/memusage.cpp
        util/platform.cpp
        util/rnd.cpp
        util/sops.cpp
        util/time.cpp

        imp.cpp)

list(TRANSFORM imp_SRC_FILES PREPEND src/imp/)
