#include "bvh.h"
#include "camera.h"
#include "common.h"
#include "constant_medium.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "quad.h"
#include "sphere.h"
#include "texture.h"
#include "triangle.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

std::string timestamp(std::string s) {
  using namespace std::chrono;
  auto now = system_clock::now();
  auto t = system_clock::to_time_t(now);
  std::tm tm = *std::localtime(&t);

  std::ostringstream ss;
  ss << std::put_time(&tm, "%Y%m%d_%H%M%S");
  return "output/" + s + "-" + ss.str() + ".hdr";
}

void bouncing_spheres() {
  hittable_list world;

  auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
  world.add(make_shared<sphere>(point3(0.0, -1000.0, 0.0), 1000.0, ground_material));

  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      auto choose_mat = random_double();
      point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

      if ((center - point3(4.0, 0.2, 0.0)).length() > 0.9) {
        shared_ptr<material> sphere_material;

        if (choose_mat < 0.8) {
          // diffuse
          auto albedo = color::random() * color::random();
          sphere_material = make_shared<lambertian>(albedo);
          auto center2 = center + vec3<double>(0.0, random_double(0.0, 0.5), 0.0);
          world.add(make_shared<sphere>(center, center2, 0.2, sphere_material));
        } else if (choose_mat < 0.95) {
          // metal
          auto albedo = color::random(0.5, 1.0);
          auto fuzz = random_double(0.0, 0.5);
          sphere_material = make_shared<metal>(albedo, fuzz);
          world.add(make_shared<sphere>(center, 0.2, sphere_material));
        } else {
          // glass
          sphere_material = make_shared<dielectric>(1.5);
          world.add(make_shared<sphere>(center, 0.2, sphere_material));
        }
      } }
  }

  auto material1 = make_shared<dielectric>(1.5);
  world.add(make_shared<sphere>(point3(0.0, 1.0, 0.0), 1.0, material1));

  auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
  world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

  auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
  world.add(make_shared<sphere>(point3(4.0, 1.0, 0.0), 1.0, material3));

  world = hittable_list(make_shared<bvh_node>(world));

  camera cam;

  cam.aspect_ratio      = 16.0 / 9.0;
  cam.image_width       = 400;
  cam.samples_per_pixel = 100;
  cam.max_depth         = 50;
  cam.background        = color(0.70, 0.80, 1.00);

  cam.vfov     = 20;
  cam.lookfrom = point3(13.0, 2.0, 3.0);
  cam.lookat   = point3(0.0, 0.0, 0.0);
  cam.vup      = vec3<double>(0.0, 1.0, 0.0);

  cam.defocus_angle = 0.6;
  cam.focus_dist    = 10.0;

  cam.render(world, timestamp("motion-blur"));
}

void checkered_spheres() {
  hittable_list world;

  auto checker = make_shared<checker_texture>(0.32, color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));

  world.add(make_shared<sphere>(point3(0.0, -10.0, 0.0), 10.0, make_shared<lambertian>(checker)));
  world.add(make_shared<sphere>(point3(0.0,  10.0, 0.0), 10.0, make_shared<lambertian>(checker)));

  world = hittable_list(make_shared<bvh_node>(world));

  camera cam;

  cam.aspect_ratio      = 16.0 / 9.0;
  cam.image_width       = 400;
  cam.samples_per_pixel = 100;
  cam.max_depth         = 50;
  cam.background        = color(0.70, 0.80, 1.00);

  cam.vfov     = 20;
  cam.lookfrom = point3(13.0, 2.0, 3.0);
  cam.lookat   = point3(0.0, 0.0, 0.0);
  cam.vup      = vec3<double>(0.0, 1.0, 0.0);

  cam.defocus_angle = 0.6;
  cam.focus_dist    = 10.0;

  cam.render(world, timestamp("checkered-spheres"));
}

