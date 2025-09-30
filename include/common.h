#pragma once

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <thread>

// C++ Std Usings

using std::make_shared;
using std::shared_ptr;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees) {
  return degrees * pi / 180.0;
}

inline double random_double() { return std::rand() / (RAND_MAX + 1.0); }

inline double random_double(double min, double max) {
  return min + (max - min) * random_double();
}

inline double random_double_threadsafe() {
  thread_local std::mt19937_64 rng([] {
    std::random_device rd;
    auto seed = (uint64_t(rd()) << 32) ^ uint64_t(rd()) ^
                std::hash<std::thread::id>{}(std::this_thread::get_id());
    return std::mt19937_64(seed);
  }());
  thread_local std::uniform_real_distribution<double> dist(0.0, 1.0);
  return dist(rng);
}

inline double random_double_threadsafe(double min, double max) {
  return min + (max - min) * random_double_threadsafe();
}

// Common Headers

#include "color.h"
#include "interval.h"
#include "ray.h"
#include "vec3.h"
