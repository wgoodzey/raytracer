#pragma once

#include "common.h"

class material;

class hit_record {
 public:
  point3<double> p;
  vec3<double> normal;
  std::shared_ptr<material> mat;
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

  virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
};
