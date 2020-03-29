#include <catch2/catch.hpp>
#include <sstream>

#include "aabb.hpp"

static constexpr float inf = std::numeric_limits<float>::infinity();

using beyond::Point3;
using lesty::AABB;
using lesty::Ray;

TEST_CASE("AABB construction", "AABB")
{
  SECTION("Default constructed AABB is at (0, 0, 0)")
  {
    const AABB box{};
    REQUIRE(box.min() == Point3{0, 0, 0});
    REQUIRE(box.max() == Point3{0, 0, 0});
  }

  SECTION("Create AABB with one point")
  {
    const Point3 p{1, 2, 2};
    const AABB box{p};
    REQUIRE(box.min() == p);
    REQUIRE(box.max() == p);
  }

  SECTION("Create AABB with two points")
  {
    const Point3 p1{1, 2, 2};
    const Point3 p2{-1, 5, 4};

    const AABB box{p1, p2};
    REQUIRE(box.min() == Point3{-1, 2, 2});
    REQUIRE(box.max() == Point3{1, 5, 4});
  }

  SECTION("Create AABB with min and max points unchecked")
  {
    const Point3 p1{1, 2, 2};
    const Point3 p2{-1, 5, 4};

    const AABB box{p1, p2, AABB::unchecked_tag};
    REQUIRE(box.min() == p1);
    REQUIRE(box.max() == p2);
  }
}

TEST_CASE("Ray/AABB intersection", "[AABB]")
{
  const AABB box({0, 0, 0}, {1, 1, 1}, AABB::unchecked_tag);

  SECTION("Intersect with ray that penetrates its middle")
  {
    const Ray r({0.5, -1, 0.5}, {0, 1, 0});
    REQUIRE(box.is_intersect_with(r, 0, inf));
  }

  SECTION("Intersect with ray that penetrates its conner")
  {
    const Ray r({0, -0.5f, 0}, {0, 1, 1});
    REQUIRE(box.is_intersect_with(r, 0, inf));
  }

  SECTION("Not intersect with ray of divert direction")
  {
    const Ray r({0, -1, 0}, {1, 1, 1});
    REQUIRE(!box.is_intersect_with(r, 0, inf));
  }

  SECTION("Not intersect with ray of negative direction")
  {
    const Ray r({0.5, -1, 0.5}, {0, -1, 0});
    REQUIRE(!box.is_intersect_with(r, 0, inf));
  }

  SECTION("Not intersect with limited t")
  {
    const Ray r({0.5f, -1, 0.5f}, {0, 1, 0});
    REQUIRE(!box.is_intersect_with(r, 0, 0.9f));
  }
}

TEST_CASE("Compose AABBs", "[AABB]")
{
  AABB box0{{0, 0, 0}, {1, 1, 1}, AABB::unchecked_tag};
  AABB box1{{-1, -1, -1}, {0.5, 0.5, 0.5}, AABB::unchecked_tag};
  REQUIRE(aabb_union(box0, box1) ==
          AABB{{-1, -1, -1}, {1, 1, 1}, AABB::unchecked_tag});
}

TEST_CASE("AABB Serialization", "[AABB]")
{
  const auto expected =
      "AABB(min: point(0.0, 0.0, 0.0), max: point(1.0, 1.0, 1.0))";

  AABB box{{0, 0, 0}, {1, 1, 1}, AABB::unchecked_tag};
  REQUIRE(to_string(box) == expected);

  std::stringstream ss;
  ss << box;
  REQUIRE(ss.str() == expected);
}
