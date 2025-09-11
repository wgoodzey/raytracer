#pragma once

#include "ray.h"
#include "vec3.h"

class hit_record {
 public:
  point3<double> p;
  vec3<double> normal;
  double t;
  bool front_face;

  void set_face_normal(const ray& r, const vec3<double>& outward_normal) {
    front_face = dot(r.direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
  }
};

class hittable {
 public:
  virtual ~hittable() = default;

  virtual bool hit(const ray& r, double ray_tmin, double ray_tmax,
                   hit_record& rec) const = 0;
};
