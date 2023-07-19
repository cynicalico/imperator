#include "baphy/baphy.hpp"
#include <array>

const auto HERE = std::filesystem::path(__FILE__).parent_path();

enum class CellState {
  empty,
  r_wire, y_wire, g_wire, b_wire,
  r_head, y_head, g_head, b_head,
  r_tail, y_tail, g_tail, b_tail,
};

// Implementation of the WireWorld variant: WireWorldRYGB
// https://lodev.org/ca/wireworldrgb.html

class WireWorldRYGB {
public:
  const static int rows{100}, cols{100};

  WireWorldRYGB() {
    for (std::size_t r = 0; r < rows; ++r) {
      cells_[r] = std::array<CellState, cols>();
      prev_cells_[r] = std::array<CellState, cols>();

      cells_[r].fill(CellState::empty);
    }
  }

  void reset() {
    for (std::size_t r = 0; r < rows; ++r)
      for (std::size_t c = 0; c < cols; ++c)
        set(r, c, CellState::empty);
  }

  void set(std::size_t r, std::size_t c, CellState s) {
    if (r < rows && c < cols)
      cells_[r][c] = s;
  }

  CellState get(std::size_t r, std::size_t c) {
    return (r < rows && c < cols) ? cells_[r][c] : CellState::empty;
  }

  void step() {
    for (std::size_t r = 0; r < rows; ++r)
      for (std::size_t c = 0; c < cols; ++c)
        prev_cells_[r][c] = cells_[r][c];

    for (std::size_t r = 0; r < rows; ++r)
      for (std::size_t c = 0; c < cols; ++c) {
        switch (prev_cells_[r][c]) {
          case CellState::empty: continue;
          case CellState::r_wire: {
            auto [r_n, y_n, g_n, b_n] = count_neighbor_heads(r, c);
            if (xor_3(
                ((r_n == 1 || r_n == 2) && (g_n >= 0 && g_n <= 7)) && (y_n + b_n == 0),
                b_n == 1 && (r_n + y_n + g_n == 0),
                y_n == 2 && (r_n + g_n + b_n == 0)
            ))
              cells_[r][c] = CellState::r_head;
          }
            break;
          case CellState::y_wire: {
            auto [r_n, y_n, g_n, b_n] = count_neighbor_heads(r, c);
            if (
                (((y_n == 1 || y_n == 2) && (b_n >= 0 && b_n <= 7)) && (r_n + g_n == 0)) ^
                ((r_n == 1 || r_n == 2) && (y_n + g_n + b_n == 0))
            )
              cells_[r][c] = CellState::y_head;
          }
            break;
          case CellState::g_wire: {
            auto [r_n, y_n, g_n, b_n] = count_neighbor_heads(r, c);
            if (xor_3(
                ((g_n == 1 || g_n == 2) && (r_n >= 0 && r_n <= 7)) && (y_n + b_n == 0),
                y_n == 1 && (r_n + g_n + b_n == 0),
                b_n == 2 && (r_n + y_n + g_n == 0)
            ))
              cells_[r][c] = CellState::g_head;
          }
            break;
          case CellState::b_wire: {
            auto [r_n, y_n, g_n, b_n] = count_neighbor_heads(r, c);
            if (
                (((b_n == 1 || b_n == 2) && (y_n >= 0 && y_n <= 7)) && (r_n + g_n == 0)) ^
                ((g_n == 1 || g_n == 2) && (r_n + y_n + b_n == 0))
            )
              cells_[r][c] = CellState::b_head;
          }
            break;
          case CellState::r_head: cells_[r][c] = CellState::r_tail; break;
          case CellState::y_head: cells_[r][c] = CellState::y_tail; break;
          case CellState::g_head: cells_[r][c] = CellState::g_tail; break;
          case CellState::b_head: cells_[r][c] = CellState::b_tail; break;
          case CellState::r_tail: cells_[r][c] = CellState::r_wire; break;
          case CellState::y_tail: cells_[r][c] = CellState::y_wire; break;
          case CellState::g_tail: cells_[r][c] = CellState::g_wire; break;
          case CellState::b_tail: cells_[r][c] = CellState::b_wire; break;
        }
      }
  }

private:
  std::array<std::array<CellState, cols>, rows> cells_{};
  std::array<std::array<CellState, cols>, rows> prev_cells_{};