void earth() {
  auto earth_texture = make_shared<image_texture>("assets/earthmap.jpg");
  auto earth_surface = make_shared<lambertian>(earth_texture);
  auto globe = make_shared<sphere>(point3(0.0, 0.0, 0.0), 2, earth_surface);

  camera cam;

  cam.aspect_ratio      = 16.0 / 9.0;
  cam.image_width       = 400;
  cam.samples_per_pixel = 100;
  cam.max_depth         = 50;
  cam.background        = color(0.70, 0.80, 1.00);

  cam.vfov     = 20;
  cam.lookfrom = point3(0,0,12);
  cam.lookat   = point3(0.0, 0.0, 0.0);
  cam.vup      = vec3<double>(0.0, 1.0, 0.0);

  cam.defocus_angle = 0;

  cam.render(hittable_list(globe), timestamp("earth"));
}

void perlin_spheres() {
  hittable_list world;

  auto pertext = make_shared<noise_texture>(4);
  world.add(make_shared<sphere>(point3(0.0, -1000.0, 0.0), 1000, make_shared<lambertian>(pertext)));
  world.add(make_shared<sphere>(point3(0.0, 2.0, 0.0), 2, make_shared<lambertian>(pertext)));

  camera cam;

  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 400;
  cam.samples_per_pixel = 100;
  cam.max_depth = 50;
  cam.background        = color(0.70, 0.80, 1.00);

  cam.vfov     = 20;
  cam.lookfrom = point3(13.0 , 2.0, 3.0);
  cam.lookat   = point3(0.0, 0.0, 0.0);
  cam.vup      = vec3<double>(0.0, 1.0, 0.0);
  cam.background        = color(0.70, 0.80, 1.00);

  cam.defocus_angle = 0;

  cam.render(world, timestamp("perlin-spheres"));
}

void quads() {
  hittable_list world;

  auto left_red     = make_shared<lambertian>(color(1.0, 0.2, 0.2));
  auto back_green   = make_shared<lambertian>(color(0.2, 1.0, 0.2));
  auto right_blue   = make_shared<lambertian>(color(0.2, 0.2, 1.0));
  auto upper_orange = make_shared<lambertian>(color(1.0, 0.5, 0.0));
  auto lower_teal   = make_shared<lambertian>(color(0.2, 0.8, 0.8));

  world.add(make_shared<quad>(point3(-3.0, -2.0, 5.0), vec3<double>(0.0, 0.0, -4.0), vec3<double>(0.0, 4.0, 0.0), left_red));
  world.add(make_shared<quad>(point3(-2.0, -2.0, 0.0), vec3<double>(4.0, 0.0, 0.0), vec3<double>(0.0, 4.0, 0.0), back_green));
  world.add(make_shared<quad>(point3( 3.0, -2.0, 1.0), vec3<double>(0.0, 0.0, 4.0), vec3<double>(0.0, 4.0, 0.0), right_blue));
  world.add(make_shared<quad>(point3(-2.0,  3.0, 1.0), vec3<double>(4.0, 0.0, 0.0), vec3<double>(0.0, 0.0, 4.0), upper_orange));
  world.add(make_shared<quad>(point3(-2.0, -3.0, 5.0), vec3<double>(4.0, 0.0, 0.0), vec3<double>(0.0, 0.0,-4.0), lower_teal));

  camera cam;

  cam.aspect_ratio      = 1.0;
  cam.image_width       = 400;
  cam.samples_per_pixel = 100;
  cam.max_depth         = 50;
  cam.background        = color(0.70, 0.80, 1.00);

  cam.vfov     = 80;
  cam.lookfrom = point3(0.0, 0.0, 9.0);
  cam.lookat   = point3(0.0, 0.0, 0.0);
  cam.vup      = vec3<double>(0.0, 1.0, 0.0);

  cam.defocus_angle = 0;

  cam.render(world, timestamp("quads"));
}

