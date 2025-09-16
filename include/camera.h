#pragma once

#include <fstream>

#include "hittable.h"

class camera {
 public:
  double aspect_ratio = 1.0;
  int image_width = 100;

  void render(const hittable& world) {
    initialize();

    color<uint8_t> raster[image_height][image_width];

    std::ofstream file("image.ppm", std::ios::binary);

    for (int j = 0; j < image_height; j++) {
      for (int i = 0; i < image_width; i++) {
        auto pixel_center =
            pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
        auto ray_direction = pixel_center - center;
        ray r(center, ray_direction);

        color<double> pixel_color = ray_color(r, world);

        raster[j][i] = color_out(pixel_color);
      }
    }

    if (file.is_open()) {
      file << "P6\n" << image_width << " " << image_height << "\n255\n";
      file.write(reinterpret_cast<const char*>(raster),
                 image_width * image_height * 3 * sizeof(char));
    }
  }

 private:
  int image_height;
  point3<double> center;
  point3<double> pixel00_loc;
  vec3<double> pixel_delta_u;
  vec3<double> pixel_delta_v;

  void initialize() {
    image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    center = point3<double>(0.0, 0.0, 0.0);

    auto focal_length = 1.0;
    auto viewport_height = 2.0;
    auto viewport_width =
        viewport_height * (double(image_width) / image_height);

    auto viewport_u = vec3<double>(viewport_width, 0.0, 0.0);
    auto viewport_v = vec3<double>(0.0, -viewport_height, 0.0);

    pixel_delta_u = viewport_u / image_width;
    pixel_delta_v = viewport_v / image_height;

    auto viewport_upper_left = center - vec3<double>(0.0, 0.0, focal_length) -
                               viewport_u / 2 - viewport_v / 2;
    pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
  }

  color<double> ray_color(const ray& r, const hittable& world) const {
    hit_record rec;

    if (world.hit(r, interval(0, infinity), rec)) {
      return 0.5 * (rec.normal + color<double>(1.0, 1.0, 1.0));
    }

    vec3<double> unit_direction = unit_vector(r.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * color<double>(1.0, 1.0, 1.0) +
           a * color<double>(0.5, 0.7, 1.0);
  }
};
