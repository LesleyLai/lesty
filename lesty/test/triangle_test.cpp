#include <catch2/catch.hpp>

#include <array>
#include <optional>

#include "hitable.hpp"
#include "material.hpp"

namespace lesty {

struct Triangle : Hitable {
  std::array<beyond::Point3, 3> vertices{};
  const Material* material;

  Triangle(std::array<beyond::Point3, 3> vertices, const Material& mat)
      : vertices{std::move(vertices)}, material{&mat}
  {
  }

  [[nodiscard]] auto bounding_box() const -> AABB override;

  [[nodiscard]] auto intersect_at(const Ray& r, float t_min, float t_max) const
      -> std::optional<HitRecord> override;
};

auto Triangle::bounding_box() const -> AABB
{
  return aabb_union(AABB(vertices[0], vertices[1]), AABB(vertices[2]));
}

auto Triangle::intersect_at(const Ray& r, float t_min, float t_max) const
    -> std::optional<HitRecord>
{
  return std::optional<HitRecord>();
}

} // namespace lesty

static const lesty::Lambertian dummy_mat{lesty::Color(0.5f, 0.5f, 0.5f)};

TEST_CASE("AABBs for triangle", "[geometry] [AABB]")
{
  lesty::Triangle tri{std::array{beyond::Point3{0, 1, 0},
                                 beyond::Point3{0, 0, 1},
                                 beyond::Point3{2, 1, 2}},
                      dummy_mat};
  const auto box = tri.bounding_box();
  REQUIRE(box == lesty::AABB({0, 0, 0}, {2, 1, 2}));
}

// TEST_CASE("Ray-Sphere intersection", "[geometry]")