void simple_light() {
  hittable_list world;

  auto pertext = make_shared<noise_texture>(4);
  world.add(make_shared<sphere>(point3(0.0, -1000.0, 0.0), 1000, make_shared<lambertian>(pertext)));
  world.add(make_shared<sphere>(point3(0.0, 2.0, 0.0), 2, make_shared<lambertian>(pertext)));

  auto difflight = make_shared<diffuse_light>(color(4.0, 4.0, 4.0));
  world.add(make_shared<sphere>(point3(0.0, 7.0, 0.0), 2, difflight));
  world.add(make_shared<quad>(point3(3.0, 1.0, -2.0), vec3<double>(2.0, 0.0, 0.0), vec3<double>(0.0, 2.0, 0.0), difflight));

  camera cam;

  cam.aspect_ratio      = 16.0 / 9.0;
  cam.image_width       = 400;
  cam.samples_per_pixel = 100;
  cam.max_depth         = 50;
  cam.background        = color(0,0,0);

  cam.vfov     = 20;
  cam.lookfrom = point3(26,3,6);
  cam.lookat   = point3(0,2,0);
  cam.vup      = vec3<double>(0,1,0);

  cam.defocus_angle = 0;

  cam.render(world, timestamp("simple-light"));
}

void cornell_box() {
  hittable_list world;

  auto red   = make_shared<lambertian>(color(.65, .05, .05));
  auto white = make_shared<lambertian>(color(.73, .73, .73));
  auto green = make_shared<lambertian>(color(.12, .45, .15));
  auto light = make_shared<diffuse_light>(color(15, 15, 15));

  world.add(make_shared<quad>(point3(555,0,0), vec3<double>(0,555,0), vec3<double>(0,0,555), green));
  world.add(make_shared<quad>(point3(0,0,0), vec3<double>(0,555,0), vec3<double>(0,0,555), red));
  world.add(make_shared<quad>(point3(343, 554, 332), vec3<double>(-130,0,0), vec3<double>(0,0,-105), light));
  world.add(make_shared<quad>(point3(0,0,0), vec3<double>(555,0,0), vec3<double>(0,0,555), white));
  world.add(make_shared<quad>(point3(555,555,555), vec3<double>(-555,0,0), vec3<double>(0,0,-555), white));
  world.add(make_shared<quad>(point3(0,0,555), vec3<double>(555,0,0), vec3<double>(0,555,0), white));


  shared_ptr<hittable> box1 = box(point3(0,0,0), point3(165,330,165), white);
  box1 = make_shared<rotate_y>(box1, 15);
  box1 = make_shared<translate>(box1, vec3<double>(265,0,295));
  world.add(box1);

  shared_ptr<hittable> box2 = box(point3(0,0,0), point3(165,165,165), white);
  box2 = make_shared<rotate_y>(box2, -18);
  box2 = make_shared<translate>(box2, vec3<double>(130,0,65));
  world.add(box2);

  camera cam;

  cam.aspect_ratio      = 1.0;
  cam.image_width       = 600;
  cam.samples_per_pixel = 200;
  cam.max_depth         = 50;
  cam.background        = color(0,0,0);

  cam.vfov     = 40;
  cam.lookfrom = point3(278, 278, -800);
  cam.lookat   = point3(278, 278, 0);
  cam.vup      = vec3<double>(0,1,0);

  cam.defocus_angle = 0;

  cam.render(world, timestamp("cornell-box"));
}

