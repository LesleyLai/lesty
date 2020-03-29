#ifndef LESTY_AABB_HPP
#define LESTY_AABB_HPP

#include <algorithm>
#include <iosfwd>

#include <beyond/math/vector.hpp>

#include <beyond/math/serial.hpp>

#include "ray.hpp"

namespace lesty {

/**
 * @brief 3D Axis aligned bounding boxes are used for bounding volume
 * hierarchies
 */
class AABB {
public:
  struct unchecked_tag_t {
  };
  static constexpr unchecked_tag_t unchecked_tag{};

  constexpr AABB() noexcept = default;

  /**
   * @brief initialize an AABB to enclose a single point:
   */
  explicit constexpr AABB(beyond::Point3 p) noexcept : min_{p}, max_{p} {}

  /**
   * @brief Construction an AABB from two points
   */
  constexpr AABB(beyond::Point3 p1, beyond::Point3 p2) noexcept
      : min_{std::min(p1.x, p2.x), std::min(p1.y, p2.y), std::min(p1.z, p2.z)},
        max_{std::max(p1.x, p2.x), std::max(p1.y, p2.y), std::max(p1.z, p2.z)}
  {
  }

  /**
   * @brief Construction an AABB from its minimal corner to maximum corner
   * @warning This function does not check whether the two points are actually
   * minimal or maximum corner
   */
  constexpr AABB(beyond::Point3 min, beyond::Point3 max,
                 unchecked_tag_t) noexcept
      : min_{min}, max_{max}
  {
  }

  [[nodiscard]] constexpr auto min() const noexcept -> beyond::Point3
  {
    return min_;
  }

  [[nodiscard]] constexpr auto max() const noexcept -> beyond::Point3
  {
    return max_;
  }

  /**
   * @brief Whether the ray r hit AABB or not
   */
  [[nodiscard]] constexpr auto is_intersect_with(const Ray& r, float t_min,
                                                 float t_max) const -> bool
  {
    constexpr std::size_t num_dim = 3;
    // Credit: Andrew Kensler at Pixar adapt this version of AABB hit method
    // Shirley, Peter. Ray Tracing: the Next Week
    for (std::size_t a = 0; a < num_dim; ++a) {
      const float invD = 1.f / r.direction[a];
      float t0 = (min_[a] - r.origin[a]) * invD;
      float t1 = (max_[a] - r.origin[a]) * invD;
      if (invD < 0) {
        std::swap(t0, t1);
      }
      t_min = std::max(t0, t_min);
      t_max = std::min(t1, t_max);

      if (t_max <= t_min)
        return false;
    }
    return true;
  }

  [[nodiscard]] friend constexpr auto operator==(const AABB& lhs,
                                                 const AABB& rhs) -> bool
  {
    return lhs.min() == rhs.min() && lhs.max() == rhs.max();
  }

  [[nodiscard]] friend constexpr auto operator!=(const AABB& lhs,
                                                 const AABB& rhs) -> bool
  {
    return !(lhs == rhs);
  }

  /**
   * @brief Computes the bounding box for two AABBs
   */
  friend constexpr auto aabb_union(const AABB& box0, const AABB& box1) -> AABB
  {
    return AABB{{std::min(box0.min().x, box1.min().x),
                 std::min(box0.min().y, box1.min().y),
                 std::min(box0.min().z, box1.min().z)},
                {std::max(box0.max().x, box1.max().x),
                 std::max(box0.max().y, box1.max().y),
                 std::max(box0.max().z, box1.max().z)}};
  }

private:
  beyond::Point3 min_ = {};
  beyond::Point3 max_ = {};
};

[[nodiscard]] auto to_string(const AABB& box) -> std::string;
auto operator<<(std::ostream& os, const AABB& box) -> std::ostream&;

} // namespace lesty

#endif // LESTY_AABB_HPP
