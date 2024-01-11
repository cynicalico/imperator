#ifndef IMP_GFX_COLOR_HPP
#define IMP_GFX_COLOR_HPP

// FIXME: This is somehow getting a macro from wingdi.h? I think it's
//   coming from spdlog, but I'm not certain...should see if there's
//   a better way to deal with this
#undef RGB

#include "glm/glm.hpp"
#include "fmt/core.h"
#include <cstdint>
#include <string>

namespace imp {
class Color {
public:
  virtual ~Color() = default;
  virtual glm::vec4 gl_color() const = 0;
};

class RGB;
class HSL;
class HSV;
// class XYZ;
// class CIELab;
// class CIELCh;

/*******
 * RGB *
 *******/

RGB rgb(int r, int g, int b);
RGB rgb(std::uint32_t hex);
RGB rgb(const std::string& css_color);
RGB rgb_f(float r, float g, float b);

RGB rgba(int r, int g, int b, int a);
RGB rgba(std::uint64_t hex, bool argb = false);
RGB rgba(const std::string& css_color, int a);
RGB rgba_f(float r, float g, float b, float a);

class RGB : public Color {
public:
  int r{0}, g{0}, b{0}, a{255};

  RGB() = default;

  glm::vec4 gl_color() const override;

  friend RGB rgb(int r, int g, int b);
  friend RGB rgb(std::uint32_t hex);
  friend RGB rgb(const std::string& css_color);
  friend RGB rgb_f(float r, float g, float b);

  friend RGB rgba(int r, int g, int b, int a);
  friend RGB rgba(std::uint64_t hex, bool argb);
  friend RGB rgba(const std::string& css_color, int a);
  friend RGB rgba_f(float r, float g, float b, float a);

  RGB get_inverse() const;
  void invert();

  HSL to_hsl() const;
  HSV to_hsv() const;
  // XYZ to_xyz() const;
  // CIELab to_lab() const;
  // CIELCh to_lch() const;

private:
  RGB(int r, int g, int b, int a = 255);
};

/*******
 * HSV *
 *******/

HSV hsv(double h, double s, double v);
HSV hsv(const std::string& css_color);

HSV hsva(double h, double s, double v, int a);
HSV hsva(const std::string& css_color, int a);

class HSV : public Color {
public:
  double h{0.0}, s{0.0}, v{0.0};
  int a{255};

  HSV() = default;

  glm::vec4 gl_color() const override;

  friend HSV hsv(double h, double s, double v);
  friend HSV hsv(const std::string& css_color);

  friend HSV hsva(double h, double s, double v, int a);
  friend HSV hsva(const std::string& css_color, int a);

  HSV get_inverse() const;
  void invert();

  RGB to_rgb() const;
  HSL to_hsl() const;
  // XYZ to_xyz() const;
  // CIELab to_lab() const;
  // CIELCh to_lch() const;

private:
  HSV(double h, double s, double v, int a);
};

/*******
 * HSL *
 *******/

HSL hsl(double h, double s, double l);
HSL hsl(const std::string& css_color);

HSL hsla(double h, double s, double l, int a);
HSL hsla(const std::string& css_color, int a);

class HSL : public Color {
public:
  double h{0.0}, s{0.0}, l{0.0};
  int a{255};

  HSL() = default;

  glm::vec4 gl_color() const override;

  friend HSL hsl(double h, double s, double l);
  friend HSL hsl(const std::string& css_color);

  friend HSL hsla(double h, double s, double l, int a);
  friend HSL hsla(const std::string& css_color, int a);

  HSL get_inverse() const;
  void invert();

