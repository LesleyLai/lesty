#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4702)
#endif

#include <cxxopts.hpp>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "nlohmann/json.hpp"

#include "axis_aligned_rect.hpp"
#include "bounding_volume_hierarchy.hpp"
#include "image.hpp"
#include "material.hpp"
#include "pathtracer.hpp"
#include "scene.hpp"
#include "sphere.hpp"

using namespace lesty;

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

int main(int argc, char** argv) try {
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
      return result["input"].as<std::string>();
    } else {
      std::fputs("Error: Need an input file\n\n", stderr);
      std::exit(-1);
    }
  }();
  fmt::print("input file: {}\n", input);

  const auto spp = result["spp"].as<size_t>();
  const auto width = result["width"].as<size_t>();
  const auto height = result["height"].as<size_t>();
  const auto output_file = result["output"].as<std::string>();

  fmt::print("width: {}, height: {}, sample size: {}\n", width, height, spp);

  std::ifstream input_file{input};
  if (!input_file.is_open()) {
    fmt::print(stderr, "Error: cannot open file \"{}\"\n", input);
    std::exit(2);
  }

  nlohmann::json json;
  input_file >> json;

  if (!json["title"].is_string()) {
    std::puts("Missing title in input file");
    std::exit(3);
  }

  if (!json["objects"].is_array()) {
    std::puts("Missing the objects in the input file");
    std::exit(3);
  }

  if (!json["materials"].is_array()) {
    std::puts("Missing the materials in the input file");
    std::exit(3);
  }

  const auto title = json["title"].get<std::string>();
  fmt::print("Title: {}\n", title);

  auto parse_color = [](nlohmann::json color_json) {
    return lesty::Color{color_json.at(0).get<float>(),
                        color_json.at(1).get<float>(),
                        color_json.at(2).get<float>()};
  };

  std::vector<std::unique_ptr<Material>> materials;
  for (const auto& mat_json : json["materials"]) {
    const auto type = mat_json["type"].get<std::string>();
    if (type == "Lambertian") {
      const auto albedo = parse_color(mat_json["albedo"]);
      materials.emplace_back(std::make_unique<Lambertian>(albedo));
    } else if (type == "Emission") {
      const auto albedo = parse_color(mat_json["emit"]);
      materials.emplace_back(std::make_unique<Emission>(albedo));
    } else if (type == "Metal") {
      const auto albedo = parse_color(mat_json["albedo"]);
      materials.emplace_back(
          std::make_unique<Metal>(albedo, mat_json["fuzzness"].get<float>()));
    } else if (type == "Dialectic") {
      const auto albedo = parse_color(mat_json["albedo"]);
      materials.emplace_back(std::make_unique<Dielectric>(
          albedo, mat_json["refractive_index"].get<float>()));
    } else {
      throw std::runtime_error(fmt::format("Invalid material type {}\n", type));
    }
  }

  auto parse_point2 = [](nlohmann::json pt_json) {
    return beyond::Point2{pt_json.at(0).get<float>(),
                          pt_json.at(1).get<float>()};
  };

  auto parse_point3 = [](nlohmann::json pt_json) {
    return beyond::Point3{pt_json.at(0).get<float>(),
                          pt_json.at(1).get<float>(),
                          pt_json.at(2).get<float>()};
  };

  const auto materials_count = materials.size();
  std::vector<std::unique_ptr<Hitable>> objects;
  for (const auto& obj_json : json["objects"]) {
    const auto type = obj_json["type"].get<std::string>();

    const auto material_id = obj_json["material"].get<std::size_t>();
    if (material_id >= materials_count) {
      throw std::runtime_error(
          fmt::format("Invalid material index {}, totally {} materials\n",
                      material_id, materials_count));
    }
    const auto& material = *materials[material_id];

    if (type.starts_with("Rect")) {
      auto min = parse_point2(obj_json["min"]);
      auto max = parse_point2(obj_json["max"]);

      auto normal_direction = (obj_json["normal_direction"] > 0)
                                  ? NormalDirection::Positive
                                  : NormalDirection::Negetive;

      if (type == "RectYZ") {
        objects.emplace_back(std::make_unique<Rect_YZ>(
            min, max, obj_json["x"].get<float>(), material, normal_direction));
      } else if (type == "RectXZ") {
        objects.emplace_back(std::make_unique<Rect_XZ>(
            min, max, obj_json["y"].get<float>(), material, normal_direction));
      } else if (type == "RectXY") {
        objects.emplace_back(std::make_unique<Rect_XY>(
            min, max, obj_json["z"].get<float>(), material, normal_direction));
      } else {
        throw std::runtime_error(fmt::format("Invalid object type {}\n", type));
      }
    } else if (type == "Sphere") {
      auto center = parse_point3(obj_json["center"]);

      objects.emplace_back(std::make_unique<Sphere>(
          center, obj_json["radius"].get<float>(), material));
    } else {
      throw std::runtime_error(fmt::format("Invalid object type {}\n", type));
    }
  }

  Path_tracer path_tracer;

  Image image(width, height);

  const auto aspect_ratio =
      static_cast<float>(width) / static_cast<float>(height);
  Camera camera{
      {278, 278, -800}, {278, 278, 0}, {0, 1, 0}, 40.0_deg, aspect_ratio};
  const auto scene =
      Scene(std::make_unique<BVH_node>(objects.begin(), objects.end()),
            std::move(materials));

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