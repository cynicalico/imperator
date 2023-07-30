#include "baphy/baphy.hpp"
#include "glm/gtx/hash.hpp"
#include <mutex>
#include <unordered_set>

const int ROWS = 10;
const int COLS = 10;
const int MINE_COUNT = 10;
const float SCALE = 3.0f;
const auto HERE = std::filesystem::path(__FILE__).parent_path();
const auto IMG = HERE / "res" / "minesweeper";

enum class GameStatus { playing, win, lose };

class Minesweeper {
public:
  GameStatus status{GameStatus::playing};
  baphy::Ticker timer{};

  Minesweeper() {
    for (int r = 0; r < ROWS; ++r) {
      cells_.emplace_back(COLS, 0);
      visible_.emplace_back(COLS, false);
      visited_.emplace_back(COLS, false);
    }
  }

  void update_timer() {
    if (initialized && status == GameStatus::playing)
      timer.tick();
  }

  int get(int r, int c) {
    if (!in_bounds_(r, c)) return 0;

    if (flag_pos_.contains({r, c}))
      return 11;
    if (!visible_[r][c])
      return 12;
    return get_(r, c);
  }

  bool flag(int r, int c) {
    if (!in_bounds_(r, c)) return false;
    if (visible_[r][c] && !flag_pos_.contains({r, c})) return false;

    if (flag_pos_.contains({r, c}))
      flag_pos_.erase({r, c});
    else
      flag_pos_.insert({r, c});

    if (check_win_()) {
      status = GameStatus::win;
      return true;
    }

    return false;
  }

  bool reveal(int r, int c) {
    std::call_once(initialized_, [&]{
      initialize_(r, c);
      timer.reset();
    });

    if (!in_bounds_(r, c)) return false;

    if (mine_pos_.contains({r, c})) {
      if (flag_pos_.contains({r, c})) return false;

      set_(r, c, 10);
      for (const auto &pos: mine_pos_)
        visible_[pos.x][pos.y] = true;
      status = GameStatus::lose;
      return true;

    } else {
      for (int vr = 0; vr < ROWS; ++vr)
        for (int vc = 0; vc < COLS; ++vc)
          visited_[vr][vc] = false;

      if (!visible_[r][c])
        reveal_(r, c);

      else {
        bool all_flagged{true};
        for (const auto &o: offsets_) {
          auto ro = r + o.x;
          auto co = c + o.y;
          if (get_(ro, co) == 9 && !flag_pos_.contains({ro, co})) {
            all_flagged = false;
            break;
          }
        }

        if (all_flagged)
          for (const auto &o: offsets_)
            reveal_(r + o.x, c + o.y);
      }
    }

    return false;
  }

  int mines_remaining() {
    return MINE_COUNT - flag_pos_.size();
  }

private:
  bool initialized{false};
  std::once_flag initialized_{};

