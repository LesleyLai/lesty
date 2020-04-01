#ifndef LESTY_HITABLE_HPP
#define LESTY_HITABLE_HPP

#include <beyond/geometry/ray.hpp>
#include <beyond/math/vector.hpp>
#include <beyond/types/optional.hpp>

#include "beyond/geometry/aabb3.hpp"

namespace lesty {

class Material;

/**
 * @brief Data recorded for a ray-object intersection
 */
struct HitRecord {
  float t{};
  beyond::Point3 point{}; ///< Intersection point
  beyond::Vec3
      normal{}; ///< Surface normal, need to be construct as a unit vector
  const Material* material{};
};

struct Hitable {
  Hitable() = default;
  virtual ~Hitable() = default;
  Hitable(const Hitable&) = delete;
  auto operator=(const Hitable&) = delete;
  Hitable(Hitable&&) = delete;
  auto operator=(Hitable&&) = delete;

  /**
   * @brief Get the bounding box of an object
   * @return An AABB3 for objects with bounding_box, nothing otherwise (for
   * example, infinite plane)
   */
  [[nodiscard]] virtual auto bounding_box() const -> beyond::AABB3 = 0;

  /**
   * @brief Ray-object intersection detection
   * @return A optional record of intersection information, nothing if not hit
   */
  [[nodiscard]] virtual auto intersection_with(const beyond::Ray& r,
                                               float t_min, float t_max) const
      -> beyond::optional<HitRecord> = 0;
};

} // namespace lesty

#endif // LESTY_HITABLE_HPP
