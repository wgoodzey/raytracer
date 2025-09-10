#pragma once

#include <cstdint>

#include "vec3.h"

template <typename T>
using color = vec3<T>;

template <typename T>
color<uint8_t> color_out(color<T> c) {
  return color<uint8_t>(int(c.x() * 255.999), int(c.y() * 255.999),
                        int(c.z() * 255.999));
}
