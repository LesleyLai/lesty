#include "scene_parser.hpp"
#include "bounding_volume_hierarchy.hpp"
#include "hitables/axis_aligned_rect.hpp"
#include "hitables/sphere.hpp"
#include "hitables/triangle.hpp"
#include "material.hpp"

#include "nlohmann/json.hpp"

namespace lesty {

[[nodiscard]] auto parse_scene(std::ifstream& file) -> Scene
{
  nlohmann::json json;
  file >> json;

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
    } else if (type == "Triangle") {
      const auto tri_json = obj_json["points"];
      objects.emplace_back(std::make_unique<Triangle>(
          parse_point3(tri_json.at(0)), parse_point3(tri_json.at(1)),
          parse_point3(tri_json.at(2)), material));
    } else {
      throw std::runtime_error(fmt::format("Invalid object type {}\n", type));
    }
  }

  return Scene(std::make_unique<BVH_node>(objects.begin(), objects.end()),
               std::move(materials));
}

} // namespace lesty