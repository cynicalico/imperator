#ifndef BAPHY_GFX_MODULE_BATCHER_HPP
#define BAPHY_GFX_MODULE_BATCHER_HPP

#include <utility>

#include "baphy/core/module_mgr.hpp"
#include "baphy/gfx/internal/gl/vec_buffer.hpp"
#include "baphy/gfx/internal/gl/vertex_array.hpp"
#include "baphy/gfx/module/gfx_context.hpp"
#include "baphy/gfx/module/shader_mgr.hpp"
#include "baphy/util/io.hpp"

namespace baphy {

class Batch {
public:
  Batch(const std::shared_ptr<GfxContext> &gfx, std::shared_ptr<Shader> shader,
        std::size_t floats_per_obj, bool fill_reverse)
    : shader_(std::move(shader)),
      vao_(*gfx),
      vbo_(*gfx, floats_per_obj, fill_reverse, BufTarget::array, BufUsage::dynamic_draw) {
    vao_.attrib(*shader_, vbo_, "in_pos:3f in_color:4f in_trans:3f");
  }

  std::size_t size() const;

  void clear();

  void add(std::initializer_list<float> data);

  void draw(glm::mat4 projection, float z_max);

private:
  std::shared_ptr<Shader> shader_{nullptr};

  VertexArray vao_;
  FVBuffer vbo_;
};

class BatchList {
public:
  std::shared_ptr<GfxContext> gfx;
  std::shared_ptr<Shader> shader;

  BatchList(std::shared_ptr<GfxContext> gfx, std::shared_ptr<Shader> shader,
            std::size_t floats_per_obj, bool fill_reverse)
      : gfx(std::move(gfx)), shader(std::move(shader)),
        floats_per_obj_(floats_per_obj), fill_reverse_(fill_reverse) {}

  std::size_t size() const;

  void clear();

  void add(std::initializer_list<float> data);

  void draw(glm::mat4 projection, float z_max);

private:
  std::vector<Batch> batches_{};
  std::size_t curr_batch_{0};

  std::size_t floats_per_obj_;
  bool fill_reverse_;
};

class Batcher : public Module<Batcher> {
public:
  std::shared_ptr<GfxContext> gfx{nullptr};
  std::shared_ptr<ShaderMgr> shaders{nullptr};

  Batcher() : Module<Batcher>({EPI<GfxContext>::name, EPI<ShaderMgr>::name}) {}
  ~Batcher() override = default;

  void add_o_primitive(float z, std::initializer_list<float> data);

private:
  float z_max_{1.0f};

  std::shared_ptr<Shader> primitive_shader_{nullptr};
  std::unique_ptr<BatchList> o_primitive_batches_{nullptr};

  void e_initialize_(const EInitialize &e) override;
  void e_shutdown_(const EShutdown &e) override;

  void e_draw_(const EDraw &e);
};

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::Batcher);

#endif//BAPHY_GFX_MODULE_BATCHER_HPP
