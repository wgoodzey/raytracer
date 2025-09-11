#include <cstdint>
#include <fstream>

#include "common.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"

color<double> ray_color(const ray& r, const hittable& world) {
  hit_record rec;
  if (world.hit(r, 0, infinity, rec)) {
    return 0.5 * (rec.normal + color<double>(1, 1, 1));
  }

  vec3 unit_direction = unit_vector(r.direction());
  auto a = 0.5 * (unit_direction.y() + 1.0);
  return (1.0 - a) * color<double>(1.0, 1.0, 1.0) +
         a * color<double>(0.5, 0.7, 1.0);
}

int main(int argc, char* argv[]) {
  // Image
  auto aspect_ratio = 16.0 / 9.0;
  const int image_width = 400;
  int image_height_ = int(image_width / aspect_ratio);
  const int image_height = (image_height_ < 1) ? 1 : image_height_;

  color<uint8_t> raster[image_height][image_width];

  // World
  hittable_list world;
  world.add(std::make_shared<sphere>(point3<double>(0, 0, -1), 0.5));
  world.add(std::make_shared<sphere>(point3<double>(0, -100.5, -1), 100));

  // Camera
  auto focal_length = 1.0;
  auto viewport_height = 2.0;
  auto viewport_width = viewport_height * (double(image_width) / image_height);
  auto camera_center = point3<double>(0, 0, 0);

  auto viewport_u = vec3<double>(viewport_width, 0, 0);
  auto viewport_v = vec3<double>(0, -viewport_height, 0);

  auto pixel_delta_u = viewport_u / image_width;
  auto pixel_delta_v = viewport_v / image_height;

  auto viewport_upper_left = camera_center - vec3<double>(0, 0, focal_length) -
                             viewport_u / 2 - viewport_v / 2;
  auto pixel00_loc =
      viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

  // Render
  for (int j = 0; j < image_height; j++) {
    for (int i = 0; i < image_width; i++) {
      auto pixel_center =
          pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
      auto ray_direction = pixel_center - camera_center;
      ray r(camera_center, ray_direction);

      color<double> pixel_color = ray_color(r, world);

      raster[j][i] = color_out(pixel_color);
    }
  }

  // Dumping contents of raster to the image file
  std::ofstream file("image.ppm", std::ios::binary);

  if (file.is_open()) {
    file << "P6\n" << image_width << " " << image_height << "\n255\n";
    file.write(reinterpret_cast<const char*>(raster),
               image_width * image_height * 3 * sizeof(char));
  }

  // std::ofstream readable_file("image-readable.ppm");
  //
  // if (readable_file.is_open()) {
  //   readable_file << "P3\n" << image_width << " " << image_height <<
  //   "\n255\n"; for (int j = 0; j < image_height; j++) {
  //     for (int i = 0; i < image_width; i++) {
  //       readable_file << int(raster[j][i].x()) << " " <<
  //       int(raster[j][i].y())
  //                     << " " << int(raster[j][i].z()) << "\n";
  //     }
  //   }
  // }

  return 0;
}
