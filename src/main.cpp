#include <cstdint>
#include <fstream>

#include "color.h"

#define normalize(i, dimension) \
  static_cast<uint8_t>(static_cast<double>(i) / (dimension - 1) * 255.999)

int main(int argc, char *argv[]) {
  // Image
  auto aspect_ratio = 16.0 / 9.0;
  int image_width = 256;
  int image_height = int(image_width / aspect_ratio);

  color<uint8_t> raster[image_height][image_width];

  // Rendering
  for (int j = 0; j < image_height; j++) {
    for (int i = 0; i < image_width; i++) {
      raster[j][i] = color<uint8_t>(normalize(i, image_width),
                                    normalize(j, image_height), 0);
    }
  }

  // Dumping contents of raster to the image file
  std::ofstream file("image.ppm", std::ios::binary);

  if (file.is_open()) {
    file << "P6\n" << image_width << " " << image_height << "\n255\n";
    file.write(reinterpret_cast<const char *>(raster),
               image_width * image_height * 3 * sizeof(char));
  }

  return 0;
}
