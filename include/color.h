#pragma once

#include <cstdint>

#include "interval.h"
#include "vec3.h"

using color = vec3<double>;
using color8 = vec3<uint8_t>;

inline double linear_to_gama(double linear_component) {
  if (linear_component > 0) {
    return std::sqrt(linear_component);
  }
  return 0;
}

inline color8 color_out(color c) {
  auto r = linear_to_gama(c.x());
  auto g = linear_to_gama(c.y());
  auto b = linear_to_gama(c.z());

  static const interval intensity(0.000, 0.999);
  return color8(int(256 * intensity.clamp(r)),
                int(256 * intensity.clamp(g)),
                int(256 * intensity.clamp(b)));
}
