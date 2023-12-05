/* Code is not mine, but cleaning up/making typesafe  */
/* https://github.com/tyt2y3/vaserenderer */

#ifndef EXAMPLE_INDEV_HPP
#define EXAMPLE_INDEV_HPP

#include "imperator/imperator.hpp"
#include "imperator/imperator_gl_wrappers.hpp"

namespace imp {
using Point = glm::dvec2;
using Color = glm::dvec4;

// Critical weight to do approximation rather than real joint processing
constexpr double CRITICAL_SEGMENT_APPROX_WEIGHT = 1.6;

// Smallest value not regarded as zero
constexpr double MIN_ALW = 0.00000000001;

struct PolylineVertex {
  float x, y;
  float r, g, b, a;
};

enum class PolylineJoin {
  miter,
  bevel,
  round
};

enum class PolylineCap {
  unset = -1,
  butt,
  round,
  square,
  rect,
  both,
  first = 10,
  last = 20,
  none = 30
};

enum class PolylineOptInternal {
  none = 0,
  const_color = 1 << 0,
  const_weight = 1 << 1,
  no_cap_first = 1 << 2,
  no_cap_last = 1 << 3,
  join_first = 1 << 4,
  join_last = 1 << 5
};

struct PolylineOpt {
  PolylineJoin join{PolylineJoin::miter};
  PolylineCap cap{PolylineCap::butt};
  bool feather{false};
  double feathering{0.0};
  bool no_feather_at_cap{false};
  bool no_feather_at_core{false};
};

// Info for anchor_late to perform triangulation
struct Polyline {
  // For all joints; always outward
  Point vP; // Vector to intersection point
  Point vR; // Fading vector at sharp end

  // For djoint == PolylineJoin::bevel; T, R, T1, and R1 are always outward
  Point T; // Core thickness of a line
  Point R; // Fading edge of a line
  Point bR; // Out stepping vector, same direction as cap
  Point T1, R1; // Alternate vectors, same direction as T21;

  // For djoint == PolylineJoin::round
  double t, r;

  // For degeneration case
  bool degenT; // Core degenerated
  bool degenR; // Fade degenerated
  bool pre_full; // Draw the preceding segment in full
  Point PT, PR;
  double pt; // Parameter at intersection
  bool R_full_degen;

  PolylineCap djoint; // Determined joint
  // e.g. originally a joint is PLJ_miter. but it is smaller than critical angle, should then set djoint to PLJ_bevel
};

// Working memory for anchor
struct Anchor {
  Point P[3];
  Color C[3];
  double W[3];

  Point cap_start, cap_end;
  Polyline SL[3];
};
}

ENUM_ENABLE_BITOPS(imp::PolylineOptInternal);

