#include <catch2/catch.hpp>

#include <array>
#include <optional>

#include "hitable.hpp"
#include "material.hpp"

#include <beyond/core/math/vector.hpp>

namespace lesty {

struct Triangle : Hitable {
  std::array<beyond::Point3, 3> vertices{};
  const Material* material;

  Triangle(const beyond::Point3& p1, const beyond::Point3& p2,
           const beyond::Point3& p3, const Material& mat)
      : vertices{p1, p2, p3}, material{&mat}
  {
  }

  [[nodiscard]] auto bounding_box() const -> AABB override;

  [[nodiscard]] auto intersect_at(const Ray& r, float t_min, float t_max) const
      -> std::optional<HitRecord> override;

  // Calculate the normal of a triangle
  [[nodiscard]] constexpr auto normal() const -> beyond::Vec3;
};

[[nodiscard]] auto Triangle::bounding_box() const -> AABB
{
  return aabb_union(AABB(vertices[0], vertices[1]), AABB(vertices[2]));
}

[[nodiscard]] constexpr auto Triangle::normal() const -> beyond::Vec3
{
  return beyond::normalize(
      beyond::cross(vertices[1] - vertices[0], vertices[2] - vertices[0]));
}

[[nodiscard]] auto Triangle::intersect_at(const Ray& r, float t_min,
                                          float t_max) const
    -> std::optional<HitRecord>
{

  const auto vd = r.direction;
  const auto ve = r.origin;
  const auto va = vertices[0];
  const auto vb = vertices[1];
  const auto vc = vertices[2];

  const auto a = va.x - vb.x;
  const auto b = va.y - vb.y;
  const auto c = va.z - vb.z;
  const auto d = va.x - vc.x;
  const auto e = va.y - vc.y;
  const auto f = va.z - vc.z;
  const auto g = vd.x;
  const auto h = vd.y;
  const auto i = vd.z;
  const auto j = va.x - ve.x;
  const auto k = va.y - ve.y;
  const auto l = va.z - ve.z;

  const auto ei_hf = e * i - h * f;
  const auto gf_di = g * f - d * i;
  const auto dh_eg = d * h - e * g;
  const auto ak_jb = a * k - j * b;
  const auto jc_al = j * c - a * l;
  const auto bl_kc = b * l - k * c;

  const auto M = a * ei_hf + b * gf_di + c * dh_eg;

  // compute t
  const auto t = -(f * ak_jb + e * jc_al + d * bl_kc) / M;
  if ((t < t_min) || (t > t_max))
    return {};

  // compute gamma
  const auto gamma = (i * ak_jb + h * jc_al + g * bl_kc) / M;
  if ((gamma < 0) || (gamma > 1))
    return {};

  // compute beta
  const auto beta = (j * ei_hf + k * gf_di + l * dh_eg) / M;
  if ((beta < 0) || (beta > (1 - gamma)))
    return {};

  const auto n = normal();
  return HitRecord{t, r(t), n, material};
}

} // namespace lesty

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
        const auto hit_record = tri.intersect_at(r, 0, inf);
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
        const auto hit_record = tri.intersect_at(r, 0, inf);
        REQUIRE(!hit_record);
      }
    }

    AND_GIVEN("A ray r that misses p1-p3 edge")
    {
      const lesty::Ray r({1, 1, -2}, {0, 0, 1});
      THEN("The triangle tri and ray r do not intersect")
      {
        const auto hit_record = tri.intersect_at(r, 0, inf);
        REQUIRE(!hit_record);
      }
    }

    AND_GIVEN("A ray r that misses p2-p3 edge")
    {
      const lesty::Ray r({0, -1, -2}, {0, 0, 1});
      THEN("The triangle tri and ray r do not intersect")
      {
        const auto hit_record = tri.intersect_at(r, 0, inf);
        REQUIRE(!hit_record);
      }
    }
  }
}

// TEST_CASE("Ray-Sphere intersection", "[geometry]")
