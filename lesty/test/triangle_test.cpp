#include <catch2/catch.hpp>

#include "triangle.hpp"

static const lesty::Lambertian dummy_mat{lesty::Color(0.5f, 0.5f, 0.5f)};
static constexpr float inf = std::numeric_limits<float>::infinity();

TEST_CASE("AABBs for triangle", "[geometry] [AABB] [triangle]")
{
  const lesty::Triangle tri{{0, 1, 0}, {0, 0, 1}, {2, 1, 2}, dummy_mat};
  const auto box = tri.bounding_box();
  REQUIRE(box == lesty::AABB({0, 0, 0}, {2, 1, 2}));
}

TEST_CASE("Ray-Triangle intersection", "[geometry] [triangle]")
{
  GIVEN("A triangle tri")
  {
    const lesty::Triangle tri{{0, 1, 0}, {-1, 0, 0}, {1, 0, 0}, dummy_mat};

    AND_GIVEN("A ray r that intersects the triangle")
    {
      const lesty::Ray r({0, 0, 2}, {0, 0, -1});

      THEN("Intersect at (0, 0, 0)")
      {
        const auto hit_record = tri.intersection_with(r, 0, inf);
        REQUIRE(hit_record);
        REQUIRE(hit_record->t == Approx(2));
        REQUIRE(hit_record->point == beyond::Point3(0, 0, 0));
        REQUIRE(hit_record->normal == beyond::Point3(0, 0, 1));
      }
    }

    AND_GIVEN("A ray r that misses p1-p2 edge")
    {
      const lesty::Ray r({-1, 1, -2}, {0, 0, 1});
      THEN("The triangle tri and ray r do not intersect")
      {
        const auto hit_record = tri.intersection_with(r, 0, inf);
        REQUIRE(!hit_record);
      }
    }

    AND_GIVEN("A ray r that misses p1-p3 edge")
    {
      const lesty::Ray r({1, 1, -2}, {0, 0, 1});
      THEN("The triangle tri and ray r do not intersect")
      {
        const auto hit_record = tri.intersection_with(r, 0, inf);
        REQUIRE(!hit_record);
      }
    }

    AND_GIVEN("A ray r that misses p2-p3 edge")
    {
      const lesty::Ray r({0, -1, -2}, {0, 0, 1});
      THEN("The triangle tri and ray r do not intersect")
      {
        const auto hit_record = tri.intersection_with(r, 0, inf);
        REQUIRE(!hit_record);
      }
    }
  }
}