void cornell_smoke() {
  hittable_list world;

  auto red   = make_shared<lambertian>(color(.65, .05, .05));
  auto white = make_shared<lambertian>(color(.73, .73, .73));
  auto green = make_shared<lambertian>(color(.12, .45, .15));
  auto light = make_shared<diffuse_light>(color(7, 7, 7));

  world.add(make_shared<quad>(point3(555,0,0), vec3<double>(0,555,0), vec3<double>(0,0,555), green));
  world.add(make_shared<quad>(point3(0,0,0), vec3<double>(0,555,0), vec3<double>(0,0,555), red));
  world.add(make_shared<quad>(point3(113,554,127), vec3<double>(330,0,0), vec3<double>(0,0,305), light));
  world.add(make_shared<quad>(point3(0,555,0), vec3<double>(555,0,0), vec3<double>(0,0,555), white));
  world.add(make_shared<quad>(point3(0,0,0), vec3<double>(555,0,0), vec3<double>(0,0,555), white));
  world.add(make_shared<quad>(point3(0,0,555), vec3<double>(555,0,0), vec3<double>(0,555,0), white));

  shared_ptr<hittable> box1 = box(point3(0,0,0), point3(165,330,165), white);
  box1 = make_shared<rotate_y>(box1, 15);
  box1 = make_shared<translate>(box1, vec3<double>(265,0,295));

  shared_ptr<hittable> box2 = box(point3(0,0,0), point3(165,165,165), white);
  box2 = make_shared<rotate_y>(box2, -18);
  box2 = make_shared<translate>(box2, vec3<double>(130,0,65));

  world.add(make_shared<constant_medium>(box1, 0.01, color(0,0,0)));
  world.add(make_shared<constant_medium>(box2, 0.01, color(1,1,1)));

  camera cam;

  cam.aspect_ratio      = 1.0;
  cam.image_width       = 600;
  cam.samples_per_pixel = 200;
  cam.max_depth         = 50;
  cam.background        = color(0,0,0);

  cam.vfov     = 40;
  cam.lookfrom = point3(278, 278, -800);
  cam.lookat   = point3(278, 278, 0);
  cam.vup      = vec3<double>(0,1,0);

  cam.defocus_angle = 0;

  cam.render(world, timestamp("cornel-smoke"));
}

void final_scene(int image_width, int samples_per_pixel, int max_depth) {
  hittable_list boxes1;
  auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

  int boxes_per_side = 20;
  for (int i = 0; i < boxes_per_side; i++) {
    for (int j = 0; j < boxes_per_side; j++) {
      auto w = 100.0;
      auto x0 = -1000.0 + i*w;
      auto z0 = -1000.0 + j*w;
      auto y0 = 0.0;
      auto x1 = x0 + w;
      auto y1 = random_double(1,101);
      auto z1 = z0 + w;

      boxes1.add(box(point3(x0,y0,z0), point3(x1,y1,z1), ground));
    }
  }

  hittable_list world;

  world.add(make_shared<bvh_node>(boxes1));

  auto light = make_shared<diffuse_light>(color(7, 7, 7));
  world.add(make_shared<quad>(point3(123,554,147), vec3<double>(300,0,0), vec3<double>(0,0,265), light));

  auto center1 = point3(400, 400, 200);
  auto center2 = center1 + vec3<double>(30,0,0);
  auto sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
  world.add(make_shared<sphere>(center1, center2, 50, sphere_material));

  world.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
  world.add(make_shared<sphere>(
    point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)
  ));

  auto boundary = make_shared<sphere>(point3(360,150,145), 70, make_shared<dielectric>(1.5));
  world.add(boundary);
  world.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
  boundary = make_shared<sphere>(point3(0,0,0), 5000, make_shared<dielectric>(1.5));
  world.add(make_shared<constant_medium>(boundary, .0001, color(1,1,1)));

  auto emat = make_shared<lambertian>(make_shared<image_texture>("assets/earthmap.jpg"));
  world.add(make_shared<sphere>(point3(400,200,400), 100, emat));
  auto pertext = make_shared<noise_texture>(0.2);
  world.add(make_shared<sphere>(point3(220,280,300), 80, make_shared<lambertian>(pertext)));

  hittable_list boxes2;
  auto white = make_shared<lambertian>(color(.73, .73, .73));
  int ns = 1000;
  for (int j = 0; j < ns; j++) {
    boxes2.add(make_shared<sphere>(point3::random(0,165), 10, white));
  }

  world.add(make_shared<translate>(
    make_shared<rotate_y>(
      make_shared<bvh_node>(boxes2), 15),
      vec3<double>(-100,270,395)
    )
  );

  camera cam;

  cam.aspect_ratio      = 1.0;
  cam.image_width       = image_width;
  cam.samples_per_pixel = samples_per_pixel;
  cam.max_depth         = max_depth;
  cam.background        = color(0,0,0);

  cam.vfov     = 40;
  cam.lookfrom = point3(478, 278, -600);
  cam.lookat   = point3(278, 278, 0);
  cam.vup      = vec3<double>(0,1,0);

  cam.defocus_angle = 0;

  cam.render(world, timestamp("final-scene"));
}

