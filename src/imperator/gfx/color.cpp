#include "imperator/gfx/color.hpp"

#include <array>
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace imp {
namespace internal {
template<typename T>
T clamp(T x, T min, T max) {
  return (x < min) ? min : (x > max) ? max : x;
}

std::array<double, 4> rgb_to_hvcl(int r, int g, int b) {
  double rd = r / 255.0;
  double gd = g / 255.0;
  double bd = b / 255.0;

  double rgb_max = std::max({rd, gd, bd});
  double rgb_min = std::min({rd, gd, bd});

  double v = rgb_max;
  double c = rgb_max - rgb_min;
  double l = (rgb_max + rgb_min) / 2.0;

  double h;
  if (c == 0.0)
    h = 0.0;
  else if (v == rd)
    h = 60.0 * ((gd - bd) / c);
  else if (v == gd)
    h = 60.0 * (2.0 + (bd - rd) / c);
  else
    h = 60.0 * (4.0 + (rd - gd) / c);

  return {h, v, c, l};
}

HSV rgba_to_hsv(int r, int g, int b, int a) {
  auto hvcl = rgb_to_hvcl(r, g, b);
  double s = (hvcl[1] == 0.0) ? 0.0 : hvcl[2] / hvcl[1];

  return hsva(hvcl[0], s, hvcl[1], a);
}

HSL rgba_to_hsl(int r, int g, int b, int a) {
  auto hvcl = rgb_to_hvcl(r, g, b);
  double s = (hvcl[3] == 0.0 || hvcl[3] == 1.0)
               ? 0.0
               : (hvcl[1] - hvcl[3]) / std::min(hvcl[3], 1.0 - hvcl[3]);

  return hsla(hvcl[0], s, hvcl[3], a);
}
} // namespace internal

std::array<int, 3>& get_css_color(const std::string& name) {
  static std::unordered_map<std::string, std::array<int, 3>> mapping = {
    {"aliceblue", {240, 248, 255}},
    {"antiquewhite", {250, 235, 215}},
    {"aqua", {0, 255, 255}},
    {"aquamarine", {127, 255, 212}},
    {"azure", {240, 255, 255}},
    {"beige", {245, 245, 220}},
    {"bisque", {255, 228, 196}},
    {"black", {0, 0, 0}},
    {"blanchedalmond", {255, 235, 205}},
    {"blue", {0, 0, 255}},
    {"blueviolet", {138, 43, 226}},
    {"brown", {165, 42, 42}},
    {"burlywood", {222, 184, 135}},
    {"cadetblue", {95, 158, 160}},
    {"chartreuse", {127, 255, 0}},
    {"chocolate", {210, 105, 30}},
    {"coral", {255, 127, 80}},
    {"cornflowerblue", {100, 149, 237}},
    {"cornsilk", {255, 248, 220}},
    {"crimson", {220, 20, 60}},
    {"cyan", {0, 255, 255}},
    {"darkblue", {0, 0, 139}},
    {"darkcyan", {0, 139, 139}},
    {"darkgoldenrod", {184, 134, 11}},
    {"darkgray", {169, 169, 169}},
    {"darkgreen", {0, 100, 0}},
    {"darkgrey", {169, 169, 169}},
    {"darkkhaki", {189, 183, 107}},
    {"darkmagenta", {139, 0, 139}},
    {"darkolivegreen", {85, 107, 47}},
    {"darkorange", {255, 140, 0}},
    {"darkorchid", {153, 50, 204}},
    {"darkred", {139, 0, 0}},
    {"darksalmon", {233, 150, 122}},
    {"darkseagreen", {143, 188, 143}},
    {"darkslateblue", {72, 61, 139}},
    {"darkslategray", {47, 79, 79}},
    {"darkslategrey", {47, 79, 79}},
    {"darkturquoise", {0, 206, 209}},
    {"darkviolet", {148, 0, 211}},
    {"deeppink", {255, 20, 147}},
    {"deepskyblue", {0, 191, 255}},
    {"dimgray", {105, 105, 105}},
    {"dimgrey", {105, 105, 105}},
    {"dodgerblue", {30, 144, 255}},
    {"firebrick", {178, 34, 34}},
    {"floralwhite", {255, 250, 240}},
    {"forestgreen", {34, 139, 34}},
    {"fuchsia", {255, 0, 255}},
    {"gainsboro", {220, 220, 220}},
    {"ghostwhite", {248, 248, 255}},
    {"gold", {255, 215, 0}},
    {"goldenrod", {218, 165, 32}},
    {"gray", {128, 128, 128}},
    {"green", {0, 128, 0}},
    {"greenyellow", {173, 255, 47}},
    {"grey", {128, 128, 128}},
    {"honeydew", {240, 255, 240}},
    {"hotpink", {255, 105, 180}},
    {"indianred", {205, 92, 92}},
    {"indigo", {75, 0, 130}},
    {"ivory", {255, 255, 240}},
    {"khaki", {240, 230, 140}},
    {"lavender", {230, 230, 250}},
    {"lavenderblush", {255, 240, 245}},
    {"lawngreen", {124, 252, 0}},
    {"lemonchiffon", {255, 250, 205}},
    {"lightblue", {173, 216, 230}},
    {"lightcoral", {240, 128, 128}},
    {"lightcyan", {224, 255, 255}},
    {"lightgoldenrodyellow", {250, 250, 210}},
    {"lightgray", {211, 211, 211}},
    {"lightgreen", {144, 238, 144}},
    {"lightgrey", {211, 211, 211}},
    {"lightpink", {255, 182, 193}},
    {"lightsalmon", {255, 160, 122}},
    {"lightseagreen", {32, 178, 170}},
    {"lightskyblue", {135, 206, 250}},
    {"lightslategray", {119, 136, 153}},
    {"lightslategrey", {119, 136, 153}},
    {"lightsteelblue", {176, 196, 222}},
    {"lightyellow", {255, 255, 224}},
    {"lime", {0, 255, 0}},
    {"limegreen", {50, 205, 50}},
    {"linen", {250, 240, 230}},
    {"magenta", {255, 0, 255}},
    {"maroon", {128, 0, 0}},
    {"mediumaquamarine", {102, 205, 170}},
    {"mediumblue", {0, 0, 205}},
    {"mediumorchid", {186, 85, 211}},
    {"mediumpurple", {147, 112, 219}},
    {"mediumseagreen", {60, 179, 113}},
    {"mediumslateblue", {123, 104, 238}},
    {"mediumspringgreen", {0, 250, 154}},
    {"mediumturquoise", {72, 209, 204}},
    {"mediumvioletred", {199, 21, 133}},
    {"midnightblue", {25, 25, 112}},
    {"mintcream", {245, 255, 250}},
    {"mistyrose", {255, 228, 225}},
    {"moccasin", {255, 228, 181}},
    {"navajowhite", {255, 222, 173}},
    {"navy", {0, 0, 128}},
    {"oldlace", {253, 245, 230}},
    {"olive", {128, 128, 0}},
    {"olivedrab", {107, 142, 35}},
    {"orange", {255, 165, 0}},
    {"orangered", {255, 69, 0}},
    {"orchid", {218, 112, 214}},
    {"palegoldenrod", {238, 232, 170}},
    {"palegreen", {152, 251, 152}},
    {"paleturquoise", {175, 238, 238}},
    {"palevioletred", {219, 112, 147}},
    {"papayawhip", {255, 239, 213}},
    {"peachpuff", {255, 218, 185}},
    {"peru", {205, 133, 63}},
    {"pink", {255, 192, 203}},
    {"plum", {221, 160, 221}},
    {"powderblue", {176, 224, 230}},
    {"purple", {128, 0, 128}},
    {"rebeccapurple", {102, 51, 153}},
    {"red", {255, 0, 0}},
    {"rosybrown", {188, 143, 143}},
    {"royalblue", {65, 105, 225}},
    {"saddlebrown", {139, 69, 19}},
    {"salmon", {250, 128, 114}},
    {"sandybrown", {244, 164, 96}},
    {"seagreen", {46, 139, 87}},
    {"seashell", {255, 245, 238}},
    {"sienna", {160, 82, 45}},
    {"silver", {192, 192, 192}},
    {"skyblue", {135, 206, 235}},
    {"slateblue", {106, 90, 205}},
    {"slategray", {112, 128, 144}},
    {"slategrey", {112, 128, 144}},
    {"snow", {255, 250, 250}},
    {"springgreen", {0, 255, 127}},
    {"steelblue", {70, 130, 180}},
    {"tan", {210, 180, 140}},
    {"teal", {0, 128, 128}},
    {"thistle", {216, 191, 216}},
    {"tomato", {255, 99, 71}},
    {"turquoise", {64, 224, 208}},
    {"violet", {238, 130, 238}},
    {"wheat", {245, 222, 179}},
    {"white", {255, 255, 255}},
    {"whitesmoke", {245, 245, 245}},
    {"yellow", {255, 255, 0}},
    {"yellowgreen", {154, 205, 50}}
  };

  return mapping[name];
}

/*******
 * RGB *
 *******/

RGB::RGB(int r, int g, int b, int a)
  : r(internal::clamp(r, 0, 255)),
    g(internal::clamp(g, 0, 255)),
    b(internal::clamp(b, 0, 255)),
    a(internal::clamp(a, 0, 255)) {}

RGB rgb(int r, int g, int b) {
  return {r, g, b, 255};
}

RGB rgb(std::uint32_t hex) {
  return {
    static_cast<int>((hex >> 16) & 0xff),
    static_cast<int>((hex >> 8) & 0xff),
    static_cast<int>(hex & 0xff),
    255
  };
}

RGB rgb(const std::string& css_color) {
  auto c = get_css_color(css_color);
  return {c[0], c[1], c[2], 255};
}

RGB rgba(int r, int g, int b, int a) {
  return {r, g, b, a};
}

RGB rgba(std::uint64_t hex) {
  return {
    static_cast<int>((hex >> 24) & 0xff),
    static_cast<int>((hex >> 16) & 0xff),
    static_cast<int>((hex >> 8) & 0xff),
    static_cast<int>(hex & 0xff),
  };
}

RGB rgba(const std::string& css_color, int a) {
  auto c = get_css_color(css_color);
  return {c[0], c[1], c[2], a};
}

RGB rgb_f(float r, float g, float b) {
  return {
    static_cast<int>(r * 255),
    static_cast<int>(g * 255),
    static_cast<int>(b * 255),
    255
  };
}

RGB rgba_f(float r, float g, float b, float a) {
  return {
    static_cast<int>(r * 255),
    static_cast<int>(g * 255),
    static_cast<int>(b * 255),
    static_cast<int>(a * 255)
  };
}

RGB RGB::get_inverse() const {
  return {255 - r, 255 - g, 255 - b, a};
}

void RGB::invert() {
  r = 255 - r;
  g = 255 - g;
  b = 255 - b;
}

glm::vec3 RGB::vec3() const {
  return {
    static_cast<float>(r) / 255.0f,
    static_cast<float>(g) / 255.0f,
    static_cast<float>(b) / 255.0f
  };
}

glm::vec4 RGB::vec4() const {
  return {
    static_cast<float>(r) / 255.0f,
    static_cast<float>(g) / 255.0f,
    static_cast<float>(b) / 255.0f,
    static_cast<float>(a) / 255.0f
  };
}

HSL RGB::to_hsl() const {
  return internal::rgba_to_hsl(r, g, b, a);
}

HSV RGB::to_hsv() const {
  return internal::rgba_to_hsv(r, g, b, a);
}

// XYZ RGB::to_xyz() const {
//   double rd = static_cast<double>(r) / 255.0;
//   double gd = static_cast<double>(g) / 255.0;
//   double bd = static_cast<double>(b) / 255.0;
//
//   rd = (rd > 0.04045)
//          ? std::pow(((rd + 0.055) / 1.055), 2.4)
//          : rd / 12.92;
//   gd = (gd > 0.04045)
//          ? std::pow(((gd + 0.055) / 1.055), 2.4)
//          : gd / 12.92;
//   bd = (bd > 0.04045)
//          ? std::pow(((bd + 0.055) / 1.055), 2.4)
//          : bd / 12.92;
//
//   rd *= 100.0;
//   gd *= 100.0;
//   bd *= 100.0;
//
//   return xyza(
//     rd * 0.4124564 + gd * 0.3575761 + bd * 0.1804375,
//     rd * 0.2126729 + gd * 0.7151522 + bd * 0.0721750,
//     rd * 0.0193339 + gd * 0.1191920 + bd * 0.9503041,
//     a
//   );
// }
//
// CIELab RGB::to_lab() const {
//   return to_xyz().to_lab();
// }
//
// CIELCh RGB::to_lch() const {
//   return to_xyz().to_lch();
// }

/*******
 * HSV *
 *******/

HSV::HSV(double h, double s, double v, int a)
  : h(internal::clamp(h, 0.0, 360.0)),
    s(internal::clamp(s, 0.0, 1.0)),
    v(internal::clamp(v, 0.0, 1.0)),
    a(internal::clamp(a, 0, 255)) {}

HSV hsv(double h, double s, double v) {
  return {h, s, v, 255};
}

HSV hsv(const std::string& css_color) {
  auto c = get_css_color(css_color);
  return internal::rgba_to_hsv(c[0], c[1], c[2], 255);
}

HSV hsva(double h, double s, double v, int a) {
  return {h, s, v, a};
}

HSV hsva(const std::string& css_color, int a) {
  auto c = get_css_color(css_color);
  return internal::rgba_to_hsv(c[0], c[1], c[2], a);
}

HSV HSV::get_inverse() const {
  return {std::fmod(h + 180.0, 360.0), s, v, a};
}

void HSV::invert() {
  h = std::fmod(h + 180.0, 360.0);
}

RGB HSV::to_rgb() const {
  double k5 = std::fmod((5.0 + h / 60.0), 6.0);
  double k3 = std::fmod((3.0 + h / 60.0), 6.0);
  double k1 = std::fmod((1.0 + h / 60.0), 6.0);

  double rd = v - v * s * std::max(0.0, std::min({k5, 4.0 - k5, 1.0}));
  double gd = v - v * s * std::max(0.0, std::min({k3, 4.0 - k3, 1.0}));
  double bd = v - v * s * std::max(0.0, std::min({k1, 4.0 - k1, 1.0}));

  return rgba(
    static_cast<int>(255.0 * rd),
    static_cast<int>(255.0 * gd),
    static_cast<int>(255.0 * bd),
    a
  );
}

HSL HSV::to_hsl() const {
  double l = v * (1 - (s / 2.0));
  double s = (l == 0.0 || l == 1.0) ? 0.0 : (v - l) / std::min(l, 1 - l);
  return hsla(h, s, l, a);
}

// XYZ HSV::to_xyz() const {
//   return to_rgb().to_xyz();
// }
//
// CIELab HSV::to_lab() const {
//   return to_rgb().to_lab();
// }
//
// CIELCh HSV::to_lch() const {
//   return to_rgb().to_lch();
// }

/*******
 * HSL *
 *******/

HSL::HSL(double h, double s, double l, int a)
  : h(internal::clamp(h, 0.0, 360.0)),
    s(internal::clamp(s, 0.0, 1.0)),
    l(internal::clamp(l, 0.0, 1.0)),
    a(internal::clamp(a, 0, 255)) {}

HSL hsl(double h, double s, double l) {
  return {h, s, l, 255};
}

HSL hsl(const std::string& css_color) {
  auto c = get_css_color(css_color);
  return internal::rgba_to_hsl(c[0], c[1], c[2], 255);
}

HSL hsla(double h, double s, double l, int a) {
  return {h, s, l, a};
}

HSL hsla(const std::string& css_color, int a) {
  auto c = get_css_color(css_color);
  return internal::rgba_to_hsl(c[0], c[1], c[2], a);
}

HSL HSL::get_inverse() const {
  return {std::fmod(h + 180.0, 360.0), s, l, a};
}

void HSL::invert() {
  h = std::fmod(h + 180.0, 360.0);
}

RGB HSL::to_rgb() const {
  double k0 = std::fmod((h / 30.0), 12.0);
  double k4 = std::fmod((4.0 + h / 30.0), 12.0);
  double k8 = std::fmod((8.0 + h / 30.0), 12.0);
  double alpha = s * std::min(l, 1 - l);

  double rd = l - alpha * std::max(-1.0, std::min({k0 - 3.0, 9.0 - k0, 1.0}));
  double gd = l - alpha * std::max(-1.0, std::min({k4 - 3.0, 9.0 - k4, 1.0}));
  double bd = l - alpha * std::max(-1.0, std::min({k8 - 3.0, 9.0 - k8, 1.0}));

  return rgba(
    static_cast<int>(255.0 * rd),
    static_cast<int>(255.0 * gd),
    static_cast<int>(255.0 * bd),
    a
  );
}

HSV HSL::to_hsv() const {
  double v = l + s * std::min(l, 1 - l);
  double s = (v == 0.0) ? 0.0 : 2 * (1.0 - (l / v));
  return hsva(h, s, v, a);
}

// XYZ HSL::to_xyz() const {
//   return to_rgb().to_xyz();
// }
//
// CIELab HSL::to_lab() const {
//   return to_rgb().to_lab();
// }
//
// CIELCh HSL::to_lch() const {
//   return to_rgb().to_lch();
// }

// /*******
//  * XYZ *
//  *******/
//
// XYZ::XYZ(double x, double y, double z, int a)
//   : x(internal::clamp(x, 0.0, x)), // FIXME: does this have an upper limit?
//     y(internal::clamp(y, 0.0, y)), // FIXME: does this have an upper limit?
//     z(internal::clamp(z, 0.0, z)), // FIXME: does this have an upper limit?
//     a(internal::clamp(a, 0, 255)) {}
//
// XYZ xyz(double x, double y, double z) {
//   return {x, y, z, 255};
// }
//
// XYZ xyz(const std::string& css_color) {
//   auto c = get_css_color(css_color);
//   return rgba(c[0], c[1], c[2], 255).to_xyz();
// }
//
// XYZ xyza(double x, double y, double z, int a) {
//   return {x, y, z, a};
// }
//
// XYZ xyza(const std::string& css_color, int a) {
//   auto c = get_css_color(css_color);
//   return rgba(c[0], c[1], c[2], a).to_xyz();
// }
//
// RGB XYZ::to_rgb() const {
//   double lx = x / 100.0;
//   double ly = y / 100.0;
//   double lz = z / 100.0;
//
//   double rd = lx * 3.2404542 + ly * -1.5371385 + lz * -0.4985314;
//   double gd = lx * -0.9692660 + ly * 1.8760108 + lz * 0.0415560;
//   double bd = lx * 0.0556434 + ly * -0.2040259 + lz * 1.0572252;
//
//   rd = (rd > 0.0031308)
//          ? 1.055 * (std::pow(rd, 1.0 / 2.4)) - 0.055
//          : 12.92 * rd;
//   gd = (gd > 0.0031308)
//          ? 1.055 * (std::pow(gd, 1.0 / 2.4)) - 0.055
//          : 12.92 * gd;
//   bd = (bd > 0.0031308)
//          ? 1.055 * (std::pow(bd, 1.0 / 2.4)) - 0.055
//          : 12.92 * bd;
//
//   // Using std::round because 254.9999999
//   // should probably be 255 and not 254
//   return rgba(
//     static_cast<int>(std::round(rd * 255.0)),
//     static_cast<int>(std::round(gd * 255.0)),
//     static_cast<int>(std::round(bd * 255.0)),
//     a
//   );
// }
//
// HSV XYZ::to_hsv() const {
//   return to_rgb().to_hsv();
// }
//
// HSL XYZ::to_hsl() const {
//   return to_rgb().to_hsl();
// }
//
// CIELab XYZ::to_lab() const {
//   // TODO: support other reference illuminants
//   // D65/2° illuminant
//   static double ref_x = 95.047;
//   static double ref_y = 100.0;
//   static double ref_z = 108.883;
//
//   double lx = x / ref_x;
//   double ly = y / ref_y;
//   double lz = z / ref_z;
//
//   lx = (lx > 0.008856)
//          ? std::pow(lx, 1.0 / 3.0)
//          : (7.787 * lx) + (16.0 / 116.0);
//   ly = (ly > 0.008856)
//          ? std::pow(ly, 1.0 / 3.0)
//          : (7.787 * ly) + (16.0 / 116.0);
//   lz = (lz > 0.008856)
//          ? std::pow(lz, 1.0 / 3.0)
//          : (7.787 * lz) + (16.0 / 116.0);
//
//   return laba(
//     (116.0 * ly) - 16.0,
//     500.0 * (lx - ly),
//     200.0 * (ly - lz),
//     a
//   );
// }
//
// CIELCh XYZ::to_lch() const {
//   return to_lab().to_lch();
// }

// /**********
//  * CIELab *
//  **********/
//
// CIELab::CIELab(double l, double a, double b, int alpha)
//   : l(internal::clamp(l, 0.0, 100.0)),
//     a(internal::clamp(a, -128.0, 127.0)),
//     b(internal::clamp(b, -128.0, 127.0)),
//     alpha(internal::clamp(alpha, 0, 255)) {}
//
// CIELab lab(double l, double a, double b) {
//   return {l, a, b, 255};
// }
//
// CIELab lab(const std::string& css_color) {
//   auto c = get_css_color(css_color);
//   return rgba(c[0], c[1], c[2], 255).to_lab();
// }
//
// CIELab laba(double l, double a, double b, int alpha) {
//   return {l, a, b, alpha};
// }
//
// CIELab laba(const std::string& css_color, int alpha) {
//   auto c = get_css_color(css_color);
//   return rgba(c[0], c[1], c[2], alpha).to_lab();
// }
//
// RGB CIELab::to_rgb() const {
//   return to_xyz().to_rgb();
// }
//
// HSV CIELab::to_hsv() const {
//   return to_rgb().to_hsv();
// }
//
// HSL CIELab::to_hsl() const {
//   return to_rgb().to_hsl();
// }
//
// XYZ CIELab::to_xyz() const {
//   // TODO: support other reference illuminants
//   // D65/2° illuminant
//   static double ref_x = 95.047;
//   static double ref_y = 100.0;
//   static double ref_z = 108.883;
//
//   double y = (l + 16.0) / 116.0;
//   double x = a / 500.0 + y;
//   double z = y - b / 200.0;
//
//   double y3 = y * y * y;
//   y = (y3 > 0.008856) ? y3 : (y - 16.0 / 116.0) / 7.787;
//   double x3 = x * x * x;
//   x = (x3 > 0.008856) ? x3 : (x - 16.0 / 116.0) / 7.787;
//   double z3 = z * z * z;
//   z = (z3 > 0.008856) ? z3 : (z - 16.0 / 116.0) / 7.787;
//
//   return xyza(
//     x * ref_x,
//     y * ref_y,
//     z * ref_z,
//     alpha
//   );
// }
//
// CIELCh CIELab::to_lch() const {
//   double h = std::atan2(b, a);
//   h = (h > 0.0)
//         ? (h / std::numbers::pi) * 180.0
//         : 360.0 - (std::abs(h) / std::numbers::pi) * 180.0;
//
//   return lcha(
//     l,
//     std::sqrt(a * a + b * b),
//     h,
//     alpha
//   );
// }

// /**********
//  * CIELCh *
//  **********/
//
// CIELCh::CIELCh(double l, double c, double h, int a)
//   : l(internal::clamp(l, 0.0, 100.0)),
//     c(internal::clamp(c, 0.0, c)), // FIXME: does this have an upper limit?
//     h(internal::clamp(h, 0.0, 360.0)),
//     a(internal::clamp(a, 0, 255)) {}
//
// CIELCh lch(double l, double c, double h) {
//   return {l, c, h, 255};
// }
//
// CIELCh lch(const std::string& css_color) {
//   auto c = get_css_color(css_color);
//   return rgba(c[0], c[1], c[2], 255).to_lch();
// }
//
// CIELCh lcha(double l, double c, double h, int a) {
//   return {l, c, h, a};
// }
//
// CIELCh lcha(const std::string& css_color, int a) {
//   auto c = get_css_color(css_color);
//   return rgba(c[0], c[1], c[2], a).to_lch();
// }
//
// RGB CIELCh::to_rgb() const {
//   return to_lab().to_rgb();
// }
//
// HSV CIELCh::to_hsv() const {
//   return to_lab().to_hsv();
// }
//
// HSL CIELCh::to_hsl() const {
//   return to_lab().to_hsl();
// }
//
// XYZ CIELCh::to_xyz() const {
//   return to_lab().to_xyz();
// }
//
// CIELab CIELCh::to_lab() const {
//   return laba(
//     l,
//     std::cos(h * (std::numbers::pi / 180.0)) * c,
//     std::sin(h * (std::numbers::pi / 180.0)) * c,
//     a
//   );
// }
} // namespace imp
