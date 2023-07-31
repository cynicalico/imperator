#ifndef BAPHY_GFX_MODULE_BATCHER_HPP
#define BAPHY_GFX_MODULE_BATCHER_HPP

#include "baphy/core/module_mgr.hpp"
#include "baphy/gfx/internal/gl/vec_buffer.hpp"
#include "baphy/gfx/internal/gl/vertex_array.hpp"
#include "baphy/gfx/module/gfx_context.hpp"
#include "baphy/gfx/module/shader_mgr.hpp"

namespace baphy {

enum class TBatchType {
  none, tri, line, point, tex
};

using DrawCall = std::function<void(GladGLContext &, glm::mat4, float)>;

const inline std::size_t BATCH_SIZE_LIMIT = 600'000;

class OBatch {
public:
  OBatch(GfxContext &gfx, Shader &shader,
         std::size_t floats_per_obj, const std::string &attrib_format,
         DrawMode draw_mode, std::size_t draw_divisor)
      : shader_(shader),
        vao_(gfx),
        vbo_(gfx, floats_per_obj, true, BufTarget::array, BufUsage::dynamic_draw),
        draw_mode_(draw_mode),
        draw_divisor_(draw_divisor) {
    vao_.attrib(shader_, vbo_, attrib_format);
  }

  std::size_t size() const;

  void clear();

  void add(std::initializer_list<float> data);

  void draw(glm::mat4 projection, float z_max);

private:
  Shader &shader_;

  VertexArray vao_;
  FVBuffer vbo_;

  DrawMode draw_mode_;
  std::size_t draw_divisor_;
};

class OBatchList {
public:
  OBatchList(std::shared_ptr<GfxContext> gfx, std::shared_ptr<Shader> shader,
             std::size_t floats_per_obj, std::string attrib_format,
             DrawMode draw_mode, std::size_t draw_divisor)
      : gfx_(std::move(gfx)), shader_(std::move(shader)),
        floats_per_obj_(floats_per_obj),  attrib_format_(std::move(attrib_format)),
        draw_mode_(draw_mode), draw_divisor_(draw_divisor) {}

  std::size_t size() const;

  void clear();

  void add(std::initializer_list<float> data);

  void draw(glm::mat4 projection, float z_max);

private:
  std::shared_ptr<GfxContext> gfx_;
  std::shared_ptr<Shader> shader_;

  std::vector<std::unique_ptr<OBatch>> batches_{};
  std::size_t curr_batch_{0};

  std::size_t floats_per_obj_;
  std::string attrib_format_;

  DrawMode draw_mode_;
  std::size_t draw_divisor_;
};

class TBatch {
public:
  TBatch(GfxContext &gfx, Shader &shader,
         std::size_t floats_per_obj, const std::string &attrib_format,
         DrawMode draw_mode, std::size_t draw_divisor)
      : shader_(shader),
        vao_(gfx),
        vbo_(gfx, floats_per_obj, false, BufTarget::array, BufUsage::dynamic_draw),
        draw_mode_(draw_mode),
        draw_divisor_(draw_divisor) {
    vao_.attrib(shader_, vbo_, attrib_format);
  }

  std::size_t size() const;

  void clear();

  void add(std::initializer_list<float> data);

  DrawCall get_draw_call();

private:
  Shader &shader_;

  VertexArray vao_;
  FVBuffer vbo_;

  DrawMode draw_mode_;
  std::size_t draw_divisor_;

  std::size_t draw_start_offset_{0};
};

class TBatchList {
public:
  TBatchList(std::shared_ptr<GfxContext> gfx, std::shared_ptr<Shader> shader,
             std::size_t floats_per_obj, std::string attrib_format,
             DrawMode draw_mode, std::size_t draw_divisor)
      : gfx_(std::move(gfx)), shader_(std::move(shader)),
        floats_per_obj_(floats_per_obj),  attrib_format_(std::move(attrib_format)),
        draw_mode_(draw_mode), draw_divisor_(draw_divisor) {}

  std::size_t size() const;

  void clear();

  void add(std::initializer_list<float> data);

  std::vector<DrawCall> get_draw_calls();

private:
  std::shared_ptr<GfxContext> gfx_;
  std::shared_ptr<Shader> shader_;

  std::vector<std::unique_ptr<TBatch>> batches_{};
  std::size_t curr_batch_{0};

  std::size_t floats_per_obj_;
  std::string attrib_format_;

  DrawMode draw_mode_;
  std::size_t draw_divisor_;

  std::vector<DrawCall> stored_draw_calls_{};
};

class OTexBatch {
public:
  OTexBatch(GfxContext &gfx, Shader &shader,
         std::size_t floats_per_obj, const std::string &attrib_format,
         DrawMode draw_mode, std::size_t draw_divisor)
      : shader_(shader),
        vao_(gfx),
        vbo_(gfx, BATCH_SIZE_LIMIT, true, BufTarget::array, BufUsage::dynamic_draw),
        draw_mode_(draw_mode),
        draw_divisor_(draw_divisor) {
    vao_.attrib(shader_, vbo_, attrib_format);
  }

  std::size_t size() const;
  std::size_t capacity() const;

  void clear();

  void add(std::initializer_list<float> data);

  DrawCall get_draw_call(GLuint id);

private:
  Shader &shader_;

