#pragma once

#ifndef LESTY_TRIANGLE_HPP
#define LESTY_TRIANGLE_HPP

#include <array>
#include <beyond/math/vector.hpp>
#include <optional>

#include "hitable.hpp"
#include "material.hpp"

namespace lesty {

[[nodiscard]] constexpr auto triangle_bounding_box(const beyond::Point3& p0,
                                                   const beyond::Point3& p1,
                                                   const beyond::Point3& p2)
    -> AABB
{
  return aabb_union(AABB(p0, p1), AABB(p2));
}

[[nodiscard]] constexpr auto
triangle_ray_intersection(const Ray& r, float t_min, float t_max,
                          const beyond::Point3& p1, const beyond::Point3& p2,
                          const beyond::Point3& p3) -> beyond::optional<float>
{
  const auto vd = r.direction;
  const auto ve = r.origin;

  const auto a = p1.x - p2.x;
  const auto b = p1.y - p2.y;
  const auto c = p1.z - p2.z;
  const auto d = p1.x - p3.x;
  const auto e = p1.y - p3.y;
  const auto f = p1.z - p3.z;
  const auto g = vd.x;
  const auto h = vd.y;
  const auto i = vd.z;
  const auto j = p1.x - ve.x;
  const auto k = p1.y - ve.y;
  const auto l = p1.z - ve.z;

  const auto ei_hf = e * i - h * f;
  const auto gf_di = g * f - d * i;
  const auto dh_eg = d * h - e * g;
  const auto ak_jb = a * k - j * b;
  const auto jc_al = j * c - a * l;
  const auto bl_kc = b * l - k * c;

  const auto M = a * ei_hf + b * gf_di + c * dh_eg;

  // compute t
  const auto t = -(f * ak_jb + e * jc_al + d * bl_kc) / M;
  if ((t < t_min) || (t > t_max)) {
    return {};
  }

  // compute gamma
  const auto gamma = (i * ak_jb + h * jc_al + g * bl_kc) / M;
  if ((gamma < 0) || (gamma > 1)) {
    return {};
  }

  // compute beta
  const auto beta = (j * ei_hf + k * gf_di + l * dh_eg) / M;
  if ((beta < 0) || (beta > (1 - gamma))) {
    return {};
  }

  return t;
}

[[nodiscard]] inline auto triangle_normal(const beyond::Point3& p0,
                                          const beyond::Point3& p1,
                                          const beyond::Point3& p2)
{
  return beyond::normalize(beyond::cross(p1 - p0, p2 - p1));
}

} // namespace lesty

#endif // LESTY_TRIANGLE_HPP