  std::array<int, 4> count_neighbor_heads(std::size_t r, std::size_t c) {
    static std::array<glm::ivec2, 8> offsets{{
        {-1, -1}, {-1, 0}, {-1, 1},
        { 0, -1},          { 0, 1},
        { 1, -1}, { 1, 0}, { 1, 1}
    }};

    int r_n{0};
    int y_n{0};
    int g_n{0};
    int b_n{0};
    for (const auto &o: offsets) {
      std::size_t o_r = r + o.x;
      std::size_t o_c = c + o.y;
      if (o_r >= rows || o_c >= cols) continue;

      switch (prev_cells_[o_r][o_c]) {
        case CellState::r_head: r_n++; break;
        case CellState::y_head: y_n++; break;
        case CellState::g_head: g_n++; break;
        case CellState::b_head: b_n++; break;
        default: break;
      }
    }

    return {r_n, y_n, g_n, b_n};
  }

  inline bool xor_3(bool a, bool b, bool c) {
    return (a ^ b ^ c) && !(a && b && c);
  }
};

template<typename T>
class ScrollSelector {
public:
  explicit ScrollSelector(std::vector<std::vector<T>> options) : options_(options) {}

  void scroll_up() { row_ = (row_ + 1) % options_.size(); }
  void scroll_down() { row_ = (row_ - 1) % options_.size(); }

  std::vector<T> row() { return options_[row_]; }

  std::size_t col() { return col_; }
  void set_col(std::size_t new_col) { col_ = new_col; }

  T selected() { return options_[row_][col_]; }

private:
  std::vector<std::vector<T>> options_{};
  std::size_t row_{0};
  std::size_t col_{0};
};

class WireWorldRYBGApp : public baphy::Application {
public:
  WireWorldRYGB ww{};

  int cell_size = 10;
  int x_off{};
  int y_off{};

  baphy::RGB sel_color = baphy::rgb(0xbfbfbf);
  baphy::RGB bg_color_no_sim = baphy::rgb(0x260000);
  baphy::RGB bg_color_sim = baphy::rgb(0x002600);
  std::unordered_map<CellState, baphy::RGB> cell_colors{
      {CellState::empty,  baphy::rgb(0x262626)},
      {CellState::r_wire, baphy::rgb(0x800000)},
      {CellState::y_wire, baphy::rgb(0x808000)},
      {CellState::g_wire, baphy::rgb(0x008000)},
      {CellState::b_wire, baphy::rgb(0x000080)},
      {CellState::r_head, baphy::rgb(0xff4d4d)},
      {CellState::y_head, baphy::rgb(0xffff4d)},
      {CellState::g_head, baphy::rgb(0x4dff4d)},
      {CellState::b_head, baphy::rgb(0x4d4dff)},
      {CellState::r_tail, baphy::rgb(0xb33636)},
      {CellState::y_tail, baphy::rgb(0xb3b336)},
      {CellState::g_tail, baphy::rgb(0x36b336)},
      {CellState::b_tail, baphy::rgb(0x3636b3)},
  };

  bool left_pen_down{false};
  bool right_pen_down{false};

  int pen_indicator_size = 10;
  int pen_indicator_padding = 3;
  ScrollSelector<CellState> pen{{
      {CellState::r_wire, CellState::r_head, CellState::r_tail},
      {CellState::y_wire, CellState::y_head, CellState::y_tail},
      {CellState::g_wire, CellState::g_head, CellState::g_tail},
      {CellState::b_wire, CellState::b_head, CellState::b_tail}
  }};

  std::string sim_timer{};
  double sim_timer_delay{0.25};
  double sim_timer_counter{0};
  bool sim_timer_debounce_resume{false};

  void initialize() override {
    x_off = std::floor((window->w() - (ww.cols * cell_size)) / 2.0f);
    y_off = std::floor((window->h() - (ww.rows * cell_size)) / 2.0f);

    sim_timer = timer->every(0.1, [&] { ww.step(); });
    timer->pause(sim_timer);
  }

