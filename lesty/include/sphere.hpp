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
  beyond::Point3 center{};
  float radius = 1;

  Sphere(beyond::Point3 c, float r, const Material& mat)
      : center{c}, radius{r}, material{&mat}
  {
  }

  [[nodiscard]] auto bounding_box() const -> AABB override;

  /**
   * @brief Ray-sphere intersection detection
   * @see Hitable::intersection_with
   */
  [[nodiscard]] auto intersection_with(const Ray& r, float t_min,
                                       float t_max) const
      -> std::optional<HitRecord> override;

  const Material* const material;
};

} // namespace lesty

#endif // LESTY_SPHERE_HPP
