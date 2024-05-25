#ifndef IMPERATOR_GL_ENUM_TYPES_H
#define IMPERATOR_GL_ENUM_TYPES_H

#include "imperator/util/enum_bitops.h"
#include "glad/gl.h"

namespace imp {
enum class Capability {
  blend = GL_BLEND,
  color_logic_op = GL_COLOR_LOGIC_OP,
  cull_face = GL_CULL_FACE,
  depth_clamp = GL_DEPTH_CLAMP,
  depth_test = GL_DEPTH_TEST,
  dither = GL_DITHER,
  framebuffer_srgb = GL_FRAMEBUFFER_SRGB,
  line_smooth = GL_LINE_SMOOTH,
  multisample = GL_MULTISAMPLE,
  polygon_offset_fill = GL_POLYGON_OFFSET_FILL,
  polygon_offset_line = GL_POLYGON_OFFSET_LINE,
  polygon_offset_point = GL_POLYGON_OFFSET_POINT,
  polygon_smooth = GL_POLYGON_SMOOTH,
  primitive_restart = GL_PRIMITIVE_RESTART,
  sample_alpha_to_coverage = GL_SAMPLE_ALPHA_TO_COVERAGE,
  sample_alpha_to_one = GL_SAMPLE_ALPHA_TO_ONE,
  sample_coverage = GL_SAMPLE_COVERAGE,
  scissor_test = GL_SCISSOR_TEST,
  stencil_test = GL_STENCIL_TEST,
  texture_cube_map_seamless = GL_TEXTURE_CUBE_MAP_SEAMLESS,
  program_point_size = GL_PROGRAM_POINT_SIZE
};

enum class ClearBit {
  stencil = GL_STENCIL_BUFFER_BIT,
  color = GL_COLOR_BUFFER_BIT,
  depth = GL_DEPTH_BUFFER_BIT
};

enum class BlendFunc {
  zero = GL_ZERO,
  one = GL_ONE,
  src_color = GL_SRC_COLOR,
  one_minus_src_color = GL_ONE_MINUS_SRC_COLOR,
  dst_color = GL_DST_COLOR,
  one_minus_dst_color = GL_ONE_MINUS_DST_COLOR,
  src_alpha = GL_SRC_ALPHA,
  one_minus_src_alpha = GL_ONE_MINUS_SRC_ALPHA,
  dst_alpha = GL_DST_ALPHA,
  one_minus_dst_alpha = GL_ONE_MINUS_DST_ALPHA,
  constant_color = GL_CONSTANT_COLOR,
  one_minus_constant_color = GL_ONE_MINUS_CONSTANT_COLOR,
  constant_alpha = GL_CONSTANT_ALPHA,
  one_minus_constant_alpha = GL_ONE_MINUS_CONSTANT_ALPHA,
  src_alpha_saturate = GL_SRC_ALPHA_SATURATE,
  src1_color = GL_SRC1_COLOR,
  one_minus_src1_color = GL_ONE_MINUS_SRC1_COLOR,
  src1_alpha = GL_SRC1_ALPHA,
  one_minus_src1_alpha = GL_ONE_MINUS_SRC1_ALPHA
};
} // namespace imp

ENUM_ENABLE_BITOPS(imp::ClearBit);

#endif//IMPERATOR_GL_ENUM_TYPES_H
