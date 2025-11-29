#pragma once

#include "hittable.h"

class triag : public hittable {

 public:
  triag(
    const point3& Q,
    const vec3<double>& u,
    const vec3<double>& v,
    shared_ptr<material> mat) 
  : Q(Q), u(u), v(v), mat(mat) 
  {
    normal = unit_vector(cross(u, v));
    w = cross(u, normal);
    D = dot(normal, Q);

    set_bounding_box();
  }

  virtual void set_bounding_box() {
    auto p0 = Q;
    auto p1 = Q + u;
    auto p2 = Q + v;

    auto min_x = std::fmin(p0.x(), std::fmin(p1.x(), p2.x()));
    auto min_y = std::fmin(p0.y(), std::fmin(p1.y(), p2.y()));
    auto min_z = std::fmin(p0.z(), std::fmin(p1.z(), p2.z()));

    auto max_x = std::fmax(p0.x(), std::fmax(p1.x(), p2.x()));
    auto max_y = std::fmax(p0.y(), std::fmax(p1.y(), p2.y()));
    auto max_z = std::fmax(p0.z(), std::fmax(p1.z(), p2.z()));

    point3 min(min_x, min_y, min_z);
    point3 max(max_x, max_y, max_z);

    bbox = aabb(min, max);
  }

  aabb bounding_box() const override { return bbox; }

  bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
    auto denom = dot(normal, r.direction());
    if (fabs(denom) < 1e-8) {
        return false;
    }

    auto t = (D - dot(normal, r.origin())) / denom;
    if (!ray_t.contains(t)) {
        return false;
    }

    auto p = r.at(t);
    auto planar = p - Q;

    auto uu = dot(u, u);
    auto uv = dot(u, v);
    auto vv = dot(v, v);
    auto pu = dot(planar, u);
    auto pv = dot(planar, v);

    auto denom_ab = uu * vv - uv * uv;
    if (fabs(denom_ab) < 1e-12) {
      return false;
    }

    double a = (pu * vv - pv * uv) / denom_ab;
    double b = (pv * uu - pu * uv) / denom_ab;

    if (!is_interior(a, b, rec)) {
      return false;
    }

    rec.t = t;
    rec.p = p;
    rec.set_face_normal(r, normal);
    rec.mat = mat;

    return true;
  }

  virtual bool is_interior(double a, double b, hit_record& rec) const {
    interval unit_interval(0.0, 1.0);
    if (a < 0 || b < 0 || a + b > 1)
      return false;

    rec.u = a;
    rec.v = b;
    return true;
  }

 private:
  point3 Q;
  vec3<double> u, v;
  vec3<double> w;
  aabb bbox;
  vec3<double> normal;
  shared_ptr<material> mat;
  double D;
};
