#ifndef BAPHY_GFX_PARTICLE_SYSTEM_HPP
#define BAPHY_GFX_PARTICLE_SYSTEM_HPP

#include "baphy/gfx/module/texture_batcher.hpp"
#include "baphy/gfx/color.hpp"
#include <numbers>
#include <stack>
#include <vector>

namespace baphy {

struct Particle {
  double ttl;
  float angle;
  float radial_vel;
  float radial_accel;

  float ldx, ldy;
  float lax, lay;

  float spin;

  float x, y;
  float w, h;
  float tex_angle;
  RGB color;

  std::size_t color_idx{0};
  double acc{0};
  bool alive{true};

  Particle(
      double ttl,
      float angle,
      float radial_vel,
      float radial_accel,
      float ldx, float ldy,
      float lax, float lay,
      float spin,
      float x, float y,
      float w, float h,
      float tex_angle = 0.0f,
      RGB color = rgb(0xffffff)
  ) : ttl(ttl),
      angle(angle),
      radial_vel(radial_vel),
      radial_accel(radial_accel),
      ldx(ldx), ldy(ldy),
      lax(lax), lay(lay),
      spin(spin),
      x(x), y(y),
      w(w), h(h),
      tex_angle(angle),
      color(color) {};
};

class ParticleSystem {
public:
  explicit ParticleSystem(std::shared_ptr<Texture> tex);

  std::size_t live_count();

  void set_emitter_pos(float x, float y);
  void set_emitter_rate(float particles_per_second);

  void set_particle_limit(std::size_t particle_limit);

  void set_ttl(double min, double max);

  void set_spread(float min, float max);
  void set_radial_vel(float min, float max);
  void set_radial_accel(float min, float max);

  void set_linear_vel(float xmin, float xmax, float ymin, float ymax);
  void set_linear_accel(float xmin, float xmax, float ymin, float ymax);

  void set_spin(float min, float max);

  void set_colors(const std::vector<RGB> &colors);

  void emit_count(std::size_t count, float x, float y);
  void emit_count(std::size_t count);

  void emit(double dt, float x, float y);
  void emit(double dt);

  void draw();

private:
  std::shared_ptr<Texture> tex_{nullptr};

  std::vector<Particle> particles_{};
  std::size_t live_count_{0};
  std::stack<std::size_t> dead_positions_{};

  struct {
    glm::vec2 emitter_pos{0, 0};
    float emitter_rate{0};
    double emitter_acc{0};

    std::size_t particle_limit{std::numeric_limits<std::size_t>::max()};

    double ttl_min{0}, ttl_max{0};
    float angle_min{0}, angle_max{2 * std::numbers::pi};
    float delta_min{0}, delta_max{0};
    float accel_min{0}, accel_max{0};

    float ldx_min{0}, ldx_max{0};
    float ldy_min{0}, ldy_max{0};
    float lax_min{0}, lax_max{0};
    float lay_min{0}, lay_max{0};

    float spin_min{0}, spin_max{0};

    std::vector<std::tuple<float, RGB>> colors{
        {0.0f, rgb(0xffffff)},
        {1.0f, rgb(0xffffff)}
    };
  } params_;

  void find_insert_particle_(float x, float y);

  void e_update_(const EUpdate &e);
};

} // namespace baphy

#endif//BAPHY_GFX_PARTICLE_SYSTEM_HPP
