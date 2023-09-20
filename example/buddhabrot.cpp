#include "baphy/baphy.hpp"
#include <fstream>

//#define USE_MUTEX

const auto HERE = std::filesystem::path(__FILE__).parent_path();

class BuddhabrotApp : public baphy::Application {
public:
  std::shared_ptr<baphy::Texture> font_tex;
  std::shared_ptr<baphy::CP437Font> font;

  const std::size_t FRACTAL_W{1024 * 16};
  const std::size_t FRACTAL_H{1024 * 16};

  const std::uint64_t POINTS = 100'000'000;
  const std::size_t MIN_PATH_SIZE{10};
  const std::size_t MIN_MUTATE_PATH_SIZE{8000};
  const std::size_t MAX_ITERS{20000};

  const std::size_t RED_MIN = 10000;
  const std::size_t RED_MAX = MAX_ITERS;
  const std::size_t GRN_MIN = 1000;
  const std::size_t GRN_MAX = 5000;
  const std::size_t BLU_MIN = 10;
  const std::size_t BLU_MAX = 1000;

  std::vector<std::uint64_t> r_hits{};
  std::vector<std::uint64_t> g_hits{};
  std::vector<std::uint64_t> b_hits{};
  std::uint64_t hits{0}, valid{0}, invalid{0}, mutates{0}, long_paths{0}, failed_mutate{0};
  std::size_t hps{};
  baphy::SMA hps_avg{10};
  std::size_t vps{};
  baphy::SMA vps_avg{10};
#ifdef USE_MUTEX
  std::mutex hit_mutex;
#endif

  std::chrono::system_clock::time_point start_time;
  std::chrono::system_clock::time_point end_time;
  std::vector<std::jthread> ip_threads;
  const std::uint64_t THREAD_COUNT{8};
  bool running{true};
  bool saved{false};

  bool saving{false};
  double ppm_progress{0};

  void initialize() override {
    font_tex = textures->load(HERE / "res" / "img" / "CGA8x8thick.png", true);
    font = fonts->load<baphy::CP437Font>("cga8x8thick", font_tex, 8, 8);

    r_hits.resize(FRACTAL_W * FRACTAL_H, 0);
    g_hits.resize(FRACTAL_W * FRACTAL_H, 0);
    b_hits.resize(FRACTAL_W * FRACTAL_H, 0);

    double n = 0.99999;
    for (std::size_t m = 0; m < THREAD_COUNT; ++m) {
      ip_threads.emplace_back([&, n] {
        std::size_t stored_point_limit = 10;
        std::size_t p_idx = 0;
        std::vector<int> xs(MAX_ITERS * stored_point_limit, 0);
        std::vector<int> ys(MAX_ITERS * stored_point_limit, 0);
        std::vector<std::uint64_t> path_lens(stored_point_limit, 0);

        double x = 0;
        double y = 0;
        double lx = 0;
        double ly = 0;
        bool mutate{false};
        double mutate_chance = n;

        std::uint64_t hits_acc{0};
        std::uint64_t hps_acc{0};
        std::uint64_t valid_acc{0};
        std::uint64_t vps_acc{0};
        std::uint64_t invalid_acc{0};
        std::uint64_t mutates_acc{0};
        std::uint64_t failed_mutate_acc{0};
        std::uint64_t long_paths_acc{0};

        do {
          p_idx = 0;
          hits_acc = 0;
          hps_acc = 0;
          valid_acc = 0;
          vps_acc = 0;
          invalid_acc = 0;
          mutates_acc = 0;
          failed_mutate_acc = 0;
          long_paths_acc = 0;

          std::size_t i = 0;
          while (running && valid_acc < stored_point_limit) {
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
              y = baphy::rnd::get<double>(-2.0, 2.0);
            }

            std::size_t iters;
            std::size_t last_p_idx = p_idx;
            p_idx = iterate_point(xs, ys, p_idx, iters, x, y);

            std::size_t count = p_idx - last_p_idx;
            if ((!mutate && count >= MIN_PATH_SIZE) ||
                (mutate && count >= MIN_MUTATE_PATH_SIZE)) {
              hits_acc += count;
              hps_acc += count;
              valid_acc++;
              vps_acc++;
              path_lens[i] = count;
              ++i;

              if (mutate)
                mutates_acc++;

              mutate = count >= MIN_MUTATE_PATH_SIZE && baphy::rnd::get<bool>(mutate_chance);
            } else {
              invalid_acc++;

              p_idx = last_p_idx;
            }

            if (mutate && count < MIN_MUTATE_PATH_SIZE) {
              failed_mutate_acc++;
              x = lx;
              y = ly;
            }

            if (count >= MIN_MUTATE_PATH_SIZE)
              long_paths_acc++;
          }

          if (!running)
            break;

#ifdef USE_MUTEX
          hit_mutex.lock();
#endif

          std::size_t j_start = 0;
          for (i = 0; i < stored_point_limit; ++i) {
            std::size_t count = path_lens[i];
            if (count == 0) continue;

            std::size_t last_idx = FRACTAL_W * FRACTAL_H + 1; // Impossible index
            for (std::size_t j = j_start; j < j_start + count; ++j) {
              std::size_t idx = (ys[j] * FRACTAL_W) + xs[j];
              if (idx == last_idx) {
                hits_acc--;
                continue;
              }
              last_idx = idx;

              if (count >= RED_MIN && count <= RED_MAX) ++r_hits[idx];
              if (count >= GRN_MIN && count <= GRN_MAX) ++g_hits[idx];
              if (count >= BLU_MIN && count <= BLU_MAX) ++b_hits[idx];
            }

            j_start += count;
          }

          hits += hits_acc;
          hps += hps_acc;
          valid += valid_acc;
          vps += vps_acc;
          mutates += mutates_acc;
          invalid += invalid_acc;
          failed_mutate += failed_mutate_acc;
          long_paths += long_paths_acc;

          running = valid < POINTS;

#ifdef USE_MUTEX
          hit_mutex.unlock();
#endif
        } while (running);
      });
      n *= n;
    }

