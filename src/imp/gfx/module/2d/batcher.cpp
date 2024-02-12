#include "imp/gfx/module/2d/batcher.hpp"

#include "imp/util/io.hpp"
#include <ranges>

namespace imp {
Batch::Batch(
  GfxContext& ctx,
  Shader& shader,
  const DrawMode draw_mode, const std::string& attrib_desc,
  std::size_t vertices_per_obj, std::size_t floats_per_vertex, bool fill_reverse
) : shader_(shader),
    vao_(ctx),
    vbo_(ctx, vertices_per_obj * floats_per_vertex, false, BufTarget::array, BufUsage::dynamic_draw),
    ebo_(ctx, vertices_per_obj, fill_reverse, BufTarget::element_array, BufUsage::dynamic_draw),
    draw_mode_(draw_mode),
    floats_per_vertex_(floats_per_vertex),
    fill_reverse_(fill_reverse) {
  vao_.attrib(shader, vbo_, attrib_desc);
  vao_.element_array(ebo_);
}

std::size_t Batch::size() const {
  return vbo_.size();
}

void Batch::clear() {
  vbo_.clear();
  ebo_.clear();
  draw_start_offset_ = 0;
  ebo_offset_ = 0;
}

void Batch::add(std::initializer_list<float> data, std::initializer_list<unsigned int> indices, bool insert_restart) {
  vbo_.add(data);
  if (insert_restart) {
    ebo_.add({std::numeric_limits<GLuint>::max()});
  }
  ebo_.add(std::ranges::views::transform(indices, [&](const auto& i) { return i + ebo_offset_; }));
  ebo_offset_ += data.size() / floats_per_vertex_;
}

DrawCall Batch::get_draw_call_tex(GLuint id) {
  int count, first;
  if (fill_reverse_) {
    count = ebo_.size();
    first = ebo_.front();
  } else {
    count = ebo_.size() - draw_start_offset_;
    first = draw_start_offset_;
    draw_start_offset_ = ebo_.size();
  }

  return [&, id, count, first](GladGLContext& gl_, glm::mat4 mvp, float z_max) {
    vbo_.sync();
    ebo_.sync();

    shader_.use();
    shader_.uniform_mat4f("mvp", mvp);
    shader_.uniform_1f("z_max", z_max);

    gl_.BindTexture(GL_TEXTURE_2D, id);

    vao_.bind();
    vao_.gl.DrawElements(
      unwrap(draw_mode_),
      count,
      GL_UNSIGNED_INT,
      reinterpret_cast<void*>(first * sizeof(unsigned int))
    );
    vao_.unbind();
  };
}

DrawCall Batch::get_draw_call() {
  return get_draw_call_tex(0);
}

BatchList::BatchList(
  GfxContext& ctx,
  Shader& shader,
  const DrawMode draw_mode, const std::string& attrib_desc,
  std::size_t vertices_per_obj, std::size_t floats_per_vertex, bool fill_reverse
) : ctx_(ctx),
    shader_(shader),
    draw_mode_(draw_mode),
    vertices_per_obj_(vertices_per_obj),
    attrib_desc_(attrib_desc),
    floats_per_vertex_(floats_per_vertex),
    fill_reverse_(fill_reverse) {}

std::size_t BatchList::size() const {
  return batches_[curr_batch_].size();
}

void BatchList::clear() {
  std::ranges::for_each(batches_, [](auto& b) { b.clear(); });
  curr_batch_ = 0;
  stored_draw_calls_.clear();
}

void BatchList::add_tex(GLuint id, std::initializer_list<float> data, std::initializer_list<unsigned> indices,
                        bool insert_restart) {
  if (batches_.empty()) {
    batches_.emplace_back(ctx_, shader_, draw_mode_, attrib_desc_, vertices_per_obj_, floats_per_vertex_,
                          fill_reverse_);
  } else if (batches_[curr_batch_].size() > BATCH_SIZE_LIMIT) {
    stored_draw_calls_.emplace_back(batches_[curr_batch_].get_draw_call_tex(id));

    if (curr_batch_ == batches_.size() - 1) {
      batches_.emplace_back(ctx_, shader_, draw_mode_, attrib_desc_, vertices_per_obj_, floats_per_vertex_,
                            fill_reverse_);
    }
    curr_batch_++;
  }

  batches_[curr_batch_].add(data, indices, insert_restart);
}

void BatchList::add(std::initializer_list<float> data, std::initializer_list<unsigned int> indices,
                    bool insert_restart) {
  add_tex(0, data, indices, insert_restart);
}

std::vector<DrawCall> BatchList::get_draw_calls_tex(GLuint id) {
  std::vector<DrawCall> draw_calls{};

  if (!stored_draw_calls_.empty()) {
    draw_calls.insert(draw_calls.begin(), stored_draw_calls_.begin(), stored_draw_calls_.end());
    stored_draw_calls_.clear();
  }

  if (!batches_.empty()) {
    draw_calls.emplace_back(batches_[curr_batch_].get_draw_call_tex(id));
  }

  return draw_calls;
}

std::vector<DrawCall> BatchList::get_draw_calls() {
  return get_draw_calls_tex(0);
}

Batcher::Batcher(const std::weak_ptr<ModuleMgr>& module_mgr) : Module(module_mgr) {
  ctx = module_mgr.lock()->get<GfxContext>();
  shaders = module_mgr.lock()->get<ShaderMgr>();

  if (const auto src = ShaderSrc::parse(DATA_FOLDER / "shader" / "points.glsl")) {
    auto points_shader = shaders->compile(*src);

    shaders_.emplace(DrawMode::points, points_shader);
    attrib_descs_.emplace(DrawMode::points, "in_pos:3f in_color:4f");
    vertices_per_obj_.emplace(DrawMode::points, 1);
    floats_per_vertex_.emplace(DrawMode::points, 7);
  }

  if (const auto src = ShaderSrc::parse(DATA_FOLDER / "shader" / "lines.glsl")) {
    auto lines_shader = shaders->compile(*src);

    shaders_.emplace(DrawMode::lines, lines_shader);
    attrib_descs_.emplace(DrawMode::lines, "in_pos:3f in_color:4f in_trans:3f");
    vertices_per_obj_.emplace(DrawMode::lines, 2);
    floats_per_vertex_.emplace(DrawMode::lines, 10);

    shaders_.emplace(DrawMode::line_loop, lines_shader);
    attrib_descs_.emplace(DrawMode::line_loop, "in_pos:3f in_color:4f in_trans:3f");
    vertices_per_obj_.emplace(DrawMode::line_loop, 5);
    floats_per_vertex_.emplace(DrawMode::line_loop, 10);
  }

  if (const auto src = ShaderSrc::parse(DATA_FOLDER / "shader" / "triangles.glsl")) {
    auto triangles_shader = shaders->compile(*src);

    shaders_.emplace(DrawMode::triangles, triangles_shader);
    attrib_descs_.emplace(DrawMode::triangles, "in_pos:3f in_color:4f in_trans:3f");
    vertices_per_obj_.emplace(DrawMode::triangles, 3);
    floats_per_vertex_.emplace(DrawMode::triangles, 10);
  }

  if (const auto src = ShaderSrc::parse(DATA_FOLDER / "shader" / "textures.glsl")) {
    tex_shader_ = shaders->compile(*src);
  }
}

void Batcher::add_opaque(const DrawMode& mode, const std::initializer_list<float> data,
                         std::initializer_list<unsigned int> indices, bool insert_restart) {
  auto it = opaque_batches_.find(mode);
  if (it == opaque_batches_.end()) {
    it = opaque_batches_.emplace_hint(
      it,
      mode,
      BatchList(
        *ctx,
        *shaders_[mode],
        mode,
        attrib_descs_[mode],
        vertices_per_obj_[mode],
        floats_per_vertex_[mode],
        true
      )
    );
  }

  it->second.add(data, indices, insert_restart);
  z += 1.0f;
}

void Batcher::add_trans(const DrawMode& mode, std::initializer_list<float> data,
                        std::initializer_list<unsigned int> indices, bool insert_restart) {
  if (last_trans_draw_mode_ != DrawMode::none && last_trans_draw_mode_ != mode) {
    collect_trans_draw_calls_();
  }
  last_trans_draw_mode_ = mode;

  auto it = trans_batches_.find(mode);
  if (!trans_batches_.contains(mode)) {
    it = trans_batches_.emplace_hint(
      it,
      mode,
      BatchList(
        *ctx,
        *shaders_[mode],
        mode,
        attrib_descs_[mode],
        vertices_per_obj_[mode],
        floats_per_vertex_[mode],
        false
      )
    );
  }

  it->second.add(data, indices, insert_restart);
  z += 1.0f;
}

void Batcher::add_opaque_tex(GLuint id, std::initializer_list<float> data, std::initializer_list<unsigned> indices) {
  add_trans_tex(id, data, indices);  // TODO: Specialize so we can take advantage of z ordering here
}

void Batcher::add_trans_tex(GLuint id, std::initializer_list<float> data, std::initializer_list<unsigned> indices) {
  if ((last_trans_draw_mode_ != DrawMode::none && last_trans_draw_mode_ != DrawMode::tex) ||
      (last_tex_id_ != 0 && last_tex_id_ != id)) {
    collect_trans_draw_calls_();
  }
  last_trans_draw_mode_ = DrawMode::tex;
  last_tex_id_ = id;

  while (tex_batches_.size() <= id) {
    tex_batches_.emplace_back(
      *ctx,
      *tex_shader_,
      DrawMode::triangles,
      "in_pos:3f in_color:4f in_tex_coords:2f in_trans:3f",
      4,
      12,
      false
    );
  }

  tex_batches_[id].add_tex(id, data, indices, false);
  z += 1.0f;
}

void Batcher::draw(const glm::mat4& projection) {
  ctx->enable(Capability::primitive_restart);
  ctx->gl.PrimitiveRestartIndex(std::numeric_limits<GLuint>::max());

  ctx->enable(Capability::depth_test);

  collect_opaque_draw_calls_();
  for (const auto& c: opaque_draw_calls_ | std::views::reverse) {
    c(ctx->gl, projection, z);
  }
  clear_opaque_();

  ctx->blend_func_separate(
    BlendFunc::one, BlendFunc::one_minus_src_alpha,
    BlendFunc::one_minus_dst_alpha, BlendFunc::one);
  ctx->enable(Capability::blend);
  ctx->depth_mask(false);

  collect_trans_draw_calls_();
  for (const auto& c: trans_draw_calls_) {
    c(ctx->gl, projection, z);
  }
  clear_trans_();

  ctx->depth_mask(true);
  ctx->disable(Capability::blend);

  ctx->disable(Capability::depth_test);

  z = 1.0f;
}

void Batcher::collect_opaque_draw_calls_() {
  for (auto& b: opaque_batches_ | std::views::values) {
    auto draw_calls = b.get_draw_calls();
    opaque_draw_calls_.insert(opaque_draw_calls_.end(), draw_calls.begin(), draw_calls.end());
  }
}

void Batcher::collect_trans_draw_calls_() {
  if (last_trans_draw_mode_ != DrawMode::none) {
    std::vector<DrawCall> draw_calls;
    if (last_trans_draw_mode_ == DrawMode::tex)
      draw_calls = tex_batches_.at(last_tex_id_).get_draw_calls_tex(last_tex_id_);
    else
      draw_calls = trans_batches_.at(last_trans_draw_mode_).get_draw_calls();
    trans_draw_calls_.insert(trans_draw_calls_.end(), draw_calls.begin(), draw_calls.end());
  }
}

void Batcher::clear_opaque_() {
  std::ranges::for_each(opaque_batches_ | std::views::values, [](auto& b) { b.clear(); });
  opaque_draw_calls_.clear();
}

void Batcher::clear_trans_() {
  std::ranges::for_each(trans_batches_ | std::views::values, [](auto& b) { b.clear(); });
  std::ranges::for_each(tex_batches_, [](auto& b) { b.clear(); });
  trans_draw_calls_.clear();
  last_trans_draw_mode_ = DrawMode::none;
}
} // namespace imp
