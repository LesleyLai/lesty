#include <cmath>
#include <utility>

#include "ray.hpp"
#include "sphere.hpp"

namespace lesty {

auto Sphere::bounding_box() const -> AABB
{
  const beyond::Vec3 offset(radius, radius, radius);
  return AABB{center - offset, center + offset, AABB::unchecked_tag};
}

auto Sphere::intersection_with(const Ray& r, float t_min, float t_max) const
    -> beyond::optional<HitRecord>
{
  const auto oc = r.origin - center;

  const auto a = dot(r.direction, r.direction);
  const auto b = 2 * dot(r.direction, oc);
  const auto c = dot(oc, oc) - radius * radius;
  const auto discrimination = b * b - 4 * a * c;

  if (discrimination < 0) {
    return {};
  }

  const auto sqrt_delta = std::sqrt(discrimination);
  const auto t1 = (-b - sqrt_delta) / (2 * a);
  const auto t2 = (-b + sqrt_delta) / (2 * a);

  auto hit_record_from_t = [&r, this](float t) -> beyond::optional<HitRecord> {
    const auto point = r(t);
    const auto normal = (point - center) / radius;
    return HitRecord{t, point, normal, material};
  };

  // Get the smaller non-negative value of t1, t2
  if (t1 >= t_min && t1 < t_max) {
    return hit_record_from_t(t1);
  }
  if (t2 >= t_min && t2 < t_max) {
    return hit_record_from_t(t2);
  }
  return {};
}

} // namespace lesty