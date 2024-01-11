#include "imp/util/helpers.hpp"

namespace imp {

double lerp(double t, double min, double max) {
  return (1 - t) * min + t * max;
}

double normalize(double n, double n_min, double n_max, double new_min, double new_max) {
  return (new_max - new_min) * ((n - n_min) / (n_max - n_min)) + new_min;
}

} // namespace imp
