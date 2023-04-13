#include "myco/util/helpers.hpp"

namespace myco {

double lerp(double t, double min, double max) {
  return (1 - t) * min + t * max;
}

double normalize(double x, double min, double max) {
  return (x - min) / (max - min);
}

} // namespace myco
