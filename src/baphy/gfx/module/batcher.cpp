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

  vao_.draw_arrays(draw_mode_, vbo_.size() / draw_divisor_, vbo_.front() / draw_divisor_);
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
    batches_.emplace_back(gfx, shader, floats_per_obj_, fill_reverse_, attrib_format_, draw_mode_, draw_divisor_);

  else if (batches_[curr_batch_].size() > 500'000) {
    if (curr_batch_ == batches_.size() - 1)
      batches_.emplace_back(gfx, shader, floats_per_obj_, fill_reverse_, attrib_format_, draw_mode_, draw_divisor_);

    curr_batch_++;
  }

  batches_[curr_batch_].add(data);
}

void BatchList::draw(glm::mat4 projection, float z_max) {
  if (batches_.empty())
    return;

  for (int i = curr_batch_; i >= 0; --i)
    batches_[i].draw(projection, z_max);
}

void Batcher::add_o_tri(float z, std::initializer_list<float> data) {
  o_tri_batches_->add(data);
  z_max_ = std::max(z, z_max_);
}

void Batcher::add_o_line(float z, std::initializer_list<float> data) {
  o_line_batches_->add(data);
  z_max_ = std::max(z, z_max_);
}

void Batcher::add_o_point(float z, std::initializer_list<float> data) {
  o_point_batches_->add(data);
  z_max_ = std::max(z, z_max_);
}

void Batcher::add_t_tri(float z, std::initializer_list<float> data) { /* TODO */ }
void Batcher::add_t_line(float z, std::initializer_list<float> data) { /* TODO */ }
void Batcher::add_t_point(float z, std::initializer_list<float> data) { /* TODO */ }

void Batcher::e_initialize_(const baphy::EInitialize &e) {
  gfx = module_mgr->get<GfxContext>();
  shaders = module_mgr->get<ShaderMgr>();

  auto tris_src = ShaderSrc::parse(DATA_FOLDER / "shader" / "tris.glsl");
  if (tris_src)
    tri_shader_ = shaders->compile(*tris_src);

  auto line_src = ShaderSrc::parse(DATA_FOLDER / "shader" / "lines.glsl");
  if (line_src)
    line_shader_ = shaders->compile(*line_src);

  auto point_src = ShaderSrc::parse(DATA_FOLDER / "shader" / "points.glsl");
  if (point_src)
    point_shader_ = shaders->compile(*point_src);

  o_tri_batches_ = std::make_unique<BatchList>(gfx, tri_shader_, 10 * 3, true, "in_pos:3f in_color:4f in_trans:3f", DrawMode::triangles, 10);
  o_line_batches_ = std::make_unique<BatchList>(gfx, line_shader_, 10 * 2, true, "in_pos:3f in_color:4f in_trans:3f", DrawMode::lines, 10);
  o_point_batches_ = std::make_unique<BatchList>(gfx, point_shader_, 7 * 1, true, "in_pos:3f in_color:4f", DrawMode::points, 7);

  EventBus::sub<EDraw>(module_name, {EPI<Application>::name}, [&](const auto &e) { e_draw_(e); });

  Module::e_initialize_(e);
}

void Batcher::e_shutdown_(const baphy::EShutdown &e) {
  Module::e_shutdown_(e);
}

void Batcher::e_draw_(const EDraw &e) {
  o_tri_batches_->draw(gfx->ortho_projection(), z_max_);
  o_line_batches_->draw(gfx->ortho_projection(), z_max_);
  o_point_batches_->draw(gfx->ortho_projection(), z_max_);

  o_tri_batches_->clear();
  o_line_batches_->clear();
  o_point_batches_->clear();

  z_max_ = 1.0f;
}

} // namespace baphy
