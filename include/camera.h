#pragma once

#include <atomic>
#include <iomanip>
#include <fstream>
#include <thread>

#include "hittable.h"
#include "material.h"

class camera {
 public:
  double aspect_ratio      = 1.0;
  int    image_width       = 100;
  int    samples_per_pixel = 10;
  int    max_depth         = 10;
  color  background;

  double       vfov     = 90;
  point3       lookfrom = point3(0.0, 0.0, 0.0);
  point3       lookat   = point3(0.0, 0.0, -1.0);
  vec3<double> vup      = vec3<double>(0.0, 1.0, 0.0);

  double defocus_angle = 0;
  double focus_dist    = 0;

  int idx(int i, int j) const { return j * image_width + i; }

  void render(const hittable& world, std::string filename) {
    initialize();

    std::vector<color8> raster(image_height * image_width);
    std::atomic<int> rows_done = 0;

    std::ofstream file(filename, std::ios::binary);

    if (file.is_open()) {
      const unsigned hw = std::thread::hardware_concurrency();
      const unsigned num_threads = hw ? hw : 1;

      auto start_time = std::chrono::steady_clock::now();

      auto worker = [&](int y0, int y1) {
        for (int j = y0; j < y1; j++) {
          for (int i = 0; i < image_width; i++) {
            color pixel_color(0.0, 0.0, 0.0);
            for (int sample = 0; sample < samples_per_pixel; sample++) {
              ray r = get_ray(i, j);
              pixel_color += ray_color(r, max_depth, world);
            }

            raster[idx(i, j)] = color_out(pixel_sample_scale * pixel_color);
          }

          int done = ++rows_done;
            if (done % 10 == 0) {
              double pct = 100.0 * done / image_height;
              std::cout << "\rRendering: " << std::fixed << std::setprecision(1)
                        << pct << "% completed" << std::flush;
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

      auto end_time = std::chrono::steady_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

      file << "P6\n" << image_width << " " << image_height << "\n255\n";
      file.write(reinterpret_cast<const char*>(raster.data()),
                 image_width * image_height * 3 * sizeof(char));

      int hours = static_cast<int>(duration / 3600000);
      duration %= 3600000;
      int minutes = static_cast<int>(duration / 60000);
      duration %= 60000;
      int seconds = static_cast<int>(duration / 1000);
      int milliseconds = static_cast<int>(duration % 1000);


      std::cout << "\nDone in "
                << std::setfill('0') << std::setw(2) << hours << ":"
                << std::setw(2) << minutes << ":"
                << std::setw(2) << seconds << ":"
                << std::setw(3) << milliseconds << "\n";
    }

    std::cout << "Render path: " << filename << std::endl;
  }

  void render(const hittable& world) {
    render(world, "image.ppm");
  }

 private:
  int          image_height;
  double       pixel_sample_scale;
  point3       center;
  point3       pixel00_loc;
  vec3<double> pixel_delta_u;
  vec3<double> pixel_delta_v;
  vec3<double> u, v, w;
  vec3<double> defocus_disk_u;
  vec3<double> defocus_disk_v;

  void initialize() {
    if (focus_dist <= 0) {
      focus_dist = (lookfrom - lookat).length();
    }

    image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    pixel_sample_scale = 1.0 / samples_per_pixel;

    center = lookfrom;

    auto theta = degrees_to_radians(vfov);
    auto h = std::tan(theta / 2);
    auto viewport_height = 2 * h * focus_dist;
    auto viewport_width =
        viewport_height * (double(image_width) / image_height);

    w = unit_vector(lookfrom - lookat);
    u = unit_vector(cross(vup, w));
    v = cross(w, u);

    auto viewport_u = viewport_width * u;
    auto viewport_v = viewport_height * -v;

    pixel_delta_u = viewport_u / image_width;
    pixel_delta_v = viewport_v / image_height;

    auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2.0 
        - viewport_v / 2.0;
    pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
    defocus_disk_u = u * defocus_radius;
    defocus_disk_v = v * defocus_radius;
  }

  ray get_ray(int i, int j) const {
    auto offset = sample_square();
    auto pixel_sample = pixel00_loc 
                      + ((i + offset.x()) * pixel_delta_u)
                      + ((j + offset.y()) * pixel_delta_v);

    auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
    auto ray_direction = pixel_sample - ray_origin;
    auto ray_time = random_double();

    return ray(ray_origin, ray_direction, ray_time);
  }

  vec3<double> sample_square() const {
    return vec3<double>(random_double() - 0.5, random_double() - 0.5, 0);
  }

  point3 defocus_disk_sample() const {
    auto p = random_in_unit_disk();
    return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
  }

  color ray_color(const ray& r, int depth,
                          const hittable& world) const {
    if (depth <= 0) {
      return color(0.0, 0.0, 0.0);
    }

    hit_record rec;

    if (!world.hit(r, interval(0.001, infinity), rec))
      return background;

    ray scattered;
    color attenuation;
    color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat->scatter(r, rec, attenuation, scattered))
      return color_from_emission;

    color color_from_scatter = attenuation * ray_color(scattered, depth - 1, world);

    return color_from_emission + color_from_scatter;

    // if (world.hit(r, interval(0.001, infinity), rec)) {
    //   ray scattered;
    //   color attenuation;
    //   if (rec.mat->scatter(r, rec, attenuation, scattered)) {
    //     return attenuation * ray_color(scattered, depth - 1, world);
    //   }
    //   return color(0.0, 0.0, 0.0);
    // }
    //
    // vec3<double> unit_direction = unit_vector(r.direction());
    // auto a = 0.5 * (unit_direction.y() + 1.0);
    // return (1.0 - a) * color(1.0, 1.0, 1.0) +
    //        a * color(0.5, 0.7, 1.0);
  }
};
