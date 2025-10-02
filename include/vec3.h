#pragma once

#include <cmath>
#include <ostream>

#include "common.h"

template <typename T>
class vec3 {
 public:
  T e[3];

  constexpr vec3() : e{T(0), T(0), T(0)} {}
  constexpr vec3(T e0, T e1, T e2) : e{e0, e1, e2} {}

  constexpr T x() const { return e[0]; }
  constexpr T y() const { return e[1]; }
  constexpr T z() const { return e[2]; }

  constexpr vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
  constexpr T operator[](int i) const { return e[i]; }
  constexpr T& operator[](int i) { return e[i]; }

  constexpr vec3& operator+=(const vec3& v) {
    e[0] += v.e[0];
    e[1] += v.e[1];
    e[2] += v.e[2];
    return *this;
  }

  constexpr vec3& operator*=(double t) {
    e[0] *= t;
    e[1] *= t;
    e[2] *= t;
    return *this;
  }

  constexpr vec3& operator/=(double t) { return *this *= 1 / t; }

  constexpr T length_squared() const {
    return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
  }

  constexpr bool near_zero() const {
    auto s = 1e-8;
    return (std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) &&
           (std::fabs(e[2]) < s);
  }

  T length() const { return std::sqrt(length_squared()); }

  static vec3<T> random() {
    return vec3<T>(static_cast<T>(random_double()),
                   static_cast<T>(random_double()),
                   static_cast<T>(random_double()));
  }

  static vec3<T> random(T min, T max) {
    return vec3<T>(static_cast<T>(random_double(min, max)),
                   static_cast<T>(random_double(min, max)),
                   static_cast<T>(random_double(min, max)));
  }
};

template <typename T>
using point3 = vec3<T>;

template <typename T>
inline std::ostream& operator<<(std::ostream& out, const vec3<T>& v) {
  return out << v.e[0] << " " << v.e[1] << " " << v.e[2];
}

template <typename T>
constexpr inline vec3<T> operator+(const vec3<T>& u, const vec3<T>& v) {
  return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

template <typename T>
constexpr inline vec3<T> operator-(const vec3<T>& u, const vec3<T>& v) {
  return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

template <typename T>
constexpr inline vec3<T> operator*(const vec3<T>& u, const vec3<T>& v) {
  return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

template <typename T>
constexpr inline vec3<T> operator*(double t, const vec3<T>& v) {
  return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

template <typename T>
constexpr inline vec3<T> operator*(const vec3<T>& v, double t) {
  return t * v;
}

template <typename T>
constexpr inline vec3<T> operator/(const vec3<T>& v, double t) {
  return (1 / t) * v;
}

template <typename T>
constexpr inline auto dot(const vec3<T>& u, const vec3<T>& v) {
  return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

template <typename T>
constexpr inline vec3<T> cross(const vec3<T>& u, const vec3<T>& v) {
  return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
              u.e[2] * v.e[0] - u.e[0] * v.e[2],
              u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

template <typename T>
inline vec3<T> unit_vector(const vec3<T>& v) {
  return v / v.length();
}

template <typename T>
inline vec3<T> random_unit_vector() {
  while (true) {
    auto p = vec3<T>::random(static_cast<T>(-1), static_cast<T>(1));
    auto lensq = p.length_squared();
    if (1e-160 < lensq && lensq <= static_cast<T>(1)) {
      return p / std::sqrt(lensq);
    }
  }
}

template <typename T>
inline vec3<T> random_on_hemisphere(const vec3<T>& normal) {
  vec3<T> on_unit_sphere = random_unit_vector<T>();
  return dot(on_unit_sphere, normal) > static_cast<T>(0.0) ? on_unit_sphere
                                                           : -on_unit_sphere;
}

template <typename T>
inline vec3<T> reflect(const vec3<T>& v, const vec3<T>& n) {
  return v - 2 * dot(v, n) * n;
}

template <typename T>
inline vec3<T> refract(const vec3<T>& uv, const vec3<T>& n,
                       double etai_over_etat) {
  auto cos_theta = std::fmin(dot(-uv, n), 1.0);
  vec3<T> r_out_perp = etai_over_etat * (uv + cos_theta * n);
  vec3<T> r_out_parallel =
      -std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * n;
  return r_out_perp + r_out_parallel;
}
