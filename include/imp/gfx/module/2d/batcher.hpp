#ifndef IMP_GFX_MODULE_BATCHER_HPP
#define IMP_GFX_MODULE_BATCHER_HPP

#include "../../../core/module_mgr.hpp"
#include "../../gl/vec_buffer.hpp"
#include "../../gl/vertex_array.hpp"
#include "../shader_mgr.hpp"

namespace imp {
inline constexpr std::size_t BATCH_SIZE_LIMIT = 600'000;
using DrawCall = std::function<void(GladGLContext&, glm::mat4, float)>;

class Batch {
public:
  Batch(
    GfxContext& ctx, Shader& shader,
    DrawMode draw_mode, const std::string& attrib_desc,
    std::size_t vertices_per_obj, std::size_t floats_per_vertex, bool fill_reverse
  );

  std::size_t size() const;
  void clear();

  void add(std::initializer_list<float> data, std::initializer_list<unsigned int> indices, bool insert_restart);

  DrawCall get_draw_call_tex(GLuint id);
  DrawCall get_draw_call();

private:
  Shader& shader_;
  VertexArray vao_;
  FVBuffer vbo_;
  UVBuffer ebo_;
  unsigned int ebo_offset_{0};

  DrawMode draw_mode_;
  std::size_t floats_per_vertex_;
  bool fill_reverse_;
  int draw_start_offset_{0};
};

class BatchList {
public:
  BatchList(
    GfxContext& ctx, Shader& shader,
    DrawMode draw_mode, const std::string& attrib_desc,
    std::size_t vertices_per_obj, std::size_t floats_per_vertex, bool fill_reverse
  );

  std::size_t size() const;
  void clear();

  void add_tex(GLuint id, std::initializer_list<float> data, std::initializer_list<unsigned int> indices, bool insert_restart);
  void add(std::initializer_list<float> data, std::initializer_list<unsigned int> indices, bool insert_restart);

  std::vector<DrawCall> get_draw_calls_tex(GLuint id);
  std::vector<DrawCall> get_draw_calls();

private:
  GfxContext& ctx_;

  Shader& shader_;

  std::vector<Batch> batches_{};
  std::size_t curr_batch_{0};
  std::vector<DrawCall> stored_draw_calls_{};

  DrawMode draw_mode_;
  std::size_t vertices_per_obj_;
  std::string attrib_desc_;

  std::size_t floats_per_vertex_;
  bool fill_reverse_;
};

class Batcher : public Module<Batcher> {
public:
  float z{1.0f};

  std::shared_ptr<GfxContext> ctx{nullptr};
  std::shared_ptr<ShaderMgr> shaders{nullptr};

  explicit Batcher(const std::weak_ptr<ModuleMgr>& module_mgr);

  void add_opaque(const DrawMode& mode, std::initializer_list<float> data, std::initializer_list<unsigned int> indices, bool insert_restart = false);
  void add_trans(const DrawMode& mode, std::initializer_list<float> data, std::initializer_list<unsigned int> indices, bool insert_restart = false);

  void add_opaque_tex(GLuint id, std::initializer_list<float> data, std::initializer_list<unsigned int> indices);
  void add_trans_tex(GLuint id, std::initializer_list<float> data, std::initializer_list<unsigned int> indices);

  void draw(const glm::mat4& projection);

private:
  /* PRIMITIVES */
  std::unordered_map<DrawMode, std::shared_ptr<Shader>> shaders_{};
  std::unordered_map<DrawMode, std::string> attrib_descs_{};
  std::unordered_map<DrawMode, std::size_t> vertices_per_obj_{};
  std::unordered_map<DrawMode, std::size_t> floats_per_vertex_{};

  std::unordered_map<DrawMode, BatchList> opaque_batches_{};
  std::unordered_map<DrawMode, BatchList> trans_batches_{};

  /* TEXTURES */
  std::shared_ptr<Shader> tex_shader_{};
  std::vector<BatchList> tex_batches_{};

  GLuint last_tex_id_{0};

  /* GENERAL */
  DrawMode last_trans_draw_mode_{DrawMode::none};

  std::vector<DrawCall> opaque_draw_calls_{};
  std::vector<DrawCall> trans_draw_calls_{};

  void collect_opaque_draw_calls_();
  void collect_trans_draw_calls_();

  void clear_opaque_();
  void clear_trans_();
};
} // namespace imp

IMP_PRAISE_HERMES(imp::Batcher);

#endif//IMP_GFX_MODULE_BATCHER_HPP
