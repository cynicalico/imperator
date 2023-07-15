#ifndef BAPHY_UTIL_NOISE_OPENSIMPLEX_HPP
#define BAPHY_UTIL_NOISE_OPENSIMPLEX_HPP

/**
 * This is a straight(ish) translation of the Java implementation of
 * OpenSimplex2 by KdotJPG which can be found here:
 *      https://github.com/KdotJPG/OpenSimplex2/tree/master/java
 *
 *
 * Notable changes:
 * - Combined fast and smooth variants into single file--smooth is default, fast is suffixed
 * - Octave methods for every variant
 * - Naming conventions to match my own
 * - Stateless variants are in the repo, I added stateful ones with a seed maintained
 *   as a static data member--_sl suffix means you provide your own seed
 *
 *
 * Which method should I use? (a guide)
 * NOTE: This applies to both the smooth and fast versions
 *
 * *** 2D ***
 * - noise2d => standard lattice orientation
 *
 * - noise2d_improve_x => Y pointing down the main diagonal.
 *     Might be better for a 2D sandbox style game, where Y is vertical.
 *     Probably slightly less optimal for heightmaps or continent maps,
 *     unless your map is centered around an equator. It's a subtle
 *     difference, but the option is here to make it an easy choice.
 *
 * *** 3D ***
 * - noise3d_improve_xy => better visual isotropy in (X, Y).
 *     Recommended for 3D terrain and time-varied animations.
 *     The Z coordinate should always be the "different" coordinate in whatever your use case is.
 *     If Y is vertical in world coordinates, call Noise3_ImproveXZ(x, z, Y) or use noise3_XZBeforeY.
 *     If Z is vertical in world coordinates, call Noise3_ImproveXZ(x, y, Z).
 *     For a time varied animation, call Noise3_ImproveXY(x, y, T).
 *
 * - noise3d_improve_xz => better visual isotropy in (X, Z).
 *     Recommended for 3D terrain and time-varied animations.
 *     The Y coordinate should always be the "different" coordinate in whatever your use case is.
 *     If Y is vertical in world coordinates, call Noise3_ImproveXZ(x, Y, z).
 *     If Z is vertical in world coordinates, call Noise3_ImproveXZ(x, Z, y) or use Noise3_ImproveXY.
 *     For a time varied animation, call Noise3_ImproveXZ(x, T, y) or use Noise3_ImproveXY.
 *
 * - noise3d_fallback => fallback rotation option
 *     Use noise_improve_xy or noise_improve_xz instead, wherever appropriate.
 *     They have less diagonal bias. This function's best use is as a fallback.
 *
 * *** 4D ***
 * - noise4d_improve_xyz_improve_xy => XYZ oriented like noise3d_improve_xy
 *     and W for an extra degree of freedom. W repeats eventually.
 *     Recommended for time-varied animations which texture a 3D object (W=time)
 *     in a space where Z is vertical
 *
 * - noise4d_improve_xyz_improve_xz => XYZ oriented like noise3d_improve_xz
 *     and W for an extra degree of freedom. W repeats eventually.
 *     Recommended for time-varied animations which texture a 3D object (W=time)
 *     in a space where Y is vertical
 *
 * - noise4d_improve_xyz => XYZ oriented like noise3d_fallback
 *     and W for an extra degree of freedom. W repeats eventually.
 *     Recommended for time-varied animations which texture a 3D object (W=time)
 *     where there isn't a clear distinction between horizontal and vertical
 *
 * - noise4d_improve_xy_improve_zw => XY and ZW forming orthogonal triangular-based planes.
 *     Recommended for 3D terrain, where X and Y (or Z and W) are horizontal.
 *     Recommended for noise(x, y, sin(time), cos(time)) trick.
 *
 * - noise4d_fallback => fallback lattice orientation
 */

#include <array>
#include <cstdint>
#include <vector>

namespace baphy {

class opensimplex {
  static std::int64_t seed_;

public:
  static void set_seed(std::int64_t seed);
  static std::int64_t get_seed();

  static void reseed();

  ////////////
  // SMOOTH //
  ////////////

  ///////////////
  // STATELESS //
  ///////////////

  static double noise2d_sl(std::int64_t seed, double x, double y);
  static double noise2d_improve_x_sl(std::int64_t seed, double x, double y);

  static double noise3d_improve_xy_sl(std::int64_t seed, double x, double y, double z);
  static double noise3d_improve_xz_sl(std::int64_t seed, double x, double y, double z);
  static double noise3d_fallback_sl(std::int64_t seed, double x, double y, double z);

