#include <catch2/catch.hpp>

#include "aabb.hpp"

static constexpr float inf = std::numeric_limits<float>::infinity();

using beyond::Point3f;
using lesty::AABB;
using lesty::Ray;

TEST_CASE("Ray/AABB intersection", "[AABB]")
{
  const AABB box(Point3f(0, 0, 0), Point3f(1, 1, 1));

  SECTION("Intersect with ray that penetrates its middle")
  {
    const Ray r({0, -1, 0}, {0, 1, 0});
    REQUIRE(box.hit(r, 0, inf) == true);
  }

  SECTION("Intersect with ray that penetrates its conner")
  {
    const Ray r({0, -0.5f, 0}, {0, 1, 1});
    REQUIRE(box.hit(r, 0, inf) == true);
  }

  SECTION("Not intersect with ray of divert direction")
  {
    const Ray r({0, -1, 0}, {1, 1, 1});
    REQUIRE(box.hit(r, 0, inf) == false);
  }

  SECTION("Not intersect with ray of negative direction")
  {
    const Ray r({0, -1, 0}, {0, -1, 0});
    REQUIRE(box.hit(r, 0, inf) == false);
  }

  SECTION("Not intersect with limited t")
  {
    const Ray r({0, -1, 0}, {0, 1, 0});
    REQUIRE(box.hit(r, 0, 0.9f) == false);
  }
}

TEST_CASE("Compose AABBs", "[AABB]")
{
  AABB box0{{0, 0, 0}, {1, 1, 1}};
  AABB box1{{-1, -1, -1}, {0.5, 0.5, 0.5}};
  REQUIRE(surrounding_box(box0, box1) == AABB{{-1, -1, -1}, {1, 1, 1}});
}