void triangles() {
  hittable_list world;

  auto red = make_shared<lambertian>(color(.65, .05, .05));

  world.add(make_shared<triag>(point3(0.5,0,0), vec3<double>(0,0.5,0), vec3<double>(0,0,0.5), red));

  camera cam;

  cam.aspect_ratio      = 16.0 / 9.0;
  cam.image_width       = 400;
  cam.samples_per_pixel = 100;
  cam.max_depth         = 50;
  cam.background        = color(0.70, 0.80, 1.00);

  cam.vfov     = 20;
  cam.lookfrom = point3(13.0, 2.0, 3.0);
  cam.lookat   = point3(0.0, 0.0, 0.0);
  cam.vup      = vec3<double>(0.0, 1.0, 0.0);

  cam.defocus_angle = 0.6;
  cam.focus_dist    = 10.0;

  cam.render(world, timestamp("triangles"));
}

void showcase_scene() {
  hittable_list world;

  // Ground
  auto ground = make_shared<metal>(color(0.9, 0.9, 0.9), 0.2);
  world.add(make_shared<quad>(point3(-20,-5,-20), vec3<double>(40,0,0), vec3<double>(0,0,40), ground));

  // Back wall
  auto perlin_tex = make_shared<noise_texture>(2.0);
  auto back_wall = make_shared<lambertian>(perlin_tex);
  world.add(make_shared<quad>(point3(-20,-5,-20), vec3<double>(40,0,0), vec3<double>(0,25,0), back_wall));

  // Left wall
  auto checker = make_shared<checker_texture>(1.0, color(0.1, 0.1, 0.1), color(0.8, 0.8, 0.8));
  auto left_wall = make_shared<lambertian>(checker);
  world.add(make_shared<quad>(point3(-20,-5,-20), vec3<double>(0,0,40), vec3<double>(0,25,0), left_wall));

  // Right wall
  auto right_wall = make_shared<lambertian>(checker);
  world.add(make_shared<quad>(point3(20,-5,-20), vec3<double>(0,0,40), vec3<double>(0,25,0), right_wall));

  // Ceiling
  auto ceiling = make_shared<lambertian>(color(0.3, 0.3, 0.3));
  world.add(make_shared<quad>(point3(-20,20,-20), vec3<double>(40,0,0), vec3<double>(0,0,40), ceiling));

  // Rear wall
  auto rear_wall = make_shared<lambertian>(checker);
  world.add(make_shared<quad>(point3(-20,-5,20), vec3<double>(40,0,0), vec3<double>(0,25,0), rear_wall));

  // Vertical emissive bars
  auto cool_bar = make_shared<diffuse_light>(color(0.6, 0.8, 1.2));
  world.add(make_shared<quad>(point3(-15,-2,14), vec3<double>(6,0,0), vec3<double>(0,18,0), cool_bar));

  auto neutral_bar = make_shared<diffuse_light>(color(1.0, 0.9, 0.8));
  world.add(make_shared<quad>(point3(-3,-2,14), vec3<double>(6,0,0), vec3<double>(0,18,0), neutral_bar));

  auto warm_bar = make_shared<diffuse_light>(color(1.6, 1.1, 0.5));
  world.add(make_shared<quad>(point3(9,-2,14), vec3<double>(6,0,0), vec3<double>(0,18,0), warm_bar));

  // 3x3 grid of spheres
  double sphere_radius = 3;
  double grid_spacing = 6.5;
  double depth = -6;

  // Bottom row
  auto red_mat = make_shared<lambertian>(color(0.9, 0.2, 0.2));
  auto blue_mat = make_shared<lambertian>(color(0.2, 0.3, 0.9));
  auto green_mat = make_shared<lambertian>(color(0.2, 0.8, 0.3));

  world.add(make_shared<sphere>(point3(0, 7.5 - grid_spacing, depth), sphere_radius, red_mat));
  world.add(make_shared<sphere>(point3(grid_spacing, 7.5 - grid_spacing, depth), sphere_radius, blue_mat));
  world.add(make_shared<sphere>(point3(2*grid_spacing, 7.5 - grid_spacing, depth), sphere_radius, green_mat));

  // Middle row
  auto glass = make_shared<dielectric>(1.5);
  auto shiny_metal = make_shared<metal>(color(1.0, 0.85, 0.5), 0.1);
  auto rough_metal = make_shared<metal>(color(0.7, 0.7, 0.8), 0.6);

  world.add(make_shared<sphere>(point3(0, 7.5, depth), sphere_radius, shiny_metal));
  world.add(make_shared<sphere>(point3(grid_spacing, 7.5, depth), sphere_radius, rough_metal));
  world.add(make_shared<sphere>(point3(2*grid_spacing, 7.5, depth), sphere_radius, glass));
  
  // Top row
  // First smoky volume
  auto smoky_boundary = make_shared<sphere>(point3(0, 7.5 + grid_spacing, depth), sphere_radius, make_shared<dielectric>(1.5));
  world.add(smoky_boundary);
  world.add(make_shared<constant_medium>(smoky_boundary, 0.15, color(0.9, 0.9, 0.9)));

  // Earth texture sphere
  auto earth_mat = make_shared<lambertian>(make_shared<image_texture>("assets/earthmap.jpg"));
  world.add(make_shared<sphere>(point3(grid_spacing, 7.5 + grid_spacing, depth), sphere_radius, earth_mat));

  auto smoky_vol = make_shared<sphere>(point3(2*grid_spacing, 7.5 + grid_spacing, depth), sphere_radius, make_shared<lambertian>(color(0, 0, 0)));
  world.add(make_shared<constant_medium>(smoky_vol, 0.15, color(0.9, 0.9, 0.9)));

  // Rotated volume
  hittable_list boxes;
  auto box_mat = make_shared<lambertian>(color(0.75, 0.75, 0.2));
  int num_boxes = 800;
  for (int i = 0; i < num_boxes; i++) {
    auto size = random_double(0.3, 1.2);
    auto pos = point3::random(-8, 8);
    boxes.add(make_shared<sphere>(pos, size, box_mat));
  }
  world.add(make_shared<translate>(
    make_shared<rotate_y>(
      make_shared<bvh_node>(boxes), 60),
      vec3<double>(-15, 0, -8)
  ));

  world = hittable_list(make_shared<bvh_node>(world));

  camera cam;

  cam.aspect_ratio      = 4.0 / 3.0;
  cam.image_width       = 800; // 4000;
  cam.samples_per_pixel = 1000;
  cam.max_depth         = 4;
  cam.background        = color(0.1, 0.1, 0.15);

  cam.vfov     = 70;
  cam.lookfrom = point3(8, 12, 15);
  cam.lookat   = point3(0, 7.5, depth);
  cam.vup      = vec3<double>(0, 1, 0);

  cam.defocus_angle = 0;

  cam.render(world, timestamp("showcase"));
}

int main(int argc, char* argv[]) {

  int number = -1;

  if (argc > 1)
    number = std::atoi(argv[1]);

  switch (number) {
    case 1:  bouncing_spheres();          break;
    case 2:  checkered_spheres();         break;
    case 3:  earth();                     break;
    case 4:  perlin_spheres();            break;
    case 5:  quads();                     break;
    case 6:  simple_light();              break;
    case 7:  cornell_box();               break;
    case 8:  cornell_smoke();             break;
    case 9:  final_scene(800, 10000, 40); break;
    case 10: final_scene(400, 250, 4);    break;
    case 11: triangles();                 break;
    default: showcase_scene();            break;
  }
  return 0;
}
