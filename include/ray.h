#pragma once

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