  std::vector<glm::ivec2> offsets_{{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

  std::vector<std::vector<int>> cells_{};
  std::vector<std::vector<bool>> visible_{};
  std::vector<std::vector<bool>> visited_{};
  std::unordered_set<glm::ivec2> mine_pos_{};
  std::unordered_set<glm::ivec2> flag_pos_{};

  void initialize_(int r, int c) {
    std::unordered_set<glm::ivec2> bad_mine_pos_{};
    for (const auto &o: offsets_)
      bad_mine_pos_.insert({r + o.x, c + o.y});
    bad_mine_pos_.insert({r, c});

    while (mine_pos_.size() < MINE_COUNT) {
      auto new_pos = glm::ivec2{baphy::rnd::get(ROWS - 1), baphy::rnd::get(COLS - 1)};
      if (bad_mine_pos_.contains(new_pos)) continue;
      mine_pos_.insert(new_pos);
      set_(new_pos.x, new_pos.y, 9);
    }

    for (int r = 0; r < ROWS; ++r) {
      for (int c = 0; c < COLS; ++c) {
        if (get_(r, c) == 9) continue;

        int n = 0;
        for (const auto &o: offsets_)
          if (get_(r + o.x, c + o.y) == 9)
            n++;
        set_(r, c, n);
      }
    }

    initialized = true;
  }

  inline bool in_bounds_(int r, int c) {
    return r >= 0 && c >= 0 && r < ROWS && c < COLS;
  }

  void reveal_(int r, int c) {
    if (!in_bounds_(r, c)) return;

    if (visited_[r][c]) return;
    visited_[r][c] = true;

    visible_[r][c] = true;
    if (get_(r, c) == 0) {
      for (const auto &o: offsets_)
        reveal_(r + o.x, c + o.y);
    }
  }

  bool check_win_() {
    if (mines_remaining() != 0) return false;

    bool all_mines = true;
    for (const auto &p: flag_pos_)
      if (get_(p.x, p.y) != 9) {
        all_mines = false;
        break;
      }

    return all_mines;
  }

  int get_(int r, int c) {
    if (!in_bounds_(r, c)) return 0;
    return cells_[r][c];
  }

  void set_(int r, int c, int v) {
    if (!in_bounds_(r, c)) return;
    cells_[r][c] = v;
  }
};

class MinesweeperApp : public baphy::Application {
public:
  Minesweeper ms{};

  std::shared_ptr<baphy::Texture> ss_tex{nullptr};
  std::unique_ptr<baphy::Spritesheet> ss{nullptr};
  std::shared_ptr<baphy::Cursor> cursor{nullptr};

  std::shared_ptr<baphy::Sound> startup{nullptr};
  std::shared_ptr<baphy::Sound> game_over{nullptr};
  std::shared_ptr<baphy::Sound> win{nullptr};
  std::shared_ptr<baphy::Sound> explosion{nullptr};

  void initialize() override {
    audio->open_device();
    audio->open_context();
    audio->make_current();

    startup = audio->load(IMG / "sound" / "startup.wav");
    game_over = audio->load(IMG / "sound" / "game_over.wav");
    win = audio->load(IMG / "sound" / "win.wav");
    explosion = audio->load(IMG / "sound" / "explosion.wav");

    ss_tex = textures->load(IMG / "sheet.png", true);
    ss = std::make_unique<baphy::Spritesheet>(ss_tex, IMG / "sheet.json");
    ss->set_scale(SCALE);

    cursor = cursors->create(IMG / "cursor.png", 7, 7);
    cursor->set();

    auto window_w = ss->w("border_tl") + COLS * ss->w("border_t") + ss->w("border_tr");
    auto window_h = ss->h("border_tl") + (ROWS + 2) * ss->h("border_l") + ss->h("border_bl");
    window->set_size(window_w, window_h);
    window->center();

    window->set_icon_dir(IMG / "icon");

    window->show();

    startup->play();
  }

  void update(double dt) override {
    if (input->pressed("escape")) window->set_should_close(true);

    ms.update_timer();

    if (ms.status == GameStatus::playing) {
      if (input->pressed("mb_right")) {
        auto x = input->mouse_x();
        auto y = input->mouse_y();
        if (in_minefield(x, y)) {
          auto c = minefield_coords(x, y);
          if (ms.flag(c.y, c.x))
            win->play();
        }
      }

      if (input->pressed("mb_left")) {
        auto x = input->mouse_x();
        auto y = input->mouse_y();
        if (in_minefield(x, y)) {
          auto c = minefield_coords(x, y);
          if (ms.reveal(c.y, c.x)) {
            explosion->play();
            timer->after(1.0, [&] { game_over->play(); });
          }
        }
      }
    }
  }

  void draw() override {
    gfx->clear(baphy::rgb(0x291d2b));

    draw_border();
    draw_title();
    draw_field();
    draw_selected();
    draw_mines_remaining();
    draw_status();
    draw_timer();
  }

  bool in_minefield(double x, double y) {
    return x >= ss->w("border_l") &&
           x < window->w() - ss->w("border_r") &&
           y >= ss->h("border_t") + ss->h("title") &&
           y < window->h() - ss->h("border_b") - ss->h("border_m");
  }

  glm::ivec2 minefield_coords(double x, double y) {
    return {std::floor((x - ss->w("border_l")) / ss->w("empty")),
            std::floor((y - ss->h("border_t") - ss->h("title")) / ss->h("empty"))};
  }

  void draw_border() {
    int x, y;

    x = ss->w("border_tl");
    y = 0;
    for (; x < window->w() - ss->w("border_tr");
         x += ss->w("border_t"))
      ss->draw("border_t", x, y);

    x = 0;
    y = 0;
    ss->draw("border_tl", x, y);
    for (y = ss->h("border_tl");
         y < window->h() - ss->h("border_bl");
         y += ss->h("border_l"))
      ss->draw("border_l", x, y);
    ss->draw("border_bl", x, y);

    x = window->w() - ss->w("border_tr");
    y = 0;
    ss->draw("border_tr", x, y);
    for (y = ss->h("border_tr");
         y < window->h() - ss->h("border_br");
         y += ss->h("border_r"))
      ss->draw("border_r", x, y);
    ss->draw("border_br", x, y);

    x = ss->w("border_br");
    y = window->h() - ss->h("border_b") - ss->h("border_m");
    for (; x < window->w() - ss->w("border_br");
           x += ss->w("border_m"))
      ss->draw("border_m", x, y);

    x = ss->w("border_br");
    y = window->h() - ss->h("border_b");
    for (; x < window->w() - ss->w("border_br");
         x += ss->w("border_b"))
      ss->draw("border_b", x, y);
  }

  void draw_title() {
    ss->draw("title", ss->w("border_tl"), ss->h("border_tl"));

    int x = x = ss->w("border_l") + ss->w("title");
    int y = ss->h("border_t");
    for (; x < window->w() - ss->w("border_r"); x += ss->w("border_m"))
      ss->draw("border_m", x, y);
  }

  void draw_field() {
    float x_off = ss->w("border_tl");
    float y_off = ss->h("border_tl") + ss->h("title");

    for (int r = 0; r < ROWS; ++r) {
      for (int c = 0; c < COLS; ++c) {
        std::string sprite_name;

        switch (ms.get(r, c)) {
          case 0:  sprite_name = "empty"; break;
          case 1:  sprite_name = "hint_1"; break;
          case 2:  sprite_name = "hint_2"; break;
          case 3:  sprite_name = "hint_3"; break;
          case 4:  sprite_name = "hint_4"; break;
          case 5:  sprite_name = "hint_5"; break;
          case 6:  sprite_name = "hint_6"; break;
          case 7:  sprite_name = "hint_7"; break;
          case 8:  sprite_name = "hint_8"; break;
          case 9:  sprite_name = "bomb_hidden"; break;
          case 10: sprite_name = "bomb_exploded"; break;
          case 11: sprite_name = "flag"; break;
          case 12: sprite_name = "hidden"; break;
        }

        ss->draw(sprite_name, x_off + c * ss->w(sprite_name), y_off + r * ss->h(sprite_name));
      }
    }
  }

  void draw_selected() {
    auto x = input->mouse_x();
    auto y = input->mouse_y();
    if (ms.status == GameStatus::playing && in_minefield(x, y)) {
      auto c = minefield_coords(x, y);
      float x_off = ss->w("border_tl");
      float y_off = ss->h("border_tl") + ss->h("title");
      ss->draw("selected", x_off + c.x * ss->w("selected"), y_off + c.y * ss->h("selected"));
    }
  }

  void draw_mines_remaining() {
    int n = ms.mines_remaining();
    auto ns = fmt::format("{}", n);

    float x = window->w() - ss->w("border_r");
    float y = window->h() - ss->h("border_b") - SCALE;
    for (const auto &c : ns | std::views::reverse) {
      auto s = std::string(1, c);
      x -= ss->w(s);
      ss->draw(s, x, y - ss->h(s));
    }
  }

  void draw_status() {
    std::string sprite_name;
    switch (ms.status) {
      case GameStatus::playing: sprite_name = "indicator_playing"; break;
      case GameStatus::win: sprite_name = "indicator_win"; break;
      case GameStatus::lose: sprite_name = "indicator_lose"; break;
    }

    ss->draw(sprite_name, window->w() - ss->w("border_r") - ss->w(sprite_name), ss->h("border_t"));
  }

  void draw_timer() {
    auto sec = static_cast<int>(std::floor(ms.timer.elapsed_sec()));
    auto m = sec / 60;
    sec -= m * 60;
    auto ns = fmt::format("{:02}:{:02}", m, sec);

    float x = ss->w("border_l") + SCALE;
    float y = window->h() - ss->h("border_b") - SCALE;
    for (const auto &c : ns) {
      auto s = std::string(1, c);
      ss->draw(s, x, y - ss->h(s));
      x += ss->w(s);
    }
  }
};

BAPHY_RUN(MinesweeperApp,
    .title = "Minesweeper",
    .flags = baphy::WindowFlags::hidden | baphy::WindowFlags::vsync,
    .debug_overlay_options_path = HERE / "debug_overlay_options.json"
)
