set(imp_INCLUDE_FILES ${imp_INCLUDE_FILES}
        core/module/application.hpp
        core/module/cursor_mgr.hpp
        core/module/glfw_callbacks.hpp
        core/module/input_mgr.hpp
        core/module/window.hpp
        core/engine.hpp
        core/hermes.hpp
        core/hermes_payloads.hpp
        core/hermes_payloads_types.hpp
        core/module_mgr.hpp
        core/prio_list.hpp
        core/type_id.hpp

        gfx/gl/buffer.hpp
        gfx/gl/enum_types.hpp
        gfx/gl/renderbuffer.hpp
        gfx/gl/shader.hpp
        gfx/gl/static_buffer.hpp
        gfx/gl/tex_image.hpp
        gfx/gl/vec_buffer.hpp
        gfx/gl/vertex_array.hpp
        gfx/module/batcher.hpp
        gfx/module/dear_imgui.hpp
        gfx/module/gfx_2d.hpp
        gfx/module/gfx_context.hpp
        gfx/module/shader_mgr.hpp
        gfx/color.hpp

        util/ds/trie.hpp
        util/module/debug_overlay.hpp
        util/module/timer_mgr.hpp
        util/averagers.hpp
        util/enum_bitops.hpp
        util/helpers.hpp
        util/io.hpp
        util/log.hpp
        util/map_macro.hpp
        util/memusage.hpp
        util/oneshot_macro.hpp
        util/platform.hpp
        util/rnd.hpp
        util/sops.hpp
        util/time.hpp

        imp.hpp
        imp_gl_wrappers.hpp
        imp_imconfig.hpp)

list(TRANSFORM imp_INCLUDE_FILES PREPEND include/imp/)
