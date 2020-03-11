#include "ray.hpp"
#include "sphere.hpp"
#include <catch2/catch.hpp>
#include <limits>

using lesty::AABB;
using lesty::Color;
using lesty::Ray;
using lesty::Sphere;

static const lesty::Lambertian dummy_mat{Color(0.5f, 0.5f, 0.5f)};
constexpr float inf = std::numeric_limits<float>::infinity();

TEST_CASE("AABBs for sphere", "[geometry] [sphere] [AABB]")
{
  Sphere sphere{{0, 0, 2}, 1, dummy_mat};
  const auto box = sphere.bounding_box();
  REQUIRE(box == AABB({-1, -1, 1}, {1, 1, 3}));
}

TEST_CASE("Ray-Sphere intersection", "[geometry] [sphere]")
{
  Ray ray({0, 0, 0}, {0, 0, 1});

  SECTION("intersect_at returns nullopt if sphere does not intersect the ray")
  {
    Sphere sphere{{0, 2, 0}, 1, dummy_mat};
    REQUIRE_FALSE(sphere.intersect_at(ray, 0, inf));
  }

  SECTION(
      "intersect_at returns nullopt if intersections happened behind the ray")
  {
    Sphere sphere{{0, 0, -2}, 1, dummy_mat};
    REQUIRE_FALSE(sphere.intersect_at(ray, 0, inf));
  }

  SECTION("intersect_at returns smaller t when intersect with the sphere at "
          "two points")
  {
    Sphere sphere{{0, 0, 2}, 1, dummy_mat};
    const auto result = sphere.intersect_at(ray, 0, inf);
    REQUIRE(result);
    REQUIRE(result->t == Approx(1));
  }

  SECTION("intersect_at returns positive t if one of the intersection point "
          "happened behind the ray")
  {
    Sphere sphere{{0, 0, 0}, 2, dummy_mat};
    const auto result = sphere.intersect_at(ray, 0, inf);
    REQUIRE(result);
    REQUIRE(result->t == Approx(2));
  }
}