    timer->every(1.0, [&] {
      hps_avg.update(hps);
      hps = 0;
    });

    timer->every(1.0, [&] {
      vps_avg.update(vps);
      vps = 0;
    });

    start_time = std::chrono::system_clock::now();
  }

  ~BuddhabrotApp() override {
    running = false;
    for (auto &t: ip_threads)
      t.join();
  }

  void update(double dt) override {
    if (running)
      end_time = std::chrono::system_clock::now();

    else if (!running && !saving) {
      saving = true;

      pool->add_job([&](auto wait) {
        std::uint64_t r_max{0}, g_max{0}, b_max{0};
        for (std::size_t y = 0; y < FRACTAL_H; y++)
          for (std::size_t x = 0; x < FRACTAL_W; x++) {
            std::size_t idx = (y * FRACTAL_W) + x;
            r_max = std::max(r_max, r_hits[idx]);
            g_max = std::max(g_max, g_hits[idx]);
            b_max = std::max(b_max, b_hits[idx]);

            ppm_progress = 1.0;
          }
        ppm_progress = 0.0;

        auto ts = baphy::timestamp();
        auto ppm_fname = HERE / fmt::format("{}_buddhabrot.ppm", ts);
        auto png_fname = HERE / fmt::format("{}_buddhabrot.png", ts);
        auto brighten_path = HERE / "brighten.py";

        auto ofs = std::ofstream(ppm_fname, std::ios::app | std::ios::binary);
        if (ofs.is_open()) {
          auto header = fmt::format("P6 {} {} 255 ", FRACTAL_W, FRACTAL_H);
          ofs.write(header.c_str(), header.size() * sizeof(char));

          for (std::size_t y = 0; y < FRACTAL_H; y++)
            for (std::size_t x = 0; x < FRACTAL_W; x++) {
              std::size_t idx = (y * FRACTAL_W) + x;
              std::size_t idx_flip = y + (x * FRACTAL_H);

              auto rf = std::pow(r_hits[idx_flip], 1.0 / 2.0) / std::pow(r_max, 1.0 / 2.0);
              auto gf = std::pow(g_hits[idx_flip], 1.0 / 2.0) / std::pow(g_max, 1.0 / 2.0);
              auto bf = std::pow(b_hits[idx_flip], 1.0 / 2.0) / std::pow(b_max, 1.0 / 2.0);
              auto c_out = glm::vec3{rf, gf, bf};

              char r = (int)(255 * c_out.r) & 0xff;
              char g = (int)(255 * c_out.g) & 0xff;
              char b = (int)(255 * c_out.b) & 0xff;

              ofs.write(reinterpret_cast<char *>(&r), sizeof(char));
              ofs.write(reinterpret_cast<char *>(&g), sizeof(char));
              ofs.write(reinterpret_cast<char *>(&b), sizeof(char));

              ppm_progress = idx / (double)(FRACTAL_W * FRACTAL_H);
            }
          ppm_progress = 1.0;

          ofs.close();

          auto cmd = fmt::format("magick convert {} {}", ppm_fname.string(), png_fname.string());
          std::system(cmd.c_str());

          auto cmd2 = fmt::format("python {} {}", brighten_path.string(), png_fname.string());
          std::system(cmd2.c_str());

          saved = true;
        }
      });

    }
  }

  void draw() override {
    using namespace std::chrono_literals;

    gfx->clear(saved ? baphy::rgb("green") : saving ? baphy::rgb("darkgoldenrod") : baphy::rgb("darkred"));

    auto elapsed = std::chrono::floor<std::chrono::seconds>(end_time - start_time);
    auto time_s = fmt::format("elapsed: {:%H:%M:%S}", elapsed);

    std::chrono::duration<double> remaining{0s};
    if (running)
      remaining = std::chrono::duration<double>((POINTS - valid) / vps_avg.value());
    auto remaining_s = fmt::format("remaining: {:.2%H:%M:%S}", remaining);

    int row = 8 * 6;

    font->draw(0, row, 1, fmt::format(std::locale("en_US.UTF-8"), "hits: {:L}", hits), baphy::rgb("white"));
    row += 8;

    row += 8;

    font->draw(0, row, 1, fmt::format(std::locale("en_US.UTF-8"), "mutates: {:L}", mutates), baphy::rgb("white"));
    row += 8;
    font->draw(0, row, 1, fmt::format(std::locale("en_US.UTF-8"), "failed mutate: {:L}", failed_mutate), baphy::rgb("white"));
    row += 8;
    font->draw(0, row, 1, fmt::format(std::locale("en_US.UTF-8"), "long paths: {:L}", long_paths), baphy::rgb("white"));
    row += 8;

    row += 8;

    font->draw(0, row, 1, fmt::format(std::locale("en_US.UTF-8"), "valid: {:L}", valid), baphy::rgb("white"));
    row += 8;
    font->draw(0, row, 1, fmt::format(std::locale("en_US.UTF-8"), "invalid: {:L}", invalid), baphy::rgb("white"));
    row += 8;
    font->draw(0, row, 1, fmt::format("ratio: {:.2f}%", ((double)valid / (double)(valid + invalid)) * 100), baphy::rgb("white"));
    row += 8;

    row += 8;

    font->draw(0, row, 1, fmt::format(std::locale("en_US.UTF-8"), "hps: {:.2Lf}", hps_avg.value()), baphy::rgb("white"));
    row += 8;
    font->draw(0, row, 1, fmt::format(std::locale("en_US.UTF-8"), "vps: {:.2Lf}", vps_avg.value()), baphy::rgb("white"));
    row += 8;

    row += 8;

    font->draw(0, row, 1, time_s, baphy::rgb("white"));
    row += 8;
    font->draw(0, row, 1, remaining_s, baphy::rgb("white"));
    row += 8;

    row += 8;

    if (saving && !saved) {
      if (ppm_progress < 1.0)
        font->draw(0, row, 1, fmt::format("Saving ppm... {:.2f}%", ppm_progress * 100), baphy::rgb("white"));
      else
        font->draw(0, row, 1, "Converting ppm to png...", baphy::rgb("white"));
    }

    if (saved)
      font->draw(0, row, 1, "Saved!", baphy::rgb("white"));
  }

  std::size_t iterate_point(std::vector<int> &xs, std::vector<int> &ys, std::size_t start_idx, std::size_t &iters, double x0, double y0) {
    double q = std::pow(x0 - 0.25, 2) + std::pow(y0, 2);
    if (q * (q + (x0 - 0.25)) <= 0.25 * std::pow(y0, 2))
      return start_idx;

    if (std::pow(x0 + 1, 2) + std::pow(y0, 2) <= 0.0625)
      return start_idx;

    double x = 0.0, y = 0.0, x2 = 0.0, y2 = 0.0;

    double old_x = 0.0, old_y = 0.0;
    uint64_t cycles = 0L, cycle_limit = 1L;

    iters = 0;
    std::size_t p_idx = start_idx;
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

      auto px = static_cast<int>(baphy::normalize(x, -2.0, 2.0, 0, FRACTAL_W));
      auto py = static_cast<int>(baphy::normalize(y, -2.0, 2.0, 0, FRACTAL_H));
      if (px >= 0 && py >= 0 && px < FRACTAL_W && py < FRACTAL_H) {
        xs[p_idx] = px;
        ys[p_idx] = py;
        p_idx++;
      }
    } while (x2 + y2 <= 4 && ++iters < MAX_ITERS);

    return iters == MAX_ITERS ? start_idx : p_idx;
  }
};

BAPHY_RUN(BuddhabrotApp,
    .title = "Buddhabrot",
    .size = {8 * 30, 8 * 30},
    .flags = baphy::WindowFlags::centered | baphy::WindowFlags::vsync,
    .debug_overlay_options_path = HERE / "debug_overlay_options.json"
)
