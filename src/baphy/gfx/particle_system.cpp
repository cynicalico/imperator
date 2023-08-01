#include "baphy/gfx/particle_system.hpp"

#include "baphy/util/helpers.hpp"
#include "baphy/util/rnd.hpp"

namespace baphy {

ParticleSystem::ParticleSystem(Texture *tex) : tex_(tex) {
  EventBus::sub<EUpdate>(rnd::base58(11), [&](const auto &e) { e_update_(e); });
}

ParticleSystem::ParticleSystem(Spritesheet *ssheet, const std::vector<std::string> &sprite_names)
    : ssheet_(ssheet), sprite_names_(sprite_names) {
  EventBus::sub<EUpdate>(rnd::base58(11), [&](const auto &e) { e_update_(e); });
}

std::size_t ParticleSystem::live_count() {
  return live_count_;
}

void ParticleSystem::set_emitter_lifetime(float t) {
  params_.lifetime = t;
  params_.life = t;
}

void ParticleSystem::start() {
  params_.active = true;
}

void ParticleSystem::stop() {
  params_.active = false;
  params_.life = params_.lifetime;
  params_.emitter_acc = 0;
}

void ParticleSystem::pause() {
  params_.active = false;
}

void ParticleSystem::reset() {
  particles_.clear();
  params_.life = params_.lifetime;
  params_.emitter_acc = 0;
}

void ParticleSystem::set_pos(float x, float y) {
  params_.position = {x, y};
  params_.prev_position = params_.position;
}

void ParticleSystem::set_rate(float particles_per_second) {
  params_.emitter_rate = particles_per_second;
}

void ParticleSystem::set_limit(std::size_t particle_limit) {
  params_.particle_limit = particle_limit;
  particles_.reserve(particle_limit);
}

void ParticleSystem::set_ttl(float min, float max) {
  params_.ttl_min = min;
  params_.ttl_max = max;
}

void ParticleSystem::set_dir(float dir) {
  params_.direction = -dir;
}

void ParticleSystem::set_spread(float spread) {
  params_.spread = spread;
}

void ParticleSystem::set_speed(float min, float max) {
  params_.speed_min = min;
  params_.speed_max = max;
}

void ParticleSystem::set_radial_accel(float min, float max) {
  params_.radial_accel_min = min;
  params_.radial_accel_max = max;
}

void ParticleSystem::set_tangent_accel(float min, float max) {
  params_.tangent_accel_min = min;
  params_.tangent_accel_max = max;
}

void ParticleSystem::set_linear_accel(float xmin, float xmax, float ymin, float ymax) {
  params_.linear_accel_min = {xmin, ymin};
  params_.linear_accel_max = {xmax, ymax};
}

void ParticleSystem::set_linear_damping(float min, float max) {
  params_.linear_damping_min = min;
  params_.linear_damping_max = max;
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

void ParticleSystem::emit(std::size_t num) {
  if (!params_.active) return;

  num = std::min(num, params_.particle_limit - live_count_);
  while (num--)
    find_insert_particle_(1);
}

void ParticleSystem::draw() {
  for (const auto &p: particles_)
    if (p.alive) {
      if (ssheet_)
        ssheet_->draw_wh(p.sprite_name, p.pos.x - (p.w / 2.0), p.pos.y - (p.h / 2.0), p.w, p.h, p.pos.x, p.pos.y, p.tex_angle, p.color);
      else
        tex_->draw(p.pos.x - (p.w / 2.0), p.pos.y - (p.h / 2.0), p.w, p.h, p.pos.x, p.pos.y, p.tex_angle, p.color);
    }
}

void ParticleSystem::move_to(float x, float y) {
  params_.position = {x, y};
}

void ParticleSystem::find_insert_particle_(float t) {
  if (live_count_ >= params_.particle_limit)
    return;

  if (!dead_positions_.empty()) {
    std::size_t i = dead_positions_.top();
    dead_positions_.pop();
    init_particle_(particles_[i], t);

  } else {
    particles_.emplace_back();
    init_particle_(particles_.back(), t);
  }

  live_count_++;
}

void ParticleSystem::init_particle_(Particle &p, float t) {
  p.ttl = rnd::get<float>(params_.ttl_min, params_.ttl_max);
  p.acc = 0;

  auto pos = params_.prev_position + (params_.position - params_.prev_position) * t;
  p.pos = pos;
  p.origin = pos;

  auto dir_min = params_.direction - params_.spread / 2.0f;
  auto dir_max = params_.direction + params_.spread / 2.0f;
  auto dir = rnd::get<float>(dir_min, dir_max);

  auto speed = rnd::get<float>(params_.speed_min, params_.speed_max);

  p.vel = glm::vec2{std::cos(glm::radians(dir)), std::sin(glm::radians(dir))} * speed;

  p.linear_accel.x = rnd::get<float>(params_.linear_accel_min.x, params_.linear_accel_max.x);
  p.linear_accel.y = rnd::get<float>(params_.linear_accel_min.y, params_.linear_accel_max.y);

  p.radial_accel = rnd::get<float>(params_.radial_accel_min, params_.radial_accel_max);

  p.tangent_accel = rnd::get<float>(params_.tangent_accel_min, params_.tangent_accel_max);

  p.linear_damping = rnd::get<float>(params_.linear_damping_min, params_.linear_damping_max);

  if (ssheet_) {
    p.sprite_name = rnd::choose(sprite_names_);
    p.w = ssheet_->w(p.sprite_name);
    p.h = ssheet_->h(p.sprite_name);
  } else {
    p.w = tex_->w();
    p.h = tex_->h();
  }
  p.tex_angle = 0;

  p.spin = rnd::get<float>(params_.spin_min, params_.spin_max);

  p.color = std::get<1>(params_.colors[0]);
  p.color_idx = 0;

  p.alive = true;
}

void ParticleSystem::emit_(float dt) {
  if (params_.active) {
    auto rate = 1.0f / params_.emitter_rate;
    params_.emitter_acc += dt;
    auto tot = params_.emitter_acc - rate;

    while (params_.emitter_acc > rate) {
      find_insert_particle_(1.0f - (params_.emitter_acc - rate) / tot);
      params_.emitter_acc -= rate;
    }

    params_.life -= dt;
    if (params_.lifetime != -1 && params_.life < 0)
      stop();
  }
}

void ParticleSystem::e_update_(const EUpdate &e) {
  auto dt = static_cast<float>(e.dt);

  for (std::size_t i = 0; i < particles_.size(); ++i) {
    auto &p = particles_[i];

    if (!p.alive)
      continue;

    p.acc += dt;
    if (p.acc >= p.ttl) {
      p.alive = false;
      dead_positions_.push(i);
      live_count_--;

    } else {
      glm::vec2 radial, tangential;
      glm::vec2 ppos = p.pos;

      radial = ppos - p.origin;
      if (glm::length(radial) != 0)
        radial = glm::normalize(radial);
      tangential = radial;

      radial *= p.radial_accel;

      float a = tangential.x;
      tangential.x = -tangential.y;
      tangential.y = a;

      tangential *= p.tangent_accel;

      p.vel += (radial + tangential + p.linear_accel) * dt;
      p.vel *= 1.0f / (1.0f + p.linear_damping * dt);

      ppos += p.vel * dt;
      p.pos = ppos;

      p.tex_angle += p.spin * dt;

      // Color modification
      auto progress = p.acc / p.ttl;
      for (std::size_t j = p.color_idx; j < params_.colors.size() - 1; ++j) {
        auto [prog1, color1] = params_.colors[j];
        auto [prog2, color2] = params_.colors[j + 1];
        if (progress >= prog1 && progress <= prog2) {
          float prog = (progress - prog1) / (prog2 - prog1);
          p.color = rgba(
              lerp(prog, color1.r, color2.r),
              lerp(prog, color1.g, color2.g),
              lerp(prog, color1.b, color2.b),
              lerp(prog, color1.a, color2.a)
          );
          p.color_idx = j;
          break;
        }
      }
    }
  }

  emit_(dt);
  params_.prev_position = params_.position;
}

} // namespace baphy
