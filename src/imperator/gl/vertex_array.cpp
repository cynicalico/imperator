#include "imperator/gl/vertex_array.h"
#include "imperator/util/sops.h"

namespace imp {
VertexArray::VertexArray(GfxContext &gfx) : gl(gfx.gl) {
    gen_id_();
}

void VertexArray::bind() {
    gl.BindVertexArray(id);
}

void VertexArray::unbind() {
    gl.BindVertexArray(0);
}

void VertexArray::attrib(Shader &shader, BufTarget target, Buffer &buf, const std::string &desc) {
    const static RE2 attrib_pat(
        R"((\w+):(\d+)(i|f|u|s)(?::(n))?(?::s(\d+)(i|f|u|s)?)?(?::o(\d+)(i|f|u|s)?)?(?::i(\d+))?)");
    assert(attrib_pat.ok());

    const static RE2 spaces_pat{IMPERATOR_SPLIT_RE(R"((\s+))")};
    assert(spaces_pat.ok());

    static std::unordered_map<std::string, GLenum> type_map = {
        {"none", GL_NONE},
        {"i", GL_INT},
        {"f", GL_FLOAT},
        {"u", GL_UNSIGNED_INT},
        {"s", GL_SHORT}
    };

    static std::unordered_map<GLenum, std::size_t> size_map = {
        {GL_NONE, 1},
        {GL_INT, sizeof(int)},
        {GL_FLOAT, sizeof(float)},
        {GL_UNSIGNED_INT, sizeof(unsigned int)},
        {GL_SHORT, sizeof(std::int16_t)},
    };

    auto attrib_strs = split_re(desc, spaces_pat);
    std::vector<VertexAttrib_> vertex_attribs{};

    int stride_acc{0};
    for (const auto &s: attrib_strs) {
        std::string name;
        std::string size;
        std::string type;
        std::string normalized;
        std::string stride;
        std::string stride_type;
        std::string offset;
        std::string offset_type;
        std::string divisor;
        if (RE2::FullMatch(
                s,
                attrib_pat,
                &name,
                &size,
                &type,
                &normalized,
                &stride,
                &stride_type,
                &offset,
                &offset_type,
                &divisor)
        ) {
            auto loc = shader.get_attrib_loc(name);
            if (loc == -1) {
                continue;
            }

            auto size_i = std::stoi(size);
            auto type_e = type_map[type];
            auto type_s = size_map[type_e];

            auto stride_t = stride_type.empty() ? "none" : stride_type;
            auto offset_t = offset_type.empty() ? "none" : offset_type;

            vertex_attribs.emplace_back(
                loc,
                size_i,
                type_e,
                normalized.empty() ? GL_FALSE : GL_TRUE,
                stride.empty() ? 0 : size_map[type_map[stride_t]] * std::stoi(stride),
                offset.empty() ? stride_acc : size_map[type_map[offset_t]] * std::stoi(offset),
                divisor.empty() ? 0 : std::stoi(divisor)
            );

            stride_acc += static_cast<GLsizei>(type_s) * size_i;
        }
    }

    // For items that didn't specify a stride, this will set it assuming it's the
    // size sum of all elements in the specification provided
    for (auto &a: vertex_attribs) {
        if (a.stride == 0) {
            a.stride = stride_acc;
        }
    }

    bind();
    buf.bind(target);
    for (const auto &a: vertex_attribs) {
        gl.VertexAttribPointer(a.index, a.size, a.type, a.normalized, a.stride, reinterpret_cast<void *>(a.offset));
        if (a.divisor != 0) {
            gl.VertexAttribDivisor(a.index, a.divisor);
        }
        gl.EnableVertexAttribArray(a.index);
    }
    buf.unbind(target);
    unbind();
}

void VertexArray::attrib(Shader &shader, Buffer &buf, const std::string &desc) {
    attrib(shader, BufTarget::array, buf, desc);
}

void VertexArray::element_array(Buffer &buf) {
    bind();
    buf.bind(BufTarget::element_array);
    unbind();
}

void VertexArray::draw_arrays(const DrawMode &mode, GLsizei count, int first) {
    bind();
    gl.DrawArrays(unwrap(mode), first, count);
    unbind();
}

void VertexArray::multi_draw_arrays_indirect(const DrawMode &mode, GLsizei drawcount, GLsizei stride) {
    bind();
    gl.MultiDrawArraysIndirect(unwrap(mode), nullptr, drawcount, stride);
    unbind();
}

void VertexArray::gen_id_() {
    gl.GenVertexArrays(1, &id);
    IMPERATOR_LOG_DEBUG("GEN_ID({}): Vertex array", id);
}

void VertexArray::del_id_() {
    if (id != 0) {
        gl.DeleteVertexArrays(1, &id);
        IMPERATOR_LOG_DEBUG("DEL_ID({}): Vertex array", id);
        id = 0;
    }
}
} // namespace imp
