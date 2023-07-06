#include "baphy/gfx/module/batcher.hpp"

#include "baphy/core/module/application.hpp"

namespace baphy {

void Batcher::new_o_primitive_batch_() {
  if (curr_o_primitive_batch_ >= o_primitive_vaos_.size()) {
    o_primitive_vaos_.emplace_back(std::make_unique<VertexArray>(gfx));
    o_primitive_vbos_.emplace_back(std::make_unique<FVBuffer>(
        gfx, 10 * 3, true, BufTarget::array, BufUsage::stream_draw));
    o_primitive_max_z_.emplace_back();

    BAPHY_LOG_INFO("sdfosd: {}", o_primitive_vbos_.back()->id);

    o_primitive_vaos_.back()->attrib(
        *primitive_shader_,
        *o_primitive_vbos_.back(),
        "in_pos:3f in_color:4f in_trans:3f"
    );
  }
  curr_o_primitive_batch_++;
}

void Batcher::e_initialize_(const baphy::EInitialize &e) {
  gfx = module_mgr->get<GfxContext>();
  shaders = module_mgr->get<ShaderMgr>();

  auto primitives_src = ShaderSrc::parse(DATA_FOLDER / "shader" / "primitives.shader");
  if (primitives_src)
    primitive_shader_ = shaders->compile(*primitives_src);

  new_o_primitive_batch_();
  curr_o_primitive_batch_ = 0;  // Reset for first batch

  EventBus::sub<EOPrimitiveVertexData>(module_name, [&](const auto &e) { e_o_primitive_vertex_data_(e); });
//  EventBus::sub<ETPrimitiveVertexData>(module_name, [&](const auto &e) { e_t_primitive_vertex_data_(e); });
  EventBus::sub<EEndFrame>(module_name, {EPI<Application>::name}, [&](const auto &e) { e_end_frame_(e); });

  Module::e_initialize_(e);
}

void Batcher::e_shutdown_(const baphy::EShutdown &e) {
  Module::e_shutdown_(e);
}

void Batcher::e_o_primitive_vertex_data_(const EOPrimitiveVertexData &e) {
  o_primitive_vbos_[curr_o_primitive_batch_]->add(e.data);
  o_primitive_max_z_[curr_o_primitive_batch_] = std::max(o_primitive_max_z_[curr_o_primitive_batch_], e.z);
}

//void Batcher::e_t_primitive_vertex_data_(const ETPrimitiveVertexData &e) {}

void Batcher::e_end_frame_(const EEndFrame &e) {
  for (int i = o_primitive_vaos_.size() - 1; i >= 0; --i) {
    o_primitive_vbos_[i]->sync();

    primitive_shader_->use();
    primitive_shader_->uniform_mat4f("mvp", gfx->ortho_projection());
    primitive_shader_->uniform_1f("z_max", o_primitive_max_z_[i]);

    o_primitive_vaos_[i]->draw_arrays(
        DrawMode::triangles,
        o_primitive_vbos_[i]->size() / 10,
        o_primitive_vbos_[i]->front() / 10
    );

    o_primitive_vbos_[i]->clear();
  }

  curr_o_primitive_batch_ = 0;
}

} // namespace baphy