namespace imp {
namespace internal {
inline bool negligible(double x) {
  return -MIN_ALW < x && x < MIN_ALW;
}

inline double normalize(Point& p) {
  const double len = glm::length(p);
  p = glm::normalize(p);
  return len;
}

inline void perpen(Point& p) {
  const double tmp = p.y;
  p.y = p.x;
  p.x = -tmp;
}

inline void opposite(Point& p) {
  p.x = -p.x;
  p.y = -p.y;
}

inline Color color_at(const std::vector<Color>& colors, int i, const PolylineOptInternal& opts_internal) {
  if (is_flag_set(opts_internal, PolylineOptInternal::const_color))
    return colors[0];
  return colors[i];
}

inline double weight_at(const std::vector<double>& weights, int i, const PolylineOptInternal& opts_internal) {
  if (is_flag_set(opts_internal, PolylineOptInternal::const_weight))
    return weights[0];
  return weights[i];
}

inline Color color_between(const Color& c0, const Color& c1, double t) {
  if (t < 0.0) { t = 0.0; }
  if (t > 1.0) { t = 1.0; }
  const double t_i = 1.0 - t;

  return {
    c0.r * t_i + c1.r * t,
    c0.g * t_i + c1.g * t,
    c0.b * t_i + c1.b * t,
    c0.a * t_i + c1.a * t
  };
}

inline void push_quadf(
  std::vector<PolylineVertex>& vertices,
  const Point& P0, const Point& P1, const Point& P2, const Point& P3,
  const Color& c0, const Color& c1, const Color& c2, const Color& c3,
  bool trans0, bool trans1, bool trans2, bool trans3
) {
  //  1    3
  //
  //  0    2
  //

  IMPERATOR_LOG_INFO("{} {}", P0.x, P0.y);
  IMPERATOR_LOG_INFO("{} {}", P1.x, P1.y);
  IMPERATOR_LOG_INFO("{} {}", P2.x, P2.y);
  IMPERATOR_LOG_INFO("{} {}", P3.x, P3.y);

  vertices.emplace_back(P0.x, P0.y, c0.r / 255.0f, c0.g / 255.0f, c0.b / 255.0f, !trans0 ? c0.a / 255.0f : 0);
  vertices.emplace_back(P1.x, P1.y, c1.r / 255.0f, c1.g / 255.0f, c1.b / 255.0f, !trans1 ? c1.a / 255.0f : 0);
  vertices.emplace_back(P2.x, P2.y, c2.r / 255.0f, c2.g / 255.0f, c2.b / 255.0f, !trans2 ? c2.a / 255.0f : 0);

  vertices.emplace_back(P1.x, P1.y, c1.r / 255.0f, c1.g / 255.0f, c1.b / 255.0f, !trans1 ? c1.a / 255.0f : 0);
  vertices.emplace_back(P2.x, P2.y, c2.r / 255.0f, c2.g / 255.0f, c2.b / 255.0f, !trans2 ? c2.a / 255.0f : 0);
  vertices.emplace_back(P3.x, P3.y, c3.r / 255.0f, c3.g / 255.0f, c3.b / 255.0f, !trans3 ? c3.a / 255.0f : 0);
}

inline void push_quad(
  std::vector<PolylineVertex>& vertices,
  const Point& P0, const Point& P1, const Point& P2, const Point& P3,
  const Color& c0, const Color& c1, const Color& c2, const Color& c3
) {
  push_quadf(vertices, P0, P1, P2, P3, c0, c1, c2, c3, false, false, false, false);
}

void determine_t_r(double w, double& t, double& r);

void make_T_R_C(
  Point& P1, Point& P2,
  Point* T, Point* R, Point* C, double w,
  const PolylineOpt& opts,
  double* rr, double* tt, double* dist, bool seg_mode = false
);

void segment_late(
  std::vector<PolylineVertex>& vertices,
  const Point* P,
  const Color* C,
  Polyline* SL,
  Point cap1, Point cap2
);

void segment(
  std::vector<PolylineVertex>& vertices,
  Anchor& SA,
  const PolylineOpt& opts,
  bool cap_first, bool cap_last,
  PolylineCap last_cap_type = PolylineCap::unset
);

void poly_point_inter(
  const std::vector<Point>& points,
  const std::vector<Color>& colors,
  const std::vector<double>& weights,
  const PolylineOptInternal& opts_internal,
  Point& p, Color& c, double& w, int at, double t
);

void polyline_exact(
  std::vector<PolylineVertex>& vertices,
  const std::vector<Point>& points,
  const std::vector<Color>& colors,
  const std::vector<double>& weights,
  const PolylineOpt& opts,
  const PolylineOptInternal& opts_internal
);

inline void determine_t_r(double w, double& t, double& r) {
  const double f = w - static_cast<int>(w);

  if (w >= 0.0 && w < 1.0) {
    t = 0.05;
    r = 0.768; //r=0.48+0.32*f;
  } else if (w >= 1.0 && w < 2.0) {
    t = 0.05 + f * 0.33;
    r = 0.768 + 0.312 * f;
  } else if (w >= 2.0 && w < 3.0) {
    t = 0.38 + f * 0.58;
    r = 1.08;
  } else if (w >= 3.0 && w < 4.0) {
    t = 0.96 + f * 0.48;
    r = 1.08;
  } else if (w >= 4.0 && w < 5.0) {
    t = 1.44 + f * 0.46;
    r = 1.08;
  } else if (w >= 5.0 && w < 6.0) {
    t = 1.9 + f * 0.6;
    r = 1.08;
  } else if (w >= 6.0) {
    const double ff = w - 6.0;
    t = 2.5 + ff * 0.50;
    r = 1.08;
  }
}

inline void make_T_R_C(
  Point& P1, Point& P2,
  Point* T, Point* R, Point* C, double w,
  const PolylineOpt& opts,
  double* rr, double* tt, double* dist, bool seg_mode
) {
  double t = 1.0, r = 0.0;
  Point DP = P2 - P1;
  determine_t_r(w, t, r);

  if (opts.feather && !opts.no_feather_at_core && opts.feathering != 1.0) {
    r *= opts.feathering;
  } else if (seg_mode) {
    // TODO: handle correctly for hori/vert segments in a polyline
    if (negligible(DP.x) && P1.x == static_cast<int>(P1.x)) {
      if (w > 0.0 && w <= 1.0) {
        t = 0.5;
        r = 0.0;
        P2.x = P1.x = static_cast<int>(P1.x) + 0.5;
      }
    } else if (negligible(DP.y) && P1.y == static_cast<int>(P1.y)) {
      if (w > 0.0 && w <= 1.0) {
        t = 0.5;
        r = 0.0;
        P2.y = P1.y = static_cast<int>(P1.y) + 0.5;
      }
    }
  }

  if (tt) { *tt = t; }
  if (rr) { *rr = r; }

  const double len = normalize(DP);
  if (dist) { *dist = len; }
  if (C) { *C = DP; }
  perpen(DP);

  if (T) { *T = DP * t; }
  if (R) { *R = DP * r; }
}

inline void segment_late(
  std::vector<PolylineVertex>& vertices,
  const Point* P,
  const Color* C,
  Polyline* SL,
  Point cap1, Point cap2
) {
  Point P_0 = P[0];
  Point P_1 = P[1];
  if (SL[0].djoint == PolylineCap::butt || SL[0].djoint == PolylineCap::square) {
    P_0 -= cap1;
  }
  if (SL[1].djoint == PolylineCap::butt || SL[1].djoint == PolylineCap::square) {
    P_1 -= cap2;
  }

  Point P1, P2, P3, P4; // Core
  Point P1c, P2c, P3c, P4c; // Cap
  Point P1r, P2r, P3r, P4r; // Fade

  P1 = P_0 + SL[0].T;
  P1r = P1 + SL[0].R;
  P1c = P1r + cap1;

  P2 = P_0 - SL[0].T;
  P2r = P2 - SL[0].R;
  P2c = P2r + cap1;

  P3 = P_1 + SL[1].T;
  P3r = P3 + SL[1].R;
  P3c = P3r + cap2;

  P4 = P_1 - SL[1].T;
  P4r = P4 - SL[1].R;
  P4c = P4r + cap2;

  push_quad(vertices, P1, P2, P3, P4, C[0], C[0], C[1], C[1]);
  push_quadf(vertices, P1, P1r, P3, P3r, C[0], C[0], C[1], C[1], false, true, false, true);
  push_quadf(vertices, P2, P2r, P4, P4r, C[0], C[0], C[1], C[1], false, true, false, true);
}

inline void segment(
  std::vector<PolylineVertex>& vertices,
  Anchor& SA,
  const PolylineOpt& opts,
  bool cap_first, bool cap_last,
  PolylineCap last_cap_type
) {
  if (!SA.P || !SA.C || !SA.W) { return; }

  Point P[2];
  P[0] = SA.P[0];
  P[1] = SA.P[1];

  Color C[2];
  C[0] = SA.C[0];
  C[1] = SA.C[1];

  Point T1, T2;
  Point R1, R2;
  Point bR;
  double t, r;
  bool varying_weight = !(SA.W[0] == SA.W[1]);

  Point cap_start, cap_end;
  Polyline SL[2];

  for (int i = 0; i < 2; ++i) {
    if (SA.W[i] >= 0.0 && SA.W[i] < 1.0) {
      const double f = SA.W[i] - static_cast<int>(SA.W[i]);
      C[i].a *= f;
    }
  }

  make_T_R_C(P[0], P[1], &T2, &R2, &bR, SA.W[0], opts, &r, &t, nullptr, true);
  if (cap_first) {
    if (opts.cap == PolylineCap::square) {
      P[0] -= bR * (t + r);
    }

    cap_start = bR;
    opposite(cap_start);
    if (opts.feather && !opts.no_feather_at_cap) {
      cap_start *= opts.feathering;
    }
  }

  SL[0].djoint = opts.cap;
  SL[0].t = t;
  SL[0].r = r;
  SL[0].T = T2;
  SL[0].R = R2;
  SL[0].bR = bR * 0.01;
  SL[0].degenT = false;
  SL[0].degenR = false;

  if (varying_weight)
    make_T_R_C(P[0], P[1], &T2, &R2, &bR, SA.W[1], opts, &r, &t, nullptr, true);
  last_cap_type = last_cap_type == PolylineCap::unset ? opts.cap : last_cap_type;

  if (cap_last) {
    if (last_cap_type == PolylineCap::square) {
      P[1] += bR * (t + r);
    }

    cap_end = bR;
    if (opts.feather && !opts.no_feather_at_cap)
      cap_end *= opts.feathering;
  }

  SL[1].djoint = last_cap_type;
  SL[1].t = t;
  SL[1].r = r;
  SL[1].T = T2;
  SL[1].R = R2;
  SL[1].bR = bR * 0.01;
  SL[1].degenT = false;
  SL[1].degenR = false;

  segment_late(vertices, P, C, SL, cap_start, cap_end);
}

inline void poly_point_inter(
  const std::vector<Point>& points,
  const std::vector<Color>& colors,
  const std::vector<double>& weights,
  const PolylineOptInternal& opts_internal,
  Point& p, Color& c, double& w, int at, double t
) {
  if (t == 0.0) {
    p = points[at];
    c = color_at(colors, at, opts_internal);
    w = weight_at(weights, at, opts_internal);
  } else if (t == 1.0) {
    p = points[at + 1];
    c = color_at(colors, at + 1, opts_internal);
    w = weight_at(weights, at + 1, opts_internal);
  } else {
    p = (points[at] + points[at + 1]) * t;
    c = color_between(color_at(colors, at, opts_internal), color_at(colors, at + 1, opts_internal), t);
    w = (weight_at(weights, at, opts_internal) + weight_at(weights, at + 1, opts_internal)) * t;
  }
}

inline void polyline_exact(
  std::vector<PolylineVertex>& vertices,
  const std::vector<Point>& points,
  const std::vector<Color>& colors,
  const std::vector<double>& weights,
  const PolylineOpt& opts,
  const PolylineOptInternal& opts_internal
) {
  Point mid_l, mid_n; // Last and next midpoint
  Color c_l, c_n; // Last and next color
  double w_l, w_n; // Last and next weight
  poly_point_inter(points, colors, weights, opts_internal, mid_l, c_l, w_l, 0,
                   is_flag_set(opts_internal, PolylineOptInternal::join_first) ? 0.5 : 0);

  Anchor SA{};
  if (points.size() == 2) {
    SA.P[0] = points[0];
    SA.P[1] = points[1];
    SA.C[0] = color_at(colors, 0, opts_internal);
    SA.C[1] = color_at(colors, 1, opts_internal);
    SA.W[0] = weight_at(weights, 0, opts_internal);
    SA.W[1] = weight_at(weights, 1, opts_internal);
    segment(vertices, SA, opts, !is_flag_set(opts_internal, PolylineOptInternal::no_cap_first),
            !is_flag_set(opts_internal, PolylineOptInternal::no_cap_last));
  }
}
}

std::vector<PolylineVertex> polyline(
  const std::vector<Point>& points,
  const std::vector<Color>& colors,
  const std::vector<double>& weights,
  const PolylineOpt& opts,
  const PolylineOptInternal& opts_internal = PolylineOptInternal::none
);

inline std::vector<PolylineVertex> polyline(
  const std::vector<Point>& points,
  const std::vector<Color>& colors,
  const std::vector<double>& weights,
  const PolylineOpt& opts,
  const PolylineOptInternal& opts_internal
) {
  auto opts_copy = opts;
  auto opts_internal_copy = opts_internal;

  if (opts_copy.cap == PolylineCap::first ||
      opts_copy.cap == PolylineCap::last ||
      opts_copy.cap == PolylineCap::none
  ) {
    if (opts_copy.cap == PolylineCap::first || opts_copy.cap == PolylineCap::none) {
      opts_internal_copy |= PolylineOptInternal::no_cap_last;
    }
    if (opts_copy.cap == PolylineCap::last || opts_copy.cap == PolylineCap::none) {
      opts_internal_copy |= PolylineOptInternal::no_cap_first;
    }

    switch (opts_copy.cap) {
      case PolylineCap::first:
        opts_copy.cap = PolylineCap::none;
        break;
      case PolylineCap::last:
        opts_copy.cap = PolylineCap::first;
        break;
      case PolylineCap::none:
        opts_copy.cap = PolylineCap::last;
        break;
      default:
        break; // do nothing
    }
  }

  std::vector<PolylineVertex> vertices;

  if (is_flag_set(opts_internal_copy, PolylineOptInternal::const_weight) &&
      weights[0] < CRITICAL_SEGMENT_APPROX_WEIGHT
  ) {
    internal::polyline_exact(vertices, points, colors, weights, opts_copy, opts_internal_copy);
    return vertices;
  }

  return vertices;
}
}

#endif//EXAMPLE_INDEV_HPP
