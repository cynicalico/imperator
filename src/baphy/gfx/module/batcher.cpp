#include "baphy/gfx/module/batcher.hpp"

#include "baphy/util/io.hpp"

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

std::size_t TBatch::size() const {
  return vbo_.size();
}

void TBatch::clear() {
  vbo_.clear();
  draw_start_offset_ = 0;
}

void TBatch::add(std::initializer_list<float> data) {
  vbo_.add(data);
}

TDrawCall TBatch::get_draw_call() {
  int count = vbo_.size() - draw_start_offset_;

  int first = draw_start_offset_;
  draw_start_offset_ = vbo_.size();

  return [&, count, first](glm::mat4 projection, float z_max) {
    vbo_.sync();

    shader_->use();
    shader_->uniform_mat4f("mvp", projection);
    shader_->uniform_1f("z_max", z_max);

    vao_.draw_arrays(draw_mode_, count / draw_divisor_, first / draw_divisor_);
  };
}

std::size_t TBatchList::size() const {
  return batches_[curr_batch_].size();
}

void TBatchList::clear() {
  for (auto &b: batches_)
    b.clear();

  curr_batch_ = 0;
  stored_draw_calls_.clear();
}

void TBatchList::add(std::initializer_list<float> data) {
  if (batches_.empty())
    batches_.emplace_back(gfx, shader, floats_per_obj_, attrib_format_, draw_mode_, draw_divisor_);

  else if (batches_[curr_batch_].size() > 500'000) {
    stored_draw_calls_.emplace_back(batches_[curr_batch_].get_draw_call());

    if (curr_batch_ == batches_.size() - 1)
      batches_.emplace_back(gfx, shader, floats_per_obj_, attrib_format_, draw_mode_, draw_divisor_);

    curr_batch_++;
  }

  batches_[curr_batch_].add(data);
}

std::vector<TDrawCall> TBatchList::get_draw_calls() {
  std::vector<TDrawCall> draw_calls{};

  if (!stored_draw_calls_.empty()) {
    draw_calls.insert(draw_calls.begin(), stored_draw_calls_.begin(), stored_draw_calls_.end());
    stored_draw_calls_.clear();
  }

  draw_calls.emplace_back(batches_[curr_batch_].get_draw_call());

  return draw_calls;
}

void Batcher::o_add_tri(std::initializer_list<float> data) {
  o_tri_batches_->add(data);
  z += 1.0f;
}

void Batcher::o_add_line(std::initializer_list<float> data) {
  o_line_batches_->add(data);
  z += 1.0f;
}

void Batcher::o_add_point(std::initializer_list<float> data) {
  o_point_batches_->add(data);
  z += 1.0f;
}

void Batcher::t_add_tri(std::initializer_list<float> data) {
  check_get_draw_calls_(TBatchType::tri);
  t_tri_batches_->add(data);
  last_batch_type_ = TBatchType::tri;
  z += 1.0f;
}

void Batcher::t_add_line(std::initializer_list<float> data) {
  check_get_draw_calls_(TBatchType::line);
  t_line_batches_->add(data);
  last_batch_type_ = TBatchType::line;
  z += 1.0f;
}

void Batcher::t_add_point(std::initializer_list<float> data) {
  check_get_draw_calls_(TBatchType::point);
  t_point_batches_->add(data);
  last_batch_type_ = TBatchType::point;
  z += 1.0f;
}

void Batcher::check_get_draw_calls_(TBatchType t) {
  if (last_batch_type_ != TBatchType::none && last_batch_type_ != t) {
    std::vector<TDrawCall> new_draw_calls;
    switch (last_batch_type_) {
      case TBatchType::none:  std::unreachable();
      case TBatchType::tri:   new_draw_calls = t_tri_batches_->get_draw_calls(); break;
      case TBatchType::line:  new_draw_calls = t_line_batches_->get_draw_calls(); break;
      case TBatchType::point: new_draw_calls = t_point_batches_->get_draw_calls(); break;
      case TBatchType::tex:   break;
    }
    draw_calls_.insert(draw_calls_.begin(), new_draw_calls.begin(), new_draw_calls.end());
  }
}

