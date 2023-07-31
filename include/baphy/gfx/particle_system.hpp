#ifndef BAPHY_GFX_PARTICLE_SYSTEM_HPP
#define BAPHY_GFX_PARTICLE_SYSTEM_HPP

// This is based on and heavily utilizes the code for https://love2d.org/wiki/ParticleSystem

#include "baphy/gfx/module/texture_batcher.hpp"
#include "baphy/gfx/color.hpp"
#include "baphy/gfx/spritesheet.hpp"
#include <numbers>
#include <stack>
#include <utility>
#include <vector>

namespace baphy {

struct Particle {
  float ttl;
  float acc;

  glm::vec2 pos;
  glm::vec2 origin;

  glm::vec2 vel;
  glm::vec2 linear_accel;
  float radial_accel;
  float tangent_accel;

  float linear_damping;

  std::string sprite_name;
  float w, h;
  float tex_angle;

  float spin;

  RGB color;
  std::size_t color_idx;

  bool alive{true};
};

class ParticleSystem {
public:
  explicit ParticleSystem(Texture *tex);
  ParticleSystem(Spritesheet *ssheet, const std::vector<std::string> &sprite_names);

  std::size_t live_count();

  void set_pos(float x, float y);
  void set_rate(float particles_per_second);
  void set_limit(std::size_t particle_limit);

  void set_ttl(float min, float max);
  void set_dir(float dir);
  void set_spread(float spread);

  void set_speed(float min, float max);
  void set_radial_accel(float min, float max);
  void set_tangent_accel(float min, float max);
  void set_linear_accel(float xmin, float xmax, float ymin, float ymax);
  void set_linear_damping(float min, float max);

  void set_spin(float min, float max);

  void set_colors(const std::vector<RGB> &colors);

  void move_to(float x, float y);

  void draw();

private:
  Texture *tex_{nullptr};
  Spritesheet *ssheet_{nullptr};
  std::vector<std::string> sprite_names_{};

  std::vector<Particle> particles_{};
  std::size_t live_count_{0};
  std::stack<std::size_t> dead_positions_{};

  struct {
    bool active{true};
    float life{0};
    float lifetime{-1};
    glm::vec2 position{0, 0};
    glm::vec2 prev_position{0, 0};

    float direction{0};
    float spread{0};

    float emitter_rate{0};
    float emitter_acc{0};

    std::size_t particle_limit{std::numeric_limits<std::size_t>::max()};

    float ttl_min{0}, ttl_max{0};

    float speed_min{0}, speed_max{0};

    glm::vec2 linear_accel_min{0, 0};
    glm::vec2 linear_accel_max{0, 0};

    float radial_accel_min{0};
    float radial_accel_max{0};

    float tangent_accel_min{0};
    float tangent_accel_max{0};

    float linear_damping_min{0};
    float linear_damping_max{0};

    float spin_min{0}, spin_max{0};

    std::vector<std::tuple<float, RGB>> colors{
        {0.0f, rgb(0xffffff)},
        {1.0f, rgb(0xffffff)}
    };
  } params_;

  void find_insert_particle_(float t);
  void init_particle_(Particle &p, float t);

  void emit_(float dt);

  void e_update_(const EUpdate &e);
};

} // namespace baphy

#endif//BAPHY_GFX_PARTICLE_SYSTEM_HPP
