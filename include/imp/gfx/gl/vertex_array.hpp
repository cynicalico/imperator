#ifndef IMP_GFX_GL_VERTEX_ARRAY_HPP
#define IMP_GFX_GL_VERTEX_ARRAY_HPP

#include "imp/gfx/module/gfx_context.hpp"
#include "imp/gfx/gl/buffer.hpp"
#include "imp/gfx/gl/shader.hpp"
#include <string>

namespace imp {
enum class DrawMode {
  none = 0, // Used as sentinel
  tex = 0xdeadbeef, // Used for batching

  points = GL_POINTS,
  //  line_strip = GL_LINE_STRIP,
  line_loop = GL_LINE_LOOP,
  lines = GL_LINES,
  //  triangle_strip = GL_TRIANGLE_STRIP,
  //  triangle_fan = GL_TRIANGLE_FAN,
  triangles = GL_TRIANGLES,
};

class VertexArray {
public:
  GladGLContext& gl;

  GLuint id{0};

  explicit VertexArray(GfxContext& gfx);

  void bind();
  void unbind();

  /* Attrib format:
   *
   *   <name>:<size>(i|f|u)[:n][:s<stride>[i|f|u]][:o<offset>[i|f|u]][:i<divisor>]
   *
   * Any part of the format can be left out, but the order matters (limitations of regex)
   *
   * Ex:
   *   pos:3f          --- 3 floats
   *   pos:3f:o3f      --- 3 floats, offset 3 floats
   *   pos:3f:s3f      --- 3 floats, stride is 3 floats
   *   pos:3f color:3f --- 3 position floats, 3 color floats
   *   pos:3f:i1       --- 3 floats, instanced with a divisor of 1
   */
  void attrib(Shader& shader, BufTarget target, Buffer& buf, const std::string& desc);
  void attrib(Shader& shader, Buffer& buf, const std::string& desc);

  void element_array(Buffer& buf);

  void draw_arrays(const DrawMode& mode, GLsizei count, int first = 0);

  void multi_draw_arrays_indirect(const DrawMode& mode, GLsizei drawcount, GLsizei stride);

private:
  struct VertexAttrib_ {
    GLuint index;
    GLint size;
    GLenum type;
    GLboolean normalized;
    GLsizei stride;
    GLsizei offset;
    GLsizei divisor;
  };

  void gen_id_();
  void del_id_();
};
} // namespace imp

#endif//IMP_GFX_GL_VERTEX_ARRAY_HPP
