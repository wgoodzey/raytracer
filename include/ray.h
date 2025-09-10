#pragma once

#include "color.h"
#include "vec3.h"

class ray {
 public:
  ray() {}

  ray(const point3<double>& origin, const vec3<double>& direction)
      : orig(origin), dir(direction) {};

  const point3<double>& origin() const { return orig; }
  const vec3<double>& direction() const { return dir; }

  point3<double> at(double t) const { return orig + t * dir; }

 private:
  point3<double> orig;
  vec3<double> dir;
};

inline color<double> ray_color(const ray& r) {
  vec3<double> unit_direction = unit_vector(r.direction());
  auto a = 0.5 * (unit_direction.y() + 1.0);
  return (1.0 - a) * color<double>(1.0, 1.0, 1.0) +
         a * color<double>(0.5, 0.7, 1.0);
}
