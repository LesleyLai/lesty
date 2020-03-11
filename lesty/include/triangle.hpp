#pragma once

#ifndef LESTY_TRIANGLE_HPP
#define LESTY_TRIANGLE_HPP

#include <array>
#include <beyond/core/math/vector.hpp>
#include <optional>

#include "hitable.hpp"
#include "material.hpp"

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

  [[nodiscard]] auto intersection_with(const Ray& r, float t_min,
                                       float t_max) const
      -> std::optional<HitRecord> override;

  // Calculate the normal of a triangle
  [[nodiscard]] constexpr auto normal() const -> beyond::Vec3;
};

[[nodiscard]] constexpr auto Triangle::normal() const -> beyond::Vec3
{
  return beyond::normalize(
      beyond::cross(vertices[1] - vertices[0], vertices[2] - vertices[0]));
}

} // namespace lesty

#endif // LESTY_TRIANGLE_HPP
