#include "baphy/util/helpers.hpp"

namespace baphy {

double lerp(double t, double min, double max) {
  return (1 - t) * min + t * max;
}

double normalize(double x, double min, double max, double norm_min, double norm_max) {
  return (norm_max - norm_min) * ((x - min) / (max - min)) + norm_min;
}

} // namespace baphy
