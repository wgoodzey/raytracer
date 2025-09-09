#include <cstdint>
#include <cstring>
#include <fstream>

#define image_width 256
#define image_height 256

#define R 0
#define G 1
#define B 2

#define normalize(i, dimension) \
  static_cast<uint8_t>(static_cast<double>(i) / (dimension - 1) * 255.999)

uint8_t raster[image_height][image_width][3];

int write_image() {
  std::ofstream file("image.ppm", std::ios::binary);

  if (file.is_open()) {
    file << "P6\n" << image_width << " " << image_height << "\n255\n";
    file.write(reinterpret_cast<const char *>(raster),
               image_width * image_height * 3 * sizeof(char));
  }

  return 0;
}

int main(int argc, char *argv[]) {
  for (int j = 0; j < image_height; j++) {
    for (int i = 0; i < image_width; i++) {
      raster[j][i][R] = normalize(i, image_width);
      raster[j][i][G] = normalize(j, image_height);
      raster[j][i][B] = 0;
    }
  }

  write_image();

  return 0;
}
