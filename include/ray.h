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

inline double hit_sphere(const point3<double>& center, double radius,
                         const ray& r) {
  vec3<double> oc = center - r.origin();
  auto a = dot(r.direction(), r.direction());
  auto b = -2.0 * dot(r.direction(), oc);
  auto c = dot(oc, oc) - radius * radius;
  auto discriminant = b * b - 4 * a * c;

  if (discriminant < 0) {
    return -1.0;
  } else {
    return (-b - std::sqrt(discriminant)) / (2.0 * a);
  }
}

inline color<double> ray_color(const ray& r) {
  auto t = hit_sphere(point3<double>(0, 0, -1), 0.5, r);
  if (t > 0.0) {
    vec3<double> N = unit_vector(r.at(t) - vec3<double>(0, 0, -1));
    return 0.5 * color<double>(N.x() + 1, N.y() + 1, N.z() + 1);
  }

  vec3<double> unit_direction = unit_vector(r.direction());
  auto a = 0.5 * (unit_direction.y() + 1.0);
  return (1.0 - a) * color<double>(1.0, 1.0, 1.0) +
         a * color<double>(0.5, 0.7, 1.0);
}