void Batcher::draw_(glm::mat4 projection) {
  static auto do_draw_calls_ = [&](TBatchList &batch) {
    std::ranges::for_each(batch.get_draw_calls(), [&](auto fn) { fn(projection, z); });
  };

  o_tri_batches_->draw(projection, z);
  o_line_batches_->draw(projection, z);
  o_point_batches_->draw(projection, z);

  o_tri_batches_->clear();
  o_line_batches_->clear();
  o_point_batches_->clear();

  gfx->blend_func_separate(BlendFunc::one, BlendFunc::one_minus_src_alpha, BlendFunc::one_minus_dst_alpha, BlendFunc::one);
  gfx->enable(Capability::blend);
  gfx->depth_mask(false);

  for (const auto &call: draw_calls_)
    call(projection, z);

  switch (last_batch_type_) {
    case TBatchType::none:  break;
    case TBatchType::tri:   do_draw_calls_(*t_tri_batches_); break;
    case TBatchType::line:  do_draw_calls_(*t_line_batches_); break;
    case TBatchType::point: do_draw_calls_(*t_point_batches_); break;
    case TBatchType::tex:   break;
  }

  t_tri_batches_->clear();
  t_line_batches_->clear();
  t_point_batches_->clear();

  gfx->depth_mask(true);
  gfx->disable(Capability::blend);

  z = 1.0f;
  last_batch_type_ = TBatchType::none;
  draw_calls_.clear();
}

void Batcher::e_initialize_(const EInitialize &e) {
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

  o_tri_batches_ = std::make_unique<OBatchList>(gfx, tri_shader_, 10 * 3, "in_pos:3f in_color:4f in_trans:3f", DrawMode::triangles, 10);
  o_line_batches_ = std::make_unique<OBatchList>(gfx, line_shader_, 10 * 2, "in_pos:3f in_color:4f in_trans:3f", DrawMode::lines, 10);
  o_point_batches_ = std::make_unique<OBatchList>(gfx, point_shader_, 7 * 1, "in_pos:3f in_color:4f", DrawMode::points, 7);

  t_tri_batches_ = std::make_unique<TBatchList>(gfx, tri_shader_, 10 * 3, "in_pos:3f in_color:4f in_trans:3f", DrawMode::triangles, 10);
  t_line_batches_ = std::make_unique<TBatchList>(gfx, line_shader_, 10 * 2, "in_pos:3f in_color:4f in_trans:3f", DrawMode::lines, 10);
  t_point_batches_ = std::make_unique<TBatchList>(gfx, point_shader_, 7 * 1, "in_pos:3f in_color:4f", DrawMode::points, 7);

  EventBus::sub<EPrimitivesReqZ>(module_name, [&](const auto &e) { e_primitives_req_z_(e); });
  EventBus::sub<EPrimitivesReqAndIncZ>(module_name, [&](const auto &e) { e_primitives_req_and_inc_z_(e); });
  EventBus::sub<EFlush>(module_name, [&](const auto &e) { e_flush_(e); });

  Module::e_initialize_(e);
}

void Batcher::e_shutdown_(const EShutdown &e) {
  Module::e_shutdown_(e);
}

void Batcher::e_primitives_req_z_(const EPrimitivesReqZ &e) {
  EventBus::send_nowait<EPrimitivesRespZ>(z);
}

void Batcher::e_primitives_req_and_inc_z_(const EPrimitivesReqAndIncZ &e) {
  EventBus::send_nowait<EPrimitivesRespZ>(z);
  z += 1.0f;
}

void Batcher::e_flush_(const EFlush &e) {
  draw_(e.projection);
}

} // namespace baphy
