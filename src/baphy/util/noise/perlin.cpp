#include "baphy/util/noise/perlin.hpp"

#include <cmath>
#include <functional>

namespace baphy {

using noise1d_func = std::function<double(double)>;
using noise2d_func = std::function<double(double, double)>;
using noise3d_func = std::function<double(double, double, double)>;

inline double octave1d_base(double x, int octaves, double persistence, noise1d_func func) {
  double total = 0.0, frequency = 1.0, amplitude = 1.0, max_value = 0.0;
  for (int i = 0; i < octaves; ++i) {
    total += func(x * frequency) * amplitude;
    max_value += amplitude;
    amplitude *= persistence;
    frequency *= 2;
  }
  return total / max_value;
}

inline double octave2d_base(double x, double y, int octaves, double persistence, noise2d_func func) {
  double total = 0.0, frequency = 1.0, amplitude = 1.0, max_value = 0.0;
  for (int i = 0; i < octaves; ++i) {
    total += func(x * frequency, y * frequency) * amplitude;
    max_value += amplitude;
    amplitude *= persistence;
    frequency *= 2;
  }
  return total / max_value;
}

inline double octave3d_base(double x, double y, double z, int octaves, double persistence, noise3d_func func) {
  double total = 0.0, frequency = 1.0, amplitude = 1.0, max_value = 0.0;
  for (int i = 0; i < octaves; ++i) {
    total += func(x * frequency, y * frequency, z * frequency) * amplitude;
    max_value += amplitude;
    amplitude *= persistence;
    frequency *= 2;
  }
  return total / max_value;
}

double perlin::noise1d(double x) {
  int cx = static_cast<int>(std::floor(x)) & 255;

  x -= std::floor(x);

  double u = fade_(x);

  int a = p_[cx    ];
  int b = p_[cx + 1];

  double g1 = grad_(a, x    );
  double g2 = grad_(b, x - 1);

  return (lerp_(u, g1, g2) + 1.0) / 2.0f;
}

double perlin::noise2d(double x, double y) {
  int cx = static_cast<int>(std::floor(x)) & 255;
  int cy = static_cast<int>(std::floor(y)) & 255;

  x -= std::floor(x);
  y -= std::floor(y);

  double u = fade_(x);
  double v = fade_(y);

  int aa = p_[p_[cx    ] + cy    ];
  int ba = p_[p_[cx + 1] + cy    ];
  int ab = p_[p_[cx    ] + cy + 1];
  int bb = p_[p_[cx + 1] + cy + 1];

  double g1 = grad_(aa, x,     y    );
  double g2 = grad_(ba, x - 1, y    );
  double g3 = grad_(ab, x,     y - 1);
  double g4 = grad_(bb, x - 1, y - 1);

  double l1 = lerp_(u, g1, g2);
  double l2 = lerp_(u, g3, g4);

  return (lerp_(v, l1, l2) + 1.0) / 2.0;
}

double perlin::noise3d(double x, double y, double z) {
  int cx = static_cast<int>(std::floor(x)) & 255;
  int cy = static_cast<int>(std::floor(y)) & 255;
  int cz = static_cast<int>(std::floor(z)) & 255;

  x -= std::floor(x);
  y -= std::floor(y);
  z -= std::floor(z);

  double u = fade_(x);
  double v = fade_(y);
  double w = fade_(z);

  int aaa = p_[p_[p_[cx    ] + cy    ] + cz    ];
  int baa = p_[p_[p_[cx + 1] + cy    ] + cz    ];
  int aba = p_[p_[p_[cx    ] + cy + 1] + cz    ];
  int bba = p_[p_[p_[cx + 1] + cy + 1] + cz    ];
  int aab = p_[p_[p_[cx    ] + cy    ] + cz + 1];
  int bab = p_[p_[p_[cx + 1] + cy    ] + cz + 1];
  int abb = p_[p_[p_[cx    ] + cy + 1] + cz + 1];
  int bbb = p_[p_[p_[cx + 1] + cy + 1] + cz + 1];

  double g1 = grad_(aaa, x,     y,     z    );
  double g2 = grad_(baa, x - 1, y,     z    );
  double g3 = grad_(aba, x,     y - 1, z    );
  double g4 = grad_(bba, x - 1, y - 1, z    );
  double g5 = grad_(aab, x,     y,     z - 1);
  double g6 = grad_(bab, x - 1, y,     z - 1);
  double g7 = grad_(abb, x,     y - 1, z - 1);
  double g8 = grad_(bbb, x - 1, y - 1, z - 1);

  double l1 = lerp_(u, g1, g2);
  double l2 = lerp_(u, g3, g4);
  double l3 = lerp_(u, g5, g6);
  double l4 = lerp_(u, g7, g8);
  double l5 = lerp_(v, l1, l2);
  double l6 = lerp_(v, l3, l4);

  return (lerp_(w, l5, l6) + 1.0) / 2.0;
}

double perlin::octave1d(double x, int octaves, double persistence) {
  return octave1d_base(x, octaves, persistence, noise1d);
}

double perlin::octave2d(double x, double y, int octaves, double persistence) {
  return octave2d_base(x, y, octaves, persistence, noise2d);
}

double perlin::octave3d(double x, double y, double z, int octaves, double persistence) {
  return octave3d_base(x, y, z, octaves, persistence, noise3d);
}

double perlin::grad_(int hash, double x) {
  return hash & 1 ? -x : x;
}

double perlin::grad_(int hash, double x, double y) {
  return (hash & 1 ? -x : x) + (hash & 2 ? -y : y);
}

double perlin::grad_(int hash, double x, double y, double z) {
  int h = hash & 15;
  double u = h < 8 ? x : y;
  double v = h < 4 ? y : (h == 12 || h == 14) ? x : z;
  return (h & 1 ? -u : u) + (h & 2 ? -v : v);
}

double perlin::fade_(double t) {
  return t * t * t * (t * (t * 6 - 15) + 10);
}

double perlin::lerp_(double t, double a, double b) {
  return a + t * (b - a);
}

std::array<int, 512> perlin::p_ = {151,160,137,91,90,15,131,13,201,95,96,53,194,
    233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,190,6,148,247,120,234,
    75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,
    20,125,136,171,168,68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,
    229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,102,143,54,65,25,63,
    161,1,216,80,73,209,76,132,187,208,89,18,169,200,196,135,130,116,188,159,86,
    164,100,109,198,173,186,3,64,52,217,226,250,124,123,5,202,38,147,118,126,
    255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,
    248,152,2,44,154,163,70,221,153,101,155,167,43,172,9,129,22,39,253,19,98,
    108,110,79,113,224,232,178,185,112,104,218,246,97,228,251,34,242,193,238,
    210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,49,192,214,31,181,
    199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,138,236,205,93,222,
    114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,151,160,137,91,90,15,
    131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
    190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
    88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,134,139,48,27,166,
    77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
    102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,169,200,196,
    135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,250,124,123,
    5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
    223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,172,9,
    129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,
    251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,
    49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,
    138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180};

} // namespace baphy