  static double noise4d_improve_xyz_improve_xy_sl(std::int64_t seed, double x, double y, double z, double w);
  static double noise4d_improve_xyz_improve_xz_sl(std::int64_t seed, double x, double y, double z, double w);
  static double noise4d_improve_xyz_sl(std::int64_t seed, double x, double y, double z, double w);
  static double noise4d_improve_xy_improve_zw_sl(std::int64_t seed, double x, double y, double z, double w);
  static double noise4d_fallback_sl(std::int64_t seed, double x, double y, double z, double w);

  static double octave2d_sl(std::int64_t seed, double x, double y, int octaves, double persistence);
  static double octave2d_improve_x_sl(std::int64_t seed, double x, double y, int octaves, double persistence);

  static double octave3d_improve_xy_sl(std::int64_t seed, double x, double y, double z, int octaves, double persistence);
  static double octave3d_improve_xz_sl(std::int64_t seed, double x, double y, double z, int octaves, double persistence);
  static double octave3d_fallback_sl(std::int64_t seed, double x, double y, double z, int octaves, double persistence);

  static double octave4d_improve_xyz_improve_xy_sl(std::int64_t seed, double x, double y, double z, double w, int octaves, double persistence);
  static double octave4d_improve_xyz_improve_xz_sl(std::int64_t seed, double x, double y, double z, double w, int octaves, double persistence);
  static double octave4d_improve_xyz_sl(std::int64_t seed, double x, double y, double z, double w, int octaves, double persistence);
  static double octave4d_improve_xy_improve_zw_sl(std::int64_t seed, double x, double y, double z, double w, int octaves, double persistence);
  static double octave4d_fallback_sl(std::int64_t seed, double x, double y, double z, double w, int octaves, double persistence);

  ////////////////
  // PRE-SEEDED //
  ////////////////

  static double noise2d(double x, double y);
  static double noise2d_improve_x(double x, double y);

  static double noise3d_improve_xy(double x, double y, double z);
  static double noise3d_improve_xz(double x, double y, double z);
  static double noise3d_fallback(double x, double y, double z);

  static double noise4d_improve_xyz_improve_xy(double x, double y, double z, double w);
  static double noise4d_improve_xyz_improve_xz(double x, double y, double z, double w);
  static double noise4d_improve_xyz(double x, double y, double z, double w);
  static double noise4d_improve_xy_improve_zw(double x, double y, double z, double w);
  static double noise4d_fallback(double x, double y, double z, double w);

  static double octave2d(double x, double y, int octaves, double persistence);
  static double octave2d_improve_x(double x, double y, int octaves, double persistence);

  static double octave3d_improve_xy(double x, double y, double z, int octaves, double persistence);
  static double octave3d_improve_xz(double x, double y, double z, int octaves, double persistence);
  static double octave3d_fallback(double x, double y, double z, int octaves, double persistence);

  static double octave4d_improve_xyz_improve_xy(double x, double y, double z, double w, int octaves, double persistence);
  static double octave4d_improve_xyz_improve_xz(double x, double y, double z, double w, int octaves, double persistence);
  static double octave4d_improve_xyz(double x, double y, double z, double w, int octaves, double persistence);
  static double octave4d_improve_xy_improve_zw(double x, double y, double z, double w, int octaves, double persistence);
  static double octave4d_fallback(double x, double y, double z, double w, int octaves, double persistence);

private:
  static constexpr std::int64_t PRIME_X_{0x5205402B9270C86FL};
  static constexpr std::int64_t PRIME_Y_{0x598CD327003817B5L};
  static constexpr std::int64_t PRIME_Z_{0x5BCC226E9FA0BACBL};
  static constexpr std::int64_t PRIME_W_{0x56CC5227E58F554BL};
  static constexpr std::int64_t HASH_MULTIPLIER_{0x53A3F72DEEC546F5L};
  static constexpr std::int64_t SEED_FLIP_3D_{-0x52D547B2E96ED629L};

  static constexpr double ROOT2OVER2_{0.7071067811865476};
  static constexpr double SKEW_2D_{0.366025403784439};
  static constexpr double UNSKEW_2D_{-0.21132486540518713};

  static constexpr double ROOT3OVER3_{0.577350269189626};
  static constexpr double FALLBACK_ROTATE3_{2.0 / 3.0};
  static constexpr double ROTATE3_ORTHOGONALIZER_ = UNSKEW_2D_;

  static constexpr double SKEW_4D_{0.309016994374947};
  static constexpr double UNSKEW_4D_{-0.138196601125011};

  static constexpr int N_GRADS_2D_EXPONENT_{7};
  static constexpr int N_GRADS_3D_EXPONENT_{8};
  static constexpr int N_GRADS_4D_EXPONENT_{9};
  static constexpr int N_GRADS_2D_{1 << N_GRADS_2D_EXPONENT_};
  static constexpr int N_GRADS_3D_{1 << N_GRADS_3D_EXPONENT_};
  static constexpr int N_GRADS_4D_{1 << N_GRADS_4D_EXPONENT_};

