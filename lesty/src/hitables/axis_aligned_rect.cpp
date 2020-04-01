#include "axis_aligned_rect.hpp"

namespace {

constexpr beyond::Vec3 flip_negative_normal(beyond::Vec3 normal,
                                            lesty::NormalDirection d)
{
  return d == lesty::NormalDirection::Negetive ? -normal : normal;
}

} // anonymous namespace

namespace lesty {

[[nodiscard]] auto Rect_XY::intersection_with(const beyond::Ray& r, float t_min,
                                              float t_max) const
    -> beyond::optional<HitRecord>
{
  const float t = (z - r.origin.z) / r.direction.z;
  if (t < t_min || t > t_max) {
    return {};
  }

  const float x = r.origin.x + t * r.direction.x;
  const float y = r.origin.y + t * r.direction.y;
  if (x < min.x || x > max.x || y < min.y || y > max.y) {
    return beyond::nullopt;
  }

  return HitRecord{t, r(t),
                   flip_negative_normal(beyond::Vec3(0, 0, 1), direction),
                   material};
}

[[nodiscard]] auto Rect_XZ::intersection_with(const beyond::Ray& r, float t_min,
                                              float t_max) const
    -> beyond::optional<HitRecord>
{
  const float t = (y - r.origin.y) / r.direction.y;
  if (t < t_min || t > t_max) {
    return {};
  }

  const float x = r.origin.x + t * r.direction.x;
  const float z = r.origin.z + t * r.direction.z;
  if (x < min.x || x > max.x || z < min.y || z > max.y) {
    return {};
  }

  return HitRecord{t, r(t),
                   flip_negative_normal(beyond::Vec3(0, 1, 0), direction),
                   material};
}

[[nodiscard]] auto Rect_YZ::intersection_with(const beyond::Ray& r, float t_min,
                                              float t_max) const
    -> beyond::optional<HitRecord>
{
  const float t = (x - r.origin.x) / r.direction.x;
  if (t < t_min || t > t_max) {
    return {};
  }

  const float y = r.origin.y + t * r.direction.y;
  const float z = r.origin.z + t * r.direction.z;
  if (y < min.x || y > max.x || z < min.y || z > max.y) {
    return {};
  }

  return HitRecord{t, r(t),
                   flip_negative_normal(beyond::Vec3(1, 0, 0), direction),
                   material};
}

} // namespace lesty