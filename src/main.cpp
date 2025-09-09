#include <cstdint>
#include <fstream>

#include "color.h"
#include "ray.h"

int main(int argc, char *argv[]) {
  // Image
  auto aspect_ratio = 16.0 / 9.0;
  int image_width = 400;
  int image_height = int(image_width / aspect_ratio);
  image_height = (image_height < 1) ? 1 : image_height;

  color<uint8_t> raster[image_height][image_width];

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

      color pixel_color = ray_color(r);

      raster[j][i] = color_out(pixel_color);
    }
  }

  // Dumping contents of raster to the image file
  std::ofstream file("image.ppm", std::ios::binary);

  if (file.is_open()) {
    file << "P6\n" << image_width << " " << image_height << "\n255\n";
    file.write(reinterpret_cast<const char *>(raster),
               image_width * image_height * 3 * sizeof(char));
  }

  std::ofstream readable_file("image-readable.ppm");

  if (readable_file.is_open()) {
    readable_file << "P3\n" << image_width << " " << image_height << "\n255\n";
    for (int j = 0; j < image_height; j++) {
      for (int i = 0; i < image_width; i++) {
        readable_file << int(raster[j][i].x()) << " " << int(raster[j][i].y())
                      << " " << int(raster[j][i].z()) << "\n";
      }
    }
  }

  return 0;
}
