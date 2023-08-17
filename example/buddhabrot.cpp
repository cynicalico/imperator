#include "baphy/baphy.hpp"

//#define USE_MUTEX

const auto HERE = std::filesystem::path(__FILE__).parent_path();

class BuddhabrotApp : public baphy::Application {
public:
  std::shared_ptr<baphy::Texture> font_tex;
  std::shared_ptr<baphy::CP437> font;

  const std::size_t SCALE = 16;

  const std::uint64_t POINTS = 1'000'000'000;
  const std::size_t MIN_PATH_SIZE = 5;
  const std::size_t MIN_MUTATE_PATH_SIZE = 1000;
  const std::size_t MAX_ITERS{10'000};

  const std::size_t RED_MIN = MIN_PATH_SIZE * 3;
  const std::size_t RED_MAX = MAX_ITERS;
  const std::size_t GRN_MIN = MIN_PATH_SIZE * 2;
  const std::size_t GRN_MAX = MIN_MUTATE_PATH_SIZE + 100;
  const std::size_t BLU_MIN = MIN_PATH_SIZE;
  const std::size_t BLU_MAX = MIN_MUTATE_PATH_SIZE + 50;

  std::shared_ptr<baphy::Surface> fractal;
  const std::size_t ROWS_PER_SECOND{100};
  double rows_to_draw_this_frame{0};
  std::size_t curr_row{0};
  std::uint64_t r_max{0};
  std::uint64_t g_max{0};
  std::uint64_t b_max{0};

  std::vector<std::uint64_t> r_hits{};
  std::vector<std::uint64_t> g_hits{};
  std::vector<std::uint64_t> b_hits{};
  std::uint64_t hits{0}, valid{0}, invalid{0}, mutates{0};
  std::size_t hps{};
  baphy::SMA hps_avg{10};
#ifdef USE_MUTEX
  std::mutex hit_mutex;
#endif

  std::chrono::system_clock::time_point start_time;
  std::chrono::system_clock::time_point end_time;
  std::vector<std::jthread> ip_threads;
  const std::uint64_t THREAD_COUNT{16};
  bool running{true};
  bool finalize{false};

  void initialize() override {
    font_tex = textures->load(HERE / "res" / "img" / "CGA8x8thick.png", true);
    font = fonts->load<baphy::CP437>("cga8x8thick", font_tex, 8, 8);

    fractal = surfaces->create(window->w() * SCALE, window->h() * SCALE);
    r_hits.resize(fractal->w() * fractal->h(), 0);
    g_hits.resize(fractal->w() * fractal->h(), 0);
    b_hits.resize(fractal->w() * fractal->h(), 0);

    double n = 0.999999;
    for (std::size_t i = 0; i < THREAD_COUNT; ++i) {
      ip_threads.emplace_back([&, n] {
        std::vector<int> xs(MAX_ITERS, 0);
        std::vector<int> ys(MAX_ITERS, 0);

        double x = 0;
        double y = 0;
        double lx = 0;
        double ly = 0;
        bool mutate{false};
        double mutate_chance = n;

        do {
          lx = x;
          ly = y;
          if (mutate) {
            double r1 = 0.0001;
            double r2 = 0.1;
            double phi = baphy::rnd::get<double>() * std::numbers::pi * 2;
            double r = r2 * std::exp(-std::log(r2 / r1) * baphy::rnd::get<double>());
            x += r * std::cos(phi);
            y += r * std::sin(phi);
          } else {
            x = baphy::rnd::get<double>(-2.0, 2.0);
            y = baphy::rnd::get<double>(-1.75, 1.75);
          }

          std::size_t iters;
          std::size_t count = iterate_point(xs, ys, iters, x, y);

#ifdef USE_MUTEX
          hit_mutex.lock();
#endif

          if ((!mutate && count >= MIN_PATH_SIZE) ||
              (mutate && count >= MIN_MUTATE_PATH_SIZE)) {
            for (std::size_t i = 0; i < count; ++i) {
              std::size_t idx = (ys[i] * static_cast<std::size_t>(fractal->w())) + xs[i];
              if (iters >= RED_MIN && iters <= RED_MAX)
                r_hits[idx]++;
              if (iters >= GRN_MIN && iters <= GRN_MAX)
                g_hits[idx]++;
              if (iters >= BLU_MIN && iters <= BLU_MAX)
                b_hits[idx]++;
            }
            hits += count;
            hps += count;
            valid++;
            if (mutate)
              mutates++;

            mutate = count >= MIN_MUTATE_PATH_SIZE && baphy::rnd::get<bool>(mutate_chance);
          } else
            invalid++;

          if (mutate && count < MIN_MUTATE_PATH_SIZE) {
            x = lx;
            y = ly;
          }

#ifdef USE_MUTEX
          hit_mutex.unlock();
#endif

          if (valid >= POINTS)
            running = false;
        } while (running);
      });
      n *= n;
    }

    timer->every(1.0, [&] {
#ifdef USE_MUTEX
      hit_mutex.lock();
#endif
      hps_avg.update(hps);
      hps = 0;
#ifdef USE_MUTEX
      hit_mutex.unlock();
#endif
    });

    start_time = std::chrono::system_clock::now();
  }

  ~BuddhabrotApp() override {
    running = false;
    for (auto &t: ip_threads)
      t.join();
  }

  void update(double dt) override {
#ifdef USE_MUTEX
    hit_mutex.lock();
#endif
    running = valid < POINTS;
#ifdef USE_MUTEX
    hit_mutex.unlock();
#endif

    rows_to_draw_this_frame += ROWS_PER_SECOND * dt;

    if (input->pressed("s"))
      fractal->write_png(HERE / fmt::format("{}_buddhabrot.png", baphy::timestamp()));
  }

  void draw() override {
    gfx->clear(baphy::rgb("black"));

    fractal->draw_on([&] {
      if (frameinfo.first)
        gfx->clear(baphy::rgb(0x00000000));

      if (finalize && curr_row == (std::size_t) fractal->h())
        return;

#ifdef USE_MUTEX
      hit_mutex.lock();
#endif

      while (std::floor(rows_to_draw_this_frame) > 1) {
        if (running)
          for (std::size_t x = 0; x < fractal->w(); x++) {
            std::size_t idx = (curr_row * static_cast<std::size_t>(fractal->w())) + x;
            r_max = std::max(r_max, r_hits[idx]);
            g_max = std::max(g_max, g_hits[idx]);
            b_max = std::max(b_max, b_hits[idx]);
          }

        for (std::size_t x = 0; x < fractal->w(); x++) {
          std::size_t idx = (curr_row * static_cast<std::size_t>(fractal->w())) + x;
          int r = 255 * (std::sqrt(r_hits[idx]) / std::sqrt(r_max));
          int g = 255 * (std::sqrt(g_hits[idx]) / std::sqrt(g_max));
          int b = 255 * (std::sqrt(b_hits[idx]) / std::sqrt(b_max));
          primitives->point(curr_row, x, baphy::rgb(r, g, b));
        }

        if (running)
          curr_row = (curr_row + 1) % (std::size_t) fractal->h();
        else
          curr_row += 1;
        rows_to_draw_this_frame -= 1.0;

        if (finalize && curr_row == (std::size_t) fractal->h())
          break;

        if (!running && !finalize) {
          finalize = true;
          curr_row = 0;

          for (std::size_t y = 0; y < fractal->h(); y++)
            for (std::size_t x = 0; x < fractal->w(); x++) {
              std::size_t idx = (y * static_cast<std::size_t>(fractal->w())) + x;
              r_max = std::max(r_max, r_hits[idx]);
              g_max = std::max(g_max, g_hits[idx]);
              b_max = std::max(b_max, b_hits[idx]);
            }

          break;
        }
      }

#ifdef USE_MUTEX
      hit_mutex.unlock();
#endif
    });
    fractal->draw(0, 0, window->w(), window->h());

    if (running)
      end_time = std::chrono::system_clock::now();

    if (running || finalize)
      primitives->line(curr_row / SCALE, 0, curr_row / SCALE, window->h(), finalize ? baphy::rgb("lime") : baphy::rgb("white"));

    font->draw(0, window->h() - 8 * 5, 1, fmt::format(std::locale("en_US.UTF-8"), "hits: {:L}", hits), baphy::rgb("white"));
    font->draw(0, window->h() - 8 * 4, 1, fmt::format(std::locale("en_US.UTF-8"), "mutates: {:L}", mutates), baphy::rgb("white"));
    font->draw(0, window->h() - 8 * 3, 1, fmt::format(std::locale("en_US.UTF-8"), "valid: {:L}", valid), baphy::rgb("white"));
    font->draw(0, window->h() - 8 * 2, 1, fmt::format(std::locale("en_US.UTF-8"), "invalid: {:L}", invalid), baphy::rgb("white"));
    font->draw(0, window->h() - 8 * 1, 1, fmt::format("ratio: {:.2f}%", ((double)valid / (double)(valid + invalid)) * 100), baphy::rgb("white"));

    auto elapsed = std::chrono::floor<std::chrono::seconds>(end_time - start_time);
    auto time_s = fmt::format("elapsed: {:%H:%M:%S}", elapsed);
    font->draw(window->w() - font->bounds(1, time_s).x, window->h() - 8 * 2, 1, time_s, baphy::rgb("white"));

    auto hps_s = fmt::format(std::locale("en_US.UTF-8"), "{:.2Lf} hps", hps_avg.value());
    font->draw(window->w() - font->bounds(1, hps_s).x, window->h() - 8 * 1, 1, hps_s, baphy::rgb("white"));
  }

  std::size_t iterate_point(std::vector<int> &xs, std::vector<int> &ys, std::size_t &iters, double x0, double y0) {
    double q = pow(x0 - (1.0 / 4.0), 2) + pow(y0, 2);
    if (q * (q + (x0 - (1.0 / 4.0))) <= (1.0 / 4.0) * pow(y0, 2))
      return 0;

    if (pow(x0 + 1, 2) + pow(y0, 2) <= 1.0 / 16.0)
      return 0;

    double x = 0.0, y = 0.0, x2 = 0.0, y2 = 0.0;

    double old_x = 0.0, old_y = 0.0;
    uint64_t cycles = 0L, cycle_limit = 1L;

    iters = 0;
    std::size_t p_idx = 0;
    do {
      y = 2 * x * y + y0;
      x = x2 - y2 + x0;
      y2 = y * y;
      x2 = x * x;

      if (x == old_x && y == old_y) {
        iters = MAX_ITERS;
        break;
      }
      if (++cycles >= cycle_limit) {
        cycles = 0;
        cycle_limit *= 2;
        old_x = x;
        old_y = y;
      }

      auto px = static_cast<int>(baphy::normalize(x, -2.0, 2.0, 0, fractal->w()));
      auto py = static_cast<int>(baphy::normalize(y, -2.0, 2.0, 0, fractal->h()));
      if (px >= 0 && py >= 0 && px < fractal->w() && py < fractal->h()) {
        xs[p_idx] = px;
        ys[p_idx] = py;
        p_idx++;
      }
    } while (x2 + y2 <= 4 && ++iters < MAX_ITERS);

    return iters == MAX_ITERS ? 0 : p_idx;
  }
};

BAPHY_RUN(BuddhabrotApp,
    .title = "Buddhabrot",
    .size = {1024, 1024},
    .flags = baphy::WindowFlags::centered | baphy::WindowFlags::vsync,
    .debug_overlay_options_path = HERE / "debug_overlay_options.json"
)
