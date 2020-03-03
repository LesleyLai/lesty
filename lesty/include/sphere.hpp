/**
 * @file sphere.hpp
 * @brief File for sphere, one of the geometric primetives
 */

#ifndef LESTY_SPHERE_HPP
#define LESTY_SPHERE_HPP

#include <cassert>
#include <optional>

#include "hitable.hpp"
#include "material.hpp"

namespace lesty {

struct Sphere : Hitable {
  beyond::Point3f center{};
  float radius = 1;

  Sphere(beyond::Point3f center, float radius, const Material& mat)
      : center{center}, radius{radius}, material{&mat}
  {
  }

  std::optional<AABB> bounding_box() const noexcept override;

  /**
   * @brief Ray-sphere intersection detection
   * @see Hitable::intersect_at
   */
  Maybe_hit_t intersect_at(const Ray& r, float t_min, float t_max) const
      noexcept override;

  const Material* const material;
};

} // namespace lesty

#endif // LESTY_SPHERE_HPP
