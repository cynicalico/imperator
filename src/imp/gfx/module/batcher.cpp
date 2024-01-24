#include "imp/gfx/module/batcher.hpp"

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
    vbo_(ctx, vertices_per_obj * floats_per_vertex, fill_reverse, BufTarget::array, BufUsage::dynamic_draw),
    draw_mode_(draw_mode),
    floats_per_vertex_(floats_per_vertex),
    fill_reverse_(fill_reverse) {
  vao_.attrib(shader, vbo_, attrib_desc);
}

std::size_t Batch::size() const {
  return vbo_.size();
}

void Batch::clear() {
  vbo_.clear();
  draw_start_offset_ = 0;
}

void Batch::add(std::initializer_list<float> data) {
  vbo_.add(data);
}

DrawCall Batch::get_draw_call() {
  if (fill_reverse_) {
    return [&](GladGLContext&, glm::mat4 mvp, float z_max) {
      vbo_.sync();

      shader_.use();
      shader_.uniform_mat4f("mvp", mvp);
      shader_.uniform_1f("z_max", z_max);

      vao_.draw_arrays(draw_mode_, vbo_.size() / floats_per_vertex_, vbo_.front() / floats_per_vertex_);
    };
  }

  int count = vbo_.size() - draw_start_offset_;

  int first = draw_start_offset_;
  draw_start_offset_ = vbo_.size();

  return [&, count, first](GladGLContext&, glm::mat4 mvp, float z_max) {
    vbo_.sync();

    shader_.use();
    shader_.uniform_mat4f("mvp", mvp);
    shader_.uniform_1f("z_max", z_max);

    vao_.draw_arrays(draw_mode_, count / floats_per_vertex_, first / floats_per_vertex_);
  };
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
  return batches_[curr_batch_]->size();
}

void BatchList::clear() {
  for (auto& b: batches_) {
    b->clear();
  }

  curr_batch_ = 0;
  stored_draw_calls_.clear();
}

void BatchList::add(std::initializer_list<float> data) {
  if (batches_.empty()) {
    batches_.emplace_back(std::make_unique<Batch>(ctx_, shader_, draw_mode_, attrib_desc_, vertices_per_obj_, floats_per_vertex_,
                          fill_reverse_));
  } else if (batches_[curr_batch_]->size() > BATCH_SIZE_LIMIT) {
    stored_draw_calls_.emplace_back(batches_[curr_batch_]->get_draw_call());

    if (curr_batch_ == batches_.size() - 1) {
      batches_.emplace_back(std::make_unique<Batch>(ctx_, shader_, draw_mode_, attrib_desc_, vertices_per_obj_, floats_per_vertex_,
                            fill_reverse_));
    }
    curr_batch_++;
  }

  batches_[curr_batch_]->add(data);
}

std::vector<DrawCall> BatchList::get_draw_calls() {
  std::vector<DrawCall> draw_calls{};

  if (!stored_draw_calls_.empty()) {
    draw_calls.insert_range(draw_calls.begin(), stored_draw_calls_);
    stored_draw_calls_.clear();
  }

  if (!batches_.empty()) {
    draw_calls.emplace_back(batches_[curr_batch_]->get_draw_call());
  }

  return draw_calls;
}

Batcher::Batcher(const std::weak_ptr<ModuleMgr>& module_mgr) : Module(module_mgr) {
  ctx = module_mgr.lock()->get<GfxContext>();
  shaders = module_mgr.lock()->get<ShaderMgr>();

  if (const auto src = ShaderSrc::parse(DATA_FOLDER / "shader" / "basic.glsl")) {
    auto basic_shader = shaders->compile(*src);

    shaders_.emplace(DrawMode::lines, basic_shader);
    attrib_descs_.emplace(DrawMode::lines, "in_pos:3f in_color:4f in_trans:3f");
    vertices_per_obj_.emplace(DrawMode::lines, 2);
    floats_per_vertex_.emplace(DrawMode::lines, 10);

    shaders_.emplace(DrawMode::triangles, basic_shader);
    attrib_descs_.emplace(DrawMode::triangles, "in_pos:3f in_color:4f in_trans:3f");
    vertices_per_obj_.emplace(DrawMode::triangles, 3);
    floats_per_vertex_.emplace(DrawMode::triangles, 10);

    shaders_.emplace(DrawMode::points, basic_shader);
    attrib_descs_.emplace(DrawMode::points, "in_pos:3f in_color:4f in_trans:3f");
    vertices_per_obj_.emplace(DrawMode::points, 1);
    floats_per_vertex_.emplace(DrawMode::points, 10);
  }
}

void Batcher::add_opaque(const DrawMode& mode, std::initializer_list<float> data) {
  if (!opaque_batches_.contains(mode)) {
    opaque_batches_.emplace(
      mode,
      std::make_unique<BatchList>(
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

  opaque_batches_[mode]->add(data);
  z += 1.0f;
}

void Batcher::add_trans(const DrawMode& mode, std::initializer_list<float> data) {
  // TODO
}

void Batcher::draw(const glm::mat4& projection) {
  ctx->enable(Capability::depth_test);

  collect_opaque_draw_calls_();
  for (const auto& c: opaque_draw_calls_ | std::views::reverse) {
    c(ctx->gl, projection, z);
  }
  clear_opaque_();

  ctx->blend_func_separate(BlendFunc::one, BlendFunc::one_minus_src_alpha, BlendFunc::one_minus_dst_alpha, BlendFunc::one);
  ctx->enable(Capability::blend);
  ctx->depth_mask(false);

  collect_trans_draw_calls_();
  for (const auto& c: trans_draw_calls_) {
    c(ctx->gl, projection, z);
  }
  clear_trans_();

  ctx->depth_mask(true);
  ctx->disable(Capability::blend);

  // ctx->disable(Capability::depth_test);

  z = 1.0f;
}

void Batcher::collect_opaque_draw_calls_() {
  for (auto& b: opaque_batches_ | std::views::values) {
    opaque_draw_calls_.insert_range(opaque_draw_calls_.end(), b->get_draw_calls());
  }
}

void Batcher::collect_trans_draw_calls_() {
  // TODO
}

void Batcher::clear_opaque_() {
  for (auto& b: opaque_batches_ | std::views::values) {
    b->clear();
  }
  opaque_draw_calls_.clear();
}

void Batcher::clear_trans_() {
  // TODO
}
} // namespace imp
