#ifndef BAPHY_GFX_INTERNAL_GL_VERTEX_ARRAY_HPP
#define BAPHY_GFX_INTERNAL_GL_VERTEX_ARRAY_HPP

#include "baphy/gfx/module/gfx_context.hpp"
#include "baphy/gfx/internal/gl/buffer.hpp"
#include "baphy/gfx/internal/gl/shader.hpp"
#include <memory>
#include <string>

namespace baphy {

enum class DrawMode {
  points = GL_POINTS,
//  line_strip = GL_LINE_STRIP,
//  line_loop = GL_LINE_LOOP,
  lines = GL_LINES,
//  triangle_strip = GL_TRIANGLE_STRIP,
//  triangle_fan = GL_TRIANGLE_FAN,
  triangles = GL_TRIANGLES,
};

class VertexArray {
public:
  GladGLContext &gl;

  GLuint id{0};

  explicit VertexArray(GfxContext &gfx);

  void bind();
  void unbind();

  void attrib(Shader &shader, BufTarget target, Buffer &buf, const std::string &desc);
  void attrib(Shader &shader, Buffer &buf, const std::string &desc);

  void draw_arrays(const DrawMode &mode, GLsizei count, int first = 0);

private:
  struct VertexAttrib_ {
    GLuint index;
    GLint size;
    GLenum type;
    GLboolean normalized;
    GLsizei stride;
    GLsizei offset;
  };

  void gen_id_();
  void del_id_();
};

} // namespace baphy

#endif//BAPHY_GFX_INTERNAL_GL_VERTEX_ARRAY_HPP