  void update(double dt) override {
    if (input->pressed("escape"))
      window->set_should_close(true);

    if (input->pressed("1"))
      pen.set_col(0);

    if (input->pressed("2"))
      pen.set_col(1);

    if (input->pressed("3"))
      pen.set_col(2);

    if (input->pressed("r"))
      ww.reset();

    if (input->mouse_sy() < 0)
      pen.scroll_up();

    if (input->mouse_sy() > 0)
      pen.scroll_down();

    if (input->pressed("mb_left")) {
      set_cell_at_mouse_pos(pen.selected());
      left_pen_down = true;
    }

    if (input->pressed("mb_right")) {
      set_cell_at_mouse_pos(CellState::empty);
      right_pen_down = true;
    }

    if (input->released("mb_left"))
      left_pen_down = false;

    if (input->released("mb_right"))
      right_pen_down = false;

    if (input->mouse_moved() && left_pen_down)
      set_cell_at_mouse_pos(pen.selected());

    if (input->mouse_moved() && right_pen_down)
      set_cell_at_mouse_pos(CellState::empty);

    sim_timer_counter += dt;
    if (input->pressed("space")) {
      if (!timer->is_paused(sim_timer)) {
        timer->pause(sim_timer);
        sim_timer_debounce_resume = true;
      } else
        sim_timer_counter = 0;
    }

    if (input->down("space", 0, sim_timer_delay))
      if (!sim_timer_debounce_resume)
        timer->resume(sim_timer);

    if (input->released("space")) {
      if (sim_timer_counter < sim_timer_delay)
        ww.step();
      sim_timer_debounce_resume = false;
    }
  }

  void draw() override {
    gfx->clear(timer->is_paused(sim_timer) ? bg_color_no_sim : bg_color_sim);

    draw_empty();
    draw_cells();
    draw_pen_indicators();
  }

  void draw_empty() {
    int w = ww.cols * cell_size;
    int h = ww.rows * cell_size;
    primitives->rect(x_off, y_off, w, h, cell_colors[CellState::empty]);
  }

  void draw_cells() {
    for (std::size_t r = 0; r < ww.rows; ++r)
      for (std::size_t c = 0; c < ww.cols; ++c) {
        int x = x_off + (c * cell_size);
        int y = y_off + (r * cell_size);
        primitives->rect(x, y, cell_size, cell_size, cell_colors[ww.get(r, c)]);
      }
  }

  void draw_pen_indicators() {
    int base_x = x_off;
    int base_y = y_off - pen_indicator_size - 2;
    int off = 0;

    auto draw_selected = [&] {
      int side_len = pen_indicator_size + 1;
      primitives->line(base_x + off, base_y, base_x + off + side_len, base_y, sel_color);
      primitives->line(base_x + off + side_len, base_y, base_x + off + side_len, base_y + side_len, sel_color);
      primitives->line(base_x + off + side_len, base_y + side_len, base_x + off, base_y + side_len, sel_color);
      primitives->line(base_x + off, base_y + side_len, base_x + off, base_y, sel_color);
    };

    for (const auto &[i, c]: baphy::enumerate(pen.row())) {
      primitives->rect(base_x + off + 1, base_y + 1, pen_indicator_size, pen_indicator_size, cell_colors[c]);
      if (i == pen.col())
        draw_selected();
      off += pen_indicator_size + pen_indicator_padding;
    }
  }

  glm::ivec2 mouse_to_cell_pos() {
    return {(input->mouse_x() - x_off) / cell_size,
            (input->mouse_y() - y_off) / cell_size};
  }

  void set_cell_at_mouse_pos(CellState s) {
    auto cell_pos = mouse_to_cell_pos();
    ww.set(cell_pos.y, cell_pos.x, s);
  }
};

BAPHY_RUN(WireWorldRYBGApp,
    .title = "WireWorldRYGB",
    .size = {1100, 1100},
    .flags = baphy::WindowFlags::centered,
    .debug_overlay_options_path = HERE / "debug_overlay_options.json"
)
