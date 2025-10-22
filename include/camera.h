#pragma once

#include <fstream>

#include "hittable.h"
#include "material.h"

class camera {
 public:
  double aspect_ratio   = 1.0;
  int image_width       = 100;
  int samples_per_pixel = 10;
  int max_depth         = 10;

  double         vfov     = 90;
  point3<double> lookfrom = point3<double>(0.0, 0.0, 0.0);
  point3<double> lookat   = point3<double>(0.0, 0.0, -1.0);
  vec3<double>   vup      = vec3<double>(0.0, 1.0, 0.0);

  int idx(int i, int j) const { return j * image_width + i; }

  void render(const hittable& world) {
    initialize();

    std::vector<color<uint8_t>> raster(image_height * image_width);

    std::ofstream file("image.ppm", std::ios::binary);

    if (file.is_open()) {
      const unsigned hw = std::thread::hardware_concurrency();
      const unsigned num_threads = hw ? hw : 4;

      auto worker = [&](int y0, int y1) {
        for (int j = y0; j < y1; j++) {
          for (int i = 0; i < image_width; i++) {
            color<double> pixel_color(0.0, 0.0, 0.0);
            for (int sample = 0; sample < samples_per_pixel; sample++) {
              ray r = get_ray(i, j);
              pixel_color += ray_color(r, max_depth, world);
            }

            raster[idx(i, j)] = color_out(pixel_sample_scale * pixel_color);
          }
        }
      };

      std::vector<std::thread> threads;
      threads.reserve(num_threads);
      int row_per_thread = (image_height + static_cast<int>(num_threads) - 1) /
                           static_cast<int>(num_threads);

      int y0 = 0;
      for (unsigned t = 0; t < num_threads; ++t) {
        int y1 = std::min(image_height, y0 + row_per_thread);
        if (y0 >= y1) break;
        threads.emplace_back(worker, y0, y1);
        y0 = y1;
      }

      for (auto& th : threads) {
        th.join();
      }

      file << "P6\n" << image_width << " " << image_height << "\n255\n";
      file.write(reinterpret_cast<const char*>(raster.data()),
                 image_width * image_height * 3 * sizeof(char));
    }
  }

 private:
  int image_height;
  double pixel_sample_scale;
  point3<double> center;
  point3<double> pixel00_loc;
  vec3<double> pixel_delta_u;
  vec3<double> pixel_delta_v;
  vec3<double> u, v, w;

  void initialize() {
    image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    pixel_sample_scale = 1.0 / samples_per_pixel;

    center = lookfrom;

    auto focal_length = (lookfrom - lookat).length();
    auto theta = degrees_to_radians(vfov);
    auto h = std::tan(theta / 2);
    auto viewport_height = 2 * h * focal_length;
    auto viewport_width =
        viewport_height * (double(image_width) / image_height);

    w = unit_vector(lookfrom - lookat);
    u = unit_vector(cross(vup, w));
    v = cross(w, u);

    auto viewport_u = viewport_width * u;
    auto viewport_v = viewport_height * -v;

    pixel_delta_u = viewport_u / image_width;
    pixel_delta_v = viewport_v / image_height;

    auto viewport_upper_left = center - (focal_length * w) - viewport_u / 2.0 
        - viewport_v / 2.0;
    pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
  }

  ray get_ray(int i, int j) const {
    auto offset = sample_square();
    auto pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) +
                        ((j + offset.y()) * pixel_delta_v);

    auto ray_origin = center;
    auto ray_direction = pixel_sample - ray_origin;

    return ray(ray_origin, ray_direction);
  }

  vec3<double> sample_square() const {
    return vec3<double>(random_double() - 0.5, random_double() - 0.5, 0);
  }

  color<double> ray_color(const ray& r, int depth,
                          const hittable& world) const {
    if (depth <= 0) {
      return color<double>(0.0, 0.0, 0.0);
    }

    hit_record rec;

    if (world.hit(r, interval(0.001, infinity), rec)) {
      ray scattered;
      color<double> attenuation;
      if (rec.mat->scatter(r, rec, attenuation, scattered)) {
        return attenuation * ray_color(scattered, depth - 1, world);
      }
      return color<double>(0.0, 0.0, 0.0);
    }

    vec3<double> unit_direction = unit_vector(r.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * color<double>(1.0, 1.0, 1.0) +
           a * color<double>(0.5, 0.7, 1.0);
  }
};
