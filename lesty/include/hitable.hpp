#ifndef LESTY_HITABLE_HPP
#define LESTY_HITABLE_HPP

#include <optional>

#include <beyond/core/math/vector.hpp>

#include "aabb.hpp"

namespace lesty {

struct Ray;
class Material;

/**
 * @brief Data recorded for a ray-object intersection
 */
struct HitRecord {
  float t{};
  beyond::Point3 point{}; ///< Intersection point
  beyond::Vec3
      normal{}; ///< Surface normal, need to be construct as a unit vector
  const Material* const material{};
};

using Maybe_hit_t = std::optional<HitRecord>;

struct Hitable {
  virtual ~Hitable() = default;

  /**
   * @brief Get the bounding box of an object
   * @return An AABB for objects with bounding_box, nothing otherwise (for
   * example, infinite plane)
   */
  [[nodiscard]] virtual auto bounding_box() const -> AABB = 0;

  /**
   * @brief Ray-object intersection detection
   * @return A optional record of intersection information, nothing if not hit
   */
  [[nodiscard]] virtual auto intersect_at(const Ray& r, float t_min,
                                          float t_max) const
      -> std::optional<HitRecord> = 0;
};

} // namespace lesty

#endif // LESTY_HITABLE_HPP
