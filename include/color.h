#pragma once

#include <cstdint>

#include "interval.h"
#include "vec3.h"

template <typename T>
using color = vec3<T>;

template <typename T>
color<uint8_t> color_out(color<T> c) {
  static const interval intensity(0.000, 0.999);
  return color<uint8_t>(int(256 * intensity.clamp(c.x())),
                        int(256 * intensity.clamp(c.y())),
                        int(256 * intensity.clamp(c.z())));
}
