#ifndef IMPERATOR_GL_BUFFER_H
#define IMPERATOR_GL_BUFFER_H

#include "imperator/module/gfx/gfx_context.h"
#include <concepts>
#include <memory>

namespace imp {

enum class BufTarget {
    none = 0, // Used as sentinel
    array = GL_ARRAY_BUFFER,
    atomic_counter = GL_ATOMIC_COUNTER_BUFFER,
    copy_read = GL_COPY_READ_BUFFER,
    copy_write = GL_COPY_WRITE_BUFFER,
    dispatch_indirect = GL_DISPATCH_INDIRECT_BUFFER,
    draw_indirect = GL_DRAW_INDIRECT_BUFFER,
    element_array = GL_ELEMENT_ARRAY_BUFFER,
    pixel_pack = GL_PIXEL_PACK_BUFFER,
    pixel_unpack = GL_PIXEL_UNPACK_BUFFER,
    query = GL_QUERY_BUFFER,
    shader_storage = GL_SHADER_STORAGE_BUFFER,
    texture = GL_TEXTURE_BUFFER,
    transform_feedback = GL_TRANSFORM_FEEDBACK_BUFFER,
    uniform = GL_UNIFORM_BUFFER,
};

enum class BufUsage {
    none = 0, // Used as sentinel
    stream_draw = GL_STREAM_DRAW,
    stream_read = GL_STREAM_READ,
    stream_copy = GL_STREAM_COPY,
    static_draw = GL_STATIC_DRAW,
    static_read = GL_STATIC_READ,
    static_copy = GL_STATIC_COPY,
    dynamic_draw = GL_DYNAMIC_DRAW,
    dynamic_read = GL_DYNAMIC_READ,
    dynamic_copy = GL_DYNAMIC_COPY,
};

template<typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

class Buffer {
public:
    GladGLContext &gl;

    GLuint id{0};

    explicit Buffer(GfxContext &gfx);

    virtual ~Buffer();

    // Copy constructors don't make sense for OpenGL objects
    Buffer(const Buffer &) = delete;

    Buffer &operator=(const Buffer &) = delete;

    Buffer(Buffer &&other) noexcept;

    Buffer &operator=(Buffer &&other) noexcept;

    void bind(const BufTarget &target) const;

    void unbind(const BufTarget &target) const;

protected:
    void gen_id_();

    void del_id_();
};

} // namespace imp

#endif//IMPERATOR_GL_BUFFER_H
