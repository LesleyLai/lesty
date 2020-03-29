#include <catch2/catch.hpp>

#include "../src/triangle.hpp"

static const lesty::Lambertian dummy_mat{lesty::Color(0.5f, 0.5f, 0.5f)};
static constexpr float inf = std::numeric_limits<float>::infinity();

TEST_CASE("AABBs for triangle", "[geometry] [AABB] [triangle]")
{
  const auto box =
      lesty::triangle_bounding_box({0, 1, 0}, {0, 0, 1}, {2, 1, 2});
  REQUIRE(box == lesty::AABB({0, 0, 0}, {2, 1, 2}));
}

TEST_CASE("Ray-Triangle intersection", "[geometry] [triangle]")
{
  GIVEN("A triangle tri")
  {
    const beyond::Point3 p1{0, 1, 0};
    const beyond::Point3 p2{-1, 0, 0};
    const beyond::Point3 p3{1, 0, 0};

    THEN("Its normal is (0, 0, 1)")
    {
      REQUIRE(lesty::triangle_normal(p1, p2, p3) == beyond::Point3(0, 0, 1));
    }

    AND_GIVEN("A ray r that intersects the triangle")
    {
      const lesty::Ray r({0, 0, 2}, {0, 0, -1});

      THEN("Intersect at (0, 0, 0)")
      {
        const auto hit =
            lesty::triangle_ray_intersection(r, 0, inf, p1, p2, p3);
        REQUIRE(hit);
        const auto t = *hit;
        REQUIRE(t == 2);
        REQUIRE(r(t) == beyond::Point3(0, 0, 0));
      }
    }

    AND_GIVEN("A ray r that misses p1-p2 edge")
    {
      const lesty::Ray r({-1, 1, -2}, {0, 0, 1});
      THEN("The triangle tri and ray r do not intersect")
      {
        const auto hit =
            lesty::triangle_ray_intersection(r, 0, inf, p1, p2, p3);
        REQUIRE(!hit);
      }
    }

    AND_GIVEN("A ray r that misses p1-p3 edge")
    {
      const lesty::Ray r({1, 1, -2}, {0, 0, 1});
      THEN("The triangle tri and ray r do not intersect")
      {
        const auto hit =
            lesty::triangle_ray_intersection(r, 0, inf, p1, p2, p3);
        REQUIRE(!hit);
      }
    }

    AND_GIVEN("A ray r that misses p2-p3 edge")
    {
      const lesty::Ray r({0, -1, -2}, {0, 0, 1});
      THEN("The triangle tri and ray r do not intersect")
      {
        const auto hit =
            lesty::triangle_ray_intersection(r, 0, inf, p1, p2, p3);
        REQUIRE(!hit);
      }
    }
  }
}
