set(imperator_INCLUDE_FILES ${imperator_INCLUDE_FILES}
        core/module/application.hpp
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
        gfx/gl/shader.hpp
        gfx/gl/static_buffer.hpp
        gfx/gl/vec_buffer.hpp
        gfx/gl/vertex_array.hpp
        gfx/module/dear_imgui.hpp
        gfx/module/gfx_context.hpp

        util/module/timer_mgr.hpp
        util/averagers.hpp
        util/enum_bitops.hpp
        util/helpers.hpp
        util/io.hpp
        util/log.hpp
        util/memusage.hpp
        util/platform.hpp
        util/rnd.hpp
        util/sops.hpp
        util/time.hpp

        imperator.hpp
        imperator_gl_wrappers.hpp
        imperator_imconfig.hpp)

list(TRANSFORM imperator_INCLUDE_FILES PREPEND include/imperator/)
