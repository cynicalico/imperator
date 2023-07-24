#include "baphy/util/noise/opensimplex.hpp"

#include "baphy/util/rnd.hpp"
#include <functional>

namespace baphy {

std::int64_t opensimplex::seed_{rnd::get<std::int64_t>()};

void opensimplex::set_seed(std::int64_t seed) {
  seed_ = seed;
}

std::int64_t opensimplex::get_seed() {
  return seed_;
}

void opensimplex::reseed() {
  seed_ = rnd::get<std::int64_t>();
}

////////////////////
// OCTAVE HELPERS //
////////////////////

///////////////
// STATELESS //
///////////////

using noise2d_sl_func = std::function<double(std::int64_t, double, double)>;
using noise3d_sl_func = std::function<double(std::int64_t, double, double, double)>;
using noise4d_sl_func = std::function<double(std::int64_t, double, double, double, double)>;

inline double octave2d_base_sl(std::int64_t seed, double x, double y, int octaves, double persistence, const noise2d_sl_func& func) {
  double total = 0.0, frequency = 1.0, amplitude = 1.0, max_value = 0.0;
  for (int i = 0; i < octaves; ++i) {
    total += func(seed, x * frequency, y * frequency) * amplitude;
    max_value += amplitude;
    amplitude *= persistence;
    frequency *= 2;
  }
  return total / max_value;
}

inline double octave3d_base_sl(std::int64_t seed, double x, double y, double z, int octaves, double persistence, const noise3d_sl_func& func) {
  double total = 0.0, frequency = 1.0, amplitude = 1.0, max_value = 0.0;
  for (int i = 0; i < octaves; ++i) {
    total += func(seed, x * frequency, y * frequency, z * frequency) * amplitude;
    max_value += amplitude;
    amplitude *= persistence;
    frequency *= 2;
  }
  return total / max_value;
}

inline double octave4d_base_sl(std::int64_t seed, double x, double y, double z, double w, int octaves, double persistence, const noise4d_sl_func& func) {
  double total = 0.0, frequency = 1.0, amplitude = 1.0, max_value = 0.0;
  for (int i = 0; i < octaves; ++i) {
    total += func(seed, x * frequency, y * frequency, z * frequency, w * frequency) * amplitude;
    max_value += amplitude;
    amplitude *= persistence;
    frequency *= 2;
  }
  return total / max_value;
}

////////////////
// PRE-SEEDED //
////////////////

using noise2d_func = std::function<double(double, double)>;
using noise3d_func = std::function<double(double, double, double)>;
using noise4d_func = std::function<double(double, double, double, double)>;

inline double octave2d_base(double x, double y, int octaves, double persistence, const noise2d_func& func) {
  double total = 0.0, frequency = 1.0, amplitude = 1.0, max_value = 0.0;
  for (int i = 0; i < octaves; ++i) {
    total += func(x * frequency, y * frequency) * amplitude;
    max_value += amplitude;
    amplitude *= persistence;
    frequency *= 2;
  }
  return total / max_value;
}

inline double octave3d_base(double x, double y, double z, int octaves, double persistence, const noise3d_func& func) {
  double total = 0.0, frequency = 1.0, amplitude = 1.0, max_value = 0.0;
  for (int i = 0; i < octaves; ++i) {
    total += func(x * frequency, y * frequency, z * frequency) * amplitude;
    max_value += amplitude;
    amplitude *= persistence;
    frequency *= 2;
  }
  return total / max_value;
}

inline double octave4d_base(double x, double y, double z, double w, int octaves, double persistence, const noise4d_func& func) {
  double total = 0.0, frequency = 1.0, amplitude = 1.0, max_value = 0.0;
  for (int i = 0; i < octaves; ++i) {
    total += func(x * frequency, y * frequency, z * frequency, w * frequency) * amplitude;
    max_value += amplitude;
    amplitude *= persistence;
    frequency *= 2;
  }
  return total / max_value;
}

////////////
// SMOOTH //
////////////

///////////////
// STATELESS //
///////////////

double opensimplex::noise2d_sl(std::int64_t seed, double x, double y) {
  double s = SKEW_2D_ * (x + y);
  double xs = x + s;
  double ys = y + s;

  return noise_unskewed_base_(seed, xs, ys);
}

double opensimplex::noise2d_improve_x_sl(std::int64_t seed, double x, double y) {
  double xx = x * ROOT2OVER2_;
  double yy = y * (ROOT2OVER2_ * (1 + 2 * SKEW_2D_));

  return noise_unskewed_base_(seed, yy + xx, yy - xx);
}

double opensimplex::noise3d_improve_xy_sl(std::int64_t seed, double x, double y, double z) {
  double xy = x + y;
  double s2 = xy * ROTATE3_ORTHOGONALIZER_;
  double zz = z * ROOT3OVER3_;
  double xr = x + s2 + zz;
  double yr = y + s2 + zz;
  double zr = xy * -ROOT3OVER3_ + zz;

  return noise_unrotated_base_(seed, xr, yr, zr);
}

double opensimplex::noise3d_improve_xz_sl(std::int64_t seed, double x, double y, double z) {
  double xz = x + z;
  double s2 = xz * -0.211324865405187;
  double yy = y * ROOT3OVER3_;
  double xr = x + s2 + yy;
  double zr = z + s2 + yy;
  double yr = xz * -ROOT3OVER3_ + yy;

  return noise_unrotated_base_(seed, xr, yr, zr);
}

double opensimplex::noise3d_fallback_sl(std::int64_t seed, double x, double y, double z) {
  double r = FALLBACK_ROTATE3_ * (x + y + z);
  double xr = r - x;
  double yr = r - y;
  double zr = r - z;

  return noise_unrotated_base_(seed, xr, yr, zr);
}

double opensimplex::noise4d_improve_xyz_improve_xy_sl(std::int64_t seed, double x, double y, double z, double w) {
  double xy = x + y;
  double s2 = xy * -0.21132486540518699998;
  double zz = z * 0.28867513459481294226;
  double ww = w * 1.118033988749894;
  double xr = x + (zz + ww + s2), yr = y + (zz + ww + s2);
  double zr = xy * -0.57735026918962599998 + (zz + ww);
  double wr = z * -0.866025403784439 + ww;

  return noise_unskewed_base_(seed, xr, yr, zr, wr);
}

double opensimplex::noise4d_improve_xyz_improve_xz_sl(std::int64_t seed, double x, double y, double z, double w) {
  double xz = x + z;
  double s2 = xz * -0.21132486540518699998;
  double yy = y * 0.28867513459481294226;
  double ww = w * 1.118033988749894;
  double xr = x + (yy + ww + s2), zr = z + (yy + ww + s2);
  double yr = xz * -0.57735026918962599998 + (yy + ww);
  double wr = y * -0.866025403784439 + ww;

  return noise_unskewed_base_(seed, xr, yr, zr, wr);
}

double opensimplex::noise4d_improve_xyz_sl(std::int64_t seed, double x, double y, double z, double w) {
  double xyz = x + y + z;
  double ww = w * 1.118033988749894;
  double s2 = xyz * -0.16666666666666666 + ww;
  double xs = x + s2;
  double ys = y + s2;
  double zs = z + s2;
  double ws = -0.5 * xyz + ww;

  return noise_unskewed_base_(seed, xs, ys, zs, ws);
}

double opensimplex::noise4d_improve_xy_improve_zw_sl(std::int64_t seed, double x, double y, double z, double w) {
  double s2 = (x + y) * -0.28522513987434876941 + (z + w) * 0.83897065470611435718;
  double t2 = (z + w) * 0.21939749883706435719 + (x + y) * -0.48214856493302476942;
  double xs = x + s2;
  double ys = y + s2;
  double zs = z + t2;
  double ws = w + t2;

  return noise_unskewed_base_(seed, xs, ys, zs, ws);
}

double opensimplex::noise4d_fallback_sl(std::int64_t seed, double x, double y, double z, double w) {
  double s = SKEW_4D_ * (x + y + z + w);
  double xs = x + s;
  double ys = y + s;
  double zs = z + s;
  double ws = w + s;

  return noise_unskewed_base_(seed, xs, ys, zs, ws);
}

double opensimplex::octave2d_sl(std::int64_t seed, double x, double y, int octaves, double persistence) {
  return octave2d_base_sl(seed, x, y, octaves, persistence, noise2d_sl);
}

double opensimplex::octave2d_improve_x_sl(std::int64_t seed, double x, double y, int octaves, double persistence) {
  return octave2d_base_sl(seed, x, y, octaves, persistence, noise2d_improve_x_sl);
}

double opensimplex::octave3d_improve_xy_sl(std::int64_t seed, double x, double y, double z, int octaves, double persistence) {
  return octave3d_base_sl(seed, x, y, z, octaves, persistence, noise3d_improve_xy_sl);
}

double opensimplex::octave3d_improve_xz_sl(std::int64_t seed, double x, double y, double z, int octaves, double persistence) {
  return octave3d_base_sl(seed, x, y, z, octaves, persistence, noise3d_improve_xz_sl);
}

double opensimplex::octave3d_fallback_sl(std::int64_t seed, double x, double y, double z, int octaves, double persistence) {
  return octave3d_base_sl(seed, x, y, z, octaves, persistence, noise3d_fallback_sl);
}

double opensimplex::octave4d_improve_xyz_improve_xy_sl(std::int64_t seed, double x, double y, double z, double w, int octaves, double persistence) {
  return octave4d_base_sl(seed, x, y, z, w, octaves, persistence, noise4d_improve_xyz_improve_xy_sl);
}

double opensimplex::octave4d_improve_xyz_improve_xz_sl(std::int64_t seed, double x, double y, double z, double w, int octaves, double persistence) {
  return octave4d_base_sl(seed, x, y, z, w, octaves, persistence, noise4d_improve_xyz_improve_xz_sl);
}

double opensimplex::octave4d_improve_xyz_sl(std::int64_t seed, double x, double y, double z, double w, int octaves, double persistence) {
  return octave4d_base_sl(seed, x, y, z, w, octaves, persistence, noise4d_improve_xyz_sl);
}

double opensimplex::octave4d_improve_xy_improve_zw_sl(std::int64_t seed, double x, double y, double z, double w, int octaves, double persistence) {
  return octave4d_base_sl(seed, x, y, z, w, octaves, persistence, noise4d_improve_xy_improve_zw_sl);
}

double opensimplex::octave4d_fallback_sl(std::int64_t seed, double x, double y, double z, double w, int octaves, double persistence) {
  return octave4d_base_sl(seed, x, y, z, w, octaves, persistence, noise4d_fallback_sl);
}

////////////////
// PRE-SEEDED //
////////////////

double opensimplex::noise2d(double x, double y) {
  double s = SKEW_2D_ * (x + y);
  double xs = x + s;
  double ys = y + s;

  return noise_unskewed_base_(seed_, xs, ys);
}

double opensimplex::noise2d_improve_x(double x, double y) {
  double xx = x * ROOT2OVER2_;
  double yy = y * (ROOT2OVER2_ * (1 + 2 * SKEW_2D_));

  return noise_unskewed_base_(seed_, yy + xx, yy - xx);
}

double opensimplex::noise3d_improve_xy(double x, double y, double z) {
  double xy = x + y;
  double s2 = xy * ROTATE3_ORTHOGONALIZER_;
  double zz = z * ROOT3OVER3_;
  double xr = x + s2 + zz;
  double yr = y + s2 + zz;
  double zr = xy * -ROOT3OVER3_ + zz;

  return noise_unrotated_base_(seed_, xr, yr, zr);
}

double opensimplex::noise3d_improve_xz(double x, double y, double z) {
  double xz = x + z;
  double s2 = xz * -0.211324865405187;
  double yy = y * ROOT3OVER3_;
  double xr = x + s2 + yy;
  double zr = z + s2 + yy;
  double yr = xz * -ROOT3OVER3_ + yy;

  return noise_unrotated_base_(seed_, xr, yr, zr);
}

double opensimplex::noise3d_fallback(double x, double y, double z) {
  double r = FALLBACK_ROTATE3_ * (x + y + z);
  double xr = r - x;
  double yr = r - y;
  double zr = r - z;

  return noise_unrotated_base_(seed_, xr, yr, zr);
}

double opensimplex::noise4d_improve_xyz_improve_xy(double x, double y, double z, double w) {
  double xy = x + y;
  double s2 = xy * -0.21132486540518699998;
  double zz = z * 0.28867513459481294226;
  double ww = w * 1.118033988749894;
  double xr = x + (zz + ww + s2), yr = y + (zz + ww + s2);
  double zr = xy * -0.57735026918962599998 + (zz + ww);
  double wr = z * -0.866025403784439 + ww;

  return noise_unskewed_base_(seed_, xr, yr, zr, wr);
}

double opensimplex::noise4d_improve_xyz_improve_xz(double x, double y, double z, double w) {
  double xz = x + z;
  double s2 = xz * -0.21132486540518699998;
  double yy = y * 0.28867513459481294226;
  double ww = w * 1.118033988749894;
  double xr = x + (yy + ww + s2), zr = z + (yy + ww + s2);
  double yr = xz * -0.57735026918962599998 + (yy + ww);
  double wr = y * -0.866025403784439 + ww;

  return noise_unskewed_base_(seed_, xr, yr, zr, wr);
}

double opensimplex::noise4d_improve_xyz(double x, double y, double z, double w) {
  double xyz = x + y + z;
  double ww = w * 1.118033988749894;
  double s2 = xyz * -0.16666666666666666 + ww;
  double xs = x + s2;
  double ys = y + s2;
  double zs = z + s2;
  double ws = -0.5 * xyz + ww;

  return noise_unskewed_base_(seed_, xs, ys, zs, ws);
}

double opensimplex::noise4d_improve_xy_improve_zw(double x, double y, double z, double w) {
  double s2 = (x + y) * -0.28522513987434876941 + (z + w) * 0.83897065470611435718;
  double t2 = (z + w) * 0.21939749883706435719 + (x + y) * -0.48214856493302476942;
  double xs = x + s2;
  double ys = y + s2;
  double zs = z + t2;
  double ws = w + t2;

  return noise_unskewed_base_(seed_, xs, ys, zs, ws);
}

double opensimplex::noise4d_fallback(double x, double y, double z, double w) {
  double s = SKEW_4D_ * (x + y + z + w);
  double xs = x + s;
  double ys = y + s;
  double zs = z + s;
  double ws = w + s;

  return noise_unskewed_base_(seed_, xs, ys, zs, ws);
}

double opensimplex::octave2d(double x, double y, int octaves, double persistence) {
  return octave2d_base(x, y, octaves, persistence, noise2d);
}

double opensimplex::octave2d_improve_x(double x, double y, int octaves, double persistence) {
  return octave2d_base(x, y, octaves, persistence, noise2d_improve_x);
}

double opensimplex::octave3d_improve_xy(double x, double y, double z, int octaves, double persistence) {
  return octave3d_base(x, y, z, octaves, persistence, noise3d_improve_xy);
}

double opensimplex::octave3d_improve_xz(double x, double y, double z, int octaves, double persistence) {
  return octave3d_base(x, y, z, octaves, persistence, noise3d_improve_xz);
}

double opensimplex::octave3d_fallback(double x, double y, double z, int octaves, double persistence) {
  return octave3d_base(x, y, z, octaves, persistence, noise3d_fallback);
}

double opensimplex::octave4d_improve_xyz_improve_xy(double x, double y, double z, double w, int octaves, double persistence) {
  return octave4d_base(x, y, z, w, octaves, persistence, noise4d_improve_xyz_improve_xy);
}

double opensimplex::octave4d_improve_xyz_improve_xz(double x, double y, double z, double w, int octaves, double persistence) {
  return octave4d_base(x, y, z, w, octaves, persistence, noise4d_improve_xyz_improve_xz);
}

double opensimplex::octave4d_improve_xyz(double x, double y, double z, double w, int octaves, double persistence) {
  return octave4d_base(x, y, z, w, octaves, persistence, noise4d_improve_xyz);
}

double opensimplex::octave4d_improve_xy_improve_zw(double x, double y, double z, double w, int octaves, double persistence) {
  return octave4d_base(x, y, z, w, octaves, persistence, noise4d_improve_xy_improve_zw);
}

double opensimplex::octave4d_fallback(double x, double y, double z, double w, int octaves, double persistence) {
  return octave4d_base(x, y, z, w, octaves, persistence, noise4d_fallback);
}

double opensimplex::noise_unskewed_base_(std::int64_t seed, double xs, double ys) {
  int xsb = fast_floor_(xs);
  int ysb = fast_floor_(ys);
  double xi = xs - xsb;
  double yi = ys - ysb;

  std::int64_t xsbp = xsb * PRIME_X_;
  std::int64_t ysbp = ysb * PRIME_Y_;

  double t = (xi + yi) * UNSKEW_2D_;
  double dx0 = xi + t;
  double dy0 = yi + t;

  double a0 = RSQUARED_2D_ - dx0 * dx0 - dy0 * dy0;
  double value = (a0 * a0) * (a0 * a0) * grad_(seed, xsbp, ysbp, dx0, dy0);

  double a1 = (2 * (1 + 2 * UNSKEW_2D_) * (1 / UNSKEW_2D_ + 2)) * t + ((-2 * (1 + 2 * UNSKEW_2D_) * (1 + 2 * UNSKEW_2D_)) + a0);
  double dx1 = dx0 - (1 + 2 * UNSKEW_2D_);
  double dy1 = dy0 - (1 + 2 * UNSKEW_2D_);
  value += (a1 * a1) * (a1 * a1) * grad_(seed, xsbp + PRIME_X_, ysbp + PRIME_Y_, dx1, dy1);

  double xmyi = xi - yi;
  if (t < UNSKEW_2D_) {
    if (xi + xmyi > 1) {
      double dx2 = dx0 - (3 * UNSKEW_2D_ + 2);
      double dy2 = dy0 - (3 * UNSKEW_2D_ + 1);
      double a2 = RSQUARED_2D_ - dx2 * dx2 - dy2 * dy2;
      if (a2 > 0)
        value += (a2 * a2) * (a2 * a2) * grad_(seed, xsbp + (PRIME_X_ << 1), ysbp + PRIME_Y_, dx2, dy2);
    } else {
      double dx2 = dx0 - UNSKEW_2D_;
      double dy2 = dy0 - (UNSKEW_2D_ + 1);
      double a2 = RSQUARED_2D_ - dx2 * dx2 - dy2 * dy2;
      if (a2 > 0)
        value += (a2 * a2) * (a2 * a2) * grad_(seed, xsbp, ysbp + PRIME_Y_, dx2, dy2);
    }

    if (yi - xmyi > 1) {
      double dx3 = dx0 - (3 * UNSKEW_2D_ + 1);
      double dy3 = dy0 - (3 * UNSKEW_2D_ + 2);
      double a3 = RSQUARED_2D_ - dx3 * dx3 - dy3 * dy3;
      if (a3 > 0)
        value += (a3 * a3) * (a3 * a3) * grad_(seed, xsbp + PRIME_X_, ysbp + (PRIME_Y_ << 1), dx3, dy3);
    } else {
      double dx3 = dx0 - (UNSKEW_2D_ + 1);
      double dy3 = dy0 - UNSKEW_2D_;
      double a3 = RSQUARED_2D_ - dx3 * dx3 - dy3 * dy3;
      if (a3 > 0)
        value += (a3 * a3) * (a3 * a3) * grad_(seed, xsbp + PRIME_X_, ysbp, dx3, dy3);
    }

  } else {
    if (xi + xmyi < 0) {
      double dx2 = dx0 + (1 + UNSKEW_2D_);
      double dy2 = dy0 + UNSKEW_2D_;
      double a2 = RSQUARED_2D_ - dx2 * dx2 - dy2 * dy2;
      if (a2 > 0)
        value += (a2 * a2) * (a2 * a2) * grad_(seed, xsbp - PRIME_X_, ysbp, dx2, dy2);
    } else {
      double dx2 = dx0 - (UNSKEW_2D_ + 1);
      double dy2 = dy0 - UNSKEW_2D_;
      double a2 = RSQUARED_2D_ - dx2 * dx2 - dy2 * dy2;
      if (a2 > 0)
        value += (a2 * a2) * (a2 * a2) * grad_(seed, xsbp + PRIME_X_, ysbp, dx2, dy2);
    }

    if (yi < xmyi) {
      double dx2 = dx0 + UNSKEW_2D_;
      double dy2 = dy0 + (UNSKEW_2D_ + 1);
      double a2 = RSQUARED_2D_ - dx2 * dx2 - dy2 * dy2;
      if (a2 > 0)
        value += (a2 * a2) * (a2 * a2) * grad_(seed, xsbp, ysbp - PRIME_Y_, dx2, dy2);
    } else {
      double dx2 = dx0 - UNSKEW_2D_;
      double dy2 = dy0 - (UNSKEW_2D_ + 1);
      double a2 = RSQUARED_2D_ - dx2 * dx2 - dy2 * dy2;
      if (a2 > 0)
        value += (a2 * a2) * (a2 * a2) * grad_(seed, xsbp, ysbp + PRIME_Y_, dx2, dy2);
    }
  }

  return (value + 1.0) / 2.0;
}

double opensimplex::noise_unrotated_base_(std::int64_t seed, double xr, double yr, double zr) {
  int xrb = fast_floor_(xr);
  int yrb = fast_floor_(yr);
  int zrb = fast_floor_(zr);
  double xi = (xr - xrb);
  double yi = (yr - yrb);
  double zi = (zr - zrb);

  std::int64_t xrbp = xrb * PRIME_X_;
  std::int64_t yrbp = yrb * PRIME_Y_;
  std::int64_t zrbp = zrb * PRIME_Z_;
  std::int64_t seed2 = seed ^ -0x52D547B2E96ED629L;

  int xNMask = (int)(-0.5f - xi);
  int yNMask = (int)(-0.5f - yi);
  int zNMask = (int)(-0.5f - zi);

  double x0 = xi + xNMask;
  double y0 = yi + yNMask;
  double z0 = zi + zNMask;
  double a0 = RSQUARED_3D_ - x0 * x0 - y0 * y0 - z0 * z0;
  double value = (a0 * a0) * (a0 * a0) *
      grad_(seed, xrbp + (xNMask & PRIME_X_), yrbp + (yNMask & PRIME_Y_), zrbp + (zNMask & PRIME_Z_), x0, y0, z0);

  double x1 = xi - 0.5f;
  double y1 = yi - 0.5f;
  double z1 = zi - 0.5f;
  double a1 = RSQUARED_3D_ - x1 * x1 - y1 * y1 - z1 * z1;
  value += (a1 * a1) * (a1 * a1) *
      grad_(seed2, xrbp + PRIME_X_, yrbp + PRIME_Y_, zrbp + PRIME_Z_, x1, y1, z1);

  double xAFlipMask0 = ((xNMask | 1) << 1) * x1;
  double yAFlipMask0 = ((yNMask | 1) << 1) * y1;
  double zAFlipMask0 = ((zNMask | 1) << 1) * z1;
  double xAFlipMask1 = (-2 - (xNMask << 2)) * x1 - 1.0f;
  double yAFlipMask1 = (-2 - (yNMask << 2)) * y1 - 1.0f;
  double zAFlipMask1 = (-2 - (zNMask << 2)) * z1 - 1.0f;

  bool skip5 = false;
  double a2 = xAFlipMask0 + a0;
  if (a2 > 0) {
    double x2 = x0 - (xNMask | 1);
    double y2 = y0;
    double z2 = z0;
    value += (a2 * a2) * (a2 * a2) *
        grad_(seed, xrbp + (~xNMask & PRIME_X_), yrbp + (yNMask & PRIME_Y_), zrbp + (zNMask & PRIME_Z_), x2, y2, z2);

  } else {
    double a3 = yAFlipMask0 + zAFlipMask0 + a0;
    if (a3 > 0) {
      double x3 = x0;
      double y3 = y0 - (yNMask | 1);
      double z3 = z0 - (zNMask | 1);
      value += (a3 * a3) * (a3 * a3) *
          grad_(seed, xrbp + (xNMask & PRIME_X_), yrbp + (~yNMask & PRIME_Y_), zrbp + (~zNMask & PRIME_Z_), x3, y3, z3);
    }

    double a4 = xAFlipMask1 + a1;
    if (a4 > 0) {
      double x4 = (xNMask | 1) + x1;
      double y4 = y1;
      double z4 = z1;
      value += (a4 * a4) * (a4 * a4) *
          grad_(seed2, xrbp + (xNMask & (PRIME_X_ * 2)), yrbp + PRIME_Y_, zrbp + PRIME_Z_, x4, y4, z4);
      skip5 = true;
    }
  }

  bool skip9 = false;
  double a6 = yAFlipMask0 + a0;
  if (a6 > 0) {
    double x6 = x0;
    double y6 = y0 - (yNMask | 1);
    double z6 = z0;
    value += (a6 * a6) * (a6 * a6) *
        grad_(seed, xrbp + (xNMask & PRIME_X_), yrbp + (~yNMask & PRIME_Y_), zrbp + (zNMask & PRIME_Z_), x6, y6, z6);

  } else {
    double a7 = xAFlipMask0 + zAFlipMask0 + a0;
    if (a7 > 0) {
      double x7 = x0 - (xNMask | 1);
      double y7 = y0;
      double z7 = z0 - (zNMask | 1);
      value += (a7 * a7) * (a7 * a7) *
          grad_(seed, xrbp + (~xNMask & PRIME_X_), yrbp + (yNMask & PRIME_Y_), zrbp + (~zNMask & PRIME_Z_), x7, y7, z7);
    }

    double a8 = yAFlipMask1 + a1;
    if (a8 > 0) {
      double x8 = x1;
      double y8 = (yNMask | 1) + y1;
      double z8 = z1;
      value += (a8 * a8) * (a8 * a8) *
          grad_(seed2, xrbp + PRIME_X_, yrbp + (yNMask & (PRIME_Y_ << 1)), zrbp + PRIME_Z_, x8, y8, z8);
      skip9 = true;
    }
  }

  bool skipD = false;
  double aA = zAFlipMask0 + a0;
  if (aA > 0) {
    double xA = x0;
    double yA = y0;
    double zA = z0 - (zNMask | 1);
    value += (aA * aA) * (aA * aA) *
        grad_(seed, xrbp + (xNMask & PRIME_X_), yrbp + (yNMask & PRIME_Y_), zrbp + (~zNMask & PRIME_Z_), xA, yA, zA);

  } else {
    double aB = xAFlipMask0 + yAFlipMask0 + a0;
    if (aB > 0) {
      double xB = x0 - (xNMask | 1);
      double yB = y0 - (yNMask | 1);
      double zB = z0;
      value += (aB * aB) * (aB * aB) *
          grad_(seed, xrbp + (~xNMask & PRIME_X_), yrbp + (~yNMask & PRIME_Y_), zrbp + (zNMask & PRIME_Z_), xB, yB, zB);
    }

    double aC = zAFlipMask1 + a1;
    if (aC > 0) {
      double xC = x1;
      double yC = y1;
      double zC = (zNMask | 1) + z1;
      value += (aC * aC) * (aC * aC) *
          grad_(seed2, xrbp + PRIME_X_, yrbp + PRIME_Y_, zrbp + (zNMask & (PRIME_Z_ << 1)), xC, yC, zC);
      skipD = true;
    }
  }

  if (!skip5) {
    double a5 = yAFlipMask1 + zAFlipMask1 + a1;
    if (a5 > 0) {
      double x5 = x1;
      double y5 = (yNMask | 1) + y1;
      double z5 = (zNMask | 1) + z1;
      value += (a5 * a5) * (a5 * a5) *
          grad_(seed2, xrbp + PRIME_X_, yrbp + (yNMask & (PRIME_Y_ << 1)), zrbp + (zNMask & (PRIME_Z_ << 1)), x5, y5, z5);
    }
  }

  if (!skip9) {
    double a9 = xAFlipMask1 + zAFlipMask1 + a1;
    if (a9 > 0) {
      double x9 = (xNMask | 1) + x1;
      double y9 = y1;
      double z9 = (zNMask | 1) + z1;
      value += (a9 * a9) * (a9 * a9) *
          grad_(seed2, xrbp + (xNMask & (PRIME_X_ * 2)), yrbp + PRIME_Y_, zrbp + (zNMask & (PRIME_Z_ << 1)), x9, y9, z9);
    }
  }

  if (!skipD) {
    double aD = xAFlipMask1 + yAFlipMask1 + a1;
    if (aD > 0) {
      double xD = (xNMask | 1) + x1;
      double yD = (yNMask | 1) + y1;
      double zD = z1;
      value += (aD * aD) * (aD * aD) *
          grad_(seed2, xrbp + (xNMask & (PRIME_X_ << 1)), yrbp + (yNMask & (PRIME_Y_ << 1)), zrbp + PRIME_Z_, xD, yD, zD);
    }
  }

  return (value + 1.0) / 2.0;
}

double opensimplex::noise_unskewed_base_(std::int64_t seed, double xs, double ys, double zs, double ws) {
  int xsb = fast_floor_(xs);
  int ysb = fast_floor_(ys);
  int zsb = fast_floor_(zs);
  int wsb = fast_floor_(ws);
  double xsi = xs - xsb;
  double ysi = ys - ysb;
  double zsi = zs - zsb;
  double wsi = ws - wsb;

  double ssi = (xsi + ysi + zsi + wsi) * UNSKEW_4D_;
  double xi = xsi + ssi;
  double yi = ysi + ssi;
  double zi = zsi + ssi;
  double wi = wsi + ssi;

  std::int64_t xsvp = xsb * PRIME_X_;
  std::int64_t ysvp = ysb * PRIME_Y_;
  std::int64_t zsvp = zsb * PRIME_Z_;
  std::int64_t wsvp = wsb * PRIME_W_;

  int index = ((fast_floor_(xs * 4) & 3) << 0)
            | ((fast_floor_(ys * 4) & 3) << 2)
            | ((fast_floor_(zs * 4) & 3) << 4)
            | ((fast_floor_(ws * 4) & 3) << 6);

  double value = 0;
  int secondaryIndexStartAndStop = LOOKUP_4D_A_[index];
  int secondaryIndexStart = secondaryIndexStartAndStop & 0xFFFF;
  int secondaryIndexStop = secondaryIndexStartAndStop >> 16;

  for (int i = secondaryIndexStart; i < secondaryIndexStop; i++) {
    LatticeVertex4D_ c = LOOKUP_4D_B_[i];
    double dx = xi + c.dx, dy = yi + c.dy, dz = zi + c.dz, dw = wi + c.dw;
    double a = (dx * dx + dy * dy) + (dz * dz + dw * dw);
    if (a < RSQUARED_4D_) {
      a -= RSQUARED_4D_;
      a *= a;
      value += a * a * grad_(seed, xsvp + c.xsvp, ysvp + c.ysvp, zsvp + c.zsvp, wsvp + c.wsvp, dx, dy, dz, dw);
    }
  }

  return (value + 1.0) / 2.0;
}

double opensimplex::grad_(
    std::int64_t seed,
    std::int64_t xsvp, std::int64_t ysvp,
    double dx, double dy
) {
  std::int64_t hash = seed ^ xsvp ^ ysvp;
  hash *= HASH_MULTIPLIER_;
  hash ^= hash >> (64 - N_GRADS_2D_EXPONENT_ + 1);
  int gi = static_cast<int>(hash) & ((N_GRADS_2D_ - 1) << 1);
  return GRADIENTS_2D_[gi | 0] * dx + GRADIENTS_2D_[gi | 1] * dy;
}

double opensimplex::grad_(
    std::int64_t seed,
    std::int64_t xrvp, std::int64_t yrvp, std::int64_t zrvp,
    double dx, double dy, double dz
) {
  std::int64_t hash = (seed ^ xrvp) ^ (yrvp ^ zrvp);
  hash *= HASH_MULTIPLIER_;
  hash ^= hash >> (64 - N_GRADS_3D_EXPONENT_ + 2);
  int gi = static_cast<int>(hash) & ((N_GRADS_3D_ - 1) << 2);
  return GRADIENTS_3D_[gi | 0] * dx + GRADIENTS_3D_[gi | 1] * dy + GRADIENTS_3D_[gi | 2] * dz;
}

double opensimplex::grad_(
    std::int64_t seed,
    std::int64_t xsvp, std::int64_t ysvp, std::int64_t zsvp, std::int64_t wsvp,
    double dx, double dy, double dz, double dw
) {
  std::int64_t hash = seed ^ (xsvp ^ ysvp) ^ (zsvp ^ wsvp);
  hash *= HASH_MULTIPLIER_;
  hash ^= hash >> (64 - N_GRADS_4D_EXPONENT_ + 2);
  int gi = static_cast<int>(hash) & ((N_GRADS_4D_ - 1) << 2);
  return (GRADIENTS_4D_[gi | 0] * dx + GRADIENTS_4D_[gi | 1] * dy) +
         (GRADIENTS_4D_[gi | 2] * dz + GRADIENTS_4D_[gi | 3] * dw);
}

const std::array<double, opensimplex::N_GRADS_2D_ * 2> opensimplex::GRADIENTS_2D_ = std::invoke([]{
  std::array<double, 48> grad2{};
  for (int i = 0; i < grad2.size(); ++i)
    grad2[i] = static_cast<double>(GRAD2_[i] / NORMALIZER_2D_);

  std::array<double, N_GRADS_2D_ * 2> retval{};
  for (int i = 0, j = 0; i < retval.size(); ++i, ++j) {
    if (j == grad2.size())
      j = 0;
    retval[i] = grad2[j];
  }

  return retval;
});

const std::array<double, opensimplex::N_GRADS_3D_ * 4> opensimplex::GRADIENTS_3D_ = std::invoke([]{
  std::array<double, 192> grad3{};
  for (int i = 0; i < grad3.size(); ++i)
    grad3[i] = static_cast<double>(GRAD3_[i] / NORMALIZER_3D_);

  std::array<double, N_GRADS_3D_ * 4> retval{};
  for (int i = 0, j = 0; i < retval.size(); ++i, ++j) {
    if (j == grad3.size())
      j = 0;
    retval[i] = grad3[j];
  }

  return retval;
});

const std::array<double, opensimplex::N_GRADS_4D_ * 4> opensimplex::GRADIENTS_4D_ = std::invoke([]{
  std::array<double, 640> grad4{};
  for (int i = 0; i < grad4.size(); ++i)
    grad4[i] = static_cast<double>(GRAD4_[i] / NORMALIZER_4D_);

  std::array<double, N_GRADS_4D_ * 4> retval{};
  for (int i = 0, j = 0; i < retval.size(); ++i, ++j) {
    if (j == grad4.size())
      j = 0;
    retval[i] = grad4[j];
  }

  return retval;
});

const std::array<int, 256> opensimplex::LOOKUP_4D_A_ = std::invoke([]{
  std::array<int, 256> retval{};
  for (int i = 0, j = 0; i < 256; ++i) {
    int l4vc_len = static_cast<int>(LOOKUP_4D_VERTEX_CODES_[i].size());
    retval[i] = j | ((j + l4vc_len) << 16);
    j += l4vc_len;
  }

  return retval;
});

const std::array<opensimplex::LatticeVertex4D_, 3476> opensimplex::LOOKUP_4D_B_ = std::invoke([]{
  LatticeVertex4D_ lattice_vertices_by_code[256];
  for (int i = 0; i < 256; ++i) {
    int cx = ((i >> 0) & 3) - 1;
    int cy = ((i >> 2) & 3) - 1;
    int cz = ((i >> 4) & 3) - 1;
    int cw = ((i >> 6) & 3) - 1;
    lattice_vertices_by_code[i] = LatticeVertex4D_(cx, cy, cz, cw);
  }

  std::array<LatticeVertex4D_, 3476> retval{};
  for (int i = 0, j = 0; i < 256; ++i)
    for (int k : LOOKUP_4D_VERTEX_CODES_[i])
      retval[j++] = lattice_vertices_by_code[k];

  return retval;
});

//////////
// FAST //
//////////

///////////////
// STATELESS //
///////////////

double opensimplex::noise2d_sl_fast(std::int64_t seed, double x, double y) {
  double s = SKEW_2D_ * (x + y);
  double xs = x + s;
  double ys = y + s;

  return noise_unskewed_base_fast_(seed, xs, ys);
}

double opensimplex::noise2d_improve_x_sl_fast(std::int64_t seed, double x, double y) {
  double xx = x * ROOT2OVER2_;
  double yy = y * (ROOT2OVER2_ * (1 + 2 * SKEW_2D_));

  return noise_unskewed_base_fast_(seed, yy + xx, yy - xx);
}

double opensimplex::noise3d_improve_xy_sl_fast(std::int64_t seed, double x, double y, double z) {
  double xy = x + y;
  double s2 = xy * ROTATE3_ORTHOGONALIZER_;
  double zz = z * ROOT3OVER3_;
  double xr = x + s2 + zz;
  double yr = y + s2 + zz;
  double zr = xy * -ROOT3OVER3_ + zz;

  return noise_unrotated_base_fast_(seed, xr, yr, zr);
}

double opensimplex::noise3d_improve_xz_sl_fast(std::int64_t seed, double x, double y, double z) {
  double xz = x + z;
  double s2 = xz * ROTATE3_ORTHOGONALIZER_;
  double yy = y * ROOT3OVER3_;
  double xr = x + s2 + yy;
  double zr = z + s2 + yy;
  double yr = xz * -ROOT3OVER3_ + yy;

  return noise_unrotated_base_fast_(seed, xr, yr, zr);
}

double opensimplex::noise3d_fallback_sl_fast(std::int64_t seed, double x, double y, double z) {
  double r = FALLBACK_ROTATE3_ * (x + y + z);
  double xr = r - x, yr = r - y, zr = r - z;

  return noise_unrotated_base_fast_(seed, xr, yr, zr);
}

double opensimplex::noise4d_improve_xyz_improve_xy_sl_fast(std::int64_t seed, double x, double y, double z, double w) {
  double xy = x + y;
  double s2 = xy * -0.21132486540518699998;
  double zz = z * 0.28867513459481294226;
  double ww = w * 0.2236067977499788;
  double xr = x + (zz + ww + s2), yr = y + (zz + ww + s2);
  double zr = xy * -0.57735026918962599998 + (zz + ww);
  double wr = z * -0.866025403784439 + ww;

  return noise_unskewed_base_fast_(seed, xr, yr, zr, wr);
}

double opensimplex::noise4d_improve_xyz_improve_xz_sl_fast(std::int64_t seed, double x, double y, double z, double w) {
  double xz = x + z;
  double s2 = xz * -0.21132486540518699998;
  double yy = y * 0.28867513459481294226;
  double ww = w * 0.2236067977499788;
  double xr = x + (yy + ww + s2), zr = z + (yy + ww + s2);
  double yr = xz * -0.57735026918962599998 + (yy + ww);
  double wr = y * -0.866025403784439 + ww;

  return noise_unskewed_base_fast_(seed, xr, yr, zr, wr);
}

double opensimplex::noise4d_improve_xyz_sl_fast(std::int64_t seed, double x, double y, double z, double w) {
  double xyz = x + y + z;
  double ww = w * 0.2236067977499788;
  double s2 = xyz * -0.16666666666666666 + ww;
  double xs = x + s2, ys = y + s2, zs = z + s2, ws = -0.5 * xyz + ww;

  return noise_unskewed_base_fast_(seed, xs, ys, zs, ws);
}

double opensimplex::noise4d_improve_xy_improve_zw_sl_fast(std::int64_t seed, double x, double y, double z, double w) {
  double s2 = (x + y) * -0.178275657951399372 + (z + w) * 0.215623393288842828;
  double t2 = (z + w) * -0.403949762580207112 + (x + y) * -0.375199083010075342;
  double xs = x + s2, ys = y + s2, zs = z + t2, ws = w + t2;

  return noise_unskewed_base_fast_(seed, xs, ys, zs, ws);
}

double opensimplex::noise4d_fallback_sl_fast(std::int64_t seed, double x, double y, double z, double w) {
  double s = SKEW_4D_FAST_ * (x + y + z + w);
  double xs = x + s, ys = y + s, zs = z + s, ws = w + s;

  return noise_unskewed_base_fast_(seed, xs, ys, zs, ws);
}

double opensimplex::octave2d_sl_fast(std::int64_t seed, double x, double y, int octaves, double persistence) {
  return octave2d_base_sl(seed, x, y, octaves, persistence, noise2d_sl_fast);
}

double opensimplex::octave2d_improve_x_sl_fast(std::int64_t seed, double x, double y, int octaves, double persistence) {
  return octave2d_base_sl(seed, x, y, octaves, persistence, noise2d_improve_x_sl_fast);
}

double opensimplex::octave3d_improve_xy_sl_fast(std::int64_t seed, double x, double y, double z, int octaves, double persistence) {
  return octave3d_base_sl(seed, x, y, z, octaves, persistence, noise3d_improve_xy_sl_fast);
}

double opensimplex::octave3d_improve_xz_sl_fast(std::int64_t seed, double x, double y, double z, int octaves, double persistence) {
  return octave3d_base_sl(seed, x, y, z, octaves, persistence, noise3d_improve_xz_sl_fast);
}

double opensimplex::octave3d_fallback_sl_fast(std::int64_t seed, double x, double y, double z, int octaves, double persistence) {
  return octave3d_base_sl(seed, x, y, z, octaves, persistence, noise3d_fallback_sl_fast);
}

double opensimplex::octave4d_improve_xyz_improve_xy_sl_fast(std::int64_t seed, double x, double y, double z, double w, int octaves, double persistence) {
  return octave4d_base_sl(seed, x, y, z, w, octaves, persistence, noise4d_improve_xyz_improve_xy_sl_fast);
}

double opensimplex::octave4d_improve_xyz_improve_xz_sl_fast(std::int64_t seed, double x, double y, double z, double w, int octaves, double persistence) {
  return octave4d_base_sl(seed, x, y, z, w, octaves, persistence, noise4d_improve_xyz_improve_xz_sl_fast);
}

double opensimplex::octave4d_improve_xyz_sl_fast(std::int64_t seed, double x, double y, double z, double w, int octaves, double persistence) {
  return octave4d_base_sl(seed, x, y, z, w, octaves, persistence, noise4d_improve_xyz_sl_fast);
}

double opensimplex::octave4d_improve_xy_improve_zw_sl_fast(std::int64_t seed, double x, double y, double z, double w, int octaves, double persistence) {
  return octave4d_base_sl(seed, x, y, z, w, octaves, persistence, noise4d_improve_xy_improve_zw_sl_fast);
}

double opensimplex::octave4d_fallback_sl_fast(std::int64_t seed, double x, double y, double z, double w, int octaves, double persistence) {
  return octave4d_base_sl(seed, x, y, z, w, octaves, persistence, noise4d_fallback_sl_fast);
}

////////////////
// PRE-SEEDED //
////////////////

double opensimplex::noise2d_fast(double x, double y) {
  double s = SKEW_2D_ * (x + y);
  double xs = x + s;
  double ys = y + s;

  return noise_unskewed_base_fast_(seed_, xs, ys);
}

double opensimplex::noise2d_improve_x_fast(double x, double y) {
  double xx = x * ROOT2OVER2_;
  double yy = y * (ROOT2OVER2_ * (1 + 2 * SKEW_2D_));

  return noise_unskewed_base_fast_(seed_, yy + xx, yy - xx);
}

double opensimplex::noise3d_improve_xy_fast(double x, double y, double z) {
  double xy = x + y;
  double s2 = xy * ROTATE3_ORTHOGONALIZER_;
  double zz = z * ROOT3OVER3_;
  double xr = x + s2 + zz;
  double yr = y + s2 + zz;
  double zr = xy * -ROOT3OVER3_ + zz;

  return noise_unrotated_base_fast_(seed_, xr, yr, zr);
}

double opensimplex::noise3d_improve_xz_fast(double x, double y, double z) {
  double xz = x + z;
  double s2 = xz * ROTATE3_ORTHOGONALIZER_;
  double yy = y * ROOT3OVER3_;
  double xr = x + s2 + yy;
  double zr = z + s2 + yy;
  double yr = xz * -ROOT3OVER3_ + yy;

  return noise_unrotated_base_fast_(seed_, xr, yr, zr);
}

double opensimplex::noise3d_fallback_fast(double x, double y, double z) {
  double r = FALLBACK_ROTATE3_ * (x + y + z);
  double xr = r - x, yr = r - y, zr = r - z;

  return noise_unrotated_base_fast_(seed_, xr, yr, zr);
}

double opensimplex::noise4d_improve_xyz_improve_xy_fast(double x, double y, double z, double w) {
  double xy = x + y;
  double s2 = xy * -0.21132486540518699998;
  double zz = z * 0.28867513459481294226;
  double ww = w * 0.2236067977499788;
  double xr = x + (zz + ww + s2), yr = y + (zz + ww + s2);
  double zr = xy * -0.57735026918962599998 + (zz + ww);
  double wr = z * -0.866025403784439 + ww;

  return noise_unskewed_base_fast_(seed_, xr, yr, zr, wr);
}

double opensimplex::noise4d_improve_xyz_improve_xz_fast(double x, double y, double z, double w) {
  double xz = x + z;
  double s2 = xz * -0.21132486540518699998;
  double yy = y * 0.28867513459481294226;
  double ww = w * 0.2236067977499788;
  double xr = x + (yy + ww + s2), zr = z + (yy + ww + s2);
  double yr = xz * -0.57735026918962599998 + (yy + ww);
  double wr = y * -0.866025403784439 + ww;

  return noise_unskewed_base_fast_(seed_, xr, yr, zr, wr);
}

double opensimplex::noise4d_improve_xyz_fast(double x, double y, double z, double w) {
  double xyz = x + y + z;
  double ww = w * 0.2236067977499788;
  double s2 = xyz * -0.16666666666666666 + ww;
  double xs = x + s2, ys = y + s2, zs = z + s2, ws = -0.5 * xyz + ww;

  return noise_unskewed_base_fast_(seed_, xs, ys, zs, ws);
}

double opensimplex::noise4d_improve_xy_improve_zw_fast(double x, double y, double z, double w) {
  double s2 = (x + y) * -0.178275657951399372 + (z + w) * 0.215623393288842828;
  double t2 = (z + w) * -0.403949762580207112 + (x + y) * -0.375199083010075342;
  double xs = x + s2, ys = y + s2, zs = z + t2, ws = w + t2;

  return noise_unskewed_base_fast_(seed_, xs, ys, zs, ws);
}

double opensimplex::noise4d_fallback_fast(double x, double y, double z, double w) {
  double s = SKEW_4D_FAST_ * (x + y + z + w);
  double xs = x + s, ys = y + s, zs = z + s, ws = w + s;

  return noise_unskewed_base_fast_(seed_, xs, ys, zs, ws);
}

double opensimplex::octave2d_fast(double x, double y, int octaves, double persistence) {
  return octave2d_base(x, y, octaves, persistence, noise2d_fast);
}

double opensimplex::octave2d_improve_x_fast(double x, double y, int octaves, double persistence) {
  return octave2d_base(x, y, octaves, persistence, noise2d_improve_x_fast);
}

double opensimplex::octave3d_improve_xy_fast(double x, double y, double z, int octaves, double persistence) {
  return octave3d_base(x, y, z, octaves, persistence, noise3d_improve_xy_fast);
}

double opensimplex::octave3d_improve_xz_fast(double x, double y, double z, int octaves, double persistence) {
  return octave3d_base(x, y, z, octaves, persistence, noise3d_improve_xz_fast);
}

double opensimplex::octave3d_fallback_fast(double x, double y, double z, int octaves, double persistence) {
  return octave3d_base(x, y, z, octaves, persistence, noise3d_fallback_fast);
}

double opensimplex::octave4d_improve_xyz_improve_xy_fast(double x, double y, double z, double w, int octaves, double persistence) {
  return octave4d_base(x, y, z, w, octaves, persistence, noise4d_improve_xyz_improve_xy_fast);
}

double opensimplex::octave4d_improve_xyz_improve_xz_fast(double x, double y, double z, double w, int octaves, double persistence) {
  return octave4d_base(x, y, z, w, octaves, persistence, noise4d_improve_xyz_improve_xz_fast);
}

double opensimplex::octave4d_improve_xyz_fast(double x, double y, double z, double w, int octaves, double persistence) {
  return octave4d_base(x, y, z, w, octaves, persistence, noise4d_improve_xyz_fast);
}

double opensimplex::octave4d_improve_xy_improve_zw_fast(double x, double y, double z, double w, int octaves, double persistence) {
  return octave4d_base(x, y, z, w, octaves, persistence, noise4d_improve_xy_improve_zw_fast);
}

double opensimplex::octave4d_fallback_fast(double x, double y, double z, double w, int octaves, double persistence) {
  return octave4d_base(x, y, z, w, octaves, persistence, noise4d_fallback_fast);
}

double opensimplex::noise_unskewed_base_fast_(std::int64_t seed, double xs, double ys) {
  int xsb = fast_floor_(xs);
  int ysb = fast_floor_(ys);
  double xi = (xs - xsb);
  double yi = (ys - ysb);

  std::int64_t xsbp = xsb * PRIME_X_;
  std::int64_t ysbp = ysb * PRIME_Y_;

  double t = (xi + yi) * UNSKEW_2D_;
  double dx0 = xi + t;
  double dy0 = yi + t;

  double value = 0;
  double a0 = RSQUARED_2D_FAST_ - dx0 * dx0 - dy0 * dy0;
  if (a0 > 0)
    value = (a0 * a0) * (a0 * a0) * grad_fast_(seed, xsbp, ysbp, dx0, dy0);

  double a1 = (2 * (1 + 2 * UNSKEW_2D_) * (1 / UNSKEW_2D_ + 2)) * t + ((-2 * (1 + 2 * UNSKEW_2D_) * (1 + 2 * UNSKEW_2D_)) + a0);
  if (a1 > 0) {
    double dx1 = dx0 - (1 + 2 * UNSKEW_2D_);
    double dy1 = dy0 - (1 + 2 * UNSKEW_2D_);
    value += (a1 * a1) * (a1 * a1) * grad_fast_(seed, xsbp + PRIME_X_, ysbp + PRIME_Y_, dx1, dy1);
  }

  if (dy0 > dx0) {
    double dx2 = dx0 - UNSKEW_2D_;
    double dy2 = dy0 - (UNSKEW_2D_ + 1);
    double a2 = RSQUARED_2D_FAST_ - dx2 * dx2 - dy2 * dy2;
    if (a2 > 0)
      value += (a2 * a2) * (a2 * a2) * grad_fast_(seed, xsbp, ysbp + PRIME_Y_, dx2, dy2);

  } else {
    double dx2 = dx0 - (UNSKEW_2D_ + 1);
    double dy2 = dy0 - UNSKEW_2D_;
    double a2 = RSQUARED_2D_FAST_ - dx2 * dx2 - dy2 * dy2;
    if (a2 > 0)
      value += (a2 * a2) * (a2 * a2) * grad_fast_(seed, xsbp + PRIME_X_, ysbp, dx2, dy2);
  }

  return (value + 1.0) / 2.0;
}

double opensimplex::noise_unrotated_base_fast_(std::int64_t seed, double xr, double yr, double zr) {
  int xrb = fast_round_(xr);
  int yrb = fast_round_(yr);
  int zrb = fast_round_(zr);
  double xri = xr - xrb;
  double yri = yr - yrb;
  double zri = zr - zrb;

  int xNSign = (int)(-1.0f - xri) | 1;
  int yNSign = (int)(-1.0f - yri) | 1;
  int zNSign = (int)(-1.0f - zri) | 1;

  double ax0 = xNSign * -xri;
  double ay0 = yNSign * -yri;
  double az0 = zNSign * -zri;

  std::int64_t xrbp = xrb * PRIME_X_;
  std::int64_t yrbp = yrb * PRIME_Y_;
  std::int64_t zrbp = zrb * PRIME_Z_;

  double value = 0;
  double a = (RSQUARED_3D_FAST_ - xri * xri) - (yri * yri + zri * zri);
  for (int l = 0; ; l++) {
    if (a > 0)
      value += (a * a) * (a * a) * grad_fast_(seed, xrbp, yrbp, zrbp, xri, yri, zri);

    if (ax0 >= ay0 && ax0 >= az0) {
      double b = a + ax0 + ax0;
      if (b > 1) {
        b -= 1;
        value += (b * b) * (b * b) * grad_fast_(seed, xrbp - xNSign * PRIME_X_, yrbp, zrbp, xri + xNSign, yri, zri);
      }
    } else if (ay0 > ax0 && ay0 >= az0) {
      double b = a + ay0 + ay0;
      if (b > 1) {
        b -= 1;
        value += (b * b) * (b * b) * grad_fast_(seed, xrbp, yrbp - yNSign * PRIME_Y_, zrbp, xri, yri + yNSign, zri);
      }
    } else {
      double b = a + az0 + az0;
      if (b > 1) {
        b -= 1;
        value += (b * b) * (b * b) * grad_fast_(seed, xrbp, yrbp, zrbp - zNSign * PRIME_Z_, xri, yri, zri + zNSign);
      }
    }

    if (l == 1)
      break;

    ax0 = 0.5f - ax0;
    ay0 = 0.5f - ay0;
    az0 = 0.5f - az0;

    xri = xNSign * ax0;
    yri = yNSign * ay0;
    zri = zNSign * az0;

    a += (0.75f - ax0) - (ay0 + az0);

    xrbp += (xNSign >> 1) & PRIME_X_;
    yrbp += (yNSign >> 1) & PRIME_Y_;
    zrbp += (zNSign >> 1) & PRIME_Z_;

    xNSign = -xNSign;
    yNSign = -yNSign;
    zNSign = -zNSign;

    seed ^= SEED_FLIP_3D_;
  }

  return (value + 1.0) / 2.0;
}

double opensimplex::noise_unskewed_base_fast_(std::int64_t seed, double xs, double ys, double zs, double ws) {
  int xsb = fast_floor_(xs);
  int ysb = fast_floor_(ys);
  int zsb = fast_floor_(zs);
  int wsb = fast_floor_(ws);
  double xsi = xs - xsb;
  double ysi = ys - ysb;
  double zsi = zs - zsb;
  double wsi = ws - wsb;

  double siSum = (xsi + ysi) + (zsi + wsi);
  int startingLattice = (int)(siSum * 1.25);

  seed += startingLattice * SEED_OFFSET_4D_FAST_;

  double startingLatticeOffset = startingLattice * -LATTICE_STEP_4D_FAST_;
  xsi += startingLatticeOffset;
  ysi += startingLatticeOffset;
  zsi += startingLatticeOffset;
  wsi += startingLatticeOffset;

  double ssi = (siSum + startingLatticeOffset * 4) * UNSKEW_4D_FAST_;

  std::int64_t xsvp = xsb * PRIME_X_;
  std::int64_t ysvp = ysb * PRIME_Y_;
  std::int64_t zsvp = zsb * PRIME_Z_;
  std::int64_t wsvp = wsb * PRIME_W_;

  double value = 0;
  for (int i = 0; ; i++) {
    double score0 = 1.0 + ssi * (-1.0 / UNSKEW_4D_FAST_);
    if (xsi >= ysi && xsi >= zsi && xsi >= wsi && xsi >= score0) {
      xsvp += PRIME_X_;
      xsi -= 1;
      ssi -= UNSKEW_4D_FAST_;
    } else if (ysi > xsi && ysi >= zsi && ysi >= wsi && ysi >= score0) {
      ysvp += PRIME_Y_;
      ysi -= 1;
      ssi -= UNSKEW_4D_FAST_;
    } else if (zsi > xsi && zsi > ysi && zsi >= wsi && zsi >= score0) {
      zsvp += PRIME_Z_;
      zsi -= 1;
      ssi -= UNSKEW_4D_FAST_;
    } else if (wsi > xsi && wsi > ysi && wsi > zsi && wsi >= score0) {
      wsvp += PRIME_W_;
      wsi -= 1;
      ssi -= UNSKEW_4D_FAST_;
    }

    double dx = xsi + ssi, dy = ysi + ssi, dz = zsi + ssi, dw = wsi + ssi;
    double a = (dx * dx + dy * dy) + (dz * dz + dw * dw);
    if (a < RSQUARED_4D_FAST_) {
      a -= RSQUARED_4D_FAST_;
      a *= a;
      value += a * a * grad_fast_(seed, xsvp, ysvp, zsvp, wsvp, dx, dy, dz, dw);
    }

    if (i == 4)
      break;

    xsi += LATTICE_STEP_4D_FAST_;
    ysi += LATTICE_STEP_4D_FAST_;
    zsi += LATTICE_STEP_4D_FAST_;
    wsi += LATTICE_STEP_4D_FAST_;
    ssi += LATTICE_STEP_4D_FAST_ * 4 * UNSKEW_4D_FAST_;
    seed -= SEED_OFFSET_4D_FAST_;

    if (i == startingLattice) {
      xsvp -= PRIME_X_;
      ysvp -= PRIME_Y_;
      zsvp -= PRIME_Z_;
      wsvp -= PRIME_W_;
      seed += SEED_OFFSET_4D_FAST_ * 5;
    }
  }

  return (value + 1.0) / 2.0;
}

double opensimplex::grad_fast_(
    std::int64_t seed,
    std::int64_t xsvp, std::int64_t ysvp,
    double dx, double dy
) {
  std::int64_t hash = seed ^ xsvp ^ ysvp;
  hash *= HASH_MULTIPLIER_;
  hash ^= hash >> (64 - N_GRADS_2D_EXPONENT_ + 1);
  int gi = static_cast<int>(hash) & ((N_GRADS_2D_ - 1) << 1);
  return GRADIENTS_2D_FAST_[gi | 0] * dx + GRADIENTS_2D_FAST_[gi | 1] * dy;
}

double opensimplex::grad_fast_(
    std::int64_t seed,
    std::int64_t xrvp, std::int64_t yrvp, std::int64_t zrvp,
    double dx, double dy, double dz
) {
  std::int64_t hash = (seed ^ xrvp) ^ (yrvp ^ zrvp);
  hash *= HASH_MULTIPLIER_;
  hash ^= hash >> (64 - N_GRADS_3D_EXPONENT_ + 2);
  int gi = static_cast<int>(hash) & ((N_GRADS_3D_ - 1) << 2);
  return GRADIENTS_3D_FAST_[gi | 0] * dx + GRADIENTS_3D_FAST_[gi | 1] * dy + GRADIENTS_3D_FAST_[gi | 2] * dz;
}

double opensimplex::grad_fast_(
    std::int64_t seed,
    std::int64_t xsvp, std::int64_t ysvp, std::int64_t zsvp, std::int64_t wsvp,
    double dx, double dy, double dz, double dw
) {
  std::int64_t hash = seed ^ (xsvp ^ ysvp) ^ (zsvp ^ wsvp);
  hash *= HASH_MULTIPLIER_;
  hash ^= hash >> (64 - N_GRADS_4D_EXPONENT_ + 2);
  int gi = static_cast<int>(hash) & ((N_GRADS_4D_ - 1) << 2);
  return (GRADIENTS_4D_FAST_[gi | 0] * dx + GRADIENTS_4D_FAST_[gi | 1] * dy) +
         (GRADIENTS_4D_FAST_[gi | 2] * dz + GRADIENTS_4D_FAST_[gi | 3] * dw);
}

const std::array<double, opensimplex::N_GRADS_2D_ * 2> opensimplex::GRADIENTS_2D_FAST_ = std::invoke([]{
  std::array<double, 48> grad2{};
  for (int i = 0; i < grad2.size(); ++i)
    grad2[i] = static_cast<double>(GRAD2_[i] / NORMALIZER_2D_FAST_);

  std::array<double, N_GRADS_2D_ * 2> retval{};
  for (int i = 0, j = 0; i < retval.size(); ++i, ++j) {
    if (j == grad2.size())
      j = 0;
    retval[i] = grad2[j];
  }

  return retval;
});

const std::array<double, opensimplex::N_GRADS_3D_ * 4> opensimplex::GRADIENTS_3D_FAST_ = std::invoke([]{
  std::array<double, 192> grad3{};
  for (int i = 0; i < grad3.size(); ++i)
    grad3[i] = static_cast<double>(GRAD3_[i] / NORMALIZER_3D_FAST_);

  std::array<double, N_GRADS_3D_ * 4> retval{};
  for (int i = 0, j = 0; i < retval.size(); ++i, ++j) {
    if (j == grad3.size())
      j = 0;
    retval[i] = grad3[j];
  }

  return retval;
});

const std::array<double, opensimplex::N_GRADS_4D_ * 4> opensimplex::GRADIENTS_4D_FAST_ = std::invoke([]{
  std::array<double, 640> grad4{};
  for (int i = 0; i < grad4.size(); ++i)
    grad4[i] = static_cast<double>(GRAD4_[i] / NORMALIZER_4D_FAST_);

  std::array<double, N_GRADS_4D_ * 4> retval{};
  for (int i = 0, j = 0; i < retval.size(); ++i, ++j) {
    if (j == grad4.size())
      j = 0;
    retval[i] = grad4[j];
  }

  return retval;
});

/////////////
// HELPERS //
/////////////

int opensimplex::fast_floor_(double x) {
  int xi = static_cast<int>(x);
  return x < xi ? xi - 1 : xi;
}

int opensimplex::fast_round_(double x) {
  return x < 0 ? static_cast<int>(x - 0.5) : static_cast<int>(x + 0.5);
}

const std::array<double, 48> opensimplex::GRAD2_ = {
     0.38268343236509,   0.923879532511287,
     0.923879532511287,  0.38268343236509,
     0.923879532511287, -0.38268343236509,
     0.38268343236509,  -0.923879532511287,
    -0.38268343236509,  -0.923879532511287,
    -0.923879532511287, -0.38268343236509,
    -0.923879532511287,  0.38268343236509,
    -0.38268343236509,   0.923879532511287,
    //-----------------------------------//
     0.130526192220052,  0.99144486137381,
     0.608761429008721,  0.793353340291235,
     0.793353340291235,  0.608761429008721,
     0.99144486137381,   0.130526192220051,
     0.99144486137381,  -0.130526192220051,
     0.793353340291235, -0.60876142900872,
     0.608761429008721, -0.793353340291235,
     0.130526192220052, -0.99144486137381,
    -0.130526192220052, -0.99144486137381,
    -0.608761429008721, -0.793353340291235,
    -0.793353340291235, -0.608761429008721,
    -0.99144486137381,  -0.130526192220052,
    -0.99144486137381,   0.130526192220051,
    -0.793353340291235,  0.608761429008721,
    -0.608761429008721,  0.793353340291235,
    -0.130526192220052,  0.99144486137381,
};

const std::array<double, 192> opensimplex::GRAD3_ = {
     2.22474487139,       2.22474487139,      -1.0,                 0.0,
     2.22474487139,       2.22474487139,       1.0,                 0.0,
     3.0862664687972017,  1.1721513422464978,  0.0,                 0.0,
     1.1721513422464978,  3.0862664687972017,  0.0,                 0.0,
    -2.22474487139,       2.22474487139,      -1.0,                 0.0,
    -2.22474487139,       2.22474487139,       1.0,                 0.0,
    -1.1721513422464978,  3.0862664687972017,  0.0,                 0.0,
    -3.0862664687972017,  1.1721513422464978,  0.0,                 0.0,
    -1.0,                -2.22474487139,      -2.22474487139,       0.0,
     1.0,                -2.22474487139,      -2.22474487139,       0.0,
     0.0,                -3.0862664687972017, -1.1721513422464978,  0.0,
     0.0,                -1.1721513422464978, -3.0862664687972017,  0.0,
    -1.0,                -2.22474487139,       2.22474487139,       0.0,
     1.0,                -2.22474487139,       2.22474487139,       0.0,
     0.0,                -1.1721513422464978,  3.0862664687972017,  0.0,
     0.0,                -3.0862664687972017,  1.1721513422464978,  0.0,
    //----------------------------------------------------------------//
    -2.22474487139,      -2.22474487139,      -1.0,                 0.0,
    -2.22474487139,      -2.22474487139,       1.0,                 0.0,
    -3.0862664687972017, -1.1721513422464978,  0.0,                 0.0,
    -1.1721513422464978, -3.0862664687972017,  0.0,                 0.0,
    -2.22474487139,      -1.0,                -2.22474487139,       0.0,
    -2.22474487139,       1.0,                -2.22474487139,       0.0,
    -1.1721513422464978,  0.0,                -3.0862664687972017,  0.0,
    -3.0862664687972017,  0.0,                -1.1721513422464978,  0.0,
    -2.22474487139,      -1.0,                 2.22474487139,       0.0,
    -2.22474487139,       1.0,                 2.22474487139,       0.0,
    -3.0862664687972017,  0.0,                 1.1721513422464978,  0.0,
    -1.1721513422464978,  0.0,                 3.0862664687972017,  0.0,
    -1.0,                 2.22474487139,      -2.22474487139,       0.0,
     1.0,                 2.22474487139,      -2.22474487139,       0.0,
     0.0,                 1.1721513422464978, -3.0862664687972017,  0.0,
     0.0,                 3.0862664687972017, -1.1721513422464978,  0.0,
    -1.0,                 2.22474487139,       2.22474487139,       0.0,
     1.0,                 2.22474487139,       2.22474487139,       0.0,
     0.0,                 3.0862664687972017,  1.1721513422464978,  0.0,
     0.0,                 1.1721513422464978,  3.0862664687972017,  0.0,
     2.22474487139,      -2.22474487139,      -1.0,                 0.0,
     2.22474487139,      -2.22474487139,       1.0,                 0.0,
     1.1721513422464978, -3.0862664687972017,  0.0,                 0.0,
     3.0862664687972017, -1.1721513422464978,  0.0,                 0.0,
     2.22474487139,      -1.0,                -2.22474487139,       0.0,
     2.22474487139,       1.0,                -2.22474487139,       0.0,
     3.0862664687972017,  0.0,                -1.1721513422464978,  0.0,
     1.1721513422464978,  0.0,                -3.0862664687972017,  0.0,
     2.22474487139,      -1.0,                 2.22474487139,       0.0,
     2.22474487139,       1.0,                 2.22474487139,       0.0,
     1.1721513422464978,  0.0,                 3.0862664687972017,  0.0,
     3.0862664687972017,  0.0,                 1.1721513422464978,  0.0,
};

const std::array<double, 640> opensimplex::GRAD4_ = {
    -0.6740059517812944,   -0.3239847771997537,   -0.3239847771997537,    0.5794684678643381,
    -0.7504883828755602,   -0.4004672082940195,    0.15296486218853164,   0.5029860367700724,
    -0.7504883828755602,    0.15296486218853164,  -0.4004672082940195,    0.5029860367700724,
    -0.8828161875373585,    0.08164729285680945,   0.08164729285680945,   0.4553054119602712,
    -0.4553054119602712,   -0.08164729285680945,  -0.08164729285680945,   0.8828161875373585,
    -0.5029860367700724,   -0.15296486218853164,   0.4004672082940195,    0.7504883828755602,
    -0.5029860367700724,    0.4004672082940195,   -0.15296486218853164,   0.7504883828755602,
    -0.5794684678643381,    0.3239847771997537,    0.3239847771997537,    0.6740059517812944,
    -0.6740059517812944,   -0.3239847771997537,    0.5794684678643381,   -0.3239847771997537,
    -0.7504883828755602,   -0.4004672082940195,    0.5029860367700724,    0.15296486218853164,
    -0.7504883828755602,    0.15296486218853164,   0.5029860367700724,   -0.4004672082940195,
    -0.8828161875373585,    0.08164729285680945,   0.4553054119602712,    0.08164729285680945,
    -0.4553054119602712,   -0.08164729285680945,   0.8828161875373585,   -0.08164729285680945,
    -0.5029860367700724,   -0.15296486218853164,   0.7504883828755602,    0.4004672082940195,
    -0.5029860367700724,    0.4004672082940195,    0.7504883828755602,   -0.15296486218853164,
    -0.5794684678643381,    0.3239847771997537,    0.6740059517812944,    0.3239847771997537,
    -0.6740059517812944,    0.5794684678643381,   -0.3239847771997537,   -0.3239847771997537,
    -0.7504883828755602,    0.5029860367700724,   -0.4004672082940195,    0.15296486218853164,
    -0.7504883828755602,    0.5029860367700724,    0.15296486218853164,  -0.4004672082940195,
    -0.8828161875373585,    0.4553054119602712,    0.08164729285680945,   0.08164729285680945,
    -0.4553054119602712,    0.8828161875373585,   -0.08164729285680945,  -0.08164729285680945,
    -0.5029860367700724,    0.7504883828755602,   -0.15296486218853164,   0.4004672082940195,
    -0.5029860367700724,    0.7504883828755602,    0.4004672082940195,   -0.15296486218853164,
    -0.5794684678643381,    0.6740059517812944,    0.3239847771997537,    0.3239847771997537,
     0.5794684678643381,   -0.6740059517812944,   -0.3239847771997537,   -0.3239847771997537,
     0.5029860367700724,   -0.7504883828755602,   -0.4004672082940195,    0.15296486218853164,
     0.5029860367700724,   -0.7504883828755602,    0.15296486218853164,  -0.4004672082940195,
     0.4553054119602712,   -0.8828161875373585,    0.08164729285680945,   0.08164729285680945,
     0.8828161875373585,   -0.4553054119602712,   -0.08164729285680945,  -0.08164729285680945,
     0.7504883828755602,   -0.5029860367700724,   -0.15296486218853164,   0.4004672082940195,
     0.7504883828755602,   -0.5029860367700724,    0.4004672082940195,   -0.15296486218853164,
     0.6740059517812944,   -0.5794684678643381,    0.3239847771997537,    0.3239847771997537,
    //---------------------------------------------------------------------------------------//
    -0.753341017856078,    -0.37968289875261624,  -0.37968289875261624,  -0.37968289875261624,
    -0.7821684431180708,   -0.4321472685365301,   -0.4321472685365301,    0.12128480194602098,
    -0.7821684431180708,   -0.4321472685365301,    0.12128480194602098,  -0.4321472685365301,
    -0.7821684431180708,    0.12128480194602098,  -0.4321472685365301,   -0.4321472685365301,
    -0.8586508742123365,   -0.508629699630796,     0.044802370851755174,  0.044802370851755174,
    -0.8586508742123365,    0.044802370851755174, -0.508629699630796,     0.044802370851755174,
    -0.8586508742123365,    0.044802370851755174,  0.044802370851755174, -0.508629699630796,
    -0.9982828964265062,   -0.03381941603233842,  -0.03381941603233842,  -0.03381941603233842,
    -0.37968289875261624,  -0.753341017856078,    -0.37968289875261624,  -0.37968289875261624,
    -0.4321472685365301,   -0.7821684431180708,   -0.4321472685365301,    0.12128480194602098,
    -0.4321472685365301,   -0.7821684431180708,    0.12128480194602098,  -0.4321472685365301,
     0.12128480194602098,  -0.7821684431180708,   -0.4321472685365301,   -0.4321472685365301,
    -0.508629699630796,    -0.8586508742123365,    0.044802370851755174,  0.044802370851755174,
     0.044802370851755174, -0.8586508742123365,   -0.508629699630796,     0.044802370851755174,
     0.044802370851755174, -0.8586508742123365,    0.044802370851755174, -0.508629699630796,
    -0.03381941603233842,  -0.9982828964265062,   -0.03381941603233842,  -0.03381941603233842,
    -0.37968289875261624,  -0.37968289875261624,  -0.753341017856078,    -0.37968289875261624,
    -0.4321472685365301,   -0.4321472685365301,   -0.7821684431180708,    0.12128480194602098,
    -0.4321472685365301,    0.12128480194602098,  -0.7821684431180708,   -0.4321472685365301,
     0.12128480194602098,  -0.4321472685365301,   -0.7821684431180708,   -0.4321472685365301,
    -0.508629699630796,     0.044802370851755174, -0.8586508742123365,    0.044802370851755174,
     0.044802370851755174, -0.508629699630796,    -0.8586508742123365,    0.044802370851755174,
     0.044802370851755174,  0.044802370851755174, -0.8586508742123365,   -0.508629699630796,
    -0.03381941603233842,  -0.03381941603233842,  -0.9982828964265062,   -0.03381941603233842,
    -0.37968289875261624,  -0.37968289875261624,  -0.37968289875261624,  -0.753341017856078,
    -0.4321472685365301,   -0.4321472685365301,    0.12128480194602098,  -0.7821684431180708,
    -0.4321472685365301,    0.12128480194602098,  -0.4321472685365301,   -0.7821684431180708,
     0.12128480194602098,  -0.4321472685365301,   -0.4321472685365301,   -0.7821684431180708,
    -0.508629699630796,     0.044802370851755174,  0.044802370851755174, -0.8586508742123365,
     0.044802370851755174, -0.508629699630796,     0.044802370851755174, -0.8586508742123365,
     0.044802370851755174,  0.044802370851755174, -0.508629699630796,    -0.8586508742123365,
    -0.03381941603233842,  -0.03381941603233842,  -0.03381941603233842,  -0.9982828964265062,
    -0.3239847771997537,   -0.6740059517812944,   -0.3239847771997537,    0.5794684678643381,
    -0.4004672082940195,   -0.7504883828755602,    0.15296486218853164,   0.5029860367700724,
     0.15296486218853164,  -0.7504883828755602,   -0.4004672082940195,    0.5029860367700724,
     0.08164729285680945,  -0.8828161875373585,    0.08164729285680945,   0.4553054119602712,
    -0.08164729285680945,  -0.4553054119602712,   -0.08164729285680945,   0.8828161875373585,
    -0.15296486218853164,  -0.5029860367700724,    0.4004672082940195,    0.7504883828755602,
     0.4004672082940195,   -0.5029860367700724,   -0.15296486218853164,   0.7504883828755602,
     0.3239847771997537,   -0.5794684678643381,    0.3239847771997537,    0.6740059517812944,
    -0.3239847771997537,   -0.3239847771997537,   -0.6740059517812944,    0.5794684678643381,
    -0.4004672082940195,    0.15296486218853164,  -0.7504883828755602,    0.5029860367700724,
     0.15296486218853164,  -0.4004672082940195,   -0.7504883828755602,    0.5029860367700724,
     0.08164729285680945,   0.08164729285680945,  -0.8828161875373585,    0.4553054119602712,
    -0.08164729285680945,  -0.08164729285680945,  -0.4553054119602712,    0.8828161875373585,
    -0.15296486218853164,   0.4004672082940195,   -0.5029860367700724,    0.7504883828755602,
     0.4004672082940195,   -0.15296486218853164,  -0.5029860367700724,    0.7504883828755602,
     0.3239847771997537,    0.3239847771997537,   -0.5794684678643381,    0.6740059517812944,
    -0.3239847771997537,   -0.6740059517812944,    0.5794684678643381,   -0.3239847771997537,
    -0.4004672082940195,   -0.7504883828755602,    0.5029860367700724,    0.15296486218853164,
     0.15296486218853164,  -0.7504883828755602,    0.5029860367700724,   -0.4004672082940195,
     0.08164729285680945,  -0.8828161875373585,    0.4553054119602712,    0.08164729285680945,
    -0.08164729285680945,  -0.4553054119602712,    0.8828161875373585,   -0.08164729285680945,
    -0.15296486218853164,  -0.5029860367700724,    0.7504883828755602,    0.4004672082940195,
     0.4004672082940195,   -0.5029860367700724,    0.7504883828755602,   -0.15296486218853164,
     0.3239847771997537,   -0.5794684678643381,    0.6740059517812944,    0.3239847771997537,
    -0.3239847771997537,   -0.3239847771997537,    0.5794684678643381,   -0.6740059517812944,
    -0.4004672082940195,    0.15296486218853164,   0.5029860367700724,   -0.7504883828755602,
     0.15296486218853164,  -0.4004672082940195,    0.5029860367700724,   -0.7504883828755602,
     0.08164729285680945,   0.08164729285680945,   0.4553054119602712,   -0.8828161875373585,
    -0.08164729285680945,  -0.08164729285680945,   0.8828161875373585,   -0.4553054119602712,
    -0.15296486218853164,   0.4004672082940195,    0.7504883828755602,   -0.5029860367700724,
     0.4004672082940195,   -0.15296486218853164,   0.7504883828755602,   -0.5029860367700724,
     0.3239847771997537,    0.3239847771997537,    0.6740059517812944,   -0.5794684678643381,
    -0.3239847771997537,    0.5794684678643381,   -0.6740059517812944,   -0.3239847771997537,
    -0.4004672082940195,    0.5029860367700724,   -0.7504883828755602,    0.15296486218853164,
     0.15296486218853164,   0.5029860367700724,   -0.7504883828755602,   -0.4004672082940195,
     0.08164729285680945,   0.4553054119602712,   -0.8828161875373585,    0.08164729285680945,
    -0.08164729285680945,   0.8828161875373585,   -0.4553054119602712,   -0.08164729285680945,
    -0.15296486218853164,   0.7504883828755602,   -0.5029860367700724,    0.4004672082940195,
     0.4004672082940195,    0.7504883828755602,   -0.5029860367700724,   -0.15296486218853164,
     0.3239847771997537,    0.6740059517812944,   -0.5794684678643381,    0.3239847771997537,
    -0.3239847771997537,    0.5794684678643381,   -0.3239847771997537,   -0.6740059517812944,
    -0.4004672082940195,    0.5029860367700724,    0.15296486218853164,  -0.7504883828755602,
     0.15296486218853164,   0.5029860367700724,   -0.4004672082940195,   -0.7504883828755602,
     0.08164729285680945,   0.4553054119602712,    0.08164729285680945,  -0.8828161875373585,
    -0.08164729285680945,   0.8828161875373585,   -0.08164729285680945,  -0.4553054119602712,
    -0.15296486218853164,   0.7504883828755602,    0.4004672082940195,   -0.5029860367700724,
     0.4004672082940195,    0.7504883828755602,   -0.15296486218853164,  -0.5029860367700724,
     0.3239847771997537,    0.6740059517812944,    0.3239847771997537,   -0.5794684678643381,
     0.5794684678643381,   -0.3239847771997537,   -0.6740059517812944,   -0.3239847771997537,
     0.5029860367700724,   -0.4004672082940195,   -0.7504883828755602,    0.15296486218853164,
     0.5029860367700724,    0.15296486218853164,  -0.7504883828755602,   -0.4004672082940195,
     0.4553054119602712,    0.08164729285680945,  -0.8828161875373585,    0.08164729285680945,
     0.8828161875373585,   -0.08164729285680945,  -0.4553054119602712,   -0.08164729285680945,
     0.7504883828755602,   -0.15296486218853164,  -0.5029860367700724,    0.4004672082940195,
     0.7504883828755602,    0.4004672082940195,   -0.5029860367700724,   -0.15296486218853164,
     0.6740059517812944,    0.3239847771997537,   -0.5794684678643381,    0.3239847771997537,
     0.5794684678643381,   -0.3239847771997537,   -0.3239847771997537,   -0.6740059517812944,
     0.5029860367700724,   -0.4004672082940195,    0.15296486218853164,  -0.7504883828755602,
     0.5029860367700724,    0.15296486218853164,  -0.4004672082940195,   -0.7504883828755602,
     0.4553054119602712,    0.08164729285680945,   0.08164729285680945,  -0.8828161875373585,
     0.8828161875373585,   -0.08164729285680945,  -0.08164729285680945,  -0.4553054119602712,
     0.7504883828755602,   -0.15296486218853164,   0.4004672082940195,   -0.5029860367700724,
     0.7504883828755602,    0.4004672082940195,   -0.15296486218853164,  -0.5029860367700724,
     0.6740059517812944,    0.3239847771997537,    0.3239847771997537,   -0.5794684678643381,
     0.03381941603233842,   0.03381941603233842,   0.03381941603233842,   0.9982828964265062,
    -0.044802370851755174, -0.044802370851755174,  0.508629699630796,     0.8586508742123365,
    -0.044802370851755174,  0.508629699630796,    -0.044802370851755174,  0.8586508742123365,
    -0.12128480194602098,   0.4321472685365301,    0.4321472685365301,    0.7821684431180708,
     0.508629699630796,    -0.044802370851755174, -0.044802370851755174,  0.8586508742123365,
     0.4321472685365301,   -0.12128480194602098,   0.4321472685365301,    0.7821684431180708,
     0.4321472685365301,    0.4321472685365301,   -0.12128480194602098,   0.7821684431180708,
     0.37968289875261624,   0.37968289875261624,   0.37968289875261624,   0.753341017856078,
     0.03381941603233842,   0.03381941603233842,   0.9982828964265062,    0.03381941603233842,
    -0.044802370851755174,  0.044802370851755174,  0.8586508742123365,    0.508629699630796,
    -0.044802370851755174,  0.508629699630796,     0.8586508742123365,   -0.044802370851755174,
    -0.12128480194602098,   0.4321472685365301,    0.7821684431180708,    0.4321472685365301,
     0.508629699630796,    -0.044802370851755174,  0.8586508742123365,   -0.044802370851755174,
     0.4321472685365301,   -0.12128480194602098,   0.7821684431180708,    0.4321472685365301,
     0.4321472685365301,    0.4321472685365301,    0.7821684431180708,   -0.12128480194602098,
     0.37968289875261624,   0.37968289875261624,   0.753341017856078,     0.37968289875261624,
     0.03381941603233842,   0.9982828964265062,    0.03381941603233842,   0.03381941603233842,
    -0.044802370851755174,  0.8586508742123365,   -0.044802370851755174,  0.508629699630796,
    -0.044802370851755174,  0.8586508742123365,    0.508629699630796,    -0.044802370851755174,
    -0.12128480194602098,   0.7821684431180708,    0.4321472685365301,    0.4321472685365301,
     0.508629699630796,     0.8586508742123365,   -0.044802370851755174, -0.044802370851755174,
     0.4321472685365301,    0.7821684431180708,   -0.12128480194602098,   0.4321472685365301,
     0.4321472685365301,    0.7821684431180708,    0.4321472685365301,   -0.12128480194602098,
     0.37968289875261624,   0.753341017856078,     0.37968289875261624,   0.37968289875261624,
     0.9982828964265062,    0.03381941603233842,   0.03381941603233842,   0.03381941603233842,
     0.8586508742123365,   -0.044802370851755174, -0.044802370851755174,  0.508629699630796,
     0.8586508742123365,   -0.044802370851755174,  0.508629699630796,    -0.044802370851755174,
     0.7821684431180708,   -0.12128480194602098,   0.4321472685365301,    0.4321472685365301,
     0.8586508742123365,    0.508629699630796,    -0.044802370851755174, -0.044802370851755174,
     0.7821684431180708,    0.4321472685365301,   -0.12128480194602098,   0.4321472685365301,
     0.7821684431180708,    0.4321472685365301,    0.4321472685365301,   -0.12128480194602098,
     0.753341017856078,     0.37968289875261624,   0.37968289875261624,   0.37968289875261624
};

const std::array<std::vector<int>, 256> opensimplex::LOOKUP_4D_VERTEX_CODES_ = {{
    {0x15, 0x45, 0x51, 0x54, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA},
    {0x15, 0x45, 0x51, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x6A, 0x95, 0x96, 0x9A, 0xA6, 0xAA},
    {0x01, 0x05, 0x11, 0x15, 0x41, 0x45, 0x51, 0x55, 0x56, 0x5A, 0x66, 0x6A, 0x96, 0x9A, 0xA6, 0xAA},
    {0x01, 0x15, 0x16, 0x45, 0x46, 0x51, 0x52, 0x55, 0x56, 0x5A, 0x66, 0x6A, 0x96, 0x9A, 0xA6, 0xAA, 0xAB},
    {0x15, 0x45, 0x54, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x69, 0x6A, 0x95, 0x99, 0x9A, 0xA9, 0xAA},
    {0x05, 0x15, 0x45, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0x95, 0x96, 0x99, 0x9A, 0xAA},
    {0x05, 0x15, 0x45, 0x55, 0x56, 0x59, 0x5A, 0x66, 0x6A, 0x96, 0x9A, 0xAA},
    {0x05, 0x15, 0x16, 0x45, 0x46, 0x55, 0x56, 0x59, 0x5A, 0x66, 0x6A, 0x96, 0x9A, 0xAA, 0xAB},
    {0x04, 0x05, 0x14, 0x15, 0x44, 0x45, 0x54, 0x55, 0x59, 0x5A, 0x69, 0x6A, 0x99, 0x9A, 0xA9, 0xAA},
    {0x05, 0x15, 0x45, 0x55, 0x56, 0x59, 0x5A, 0x69, 0x6A, 0x99, 0x9A, 0xAA},
    {0x05, 0x15, 0x45, 0x55, 0x56, 0x59, 0x5A, 0x6A, 0x9A, 0xAA},
    {0x05, 0x15, 0x16, 0x45, 0x46, 0x55, 0x56, 0x59, 0x5A, 0x5B, 0x6A, 0x9A, 0xAA, 0xAB},
    {0x04, 0x15, 0x19, 0x45, 0x49, 0x54, 0x55, 0x58, 0x59, 0x5A, 0x69, 0x6A, 0x99, 0x9A, 0xA9, 0xAA, 0xAE},
    {0x05, 0x15, 0x19, 0x45, 0x49, 0x55, 0x56, 0x59, 0x5A, 0x69, 0x6A, 0x99, 0x9A, 0xAA, 0xAE},
    {0x05, 0x15, 0x19, 0x45, 0x49, 0x55, 0x56, 0x59, 0x5A, 0x5E, 0x6A, 0x9A, 0xAA, 0xAE},
    {0x05, 0x15, 0x1A, 0x45, 0x4A, 0x55, 0x56, 0x59, 0x5A, 0x5B, 0x5E, 0x6A, 0x9A, 0xAA, 0xAB, 0xAE, 0xAF},
    {0x15, 0x51, 0x54, 0x55, 0x56, 0x59, 0x65, 0x66, 0x69, 0x6A, 0x95, 0xA5, 0xA6, 0xA9, 0xAA},
    {0x11, 0x15, 0x51, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0x95, 0x96, 0xA5, 0xA6, 0xAA},
    {0x11, 0x15, 0x51, 0x55, 0x56, 0x5A, 0x65, 0x66, 0x6A, 0x96, 0xA6, 0xAA},
    {0x11, 0x15, 0x16, 0x51, 0x52, 0x55, 0x56, 0x5A, 0x65, 0x66, 0x6A, 0x96, 0xA6, 0xAA, 0xAB},
    {0x14, 0x15, 0x54, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0x95, 0x99, 0xA5, 0xA9, 0xAA},
    {0x15, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0x95, 0x9A, 0xA6, 0xA9, 0xAA},
    {0x15, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0x96, 0x9A, 0xA6, 0xAA, 0xAB},
    {0x15, 0x16, 0x55, 0x56, 0x5A, 0x66, 0x6A, 0x6B, 0x96, 0x9A, 0xA6, 0xAA, 0xAB},
    {0x14, 0x15, 0x54, 0x55, 0x59, 0x5A, 0x65, 0x69, 0x6A, 0x99, 0xA9, 0xAA},
    {0x15, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0x99, 0x9A, 0xA9, 0xAA, 0xAE},
    {0x15, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0x9A, 0xAA},
    {0x15, 0x16, 0x55, 0x56, 0x59, 0x5A, 0x66, 0x6A, 0x6B, 0x9A, 0xAA, 0xAB},
    {0x14, 0x15, 0x19, 0x54, 0x55, 0x58, 0x59, 0x5A, 0x65, 0x69, 0x6A, 0x99, 0xA9, 0xAA, 0xAE},
    {0x15, 0x19, 0x55, 0x59, 0x5A, 0x69, 0x6A, 0x6E, 0x99, 0x9A, 0xA9, 0xAA, 0xAE},
    {0x15, 0x19, 0x55, 0x56, 0x59, 0x5A, 0x69, 0x6A, 0x6E, 0x9A, 0xAA, 0xAE},
    {0x15, 0x1A, 0x55, 0x56, 0x59, 0x5A, 0x6A, 0x6B, 0x6E, 0x9A, 0xAA, 0xAB, 0xAE, 0xAF},
    {0x10, 0x11, 0x14, 0x15, 0x50, 0x51, 0x54, 0x55, 0x65, 0x66, 0x69, 0x6A, 0xA5, 0xA6, 0xA9, 0xAA},
    {0x11, 0x15, 0x51, 0x55, 0x56, 0x65, 0x66, 0x69, 0x6A, 0xA5, 0xA6, 0xAA},
    {0x11, 0x15, 0x51, 0x55, 0x56, 0x65, 0x66, 0x6A, 0xA6, 0xAA},
    {0x11, 0x15, 0x16, 0x51, 0x52, 0x55, 0x56, 0x65, 0x66, 0x67, 0x6A, 0xA6, 0xAA, 0xAB},
    {0x14, 0x15, 0x54, 0x55, 0x59, 0x65, 0x66, 0x69, 0x6A, 0xA5, 0xA9, 0xAA},
    {0x15, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0xA5, 0xA6, 0xA9, 0xAA, 0xBA},
    {0x15, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0xA6, 0xAA},
    {0x15, 0x16, 0x55, 0x56, 0x5A, 0x65, 0x66, 0x6A, 0x6B, 0xA6, 0xAA, 0xAB},
    {0x14, 0x15, 0x54, 0x55, 0x59, 0x65, 0x69, 0x6A, 0xA9, 0xAA},
    {0x15, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0xA9, 0xAA},
    {0x15, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0xAA},
    {0x15, 0x16, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0x6B, 0xAA, 0xAB},
    {0x14, 0x15, 0x19, 0x54, 0x55, 0x58, 0x59, 0x65, 0x69, 0x6A, 0x6D, 0xA9, 0xAA, 0xAE},
    {0x15, 0x19, 0x55, 0x59, 0x5A, 0x65, 0x69, 0x6A, 0x6E, 0xA9, 0xAA, 0xAE},
    {0x15, 0x19, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0x6E, 0xAA, 0xAE},
    {0x15, 0x55, 0x56, 0x59, 0x5A, 0x66, 0x69, 0x6A, 0x6B, 0x6E, 0x9A, 0xAA, 0xAB, 0xAE, 0xAF},
    {0x10, 0x15, 0x25, 0x51, 0x54, 0x55, 0x61, 0x64, 0x65, 0x66, 0x69, 0x6A, 0xA5, 0xA6, 0xA9, 0xAA, 0xBA},
    {0x11, 0x15, 0x25, 0x51, 0x55, 0x56, 0x61, 0x65, 0x66, 0x69, 0x6A, 0xA5, 0xA6, 0xAA, 0xBA},
    {0x11, 0x15, 0x25, 0x51, 0x55, 0x56, 0x61, 0x65, 0x66, 0x6A, 0x76, 0xA6, 0xAA, 0xBA},
    {0x11, 0x15, 0x26, 0x51, 0x55, 0x56, 0x62, 0x65, 0x66, 0x67, 0x6A, 0x76, 0xA6, 0xAA, 0xAB, 0xBA, 0xBB},
    {0x14, 0x15, 0x25, 0x54, 0x55, 0x59, 0x64, 0x65, 0x66, 0x69, 0x6A, 0xA5, 0xA9, 0xAA, 0xBA},
    {0x15, 0x25, 0x55, 0x65, 0x66, 0x69, 0x6A, 0x7A, 0xA5, 0xA6, 0xA9, 0xAA, 0xBA},
    {0x15, 0x25, 0x55, 0x56, 0x65, 0x66, 0x69, 0x6A, 0x7A, 0xA6, 0xAA, 0xBA},
    {0x15, 0x26, 0x55, 0x56, 0x65, 0x66, 0x6A, 0x6B, 0x7A, 0xA6, 0xAA, 0xAB, 0xBA, 0xBB},
    {0x14, 0x15, 0x25, 0x54, 0x55, 0x59, 0x64, 0x65, 0x69, 0x6A, 0x79, 0xA9, 0xAA, 0xBA},
    {0x15, 0x25, 0x55, 0x59, 0x65, 0x66, 0x69, 0x6A, 0x7A, 0xA9, 0xAA, 0xBA},
    {0x15, 0x25, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0x7A, 0xAA, 0xBA},
    {0x15, 0x55, 0x56, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0x6B, 0x7A, 0xA6, 0xAA, 0xAB, 0xBA, 0xBB},
    {0x14, 0x15, 0x29, 0x54, 0x55, 0x59, 0x65, 0x68, 0x69, 0x6A, 0x6D, 0x79, 0xA9, 0xAA, 0xAE, 0xBA, 0xBE},
    {0x15, 0x29, 0x55, 0x59, 0x65, 0x69, 0x6A, 0x6E, 0x7A, 0xA9, 0xAA, 0xAE, 0xBA, 0xBE},
    {0x15, 0x55, 0x59, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0x6E, 0x7A, 0xA9, 0xAA, 0xAE, 0xBA, 0xBE},
    {0x15, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0x6B, 0x6E, 0x7A, 0xAA, 0xAB, 0xAE, 0xBA, 0xBF},
    {0x45, 0x51, 0x54, 0x55, 0x56, 0x59, 0x65, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA},
    {0x41, 0x45, 0x51, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA6, 0xAA},
    {0x41, 0x45, 0x51, 0x55, 0x56, 0x5A, 0x66, 0x95, 0x96, 0x9A, 0xA6, 0xAA},
    {0x41, 0x45, 0x46, 0x51, 0x52, 0x55, 0x56, 0x5A, 0x66, 0x95, 0x96, 0x9A, 0xA6, 0xAA, 0xAB},
    {0x44, 0x45, 0x54, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x69, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA9, 0xAA},
    {0x45, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x6A, 0x95, 0x96, 0x99, 0x9A, 0xA6, 0xA9, 0xAA},
    {0x45, 0x55, 0x56, 0x59, 0x5A, 0x66, 0x6A, 0x95, 0x96, 0x99, 0x9A, 0xA6, 0xAA, 0xAB},
    {0x45, 0x46, 0x55, 0x56, 0x5A, 0x66, 0x6A, 0x96, 0x9A, 0x9B, 0xA6, 0xAA, 0xAB},
    {0x44, 0x45, 0x54, 0x55, 0x59, 0x5A, 0x69, 0x95, 0x99, 0x9A, 0xA9, 0xAA},
    {0x45, 0x55, 0x56, 0x59, 0x5A, 0x69, 0x6A, 0x95, 0x96, 0x99, 0x9A, 0xA9, 0xAA, 0xAE},
    {0x45, 0x55, 0x56, 0x59, 0x5A, 0x6A, 0x95, 0x96, 0x99, 0x9A, 0xAA},
    {0x45, 0x46, 0x55, 0x56, 0x59, 0x5A, 0x6A, 0x96, 0x9A, 0x9B, 0xAA, 0xAB},
    {0x44, 0x45, 0x49, 0x54, 0x55, 0x58, 0x59, 0x5A, 0x69, 0x95, 0x99, 0x9A, 0xA9, 0xAA, 0xAE},
    {0x45, 0x49, 0x55, 0x59, 0x5A, 0x69, 0x6A, 0x99, 0x9A, 0x9E, 0xA9, 0xAA, 0xAE},
    {0x45, 0x49, 0x55, 0x56, 0x59, 0x5A, 0x6A, 0x99, 0x9A, 0x9E, 0xAA, 0xAE},
    {0x45, 0x4A, 0x55, 0x56, 0x59, 0x5A, 0x6A, 0x9A, 0x9B, 0x9E, 0xAA, 0xAB, 0xAE, 0xAF},
    {0x50, 0x51, 0x54, 0x55, 0x56, 0x59, 0x65, 0x66, 0x69, 0x95, 0x96, 0x99, 0xA5, 0xA6, 0xA9, 0xAA},
    {0x51, 0x55, 0x56, 0x59, 0x65, 0x66, 0x6A, 0x95, 0x96, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA},
    {0x51, 0x55, 0x56, 0x5A, 0x65, 0x66, 0x6A, 0x95, 0x96, 0x9A, 0xA5, 0xA6, 0xAA, 0xAB},
    {0x51, 0x52, 0x55, 0x56, 0x5A, 0x66, 0x6A, 0x96, 0x9A, 0xA6, 0xA7, 0xAA, 0xAB},
    {0x54, 0x55, 0x56, 0x59, 0x65, 0x69, 0x6A, 0x95, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA},
    {0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA},
    {0x15, 0x45, 0x51, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x6A, 0x95, 0x96, 0x9A, 0xA6, 0xAA, 0xAB},
    {0x55, 0x56, 0x5A, 0x66, 0x6A, 0x96, 0x9A, 0xA6, 0xAA, 0xAB},
    {0x54, 0x55, 0x59, 0x5A, 0x65, 0x69, 0x6A, 0x95, 0x99, 0x9A, 0xA5, 0xA9, 0xAA, 0xAE},
    {0x15, 0x45, 0x54, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x69, 0x6A, 0x95, 0x99, 0x9A, 0xA9, 0xAA, 0xAE},
    {0x15, 0x45, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0x95, 0x96, 0x99, 0x9A, 0xA6, 0xA9, 0xAA, 0xAB, 0xAE},
    {0x55, 0x56, 0x59, 0x5A, 0x66, 0x6A, 0x96, 0x9A, 0xA6, 0xAA, 0xAB},
    {0x54, 0x55, 0x58, 0x59, 0x5A, 0x69, 0x6A, 0x99, 0x9A, 0xA9, 0xAA, 0xAD, 0xAE},
    {0x55, 0x59, 0x5A, 0x69, 0x6A, 0x99, 0x9A, 0xA9, 0xAA, 0xAE},
    {0x55, 0x56, 0x59, 0x5A, 0x69, 0x6A, 0x99, 0x9A, 0xA9, 0xAA, 0xAE},
    {0x55, 0x56, 0x59, 0x5A, 0x6A, 0x9A, 0xAA, 0xAB, 0xAE, 0xAF},
    {0x50, 0x51, 0x54, 0x55, 0x65, 0x66, 0x69, 0x95, 0xA5, 0xA6, 0xA9, 0xAA},
    {0x51, 0x55, 0x56, 0x65, 0x66, 0x69, 0x6A, 0x95, 0x96, 0xA5, 0xA6, 0xA9, 0xAA, 0xBA},
    {0x51, 0x55, 0x56, 0x65, 0x66, 0x6A, 0x95, 0x96, 0xA5, 0xA6, 0xAA},
    {0x51, 0x52, 0x55, 0x56, 0x65, 0x66, 0x6A, 0x96, 0xA6, 0xA7, 0xAA, 0xAB},
    {0x54, 0x55, 0x59, 0x65, 0x66, 0x69, 0x6A, 0x95, 0x99, 0xA5, 0xA6, 0xA9, 0xAA, 0xBA},
    {0x15, 0x51, 0x54, 0x55, 0x56, 0x59, 0x65, 0x66, 0x69, 0x6A, 0x95, 0xA5, 0xA6, 0xA9, 0xAA, 0xBA},
    {0x15, 0x51, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0x95, 0x96, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA, 0xAB, 0xBA},
    {0x55, 0x56, 0x5A, 0x65, 0x66, 0x6A, 0x96, 0x9A, 0xA6, 0xAA, 0xAB},
    {0x54, 0x55, 0x59, 0x65, 0x69, 0x6A, 0x95, 0x99, 0xA5, 0xA9, 0xAA},
    {0x15, 0x54, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0x95, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA, 0xAE, 0xBA},
    {0x15, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0x9A, 0xA6, 0xA9, 0xAA},
    {0x15, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0x96, 0x9A, 0xA6, 0xAA, 0xAB},
    {0x54, 0x55, 0x58, 0x59, 0x65, 0x69, 0x6A, 0x99, 0xA9, 0xAA, 0xAD, 0xAE},
    {0x55, 0x59, 0x5A, 0x65, 0x69, 0x6A, 0x99, 0x9A, 0xA9, 0xAA, 0xAE},
    {0x15, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0x99, 0x9A, 0xA9, 0xAA, 0xAE},
    {0x15, 0x55, 0x56, 0x59, 0x5A, 0x66, 0x69, 0x6A, 0x9A, 0xAA, 0xAB, 0xAE, 0xAF},
    {0x50, 0x51, 0x54, 0x55, 0x61, 0x64, 0x65, 0x66, 0x69, 0x95, 0xA5, 0xA6, 0xA9, 0xAA, 0xBA},
    {0x51, 0x55, 0x61, 0x65, 0x66, 0x69, 0x6A, 0xA5, 0xA6, 0xA9, 0xAA, 0xB6, 0xBA},
    {0x51, 0x55, 0x56, 0x61, 0x65, 0x66, 0x6A, 0xA5, 0xA6, 0xAA, 0xB6, 0xBA},
    {0x51, 0x55, 0x56, 0x62, 0x65, 0x66, 0x6A, 0xA6, 0xA7, 0xAA, 0xAB, 0xB6, 0xBA, 0xBB},
    {0x54, 0x55, 0x64, 0x65, 0x66, 0x69, 0x6A, 0xA5, 0xA6, 0xA9, 0xAA, 0xB9, 0xBA},
    {0x55, 0x65, 0x66, 0x69, 0x6A, 0xA5, 0xA6, 0xA9, 0xAA, 0xBA},
    {0x55, 0x56, 0x65, 0x66, 0x69, 0x6A, 0xA5, 0xA6, 0xA9, 0xAA, 0xBA},
    {0x55, 0x56, 0x65, 0x66, 0x6A, 0xA6, 0xAA, 0xAB, 0xBA, 0xBB},
    {0x54, 0x55, 0x59, 0x64, 0x65, 0x69, 0x6A, 0xA5, 0xA9, 0xAA, 0xB9, 0xBA},
    {0x55, 0x59, 0x65, 0x66, 0x69, 0x6A, 0xA5, 0xA6, 0xA9, 0xAA, 0xBA},
    {0x15, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0xA5, 0xA6, 0xA9, 0xAA, 0xBA},
    {0x15, 0x55, 0x56, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0xA6, 0xAA, 0xAB, 0xBA, 0xBB},
    {0x54, 0x55, 0x59, 0x65, 0x68, 0x69, 0x6A, 0xA9, 0xAA, 0xAD, 0xAE, 0xB9, 0xBA, 0xBE},
    {0x55, 0x59, 0x65, 0x69, 0x6A, 0xA9, 0xAA, 0xAE, 0xBA, 0xBE},
    {0x15, 0x55, 0x59, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0xA9, 0xAA, 0xAE, 0xBA, 0xBE},
    {0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0xAA, 0xAB, 0xAE, 0xBA, 0xBF},
    {0x40, 0x41, 0x44, 0x45, 0x50, 0x51, 0x54, 0x55, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA},
    {0x41, 0x45, 0x51, 0x55, 0x56, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA6, 0xAA},
    {0x41, 0x45, 0x51, 0x55, 0x56, 0x95, 0x96, 0x9A, 0xA6, 0xAA},
    {0x41, 0x45, 0x46, 0x51, 0x52, 0x55, 0x56, 0x95, 0x96, 0x97, 0x9A, 0xA6, 0xAA, 0xAB},
    {0x44, 0x45, 0x54, 0x55, 0x59, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA9, 0xAA},
    {0x45, 0x55, 0x56, 0x59, 0x5A, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA, 0xEA},
    {0x45, 0x55, 0x56, 0x59, 0x5A, 0x95, 0x96, 0x99, 0x9A, 0xA6, 0xAA},
    {0x45, 0x46, 0x55, 0x56, 0x5A, 0x95, 0x96, 0x9A, 0x9B, 0xA6, 0xAA, 0xAB},
    {0x44, 0x45, 0x54, 0x55, 0x59, 0x95, 0x99, 0x9A, 0xA9, 0xAA},
    {0x45, 0x55, 0x56, 0x59, 0x5A, 0x95, 0x96, 0x99, 0x9A, 0xA9, 0xAA},
    {0x45, 0x55, 0x56, 0x59, 0x5A, 0x95, 0x96, 0x99, 0x9A, 0xAA},
    {0x45, 0x46, 0x55, 0x56, 0x59, 0x5A, 0x95, 0x96, 0x99, 0x9A, 0x9B, 0xAA, 0xAB},
    {0x44, 0x45, 0x49, 0x54, 0x55, 0x58, 0x59, 0x95, 0x99, 0x9A, 0x9D, 0xA9, 0xAA, 0xAE},
    {0x45, 0x49, 0x55, 0x59, 0x5A, 0x95, 0x99, 0x9A, 0x9E, 0xA9, 0xAA, 0xAE},
    {0x45, 0x49, 0x55, 0x56, 0x59, 0x5A, 0x95, 0x96, 0x99, 0x9A, 0x9E, 0xAA, 0xAE},
    {0x45, 0x55, 0x56, 0x59, 0x5A, 0x6A, 0x96, 0x99, 0x9A, 0x9B, 0x9E, 0xAA, 0xAB, 0xAE, 0xAF},
    {0x50, 0x51, 0x54, 0x55, 0x65, 0x95, 0x96, 0x99, 0xA5, 0xA6, 0xA9, 0xAA},
    {0x51, 0x55, 0x56, 0x65, 0x66, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA, 0xEA},
    {0x51, 0x55, 0x56, 0x65, 0x66, 0x95, 0x96, 0x9A, 0xA5, 0xA6, 0xAA},
    {0x51, 0x52, 0x55, 0x56, 0x66, 0x95, 0x96, 0x9A, 0xA6, 0xA7, 0xAA, 0xAB},
    {0x54, 0x55, 0x59, 0x65, 0x69, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA, 0xEA},
    {0x45, 0x51, 0x54, 0x55, 0x56, 0x59, 0x65, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA, 0xEA},
    {0x45, 0x51, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x6A, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA, 0xAB, 0xEA},
    {0x55, 0x56, 0x5A, 0x66, 0x6A, 0x95, 0x96, 0x9A, 0xA6, 0xAA, 0xAB},
    {0x54, 0x55, 0x59, 0x65, 0x69, 0x95, 0x99, 0x9A, 0xA5, 0xA9, 0xAA},
    {0x45, 0x54, 0x55, 0x56, 0x59, 0x5A, 0x65, 0x69, 0x6A, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA, 0xAE, 0xEA},
    {0x45, 0x55, 0x56, 0x59, 0x5A, 0x6A, 0x95, 0x96, 0x99, 0x9A, 0xA6, 0xA9, 0xAA},
    {0x45, 0x55, 0x56, 0x59, 0x5A, 0x66, 0x6A, 0x95, 0x96, 0x99, 0x9A, 0xA6, 0xAA, 0xAB},
    {0x54, 0x55, 0x58, 0x59, 0x69, 0x95, 0x99, 0x9A, 0xA9, 0xAA, 0xAD, 0xAE},
    {0x55, 0x59, 0x5A, 0x69, 0x6A, 0x95, 0x99, 0x9A, 0xA9, 0xAA, 0xAE},
    {0x45, 0x55, 0x56, 0x59, 0x5A, 0x69, 0x6A, 0x95, 0x96, 0x99, 0x9A, 0xA9, 0xAA, 0xAE},
    {0x45, 0x55, 0x56, 0x59, 0x5A, 0x6A, 0x96, 0x99, 0x9A, 0xAA, 0xAB, 0xAE, 0xAF},
    {0x50, 0x51, 0x54, 0x55, 0x65, 0x95, 0xA5, 0xA6, 0xA9, 0xAA},
    {0x51, 0x55, 0x56, 0x65, 0x66, 0x95, 0x96, 0xA5, 0xA6, 0xA9, 0xAA},
    {0x51, 0x55, 0x56, 0x65, 0x66, 0x95, 0x96, 0xA5, 0xA6, 0xAA},
    {0x51, 0x52, 0x55, 0x56, 0x65, 0x66, 0x95, 0x96, 0xA5, 0xA6, 0xA7, 0xAA, 0xAB},
    {0x54, 0x55, 0x59, 0x65, 0x69, 0x95, 0x99, 0xA5, 0xA6, 0xA9, 0xAA},
    {0x51, 0x54, 0x55, 0x56, 0x59, 0x65, 0x66, 0x69, 0x6A, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA, 0xBA, 0xEA},
    {0x51, 0x55, 0x56, 0x65, 0x66, 0x6A, 0x95, 0x96, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA},
    {0x51, 0x55, 0x56, 0x5A, 0x65, 0x66, 0x6A, 0x95, 0x96, 0x9A, 0xA5, 0xA6, 0xAA, 0xAB},
    {0x54, 0x55, 0x59, 0x65, 0x69, 0x95, 0x99, 0xA5, 0xA9, 0xAA},
    {0x54, 0x55, 0x59, 0x65, 0x69, 0x6A, 0x95, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA},
    {0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA},
    {0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x6A, 0x95, 0x96, 0x9A, 0xA6, 0xA9, 0xAA, 0xAB},
    {0x54, 0x55, 0x58, 0x59, 0x65, 0x69, 0x95, 0x99, 0xA5, 0xA9, 0xAA, 0xAD, 0xAE},
    {0x54, 0x55, 0x59, 0x5A, 0x65, 0x69, 0x6A, 0x95, 0x99, 0x9A, 0xA5, 0xA9, 0xAA, 0xAE},
    {0x55, 0x56, 0x59, 0x5A, 0x65, 0x69, 0x6A, 0x95, 0x99, 0x9A, 0xA6, 0xA9, 0xAA, 0xAE},
    {0x55, 0x56, 0x59, 0x5A, 0x66, 0x69, 0x6A, 0x96, 0x99, 0x9A, 0xA6, 0xA9, 0xAA, 0xAB, 0xAE, 0xAF},
    {0x50, 0x51, 0x54, 0x55, 0x61, 0x64, 0x65, 0x95, 0xA5, 0xA6, 0xA9, 0xAA, 0xB5, 0xBA},
    {0x51, 0x55, 0x61, 0x65, 0x66, 0x95, 0xA5, 0xA6, 0xA9, 0xAA, 0xB6, 0xBA},
    {0x51, 0x55, 0x56, 0x61, 0x65, 0x66, 0x95, 0x96, 0xA5, 0xA6, 0xAA, 0xB6, 0xBA},
    {0x51, 0x55, 0x56, 0x65, 0x66, 0x6A, 0x96, 0xA5, 0xA6, 0xA7, 0xAA, 0xAB, 0xB6, 0xBA, 0xBB},
    {0x54, 0x55, 0x64, 0x65, 0x69, 0x95, 0xA5, 0xA6, 0xA9, 0xAA, 0xB9, 0xBA},
    {0x55, 0x65, 0x66, 0x69, 0x6A, 0x95, 0xA5, 0xA6, 0xA9, 0xAA, 0xBA},
    {0x51, 0x55, 0x56, 0x65, 0x66, 0x69, 0x6A, 0x95, 0x96, 0xA5, 0xA6, 0xA9, 0xAA, 0xBA},
    {0x51, 0x55, 0x56, 0x65, 0x66, 0x6A, 0x96, 0xA5, 0xA6, 0xAA, 0xAB, 0xBA, 0xBB},
    {0x54, 0x55, 0x59, 0x64, 0x65, 0x69, 0x95, 0x99, 0xA5, 0xA9, 0xAA, 0xB9, 0xBA},
    {0x54, 0x55, 0x59, 0x65, 0x66, 0x69, 0x6A, 0x95, 0x99, 0xA5, 0xA6, 0xA9, 0xAA, 0xBA},
    {0x55, 0x56, 0x59, 0x65, 0x66, 0x69, 0x6A, 0x95, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA, 0xBA},
    {0x55, 0x56, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0x96, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA, 0xAB, 0xBA, 0xBB},
    {0x54, 0x55, 0x59, 0x65, 0x69, 0x6A, 0x99, 0xA5, 0xA9, 0xAA, 0xAD, 0xAE, 0xB9, 0xBA, 0xBE},
    {0x54, 0x55, 0x59, 0x65, 0x69, 0x6A, 0x99, 0xA5, 0xA9, 0xAA, 0xAE, 0xBA, 0xBE},
    {0x55, 0x59, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA, 0xAE, 0xBA, 0xBE},
    {0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0x9A, 0xA6, 0xA9, 0xAA, 0xAB, 0xAE, 0xBA},
    {0x40, 0x45, 0x51, 0x54, 0x55, 0x85, 0x91, 0x94, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA, 0xEA},
    {0x41, 0x45, 0x51, 0x55, 0x56, 0x85, 0x91, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA6, 0xAA, 0xEA},
    {0x41, 0x45, 0x51, 0x55, 0x56, 0x85, 0x91, 0x95, 0x96, 0x9A, 0xA6, 0xAA, 0xD6, 0xEA},
    {0x41, 0x45, 0x51, 0x55, 0x56, 0x86, 0x92, 0x95, 0x96, 0x97, 0x9A, 0xA6, 0xAA, 0xAB, 0xD6, 0xEA, 0xEB},
    {0x44, 0x45, 0x54, 0x55, 0x59, 0x85, 0x94, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA9, 0xAA, 0xEA},
    {0x45, 0x55, 0x85, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA, 0xDA, 0xEA},
    {0x45, 0x55, 0x56, 0x85, 0x95, 0x96, 0x99, 0x9A, 0xA6, 0xAA, 0xDA, 0xEA},
    {0x45, 0x55, 0x56, 0x86, 0x95, 0x96, 0x9A, 0x9B, 0xA6, 0xAA, 0xAB, 0xDA, 0xEA, 0xEB},
    {0x44, 0x45, 0x54, 0x55, 0x59, 0x85, 0x94, 0x95, 0x99, 0x9A, 0xA9, 0xAA, 0xD9, 0xEA},
    {0x45, 0x55, 0x59, 0x85, 0x95, 0x96, 0x99, 0x9A, 0xA9, 0xAA, 0xDA, 0xEA},
    {0x45, 0x55, 0x56, 0x59, 0x5A, 0x85, 0x95, 0x96, 0x99, 0x9A, 0xAA, 0xDA, 0xEA},
    {0x45, 0x55, 0x56, 0x5A, 0x95, 0x96, 0x99, 0x9A, 0x9B, 0xA6, 0xAA, 0xAB, 0xDA, 0xEA, 0xEB},
    {0x44, 0x45, 0x54, 0x55, 0x59, 0x89, 0x95, 0x98, 0x99, 0x9A, 0x9D, 0xA9, 0xAA, 0xAE, 0xD9, 0xEA, 0xEE},
    {0x45, 0x55, 0x59, 0x89, 0x95, 0x99, 0x9A, 0x9E, 0xA9, 0xAA, 0xAE, 0xDA, 0xEA, 0xEE},
    {0x45, 0x55, 0x59, 0x5A, 0x95, 0x96, 0x99, 0x9A, 0x9E, 0xA9, 0xAA, 0xAE, 0xDA, 0xEA, 0xEE},
    {0x45, 0x55, 0x56, 0x59, 0x5A, 0x95, 0x96, 0x99, 0x9A, 0x9B, 0x9E, 0xAA, 0xAB, 0xAE, 0xDA, 0xEA, 0xEF},
    {0x50, 0x51, 0x54, 0x55, 0x65, 0x91, 0x94, 0x95, 0x96, 0x99, 0xA5, 0xA6, 0xA9, 0xAA, 0xEA},
    {0x51, 0x55, 0x91, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA, 0xE6, 0xEA},
    {0x51, 0x55, 0x56, 0x91, 0x95, 0x96, 0x9A, 0xA5, 0xA6, 0xAA, 0xE6, 0xEA},
    {0x51, 0x55, 0x56, 0x92, 0x95, 0x96, 0x9A, 0xA6, 0xA7, 0xAA, 0xAB, 0xE6, 0xEA, 0xEB},
    {0x54, 0x55, 0x94, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA, 0xE9, 0xEA},
    {0x55, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA, 0xEA},
    {0x55, 0x56, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA, 0xEA},
    {0x55, 0x56, 0x95, 0x96, 0x9A, 0xA6, 0xAA, 0xAB, 0xEA, 0xEB},
    {0x54, 0x55, 0x59, 0x94, 0x95, 0x99, 0x9A, 0xA5, 0xA9, 0xAA, 0xE9, 0xEA},
    {0x55, 0x59, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA, 0xEA},
    {0x45, 0x55, 0x56, 0x59, 0x5A, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA, 0xEA},
    {0x45, 0x55, 0x56, 0x5A, 0x95, 0x96, 0x99, 0x9A, 0xA6, 0xAA, 0xAB, 0xEA, 0xEB},
    {0x54, 0x55, 0x59, 0x95, 0x98, 0x99, 0x9A, 0xA9, 0xAA, 0xAD, 0xAE, 0xE9, 0xEA, 0xEE},
    {0x55, 0x59, 0x95, 0x99, 0x9A, 0xA9, 0xAA, 0xAE, 0xEA, 0xEE},
    {0x45, 0x55, 0x59, 0x5A, 0x95, 0x96, 0x99, 0x9A, 0xA9, 0xAA, 0xAE, 0xEA, 0xEE},
    {0x55, 0x56, 0x59, 0x5A, 0x95, 0x96, 0x99, 0x9A, 0xAA, 0xAB, 0xAE, 0xEA, 0xEF},
    {0x50, 0x51, 0x54, 0x55, 0x65, 0x91, 0x94, 0x95, 0xA5, 0xA6, 0xA9, 0xAA, 0xE5, 0xEA},
    {0x51, 0x55, 0x65, 0x91, 0x95, 0x96, 0xA5, 0xA6, 0xA9, 0xAA, 0xE6, 0xEA},
    {0x51, 0x55, 0x56, 0x65, 0x66, 0x91, 0x95, 0x96, 0xA5, 0xA6, 0xAA, 0xE6, 0xEA},
    {0x51, 0x55, 0x56, 0x66, 0x95, 0x96, 0x9A, 0xA5, 0xA6, 0xA7, 0xAA, 0xAB, 0xE6, 0xEA, 0xEB},
    {0x54, 0x55, 0x65, 0x94, 0x95, 0x99, 0xA5, 0xA6, 0xA9, 0xAA, 0xE9, 0xEA},
    {0x55, 0x65, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA, 0xEA},
    {0x51, 0x55, 0x56, 0x65, 0x66, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA, 0xEA},
    {0x51, 0x55, 0x56, 0x66, 0x95, 0x96, 0x9A, 0xA5, 0xA6, 0xAA, 0xAB, 0xEA, 0xEB},
    {0x54, 0x55, 0x59, 0x65, 0x69, 0x94, 0x95, 0x99, 0xA5, 0xA9, 0xAA, 0xE9, 0xEA},
    {0x54, 0x55, 0x59, 0x65, 0x69, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA, 0xEA},
    {0x55, 0x56, 0x59, 0x65, 0x6A, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA, 0xEA},
    {0x55, 0x56, 0x5A, 0x66, 0x6A, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA, 0xAB, 0xEA, 0xEB},
    {0x54, 0x55, 0x59, 0x69, 0x95, 0x99, 0x9A, 0xA5, 0xA9, 0xAA, 0xAD, 0xAE, 0xE9, 0xEA, 0xEE},
    {0x54, 0x55, 0x59, 0x69, 0x95, 0x99, 0x9A, 0xA5, 0xA9, 0xAA, 0xAE, 0xEA, 0xEE},
    {0x55, 0x59, 0x5A, 0x69, 0x6A, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA, 0xAE, 0xEA, 0xEE},
    {0x55, 0x56, 0x59, 0x5A, 0x6A, 0x95, 0x96, 0x99, 0x9A, 0xA6, 0xA9, 0xAA, 0xAB, 0xAE, 0xEA},
    {0x50, 0x51, 0x54, 0x55, 0x65, 0x95, 0xA1, 0xA4, 0xA5, 0xA6, 0xA9, 0xAA, 0xB5, 0xBA, 0xE5, 0xEA, 0xFA},
    {0x51, 0x55, 0x65, 0x95, 0xA1, 0xA5, 0xA6, 0xA9, 0xAA, 0xB6, 0xBA, 0xE6, 0xEA, 0xFA},
    {0x51, 0x55, 0x65, 0x66, 0x95, 0x96, 0xA5, 0xA6, 0xA9, 0xAA, 0xB6, 0xBA, 0xE6, 0xEA, 0xFA},
    {0x51, 0x55, 0x56, 0x65, 0x66, 0x95, 0x96, 0xA5, 0xA6, 0xA7, 0xAA, 0xAB, 0xB6, 0xBA, 0xE6, 0xEA, 0xFB},
    {0x54, 0x55, 0x65, 0x95, 0xA4, 0xA5, 0xA6, 0xA9, 0xAA, 0xB9, 0xBA, 0xE9, 0xEA, 0xFA},
    {0x55, 0x65, 0x95, 0xA5, 0xA6, 0xA9, 0xAA, 0xBA, 0xEA, 0xFA},
    {0x51, 0x55, 0x65, 0x66, 0x95, 0x96, 0xA5, 0xA6, 0xA9, 0xAA, 0xBA, 0xEA, 0xFA},
    {0x55, 0x56, 0x65, 0x66, 0x95, 0x96, 0xA5, 0xA6, 0xAA, 0xAB, 0xBA, 0xEA, 0xFB},
    {0x54, 0x55, 0x65, 0x69, 0x95, 0x99, 0xA5, 0xA6, 0xA9, 0xAA, 0xB9, 0xBA, 0xE9, 0xEA, 0xFA},
    {0x54, 0x55, 0x65, 0x69, 0x95, 0x99, 0xA5, 0xA6, 0xA9, 0xAA, 0xBA, 0xEA, 0xFA},
    {0x55, 0x65, 0x66, 0x69, 0x6A, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA, 0xBA, 0xEA, 0xFA},
    {0x55, 0x56, 0x65, 0x66, 0x6A, 0x95, 0x96, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA, 0xAB, 0xBA, 0xEA},
    {0x54, 0x55, 0x59, 0x65, 0x69, 0x95, 0x99, 0xA5, 0xA9, 0xAA, 0xAD, 0xAE, 0xB9, 0xBA, 0xE9, 0xEA, 0xFE},
    {0x55, 0x59, 0x65, 0x69, 0x95, 0x99, 0xA5, 0xA9, 0xAA, 0xAE, 0xBA, 0xEA, 0xFE},
    {0x55, 0x59, 0x65, 0x69, 0x6A, 0x95, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA, 0xAE, 0xBA, 0xEA},
    {0x55, 0x56, 0x59, 0x5A, 0x65, 0x66, 0x69, 0x6A, 0x95, 0x96, 0x99, 0x9A, 0xA5, 0xA6, 0xA9, 0xAA, 0xAB, 0xAE, 0xBA, 0xEA},
}};

} // namespace baphy