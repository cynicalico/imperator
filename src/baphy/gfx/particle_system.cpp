#include "baphy/gfx/particle_system.hpp"

#include "baphy/util/helpers.hpp"
#include "baphy/util/rnd.hpp"

namespace baphy {

ParticleSystem::ParticleSystem(std::shared_ptr<Texture> tex) : tex_(std::move(tex)) {
  EventBus::sub<EUpdate>(rnd::base58(11), [&](const auto &e) { e_update_(e); });
}

std::size_t ParticleSystem::live_count() {
  return live_count_;
}

void ParticleSystem::set_emitter_pos(float x, float y) {
  params_.emitter_pos = {x, y};
}

void ParticleSystem::set_emitter_rate(float particles_per_second) {
  params_.emitter_rate = particles_per_second;
}

void ParticleSystem::set_particle_limit(std::size_t particle_limit) {
  params_.particle_limit = particle_limit;
  particles_.reserve(particle_limit);
}

void ParticleSystem::set_ttl(double min, double max) {
  params_.ttl_min = min;
  params_.ttl_max = max;
}

void ParticleSystem::set_spread(float min, float max) {
  params_.angle_min = min;
  params_.angle_max = max;
}

void ParticleSystem::set_radial_vel(float min, float max) {
  params_.delta_min = min;
  params_.delta_max = max;
}

void ParticleSystem::set_radial_accel(float min, float max) {
  params_.accel_min = min;
  params_.accel_max = max;
}

void ParticleSystem::set_linear_vel(float xmin, float xmax, float ymin, float ymax) {
  params_.ldx_min = xmin;
  params_.ldx_max = xmax;
  params_.ldy_min = ymin;
  params_.ldy_max = ymax;
}

void ParticleSystem::set_linear_accel(float xmin, float xmax, float ymin, float ymax) {
  params_.lax_min = xmin;
  params_.lax_max = xmax;
  params_.lay_min = ymin;
  params_.lay_max = ymax;
}

void ParticleSystem::set_spin(float min, float max) {
  params_.spin_min = min;
  params_.spin_max = max;
}

void ParticleSystem::set_colors(const std::vector<RGB> &colors) {
  params_.colors.clear();

  float step = 1.0f / (colors.size() - 1);
  float acc = 0.0;
  for (const auto &color: colors) {
    params_.colors.emplace_back(acc, color);
    acc += step;
  }
}

void ParticleSystem::emit_count(std::size_t count, float x, float y) {
  if (live_count_ >= params_.particle_limit)
    return;

  for (std::size_t i = 0; i < count && live_count_ < params_.particle_limit; ++i)
    find_insert_particle_(x, y);
}

void ParticleSystem::emit_count(std::size_t count) {
  emit_count(count, params_.emitter_pos.x, params_.emitter_pos.y);
}

void ParticleSystem::emit(double dt, float x, float y) {
  if (live_count_ >= params_.particle_limit)
    return;

  auto acc_step = 1.0 / params_.emitter_rate;

  params_.emitter_acc += dt;
  while (params_.emitter_acc >= acc_step && live_count_ < params_.particle_limit) {
    find_insert_particle_(x, y);
    params_.emitter_acc -= acc_step;
  }

  // If this is true, we hit the particle limit, but still reduce
  // our acc until we wouldn't be emitting any more particles
  while (params_.emitter_acc >= acc_step) {
    params_.emitter_acc -= acc_step;
  }
}

void ParticleSystem::emit(double dt) {
  emit(dt, params_.emitter_pos.x, params_.emitter_pos.y);
}

void ParticleSystem::draw() {
  for (const auto &p: particles_)
    if (p.alive)
      tex_->draw(p.x - (p.w / 2.0), p.y - (p.h / 2.0), p.w, p.h, p.x, p.y, p.tex_angle, p.color);
}

void ParticleSystem::find_insert_particle_(float x, float y) {
  if (live_count_ >= params_.particle_limit)
    return;

  if (!dead_positions_.empty()) {
    std::size_t i = dead_positions_.top();
    dead_positions_.pop();

    particles_[i].ttl = rnd::get<float>(params_.ttl_min, params_.ttl_max);
    particles_[i].angle = rnd::get<float>(params_.angle_min, params_.angle_max);
    particles_[i].radial_vel = rnd::get<float>(params_.delta_min, params_.delta_max);
    particles_[i].radial_accel = rnd::get<float>(params_.accel_min, params_.accel_max);
    particles_[i].ldx = rnd::get<float>(params_.ldx_min, params_.ldx_max);
    particles_[i].ldy = rnd::get<float>(params_.ldy_min, params_.ldy_max);
    particles_[i].lax = rnd::get<float>(params_.lax_min, params_.lax_max);
    particles_[i].lay = rnd::get<float>(params_.lay_min, params_.lay_max);
    particles_[i].spin = rnd::get<float>(params_.spin_min, params_.spin_max);
    particles_[i].x = x;
    particles_[i].y = y;
    particles_[i].w = tex_->w();
    particles_[i].h = tex_->h();
    particles_[i].tex_angle = 0.0f;
    particles_[i].color = std::get<1>(params_.colors[0]);
    particles_[i].color_idx = 0;
    particles_[i].acc = 0;
    particles_[i].alive = true;

  } else {
    particles_.emplace_back(
        rnd::get<float>(params_.ttl_min, params_.ttl_max),
        rnd::get<float>(params_.angle_min, params_.angle_max),
        rnd::get<float>(params_.delta_min, params_.delta_max),
        rnd::get<float>(params_.accel_min, params_.accel_max),
        rnd::get<float>(params_.ldx_min, params_.ldx_max),
        rnd::get<float>(params_.ldy_min, params_.ldy_max),
        rnd::get<float>(params_.lax_min, params_.lax_max),
        rnd::get<float>(params_.lay_min, params_.lay_max),
        rnd::get<float>(params_.spin_min, params_.spin_max),
        x,
        y,
        tex_->w(),
        tex_->h(),
        0.0f,
        std::get<1>(params_.colors[0])
    );
  }

  live_count_++;
}

void ParticleSystem::e_update_(const EUpdate &e) {
  for (std::size_t i = 0; i < particles_.size(); ++i) {
    if (!particles_[i].alive)
      continue;

    particles_[i].acc += e.dt;
    if (particles_[i].acc >= particles_[i].ttl) {
      particles_[i].alive = false;
      dead_positions_.push(i);
      live_count_--;

    } else {
      // Radial movement
      particles_[i].radial_vel += particles_[i].radial_accel * e.dt;
      particles_[i].x += std::cos(particles_[i].angle) * particles_[i].radial_vel * e.dt;
      particles_[i].y += -std::sin(particles_[i].angle) * particles_[i].radial_vel * e.dt;

      // Linear movement
      particles_[i].ldx += particles_[i].lax * e.dt;
      particles_[i].ldy += particles_[i].lay * e.dt;
      particles_[i].x += particles_[i].ldx * e.dt;
      particles_[i].y += particles_[i].ldy * e.dt;

      particles_[i].tex_angle += particles_[i].spin * e.dt;

      // Color modification
      auto progress = particles_[i].acc / particles_[i].ttl;
      for (std::size_t j = particles_[i].color_idx; j < params_.colors.size() - 1; ++j) {
        auto [prog1, color1] = params_.colors[j];
        auto [prog2, color2] = params_.colors[j + 1];
        if (progress >= prog1 && progress <= prog2) {
          double prog = (progress - prog1) / (prog2 - prog1);
          particles_[i].color = rgba(
              lerp(prog, color1.r, color2.r),
              lerp(prog, color1.g, color2.g),
              lerp(prog, color1.b, color2.b),
              lerp(prog, color1.a, color2.a)
          );
          particles_[i].color_idx = j;
          break;
        }
      }
    }
  }
}

} // namespace baphy
