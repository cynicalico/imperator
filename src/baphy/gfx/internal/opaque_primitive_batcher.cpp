#include "baphy/gfx/internal/opaque_primitive_batcher.hpp"

#include "baphy/core/module_mgr.hpp"

namespace baphy {

std::size_t OBatch::size() const {
  return vbo_.size();
}

void OBatch::add(std::initializer_list<float> data) {
  vbo_.add(data);
}

void OBatch::draw(glm::mat4 projection, float z_max) {
  vbo_.sync();

  shader_->use();
  shader_->uniform_mat4f("mvp", projection);
  shader_->uniform_1f("z_max", z_max);

  vao_.draw_arrays(draw_mode_, vbo_.size() / draw_divisor_, vbo_.front() / draw_divisor_);
}

void OBatch::clear() {
  vbo_.clear();
}

std::size_t OBatchList::size() const {
  return batches_[curr_batch_].size();
}

void OBatchList::clear() {
  for (auto &b: batches_)
    b.clear();

  curr_batch_ = 0;
}

void OBatchList::add(std::initializer_list<float> data) {
  if (batches_.empty())
    batches_.emplace_back(gfx, shader, floats_per_obj_, attrib_format_, draw_mode_, draw_divisor_);

  else if (batches_[curr_batch_].size() > 500'000) {
    if (curr_batch_ == batches_.size() - 1)
      batches_.emplace_back(gfx, shader, floats_per_obj_, attrib_format_, draw_mode_, draw_divisor_);

    curr_batch_++;
  }

  batches_[curr_batch_].add(data);
}

void OBatchList::draw(glm::mat4 projection, float z_max) {
  if (batches_.empty())
    return;

  for (int i = curr_batch_; i >= 0; --i)
    batches_[i].draw(projection, z_max);
}

OpaquePrimitiveBatcher::OpaquePrimitiveBatcher(ModuleMgr &module_mgr) {
  shaders = module_mgr.get<ShaderMgr>();

  auto tris_src = ShaderSrc::parse(DATA_FOLDER / "shader" / "tris.glsl");
  if (tris_src)
    tri_shader_ = shaders->compile(*tris_src);

  auto line_src = ShaderSrc::parse(DATA_FOLDER / "shader" / "lines.glsl");
  if (line_src)
    line_shader_ = shaders->compile(*line_src);

  auto point_src = ShaderSrc::parse(DATA_FOLDER / "shader" / "points.glsl");
  if (point_src)
    point_shader_ = shaders->compile(*point_src);

  auto gfx = module_mgr.get<GfxContext>();
  o_tri_batches_ = std::make_unique<OBatchList>(gfx, tri_shader_, 10 * 3, "in_pos:3f in_color:4f in_trans:3f", DrawMode::triangles, 10);
  o_line_batches_ = std::make_unique<OBatchList>(gfx, line_shader_, 10 * 2, "in_pos:3f in_color:4f in_trans:3f", DrawMode::lines, 10);
  o_point_batches_ = std::make_unique<OBatchList>(gfx, point_shader_, 7 * 1, "in_pos:3f in_color:4f", DrawMode::points, 7);
}

void OpaquePrimitiveBatcher::add_tri(std::initializer_list<float> data) {
  o_tri_batches_->add(data);
}

void OpaquePrimitiveBatcher::add_line(std::initializer_list<float> data) {
  o_line_batches_->add(data);
}

void OpaquePrimitiveBatcher::add_point(std::initializer_list<float> data) {
  o_point_batches_->add(data);
}

void OpaquePrimitiveBatcher::draw(glm::mat4 projection, float z_max) {
  o_tri_batches_->draw(projection, z_max);
  o_line_batches_->draw(projection, z_max);
  o_point_batches_->draw(projection, z_max);

  o_tri_batches_->clear();
  o_line_batches_->clear();
  o_point_batches_->clear();
}

} // namespace baphy
