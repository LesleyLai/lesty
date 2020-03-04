#include <chrono>
#include <cstdio>
#include <iostream>
#include <memory>
#include <vector>

#include <cxxopts.hpp>
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
[[nodiscard]] auto get_elapse_time(const Duration& elapsed_time) -> std::string
{
  using namespace std::chrono;

  if (elapsed_time < 1s) {
    return fmt::format("{}ms",
                       duration_cast<milliseconds>(elapsed_time).count());
  } else if (elapsed_time < 1min) {
    return fmt::format("{}s\n", duration_cast<seconds>(elapsed_time).count());
  } else {
    auto s = duration_cast<seconds>(elapsed_time).count();
    return fmt::format("{}min {}s", s / 60, s % 60);
  }
}

int main(int argc, char** argv)
try {
  using namespace std::chrono;
  using namespace beyond::literals;

  cxxopts::Options options("lesty",
                           "Command line interface of the lesty renderer");

  options.positional_help("FILE").show_positional_help();

  // clang-format off
  options.add_options()
  ("h,help", "Print help")
  ("i,input", "File name of the input", cxxopts::value<std::string>());
  // clang-format on

  // clang-format off
  options.add_options("Renderer")
      ("spp","Samples per pixel, only useful for algorithms that support it",cxxopts::value<size_t>()->default_value("10"));
  // clang-format on

  // clang-format off
  options.add_options("Output")
  ("o,output", "File name of the output image", cxxopts::value<std::string>()->default_value("output.png"))
  ("width","Width of the output image in pixels",cxxopts::value<size_t>()->default_value("800"))
  ("height","Height of the output image in pixels",cxxopts::value<size_t>()->default_value("600"));
  // clang-format on

  options.parse_positional({"input"});

  const auto print_help = [options]() {
    std::puts(options.help({"", "Renderer", "Output"}).c_str());
  };

  auto result = [&]() {
    try {
      return options.parse(argc, argv);
    } catch (cxxopts::OptionParseException& e) {
      fmt::print(stderr, "Error: {}\n", e.what());
      std::exit(-1);
    }
  }();

  if (result.count("help")) {
    std::puts(options.help({"", "Renderer"}).c_str());
    exit(0);
  }

  const auto input = [&]() {
    if (result.count("input")) {
      return result["input"];
    } else {
      std::fputs("Error: Need an input file\n\n", stderr);
      std::exit(-1);
    }
  }();
  fmt::print("input file: {}\n", result["input"].as<std::string>());

  const auto spp = result["spp"].as<size_t>();
  const auto width = result["width"].as<size_t>();
  const auto height = result["height"].as<size_t>();
  const auto output_file = result["output"].as<std::string>();

  fmt::print("width: {}, height: {}, sample size: {}\n", width, height, spp);

  Path_tracer path_tracer;

  Image image(width, height);

  const auto aspect_ratio =
      static_cast<float>(width) / static_cast<float>(height);
  Camera camera{
      {278, 278, -800}, {278, 278, 0}, {0, 1, 0}, 40.0_deg, aspect_ratio};
  const auto scene = create_scene();

  const auto start = std::chrono::system_clock::now();

  path_tracer.run(scene, camera, image, spp);
  const auto end = std::chrono::system_clock::now();

  std::fflush(stdout);
  fmt::print("Elapsed time: {}\n", get_elapse_time(end - start));

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