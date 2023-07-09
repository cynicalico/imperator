#include "baphy/gfx/module/batcher.hpp"

#include "baphy/core/module/application.hpp"

namespace baphy {

std::size_t Batch::size() const {
  return vbo_.size();
}

void Batch::add(std::initializer_list<float> data) {
  vbo_.add(data);
}

void Batch::draw(glm::mat4 projection, float z_max) {
  vbo_.sync();

  shader_->use();
  shader_->uniform_mat4f("mvp", projection);
  shader_->uniform_1f("z_max", z_max);

  vao_.draw_arrays(DrawMode::triangles, vbo_.size() / 10, vbo_.front() / 10);
}

void Batch::clear() {
  vbo_.clear();
}

std::size_t BatchList::size() const {
  return batches_[curr_batch_].size();
}

void BatchList::clear() {
  for (auto &b: batches_)
    b.clear();

  curr_batch_ = 0;
}

void BatchList::add(std::initializer_list<float> data) {
  if (batches_.empty())
    batches_.emplace_back(gfx, shader, floats_per_obj_, fill_reverse_);

  else if (batches_[curr_batch_].size() > 500'000) {
    if (curr_batch_ == batches_.size() - 1)
      batches_.emplace_back(gfx, shader, floats_per_obj_, fill_reverse_);

    curr_batch_++;
  }

  batches_[curr_batch_].add(data);
}

void BatchList::draw(glm::mat4 projection, float z_max) {
  for (int i = curr_batch_; i >= 0; --i)
    batches_[i].draw(projection, z_max);
}

void Batcher::add_o_primitive(float z, std::initializer_list<float> data) {
  o_primitive_batches_->add(data);

  z_max_ = std::max(z, z_max_);
}

void Batcher::e_initialize_(const baphy::EInitialize &e) {
  gfx = module_mgr->get<GfxContext>();
  shaders = module_mgr->get<ShaderMgr>();

  auto primitives_src = ShaderSrc::parse(DATA_FOLDER / "shader" / "primitives.shader");
  if (primitives_src)
    primitive_shader_ = shaders->compile(*primitives_src);

  o_primitive_batches_ = std::make_unique<BatchList>(gfx, primitive_shader_, 10 * 3, true);

  EventBus::sub<EDraw>(module_name, {EPI<Application>::name}, [&](const auto &e) { e_draw_(e); });

  Module::e_initialize_(e);
}

void Batcher::e_shutdown_(const baphy::EShutdown &e) {
  Module::e_shutdown_(e);
}

void Batcher::e_draw_(const EDraw &e) {
  o_primitive_batches_->draw(gfx->ortho_projection(), z_max_);

  o_primitive_batches_->clear();

  z_max_ = 1.0f;
}

} // namespace baphy
