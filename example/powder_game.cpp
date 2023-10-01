#include "baphy/baphy.hpp"

const auto HERE = std::filesystem::path(__FILE__).parent_path();

enum class ParticleType {
  undef,
  powder,
  water
};

struct Particle {
  ParticleType type{ParticleType::undef};
  double x{0};
  double y{0};
  double angle{0};
  double vel{0};
  double accel{0};
};

class PowderGame {
public:
  PowderGame(int rows, int cols, double gravity);

  std::size_t rows() { return field_.size(); }
  std::size_t cols() { return field_[0].size(); }

  bool get_field(std::size_t r, std::size_t c);
  void set_field(std::size_t r, std::size_t c, bool v);

  void emit(double x, double y);

  void update(double dt);

private:
  std::vector<std::vector<bool>> field_;
  std::vector<Particle> particles_{};

  double gravity_;
};

PowderGame::PowderGame(int rows, int cols, double gravity) : gravity_(gravity) {
  field_ = std::vector<std::vector<bool>>(rows, std::vector<bool>(cols, false));
  for (std::size_t r = 0; r < rows; ++r) {
    field_[r][0] = true;
    field_[r][cols - 1] = true;
  }
  for (std::size_t c = 0; c < cols; ++c) {
    field_[0][c] = true;
    field_[rows - 1][c] = true;
  }
}

bool PowderGame::get_field(std::size_t r, std::size_t c) {
  return (r < rows() && c < cols()) && field_[r][c];
}

void PowderGame::set_field(std::size_t r, std::size_t c, bool v) {
  if (r < rows() && c < cols())
    field_[r][c] = v;
}

void PowderGame::emit(double x, double y) {

}

void PowderGame::update(double dt) {
  for (auto &p: particles_) {
    
  }
}

class PowderGameApp : public baphy::Application {
public:
  std::unique_ptr<PowderGame> game;
  std::size_t cell_size{4};

  void initialize() override;
  void update(double dt) override;
  void draw() override;

  void draw_field();
};

void PowderGameApp::initialize() {
  game = std::make_unique<PowderGame>(window->w() / cell_size, window->h() / cell_size, 9.8);

  cursors->create(HERE / "res" / "img" / "dot.png", 4, 4)->set();
}

void PowderGameApp::update(double dt) {
  if (input->pressed("escape")) window->set_should_close(true);

  if (input->down("mb_left"))
    game->emit(input->mouse_x() / cell_size, input->mouse_y() / cell_size);

  game->update(dt);
}

void PowderGameApp::draw() {
  draw_field();
}

void PowderGameApp::draw_field() {
  for (std::size_t r = 0; r < game->rows(); ++r)
    for (std::size_t c = 0; c < game->cols(); ++c)
      if (game->get_field(r, c))
        primitives->fill_rect(r * cell_size, c * cell_size, cell_size, cell_size, baphy::rgb("darkgray"));
}

BAPHY_RUN(PowderGameApp,
  .title = "Powder Game",
  .size = {800, 600},
  .flags = baphy::WindowFlags::centered | baphy::WindowFlags::vsync
);