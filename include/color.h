#pragma once

#include <cstdint>

#include "interval.h"
#include "vec3.h"

using color = vec3<double>;
using color8 = vec3<uint8_t>;

inline float linear_to_gama(float linear_component) {
  if (linear_component > 0) {
    return std::sqrt(linear_component);
  }
  return 0;
}