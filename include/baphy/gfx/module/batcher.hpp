#ifndef BAPHY_GFX_MODULE_BATCHER_HPP
#define BAPHY_GFX_MODULE_BATCHER_HPP

#include "baphy/core/module_mgr.hpp"
#include "baphy/gfx/internal/gl/vec_buffer.hpp"
#include "baphy/gfx/internal/gl/vertex_array.hpp"
#include "baphy/gfx/module/gfx_context.hpp"
#include "baphy/gfx/module/shader_mgr.hpp"
#include "baphy/util/io.hpp"

namespace baphy {

class Batcher : public Module<Batcher> {
public:
  std::shared_ptr<GfxContext> gfx{nullptr};
  std::shared_ptr<ShaderMgr> shaders{nullptr};

  Batcher() : Module<Batcher>({EPI<GfxContext>::name, EPI<ShaderMgr>::name}) {}
  ~Batcher() override = default;

  void add_o_primitive(const std::vector<float> &data, float z);
  void add_o_primitive(std::initializer_list<float> data, float z);

private:
  std::shared_ptr<Shader> primitive_shader_{nullptr};

  std::vector<std::unique_ptr<VertexArray>> o_primitive_vaos_{};
  std::vector<std::unique_ptr<FVBuffer>> o_primitive_vbos_{};
  float o_primitive_max_z_{1.0f};
  std::size_t curr_o_primitive_batch_{0};

  void new_o_primitive_batch_();
  void check_add_new_o_primitive_batch_();

  void e_initialize_(const EInitialize &e) override;
  void e_shutdown_(const EShutdown &e) override;

  void e_draw_(const EDraw &e);
};

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::Batcher);

#endif//BAPHY_GFX_MODULE_BATCHER_HPP
