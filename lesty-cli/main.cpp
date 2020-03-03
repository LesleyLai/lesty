#include <chrono>
#include <cstdio>
#include <iostream>
#include <memory>
#include <vector>

#include <fmt/format.h>

#include "axis_aligned_rect.hpp"
#include "bounding_volume_hierarchy.hpp"
#include "image.hpp"
#include "material.hpp"
#include "pathtracer.hpp"
#include "scene.hpp"
#include "sphere.hpp"

using namespace lesty;

namespace {
const Lambertian red{Color(0.65f, 0.05f, 0.05f)};
const Lambertian white{Color(0.73f, 0.73f, 0.73f)};
const Lambertian green{Color(0.12f, 0.45f, 0.15f)};
const Emission light{Color(1, 1, 1)};
const Metal metal{Color(0.73f, 0.73f, 0.73f), 0.8f};
const Dielectric glass(Color(1.f, 1.f, 1.f), 1.655f);
} // namespace

Scene create_scene()
{
  std::vector<std::unique_ptr<Hitable>> objects;
  std::vector<std::unique_ptr<Material>> materials;

  objects.push_back(std::make_unique<Rect_YZ>(
      beyond::Point2f(0.f, 0.f), beyond::Point2f(555.f, 555.f), 555.f, green,
      Normal_Direction::Negetive));
  objects.push_back(std::make_unique<Rect_YZ>(
      beyond::Point2f(0.f, 0.f), beyond::Point2f(555.f, 555.f), 0.f, red));

  objects.push_back(std::make_unique<Rect_XZ>(beyond::Point2f(213.f, 227.f),
                                              beyond::Point2f(343.f, 332.f),
                                              554.f, light));
  objects.push_back(std::make_unique<Rect_XZ>(
      beyond::Point2f(0.f, 0.f), beyond::Point2f(555.f, 555.f), 555.f, white,
      Normal_Direction::Negetive));
  objects.push_back(std::make_unique<Rect_XZ>(
      beyond::Point2f(0.f, 0.f), beyond::Point2f(555.f, 555.f), 0.f, white));

  objects.push_back(std::make_unique<Rect_XY>(
      beyond::Point2f(0.f, 0.f), beyond::Point2f(555.f, 555.f), 555.f, white,
      Normal_Direction::Negetive));

  objects.push_back(std::make_unique<Sphere>(
      beyond::Point3f{200.f, 100.f, 300.f}, 100.f, metal));

  objects.push_back(std::make_unique<Sphere>(
      beyond::Point3f{300.f, 110.f, 100.f}, 100.f, glass));

  return Scene(std::make_unique<BVH_node>(objects.begin(), objects.end()),
               std::move(materials));
}

template <typename Duration>
void print_elapse_time(const Duration& elapsed_time)
{
  using namespace std::chrono;

  if (elapsed_time < 1s) {
    std::cout << duration_cast<milliseconds>(elapsed_time).count() << "ms\n";
  } else if (elapsed_time < 1min) {
    std::cout << duration_cast<seconds>(elapsed_time).count() << "s\n";
  } else {
    auto s = duration_cast<seconds>(elapsed_time).count();
    std::cout << (s / 60) << "min " << (s % 60) << "s\n";
  }
}

// static const char USAGE[] =
//    R"(Lesty
//
//    Usage:
//      lesty FILE [options]
//      lesty (-h | --help)
//      lesty (-v | --version)
//
//    Options:
//      -h --help               Show this screen.
//      -v --version            Show version.
//      --spp=<s>               Samples per pixel, only useful for algorithms
//      that support it [default: 10]. -o FILE, --output FILE  Name of the
//      output image
//      --width n               The pixel width of the output image [default:
//      800].
//      --height n              The pixel height of the output image [default:
//      600].
//)";

int main(/*int argc, const char** argv*/)
try {
  using namespace std::chrono;
  using namespace beyond::literals;

  //  std::map<std::string, docopt::value> args =
  //      docopt::docopt(USAGE, {argv + 1, argv + argc}, true, "lesty 0.0.0");
  //
  //  const std::string input_file = args["FILE"].asString();
  //  fmt::print("Reading from {}\n", input_file);
  //
  //  const std::string output_file = [&]() {
  //    const auto arg = args["--output"];
  //    if (arg) {
  //      return arg.asString();
  //    } else {
  //      return std::string{"output.png"};
  //    }
  //  }();
  //
  //  const auto spp = args["--spp"].asLong();
  //  const auto width = static_cast<size_t>(args["--width"].asLong());
  //  const auto height = static_cast<size_t>(args["--height"].asLong());

  const auto spp = 100;
  const auto width = 800;
  const auto height = 600;

  const auto output_file = "cornell.png";

  fmt::print("width: {}, height: {}, sample size: {}\n", width, height, spp);

  Path_tracer path_tracer;

  Image image(width, height);

  const auto aspect_ratio = static_cast<float>(width) / height;
  Camera camera{
      {278, 278, -800}, {278, 278, 0}, {0, 1, 0}, 40.0_deg, aspect_ratio};
  const auto scene = create_scene();

  const auto start = std::chrono::system_clock::now();

  path_tracer.run(scene, camera, image, static_cast<size_t>(spp));
  const auto end = std::chrono::system_clock::now();

  std::puts("elapsed time: ");
  print_elapse_time(end - start);

  image.saveto(output_file);
  fmt::print("Save image to {}\n", output_file);
  return 0;
} catch (const std::exception& e) {
  fmt::print(stderr, "Error: {}\n", e.what());
  throw e;
} catch (...) {
  std::fputs("Unknown exception", stderr);
  return -255;
}