  static constexpr double NORMALIZER_2D_{0.05481866495625118};
  static constexpr double NORMALIZER_3D_{0.2781926117527186};
  static constexpr double NORMALIZER_4D_{0.11127401889945551};

  static constexpr double RSQUARED_2D_{2.0 / 3.0};
  static constexpr double RSQUARED_3D_{3.0 / 4.0};
  static constexpr double RSQUARED_4D_{4.0 / 5.0};

  static double noise_unskewed_base_(std::int64_t seed, double xs, double ys);
  static double noise_unrotated_base_(std::int64_t seed, double xr, double yr, double zr);
  static double noise_unskewed_base_(std::int64_t seed, double xs, double ys, double zs, double ws);

  static double grad_(
      std::int64_t seed,
      std::int64_t xsvp, std::int64_t ysvp,
      double dx, double dy
  );

  static double grad_(
      std::int64_t seed,
      std::int64_t xrvp, std::int64_t yrvp, std::int64_t zrvp,
      double dx, double dy, double dz
  );

  static double grad_(
      std::int64_t seed,
      std::int64_t xsvp, std::int64_t ysvp, std::int64_t zsvp, std::int64_t wsvp,
      double dx, double dy, double dz, double dw
  );

  static const std::array<double, N_GRADS_2D_ * 2> GRADIENTS_2D_;
  static const std::array<double, N_GRADS_3D_ * 4> GRADIENTS_3D_;
  static const std::array<double, N_GRADS_4D_ * 4> GRADIENTS_4D_;

  struct LatticeVertex4D_ {
    double dx, dy, dz, dw;
    std::int64_t xsvp, ysvp, zsvp, wsvp;

    LatticeVertex4D_() = default;

    LatticeVertex4D_(std::int64_t xsv, std::int64_t ysv, std::int64_t zsv, std::int64_t wsv)
        : dx(-xsv - (xsv + ysv + zsv + wsv) * UNSKEW_4D_),
          dy(-ysv - (xsv + ysv + zsv + wsv) * UNSKEW_4D_),
          dz(-zsv - (xsv + ysv + zsv + wsv) * UNSKEW_4D_),
          dw(-wsv - (xsv + ysv + zsv + wsv) * UNSKEW_4D_),
          xsvp(xsv * PRIME_X_),
          ysvp(ysv * PRIME_Y_),
          zsvp(zsv * PRIME_Z_),
          wsvp(wsv * PRIME_W_) {}
  };

  static const std::array<int, 256> LOOKUP_4D_A_;
  static const std::array<LatticeVertex4D_, 3476> LOOKUP_4D_B_;

public:
  //////////
  // FAST //
  //////////

  ///////////////
  // STATELESS //
  ///////////////

  static double noise2d_sl_fast(std::int64_t seed, double x, double y);
  static double noise2d_improve_x_sl_fast(std::int64_t seed, double x, double y);

  static double noise3d_improve_xy_sl_fast(std::int64_t seed, double x, double y, double z);
  static double noise3d_improve_xz_sl_fast(std::int64_t seed, double x, double y, double z);
  static double noise3d_fallback_sl_fast(std::int64_t seed, double x, double y, double z);

  static double noise4d_improve_xyz_improve_xy_sl_fast(std::int64_t seed, double x, double y, double z, double w);
  static double noise4d_improve_xyz_improve_xz_sl_fast(std::int64_t seed, double x, double y, double z, double w);
  static double noise4d_improve_xyz_sl_fast(std::int64_t seed, double x, double y, double z, double w);
  static double noise4d_improve_xy_improve_zw_sl_fast(std::int64_t seed, double x, double y, double z, double w);
  static double noise4d_fallback_sl_fast(std::int64_t seed, double x, double y, double z, double w);

  static double octave2d_sl_fast(std::int64_t seed, double x, double y, int octaves, double persistence);
  static double octave2d_improve_x_sl_fast(std::int64_t seed, double x, double y, int octaves, double persistence);

  static double octave3d_improve_xy_sl_fast(std::int64_t seed, double x, double y, double z, int octaves, double persistence);
  static double octave3d_improve_xz_sl_fast(std::int64_t seed, double x, double y, double z, int octaves, double persistence);
  static double octave3d_fallback_sl_fast(std::int64_t seed, double x, double y, double z, int octaves, double persistence);

