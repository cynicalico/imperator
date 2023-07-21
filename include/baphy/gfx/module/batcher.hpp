#ifndef BAPHY_GFX_MODULE_BATCHER_HPP
#define BAPHY_GFX_MODULE_BATCHER_HPP

#include "baphy/core/module_mgr.hpp"
#include "baphy/gfx/internal/gl/vec_buffer.hpp"
#include "baphy/gfx/internal/gl/vertex_array.hpp"
#include "baphy/gfx/module/gfx_context.hpp"
#include "baphy/gfx/module/shader_mgr.hpp"

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

enum class TBatchType {
  none, tri, line, point, tex
};

using TDrawCall = std::function<void(glm::mat4, float)>;

class TBatch {
public:
  TBatch(const std::shared_ptr<GfxContext> &gfx, std::shared_ptr<Shader> shader,
         std::size_t floats_per_obj, const std::string &attrib_format,
         DrawMode draw_mode, std::size_t draw_divisor)
      : shader_(std::move(shader)),
        vao_(*gfx),
        vbo_(*gfx, floats_per_obj, false, BufTarget::array, BufUsage::dynamic_draw),
        draw_mode_(draw_mode),
        draw_divisor_(draw_divisor) {
    vao_.attrib(*shader_, vbo_, attrib_format);
  }

  std::size_t size() const;

  void clear();

  void add(std::initializer_list<float> data);

  TDrawCall get_draw_call();

private:
  std::shared_ptr<Shader> shader_{nullptr};

  VertexArray vao_;
  FVBuffer vbo_;

  DrawMode draw_mode_;
  std::size_t draw_divisor_;

  std::size_t draw_start_offset_{0};
};

class TBatchList {
public:
  std::shared_ptr<GfxContext> gfx;
  std::shared_ptr<Shader> shader;

  TBatchList(std::shared_ptr<GfxContext> gfx, std::shared_ptr<Shader> shader,
             std::size_t floats_per_obj, std::string attrib_format,
             DrawMode draw_mode, std::size_t draw_divisor)
      : gfx(std::move(gfx)), shader(std::move(shader)),
        floats_per_obj_(floats_per_obj),  attrib_format_(std::move(attrib_format)),
        draw_mode_(draw_mode), draw_divisor_(draw_divisor) {}

  std::size_t size() const;

  void clear();

  void add(std::initializer_list<float> data);

  std::vector<TDrawCall> get_draw_calls();

private:
  std::vector<TBatch> batches_{};
  std::size_t curr_batch_{0};

  std::size_t floats_per_obj_;
  std::string attrib_format_;

  DrawMode draw_mode_;
  std::size_t draw_divisor_;

  std::vector<TDrawCall> stored_draw_calls_{};
};

class Batcher : public Module<Batcher> {
public:
  std::shared_ptr<GfxContext> gfx{nullptr};
  std::shared_ptr<ShaderMgr> shaders{nullptr};

  float z{1.0f};

  Batcher() : Module<Batcher>({EPI<GfxContext>::name, EPI<ShaderMgr>::name}) {}

  void o_add_tri(std::initializer_list<float> data);
  void o_add_line(std::initializer_list<float> data);
  void o_add_point(std::initializer_list<float> data);

  void t_add_tri(std::initializer_list<float> data);
  void t_add_line(std::initializer_list<float> data);
  void t_add_point(std::initializer_list<float> data);

//  void add_tex(GLuint id, std::initializer_list<float> data);

private:
  std::shared_ptr<Shader> tri_shader_{nullptr};
  std::shared_ptr<Shader> line_shader_{nullptr};
  std::shared_ptr<Shader> point_shader_{nullptr};

  std::unique_ptr<OBatchList> o_tri_batches_{nullptr};
  std::unique_ptr<OBatchList> o_line_batches_{nullptr};
  std::unique_ptr<OBatchList> o_point_batches_{nullptr};

  TBatchType last_batch_type_{TBatchType::none};
  std::vector<TDrawCall> draw_calls_{};
  std::unique_ptr<TBatchList> t_tri_batches_{nullptr};
  std::unique_ptr<TBatchList> t_line_batches_{nullptr};
  std::unique_ptr<TBatchList> t_point_batches_{nullptr};

  void check_get_draw_calls_(TBatchType t);

  void draw_(glm::mat4 projection);

  void e_initialize_(const EInitialize &e) override;
  void e_shutdown_(const EShutdown &e) override;

  void e_primitives_req_z_(const EPrimitivesReqZ &e);
  void e_primitives_req_and_inc_z_(const EPrimitivesReqAndIncZ &e);
  void e_flush_(const EFlush &e);
};

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::Batcher);

#endif//BAPHY_GFX_MODULE_BATCHER_HPP
