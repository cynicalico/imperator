#include "baphy/gfx/internal/trans_primitive_batcher.hpp"

#include <ranges>

namespace baphy {

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

TransPrimitiveBatcher::TransPrimitiveBatcher(ModuleMgr &module_mgr) {
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
  tri_batches_ = std::make_unique<TBatchList>(gfx, tri_shader_, 10 * 3, "in_pos:3f in_color:4f in_trans:3f", DrawMode::triangles, 10);
  line_batches_ = std::make_unique<TBatchList>(gfx, line_shader_, 10 * 2, "in_pos:3f in_color:4f in_trans:3f", DrawMode::lines, 10);
  point_batches_ = std::make_unique<TBatchList>(gfx, point_shader_, 7 * 1, "in_pos:3f in_color:4f", DrawMode::points, 7);
}

void TransPrimitiveBatcher::add_tri(std::initializer_list<float> data) {
  check_get_draw_calls_(TBatchType::tri);
  tri_batches_->add(data);

  last_batch_type_ = TBatchType::tri;
}

void TransPrimitiveBatcher::add_line(std::initializer_list<float> data) {
  check_get_draw_calls_(TBatchType::line);
  line_batches_->add(data);

  last_batch_type_ = TBatchType::line;
}

void TransPrimitiveBatcher::add_point(std::initializer_list<float> data) {
  check_get_draw_calls_(TBatchType::point);
  point_batches_->add(data);

  last_batch_type_ = TBatchType::point;
}

void TransPrimitiveBatcher::draw(glm::mat4 projection, float z_max) {
  static auto do_draw_calls_ = [&](TBatchList &batch) {
    std::ranges::for_each(batch.get_draw_calls(), [=](auto fn) { fn(projection, z_max); });
  };

  for (const auto &call: draw_calls_)
    call(projection, z_max);

  switch (last_batch_type_) {
    case TBatchType::none:  break;
    case TBatchType::tri:   do_draw_calls_(*tri_batches_); break;
    case TBatchType::line:  do_draw_calls_(*line_batches_); break;
    case TBatchType::point: do_draw_calls_(*point_batches_); break;
  }

  tri_batches_->clear();
  line_batches_->clear();
  point_batches_->clear();

  draw_calls_.clear();
  last_batch_type_ = TBatchType::none;
}

void TransPrimitiveBatcher::check_get_draw_calls_(TBatchType t) {
  if (last_batch_type_ != TBatchType::none && last_batch_type_ != t) {
    std::vector<TDrawCall> new_draw_calls;
    switch (last_batch_type_) {
      case TBatchType::none:  std::unreachable();
      case TBatchType::tri:   new_draw_calls = tri_batches_->get_draw_calls(); break;
      case TBatchType::line:  new_draw_calls = line_batches_->get_draw_calls(); break;
      case TBatchType::point: new_draw_calls = point_batches_->get_draw_calls(); break;
    }
    draw_calls_.insert(draw_calls_.begin(), new_draw_calls.begin(), new_draw_calls.end());
  }
}

} // namespace baphy
