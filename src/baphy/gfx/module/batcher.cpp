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

  shader_.use();
  shader_.uniform_mat4f("mvp", projection);
  shader_.uniform_1f("z_max", z_max);

  vao_.draw_arrays(draw_mode_, vbo_.size() / draw_divisor_, vbo_.front() / draw_divisor_);
}

void OBatch::clear() {
  vbo_.clear();
}

std::size_t OBatchList::size() const {
  return batches_[curr_batch_]->size();
}

void OBatchList::clear() {
  for (auto &b: batches_)
    b->clear();

  curr_batch_ = 0;
}

void OBatchList::add(std::initializer_list<float> data) {
  if (batches_.empty())
    batches_.emplace_back(std::make_unique<OBatch>(*gfx_, *shader_, floats_per_obj_, attrib_format_, draw_mode_, draw_divisor_));

  else if (batches_[curr_batch_]->size() > BATCH_SIZE_LIMIT) {
    if (curr_batch_ == batches_.size() - 1)
      batches_.emplace_back(std::make_unique<OBatch>(*gfx_, *shader_, floats_per_obj_, attrib_format_, draw_mode_, draw_divisor_));

    curr_batch_++;
  }

  batches_[curr_batch_]->add(data);
}

void OBatchList::draw(glm::mat4 projection, float z_max) {
  if (batches_.empty())
    return;

  for (int i = curr_batch_; i >= 0; --i)
    batches_[i]->draw(projection, z_max);
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

DrawCall TBatch::get_draw_call() {
  int count = vbo_.size() - draw_start_offset_;

  int first = draw_start_offset_;
  draw_start_offset_ = vbo_.size();

  return [&, count, first](GladGLContext &gl, glm::mat4 projection, float z_max) {
    vbo_.sync();

    shader_.use();
    shader_.uniform_mat4f("mvp", projection);
    shader_.uniform_1f("z_max", z_max);

    vao_.draw_arrays(draw_mode_, count / draw_divisor_, first / draw_divisor_);
  };
}

std::size_t TBatchList::size() const {
  return batches_[curr_batch_]->size();
}

void TBatchList::clear() {
  for (auto &b: batches_)
    b->clear();

  curr_batch_ = 0;
  stored_draw_calls_.clear();
}

void TBatchList::add(std::initializer_list<float> data) {
  if (batches_.empty())
    batches_.emplace_back(std::make_unique<TBatch>(*gfx_, *shader_, floats_per_obj_, attrib_format_, draw_mode_, draw_divisor_));

  else if (batches_[curr_batch_]->size() > BATCH_SIZE_LIMIT) {
    stored_draw_calls_.emplace_back(batches_[curr_batch_]->get_draw_call());

    if (curr_batch_ == batches_.size() - 1)
      batches_.emplace_back(std::make_unique<TBatch>(*gfx_, *shader_, floats_per_obj_, attrib_format_, draw_mode_, draw_divisor_));

    curr_batch_++;
  }

  batches_[curr_batch_]->add(data);
}

std::vector<DrawCall> TBatchList::get_draw_calls() {
  std::vector<DrawCall> draw_calls{};

  if (!stored_draw_calls_.empty()) {
    draw_calls.insert(draw_calls.begin(), stored_draw_calls_.begin(), stored_draw_calls_.end());
    stored_draw_calls_.clear();
  }

  if (!batches_.empty())
    draw_calls.emplace_back(batches_[curr_batch_]->get_draw_call());

  return draw_calls;
}

std::size_t OTexBatch::size() const {
  return vbo_.size();
}

std::size_t OTexBatch::capacity() const {
  return vbo_.capacity();
}

void OTexBatch::clear() {
  vbo_.clear();
  draw_start_offset_ = 0;
}

void OTexBatch::add(std::initializer_list<float> data) {
  vbo_.add(data);
}

DrawCall OTexBatch::get_draw_call(GLuint id) {
  int count = vbo_.size() - draw_start_offset_;
  draw_start_offset_ = vbo_.size();

  int first = vbo_.front();

  return [&, id, count, first](GladGLContext &gl, glm::mat4 projection, float z_max) {
    vbo_.sync();

    shader_.use();
    shader_.uniform_mat4f("mvp", projection);
    shader_.uniform_1f("z_max", z_max);

    gl.BindTexture(GL_TEXTURE_2D, id);
    vao_.draw_arrays(draw_mode_, count / draw_divisor_, first / draw_divisor_);
    gl.BindTexture(GL_TEXTURE_2D, 0);
  };
}

std::size_t OTexBatchList::size() const {
  return batches_[curr_batch_]->size();
}

void OTexBatchList::clear() {
  for (auto &b: batches_)
    b->clear();

  curr_batch_ = 0;
  stored_draw_calls_.clear();
}

void OTexBatchList::add(GLuint id, std::initializer_list<float> data) {
  if (batches_.empty())
    batches_.emplace_back(std::make_unique<OTexBatch>(*gfx_, *shader_, floats_per_obj_, attrib_format_, draw_mode_, draw_divisor_));

  else if (batches_[curr_batch_]->size() + data.size() > batches_[curr_batch_]->capacity()) {
    stored_draw_calls_.emplace_back(batches_[curr_batch_]->get_draw_call(id));

    if (curr_batch_ == batches_.size() - 1)
      batches_.emplace_back(std::make_unique<OTexBatch>(*gfx_, *shader_, floats_per_obj_, attrib_format_, draw_mode_, draw_divisor_));

    curr_batch_++;
  }

  batches_[curr_batch_]->add(data);
}

std::vector<DrawCall> OTexBatchList::get_draw_calls(GLuint id) {
  std::vector<DrawCall> draw_calls{};

  if (!stored_draw_calls_.empty()) {
    draw_calls.insert(draw_calls.begin(), stored_draw_calls_.begin(), stored_draw_calls_.end());
    stored_draw_calls_.clear();
  }

  if (!batches_.empty())
    draw_calls.emplace_back(batches_[curr_batch_]->get_draw_call(id));

  return draw_calls;
}

std::size_t TTexBatch::size() const {
  return vbo_.size();
}

std::size_t TTexBatch::capacity() const {
  return vbo_.capacity();
}

void TTexBatch::clear() {
  vbo_.clear();
  draw_start_offset_ = 0;
}

void TTexBatch::add(std::initializer_list<float> data) {
  vbo_.add(data);
}

DrawCall TTexBatch::get_draw_call(GLuint id) {
  int count = vbo_.size() - draw_start_offset_;

  int first = draw_start_offset_;
  draw_start_offset_ = vbo_.size();

  return [&, id, count, first](GladGLContext &gl, glm::mat4 projection, float z_max) {
    vbo_.sync();

    shader_.use();
    shader_.uniform_mat4f("mvp", projection);
    shader_.uniform_1f("z_max", z_max);

    gl.BindTexture(GL_TEXTURE_2D, id);
    vao_.draw_arrays(draw_mode_, count / draw_divisor_, first / draw_divisor_);
    gl.BindTexture(GL_TEXTURE_2D, 0);
  };
}

std::size_t TTexBatchList::size() const {
  return batches_[curr_batch_]->size();
}

void TTexBatchList::clear() {
  for (auto &b: batches_)
    b->clear();

  curr_batch_ = 0;
  stored_draw_calls_.clear();
}

void TTexBatchList::add(GLuint id, std::initializer_list<float> data) {
  if (batches_.empty())
    batches_.emplace_back(std::make_unique<TTexBatch>(*gfx_, *shader_, floats_per_obj_, attrib_format_, draw_mode_, draw_divisor_));

  else if (batches_[curr_batch_]->size() > BATCH_SIZE_LIMIT) {
    stored_draw_calls_.emplace_back(batches_[curr_batch_]->get_draw_call(id));

    if (curr_batch_ == batches_.size() - 1)
      batches_.emplace_back(std::make_unique<TTexBatch>(*gfx_, *shader_, floats_per_obj_, attrib_format_, draw_mode_, draw_divisor_));

    curr_batch_++;
  }

  batches_[curr_batch_]->add(data);
}

std::vector<DrawCall> TTexBatchList::get_draw_calls(GLuint id) {
  std::vector<DrawCall> draw_calls{};

  if (!stored_draw_calls_.empty()) {
    draw_calls.insert(draw_calls.begin(), stored_draw_calls_.begin(), stored_draw_calls_.end());
    stored_draw_calls_.clear();
  }

  if (!batches_.empty())
    draw_calls.emplace_back(batches_[curr_batch_]->get_draw_call(id));

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

void Batcher::o_add_tex(GLuint id, std::initializer_list<float> data) {
  check_get_tex_draw_calls_(id);
  o_tex_batches_->add(id, data);
  last_o_tex_id_ = id;
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

void Batcher::t_add_tex(GLuint id, std::initializer_list<float> data) {
  check_get_draw_calls_(TBatchType::tex, id);
  t_tex_batches_->add(id, data);
  last_batch_type_ = TBatchType::tex;
  last_t_tex_id_ = id;
  z += 1.0f;
}

void Batcher::check_get_tex_draw_calls_(GLuint id) {
  if (last_o_tex_id_ != 0 && last_o_tex_id_ != id) {
    std::vector<DrawCall> new_draw_calls = o_tex_batches_->get_draw_calls(last_o_tex_id_);
    o_draw_calls_.insert(o_draw_calls_.end(), new_draw_calls.begin(), new_draw_calls.end());
  }
}

void Batcher::check_get_draw_calls_(TBatchType t, GLuint id) {
  if ((last_batch_type_ != TBatchType::none && last_batch_type_ != t) || (last_t_tex_id_ != 0 && last_t_tex_id_ != id)) {
    std::vector<DrawCall> new_draw_calls;
    switch (last_batch_type_) {
      case TBatchType::none:  std::unreachable();
      case TBatchType::tri:   new_draw_calls = t_tri_batches_->get_draw_calls(); break;
      case TBatchType::line:  new_draw_calls = t_line_batches_->get_draw_calls(); break;
      case TBatchType::point: new_draw_calls = t_point_batches_->get_draw_calls(); break;
      case TBatchType::tex:   new_draw_calls = t_tex_batches_->get_draw_calls(last_t_tex_id_); break;
    }
    t_draw_calls_.insert(t_draw_calls_.end(), new_draw_calls.begin(), new_draw_calls.end());
  }
}

void Batcher::draw_(glm::mat4 projection) {
  auto do_draw_calls_ = [&](TBatchList &batch) {
    std::ranges::for_each(batch.get_draw_calls(), [&](auto fn) { fn(gfx->gl, projection, z); });
  };

  auto do_o_tex_draw_calls_ = [&](OTexBatchList &batch) {
    std::ranges::for_each(batch.get_draw_calls(last_o_tex_id_), [&](auto fn) { fn(gfx->gl, projection, z); });
  };

  auto do_t_tex_draw_calls_ = [&](TTexBatchList &batch) {
    std::ranges::for_each(batch.get_draw_calls(last_t_tex_id_), [&](auto fn) { fn(gfx->gl, projection, z); });
  };

  gfx->enable(Capability::depth_test);

  o_tri_batches_->draw(projection, z);
  o_line_batches_->draw(projection, z);
  o_point_batches_->draw(projection, z);

  do_o_tex_draw_calls_(*o_tex_batches_);
  for (const auto &call: o_draw_calls_)
    call(gfx->gl, projection, z);

  o_tri_batches_->clear();
  o_line_batches_->clear();
  o_point_batches_->clear();
  o_tex_batches_->clear();

  last_o_tex_id_ = 0;
  o_draw_calls_.clear();

  gfx->blend_func_separate(BlendFunc::one, BlendFunc::one_minus_src_alpha, BlendFunc::one_minus_dst_alpha, BlendFunc::one);
  gfx->enable(Capability::blend);
  gfx->depth_mask(false);

  for (const auto &call: t_draw_calls_)
    call(gfx->gl, projection, z);

  switch (last_batch_type_) {
    case TBatchType::none:  break;
    case TBatchType::tri:   do_draw_calls_(*t_tri_batches_); break;
    case TBatchType::line:  do_draw_calls_(*t_line_batches_); break;
    case TBatchType::point: do_draw_calls_(*t_point_batches_); break;
    case TBatchType::tex:   do_t_tex_draw_calls_(*t_tex_batches_); break;
  }

  gfx->depth_mask(true);
  gfx->disable(Capability::blend);

  t_tri_batches_->clear();
  t_line_batches_->clear();
  t_point_batches_->clear();
  t_tex_batches_->clear();

  last_batch_type_ = TBatchType::none;
  last_t_tex_id_ = 0;
  t_draw_calls_.clear();

  z = 1.0f;
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

  auto tex_src = ShaderSrc::parse(DATA_FOLDER / "shader" / "textures.glsl");
  if (tex_src)
    tex_shader_ = shaders->compile(*tex_src);

  o_tri_batches_ = std::make_unique<OBatchList>(gfx, tri_shader_, 10 * 3, "in_pos:3f in_color:4f in_trans:3f", DrawMode::triangles, 10);
  o_line_batches_ = std::make_unique<OBatchList>(gfx, line_shader_, 10 * 2, "in_pos:3f in_color:4f in_trans:3f", DrawMode::lines, 10);
  o_point_batches_ = std::make_unique<OBatchList>(gfx, point_shader_, 7 * 1, "in_pos:3f in_color:4f", DrawMode::points, 7);
  o_tex_batches_ = std::make_unique<OTexBatchList>(gfx, tex_shader_, 12 * 6, "in_pos:3f in_color:4f in_tex_coords:2f in_trans:3f", DrawMode::triangles, 12);

  t_tri_batches_ = std::make_unique<TBatchList>(gfx, tri_shader_, 10 * 3, "in_pos:3f in_color:4f in_trans:3f", DrawMode::triangles, 10);
  t_line_batches_ = std::make_unique<TBatchList>(gfx, line_shader_, 10 * 2, "in_pos:3f in_color:4f in_trans:3f", DrawMode::lines, 10);
  t_point_batches_ = std::make_unique<TBatchList>(gfx, point_shader_, 7 * 1, "in_pos:3f in_color:4f", DrawMode::points, 7);
  t_tex_batches_ = std::make_unique<TTexBatchList>(gfx, tex_shader_, 12 * 6, "in_pos:3f in_color:4f in_tex_coords:2f in_trans:3f", DrawMode::triangles, 12);

  EventBus::sub<EFlush>(module_name, [&](const auto &e) { e_flush_(e); });

  Module::e_initialize_(e);
}

void Batcher::e_shutdown_(const EShutdown &e) {
  Module::e_shutdown_(e);
}

void Batcher::e_flush_(const EFlush &e) {
  draw_(e.projection);
}

} // namespace baphy