  VertexArray vao_;
  FVBuffer vbo_;

  DrawMode draw_mode_;
  std::size_t draw_divisor_;

  std::size_t draw_start_offset_{0};
};

class OTexBatchList {
public:
  OTexBatchList(std::shared_ptr<GfxContext> gfx, std::shared_ptr<Shader> shader,
             std::size_t floats_per_obj, std::string attrib_format,
             DrawMode draw_mode, std::size_t draw_divisor)
      : gfx_(std::move(gfx)), shader_(std::move(shader)),
        floats_per_obj_(floats_per_obj),  attrib_format_(std::move(attrib_format)),
        draw_mode_(draw_mode), draw_divisor_(draw_divisor) {}

  std::size_t size() const;

  void clear();

  void add(GLuint id, std::initializer_list<float> data);

  std::vector<DrawCall> get_draw_calls(GLuint id);

private:
  std::shared_ptr<GfxContext> gfx_;
  std::shared_ptr<Shader> shader_;

  std::vector<std::unique_ptr<OTexBatch>> batches_{};
  std::size_t curr_batch_{0};

  std::size_t floats_per_obj_;
  std::string attrib_format_;

  DrawMode draw_mode_;
  std::size_t draw_divisor_;

  std::vector<DrawCall> stored_draw_calls_{};
};

class TTexBatch {
public:
  TTexBatch(GfxContext &gfx, Shader &shader,
            std::size_t floats_per_obj, const std::string &attrib_format,
            DrawMode draw_mode, std::size_t draw_divisor)
      : shader_(shader),
        vao_(gfx),
        vbo_(gfx, floats_per_obj, false, BufTarget::array, BufUsage::dynamic_draw),
        draw_mode_(draw_mode),
        draw_divisor_(draw_divisor) {
    vao_.attrib(shader_, vbo_, attrib_format);
  }

  std::size_t size() const;
  std::size_t capacity() const;

  void clear();

  void add(std::initializer_list<float> data);

  DrawCall get_draw_call(GLuint id);

private:
  Shader &shader_;

  VertexArray vao_;
  FVBuffer vbo_;

  DrawMode draw_mode_;
  std::size_t draw_divisor_;

  std::size_t draw_start_offset_{0};
};

class TTexBatchList {
public:
  TTexBatchList(std::shared_ptr<GfxContext> gfx, std::shared_ptr<Shader> shader,
                std::size_t floats_per_obj, std::string attrib_format,
                DrawMode draw_mode, std::size_t draw_divisor)
      : gfx_(std::move(gfx)), shader_(shader),
        floats_per_obj_(floats_per_obj),  attrib_format_(std::move(attrib_format)),
        draw_mode_(draw_mode), draw_divisor_(draw_divisor) {}

  std::size_t size() const;

  void clear();

  void add(GLuint id, std::initializer_list<float> data);

  std::vector<DrawCall> get_draw_calls(GLuint id);

private:
  std::shared_ptr<GfxContext> gfx_{nullptr};
  std::shared_ptr<Shader> shader_{nullptr};

  std::vector<std::unique_ptr<TTexBatch>> batches_{};
  std::size_t curr_batch_{0};

  std::size_t floats_per_obj_;
  std::string attrib_format_;

  DrawMode draw_mode_;
  std::size_t draw_divisor_;

  std::vector<DrawCall> stored_draw_calls_{};
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
  void o_add_tex(GLuint id, std::initializer_list<float> data);

  void t_add_tri(std::initializer_list<float> data);
  void t_add_line(std::initializer_list<float> data);
  void t_add_point(std::initializer_list<float> data);
  void t_add_tex(GLuint id, std::initializer_list<float> data);

private:
  std::shared_ptr<Shader> tri_shader_{nullptr};
  std::shared_ptr<Shader> line_shader_{nullptr};
  std::shared_ptr<Shader> point_shader_{nullptr};
  std::shared_ptr<Shader> tex_shader_{nullptr};

  std::vector<DrawCall> o_draw_calls_{};
  std::unique_ptr<OBatchList> o_tri_batches_{nullptr};
  std::unique_ptr<OBatchList> o_line_batches_{nullptr};
  std::unique_ptr<OBatchList> o_point_batches_{nullptr};
  std::unique_ptr<OTexBatchList> o_tex_batches_{nullptr};

  TBatchType last_batch_type_{TBatchType::none};
  std::vector<DrawCall> t_draw_calls_{};
  std::unique_ptr<TBatchList> t_tri_batches_{nullptr};
  std::unique_ptr<TBatchList> t_line_batches_{nullptr};
  std::unique_ptr<TBatchList> t_point_batches_{nullptr};
  std::unique_ptr<TTexBatchList> t_tex_batches_{nullptr};

  GLuint last_o_tex_id_{0};
  GLuint last_t_tex_id_{0};
  void check_get_tex_draw_calls_(GLuint id);
  void check_get_draw_calls_(TBatchType t, GLuint id = 0);

  void draw_(glm::mat4 projection);

  void e_initialize_(const EInitialize &e) override;
  void e_shutdown_(const EShutdown &e) override;

  void e_flush_(const EFlush &e);
};

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::Batcher);

#endif//BAPHY_GFX_MODULE_BATCHER_HPP
