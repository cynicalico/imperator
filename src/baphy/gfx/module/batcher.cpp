#include "baphy/gfx/module/batcher.hpp"

#include "baphy/core/module/application.hpp"

namespace baphy {

void Batcher::add_o_primitive(const std::vector<float> &data, float z) {
  check_add_new_o_primitive_batch_();
  o_primitive_vbos_[curr_o_primitive_batch_]->add(data);
  o_primitive_max_z_ = std::max(o_primitive_max_z_, z);
}

void Batcher::add_o_primitive(std::initializer_list<float> data, float z) {
  check_add_new_o_primitive_batch_();
  o_primitive_vbos_[curr_o_primitive_batch_]->add(data);
  o_primitive_max_z_ = std::max(o_primitive_max_z_, z);
}

void Batcher::new_o_primitive_batch_() {
  curr_o_primitive_batch_++;

  if (curr_o_primitive_batch_ >= o_primitive_vaos_.size()) {
    o_primitive_vaos_.emplace_back(std::make_unique<VertexArray>(*gfx));
    o_primitive_vbos_.emplace_back(std::make_unique<FVBuffer>(
        *gfx, 10 * 6, true, BufTarget::array, BufUsage::dynamic_draw));

    o_primitive_vaos_.back()->attrib(
        *primitive_shader_,
        *o_primitive_vbos_.back(),
        "in_pos:3f in_color:4f in_trans:3f"
    );
  }
}

void Batcher::check_add_new_o_primitive_batch_() {
  // 2 MB batches
//  if (o_primitive_vbos_[curr_o_primitive_batch_]->size() > 500'000)
//    new_o_primitive_batch_();
}

void Batcher::e_initialize_(const baphy::EInitialize &e) {
  gfx = module_mgr->get<GfxContext>();
  shaders = module_mgr->get<ShaderMgr>();

  auto primitives_src = ShaderSrc::parse(DATA_FOLDER / "shader" / "primitives.shader");
  if (primitives_src)
    primitive_shader_ = shaders->compile(*primitives_src);

  new_o_primitive_batch_();
  curr_o_primitive_batch_ = 0;  // Reset for first batch

  EventBus::sub<EDraw>(module_name, {EPI<Application>::name}, [&](const auto &e) { e_draw_(e); });

  Module::e_initialize_(e);
}

void Batcher::e_shutdown_(const baphy::EShutdown &e) {
  Module::e_shutdown_(e);
}

void Batcher::e_draw_(const EDraw &e) {
  primitive_shader_->use();
  primitive_shader_->uniform_mat4f("mvp", gfx->ortho_projection());
  primitive_shader_->uniform_1f("z_max", o_primitive_max_z_);

  for (int i = o_primitive_vaos_.size() - 1; i >= 0; --i) {
    o_primitive_vbos_[i]->sync();

    o_primitive_vaos_[i]->draw_arrays(
        DrawMode::triangles,
        o_primitive_vbos_[i]->size() / 10,
        o_primitive_vbos_[i]->front() / 10
    );

    o_primitive_vbos_[i]->clear();
  }

  curr_o_primitive_batch_ = 0;
  o_primitive_max_z_ = 1.0f;
}

} // namespace baphy
