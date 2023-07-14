#ifndef BAPHY_GFX_INTERNAL_OPAQUE_PRIMITIVE_BATCHER_HPP
#define BAPHY_GFX_INTERNAL_OPAQUE_PRIMITIVE_BATCHER_HPP

#include "baphy/core/module_mgr.hpp"
#include "baphy/gfx/internal/gl/vec_buffer.hpp"
#include "baphy/gfx/internal/gl/vertex_array.hpp"
#include "baphy/gfx/module/gfx_context.hpp"
#include "baphy/gfx/module/shader_mgr.hpp"
#include "baphy/util/io.hpp"

namespace baphy {

class OBatch {
public:
  OBatch(const std::shared_ptr<GfxContext> &gfx, std::shared_ptr<Shader> shader,
         std::size_t floats_per_obj, const std::string &attrib_format,
         DrawMode draw_mode, std::size_t draw_divisor)
      : shader_(std::move(shader)),
        vao_(*gfx),
        vbo_(*gfx, floats_per_obj, true, BufTarget::array, BufUsage::dynamic_draw),
        draw_mode_(draw_mode),
        draw_divisor_(draw_divisor) {
    vao_.attrib(*shader_, vbo_, attrib_format);
  }

  std::size_t size() const;

  void clear();

  void add(std::initializer_list<float> data);

  void draw(glm::mat4 projection, float z_max);

private:
  std::shared_ptr<Shader> shader_{nullptr};

  VertexArray vao_;
  FVBuffer vbo_;

  DrawMode draw_mode_;
  std::size_t draw_divisor_;
};

class OBatchList {
public:
  std::shared_ptr<GfxContext> gfx;
  std::shared_ptr<Shader> shader;

  OBatchList(std::shared_ptr<GfxContext> gfx, std::shared_ptr<Shader> shader,
             std::size_t floats_per_obj, std::string attrib_format,
             DrawMode draw_mode, std::size_t draw_divisor)
      : gfx(std::move(gfx)), shader(std::move(shader)),
        floats_per_obj_(floats_per_obj),  attrib_format_(std::move(attrib_format)),
        draw_mode_(draw_mode), draw_divisor_(draw_divisor) {}

  std::size_t size() const;

  void clear();

  void add(std::initializer_list<float> data);

  void draw(glm::mat4 projection, float z_max);

private:
  std::vector<OBatch> batches_{};
  std::size_t curr_batch_{0};

  std::size_t floats_per_obj_;
  std::string attrib_format_;

  DrawMode draw_mode_;
  std::size_t draw_divisor_;
};

class OpaquePrimitiveBatcher {
public:
  std::shared_ptr<ShaderMgr> shaders{nullptr};

  OpaquePrimitiveBatcher(ModuleMgr &module_mgr);
  ~OpaquePrimitiveBatcher() = default;

  void add_tri(std::initializer_list<float> data);
  void add_line(std::initializer_list<float> data);
  void add_point(std::initializer_list<float> data);

  void draw(glm::mat4 projection, float z_max);

private:
  std::shared_ptr<Shader> tri_shader_{nullptr};
  std::shared_ptr<Shader> line_shader_{nullptr};
  std::shared_ptr<Shader> point_shader_{nullptr};

  std::unique_ptr<OBatchList> o_tri_batches_{nullptr};
  std::unique_ptr<OBatchList> o_line_batches_{nullptr};
  std::unique_ptr<OBatchList> o_point_batches_{nullptr};
};

} // namespace baphy

#endif//BAPHY_GFX_INTERNAL_OPAQUE_PRIMITIVE_BATCHER_HPP