  RGB to_rgb() const;
  HSV to_hsv() const;
  // XYZ to_xyz() const;
  // CIELab to_lab() const;
  // CIELCh to_lch() const;

private:
  HSL(double h, double s, double l, int a);
};

// /*******
//  * XYZ *
//  *******/
//
// XYZ xyz(double x, double y, double z);
// XYZ xyz(const std::string &css_color);
//
// XYZ xyza(double x, double y, double z, int a);
// XYZ xyza(const std::string &css_color, int a);
//
// class XYZ {
// public:
//   double x{0.0}, y{0.0}, z{0.0};
//   int a{255};
//
//   XYZ() = default;
//
//   friend XYZ xyz(double x, double y, double z);
//   friend XYZ xyz(const std::string &css_color);
//
//   friend XYZ xyza(double x, double y, double z, int a);
//   friend XYZ xyza(const std::string &css_color, int a);
//
//   RGB to_rgb() const;
//   HSV to_hsv() const;
//   HSL to_hsl() const;
//   CIELab to_lab() const;
//   CIELCh to_lch() const;
//
// private:
//   XYZ(double x, double y, double z, int a);
// };
//
//
// /**********
//  * CIELab *
//  **********/
//
// CIELab lab(double l, double a, double b);
// CIELab lab(const std::string &css_color);
//
// CIELab laba(double l, double a, double b, int alpha);
// CIELab laba(const std::string &css_color, int alpha);
//
// class CIELab {
// public:
//   double l{0.0}, a{0.0}, b{0.0};
//   int alpha{255};
//
//   CIELab() = default;
//
//   friend CIELab lab(double l, double a, double b);
//   friend CIELab lab(const std::string &css_color);
//
//   friend CIELab laba(double l, double a, double b, int alpha);
//   friend CIELab laba(const std::string &css_color, int alpha);
//
//   RGB to_rgb() const;
//   HSV to_hsv() const;
//   HSL to_hsl() const;
//   XYZ to_xyz() const;
//   CIELCh to_lch() const;
//
// private:
//   CIELab(double l, double a, double b, int alpha);
// };
//
// /**********
//  * CIELCh *
//  **********/
//
// CIELCh lch(double l, double c, double h);
// CIELCh lch(const std::string &css_color);
//
// CIELCh lcha(double l, double c, double h, int a);
// CIELCh lcha(const std::string &css_color, int a);
//
// class CIELCh {
// public:
//   double l{0.0}, c{0.0}, h{0.0};
//   int a{255};
//
//   CIELCh() = default;
//
//   friend CIELCh lch(double l, double c, double h);
//   friend CIELCh lch(const std::string &css_color);
//
//   friend CIELCh lcha(double l, double c, double h, int a);
//   friend CIELCh lcha(const std::string &css_color, int a);
//
//   RGB to_rgb() const;
//   HSV to_hsv() const;
//   HSL to_hsl() const;
//   XYZ to_xyz() const;
//   CIELab to_lab() const;
//
// private:
//   CIELCh(double l, double c, double h, int a);
// };
} // namespace imp


template<>
struct fmt::formatter<imp::RGB> {
  constexpr auto parse(format_parse_context& ctx) -> format_parse_context::iterator { return ctx.begin(); }

  auto format(const imp::RGB& c, format_context& ctx) const -> format_context::iterator {
    return fmt::format_to(ctx.out(), "RGB({},{},{},{})", c.r, c.g, c.b, c.a);
  }
};

template<>
struct fmt::formatter<imp::HSV> : formatter<string_view> {
  constexpr auto parse(format_parse_context& ctx) -> format_parse_context::iterator { return ctx.begin(); }

  auto format(const imp::HSV& c, format_context& ctx) const -> format_context::iterator {
    return fmt::format_to(ctx.out(), "HSV({:.2f},{:.2f},{:.2f},{})", c.h, c.s, c.v, c.a);
  }
};

template<>
struct fmt::formatter<imp::HSL> : formatter<string_view> {
  constexpr auto parse(format_parse_context& ctx) -> format_parse_context::iterator { return ctx.begin(); }

  auto format(const imp::HSL& c, format_context& ctx) const -> format_context::iterator {
    return fmt::format_to(ctx.out(), "HSL({:.2f},{:.2f},{:.2f},{})", c.h, c.s, c.l, c.a);
  }
};

#endif//IMP_GFX_COLOR_HPP