  static double octave4d_improve_xyz_improve_xy_sl_fast(std::int64_t seed, double x, double y, double z, double w, int octaves, double persistence);
  static double octave4d_improve_xyz_improve_xz_sl_fast(std::int64_t seed, double x, double y, double z, double w, int octaves, double persistence);
  static double octave4d_improve_xyz_sl_fast(std::int64_t seed, double x, double y, double z, double w, int octaves, double persistence);
  static double octave4d_improve_xy_improve_zw_sl_fast(std::int64_t seed, double x, double y, double z, double w, int octaves, double persistence);
  static double octave4d_fallback_sl_fast(std::int64_t seed, double x, double y, double z, double w, int octaves, double persistence);

  ////////////////
  // PRE-SEEDED //
  ////////////////

  static double noise2d_fast(double x, double y);
  static double noise2d_improve_x_fast(double x, double y);

  static double noise3d_improve_xy_fast(double x, double y, double z);
  static double noise3d_improve_xz_fast(double x, double y, double z);
  static double noise3d_fallback_fast(double x, double y, double z);

  static double noise4d_improve_xyz_improve_xy_fast(double x, double y, double z, double w);
  static double noise4d_improve_xyz_improve_xz_fast(double x, double y, double z, double w);
  static double noise4d_improve_xyz_fast(double x, double y, double z, double w);
  static double noise4d_improve_xy_improve_zw_fast(double x, double y, double z, double w);
  static double noise4d_fallback_fast(double x, double y, double z, double w);

  static double octave2d_fast(double x, double y, int octaves, double persistence);
  static double octave2d_improve_x_fast(double x, double y, int octaves, double persistence);

  static double octave3d_improve_xy_fast(double x, double y, double z, int octaves, double persistence);
  static double octave3d_improve_xz_fast(double x, double y, double z, int octaves, double persistence);
  static double octave3d_fallback_fast(double x, double y, double z, int octaves, double persistence);

  static double octave4d_improve_xyz_improve_xy_fast(double x, double y, double z, double w, int octaves, double persistence);
  static double octave4d_improve_xyz_improve_xz_fast(double x, double y, double z, double w, int octaves, double persistence);
  static double octave4d_improve_xyz_fast(double x, double y, double z, double w, int octaves, double persistence);
  static double octave4d_improve_xy_improve_zw_fast(double x, double y, double z, double w, int octaves, double persistence);
  static double octave4d_fallback_fast(double x, double y, double z, double w, int octaves, double persistence);
  
private:
  static constexpr std::int64_t SEED_OFFSET_4D_FAST_{0xE83DC3E0DA7164DL};

  static constexpr double SKEW_4D_FAST_{-0.138196601125011};
  static constexpr double UNSKEW_4D_FAST_{0.309016994374947};
  static constexpr double LATTICE_STEP_4D_FAST_{0.2};

  static constexpr double NORMALIZER_2D_FAST_{0.01001634121365712};
  static constexpr double NORMALIZER_3D_FAST_{0.07969837668935331};
  static constexpr double NORMALIZER_4D_FAST_{0.0220065933241897};

  static constexpr double RSQUARED_2D_FAST_{0.5};
  static constexpr double RSQUARED_3D_FAST_{0.6};
  static constexpr double RSQUARED_4D_FAST_{0.6};

  static double noise_unskewed_base_fast_(std::int64_t seed, double xs, double ys);
  static double noise_unrotated_base_fast_(std::int64_t seed, double xr, double yr, double zr);
  static double noise_unskewed_base_fast_(std::int64_t seed, double xs, double ys, double zs, double ws);

  static double grad_fast_(
      std::int64_t seed,
      std::int64_t xsvp, std::int64_t ysvp,
      double dx, double dy
  );

  static double grad_fast_(
      std::int64_t seed,
      std::int64_t xrvp, std::int64_t yrvp, std::int64_t zrvp,
      double dx, double dy, double dz
  );

  static double grad_fast_(
      std::int64_t seed,
      std::int64_t xsvp, std::int64_t ysvp, std::int64_t zsvp, std::int64_t wsvp,
      double dx, double dy, double dz, double dw
  );

  static const std::array<double, N_GRADS_2D_ * 2> GRADIENTS_2D_FAST_;
  static const std::array<double, N_GRADS_3D_ * 4> GRADIENTS_3D_FAST_;
  static const std::array<double, N_GRADS_4D_ * 4> GRADIENTS_4D_FAST_;

private:
  /////////////
  // HELPERS //
  /////////////

  static int fast_floor_(double x);
  static int fast_round_(double x);

  static const std::array<double, 48> GRAD2_;
  static const std::array<double, 192> GRAD3_;
  static const std::array<double, 640> GRAD4_;

  static const std::array<std::vector<int>, 256> LOOKUP_4D_VERTEX_CODES_;
};

} // namespace baphy

#endif//BAPHY_UTIL_NOISE_OPENSIMPLEX_HPP
