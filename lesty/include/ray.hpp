#ifndef LESTY_RAY_HPP
#define LESTY_RAY_HPP

#include <cassert>

#include <beyond/core/math/vector.hpp>

namespace lesty {

/**
 * @brief Represent a function of ray F(t)=A+Bt where x and y are two vectors
 * that represent origin and direction
 *
 * @warning Ray do not garentee that its direction is a unit vector
 */
struct Ray {
  beyond::Point3 origin = {0, 0, 0};
  beyond::Vec3 direction = {1, 0, 0};

  /**
   * @brief Default construct a ray with origin at <0,0,0> and facing 0
   * direciton
   */
  constexpr Ray() = default;

  /**
   * @brief Construct a ray by its origin and direction
   * @related Ray
   */
  constexpr Ray(beyond::Point3 a, beyond::Vec3 b)
      : origin{std::move(a)}, direction{std::move(b)}
  {
  }

  /**
   * @brief Gets the result point after we put the parameter t into the ray
   * function
   *
   * @pre t >= 0
   */
  constexpr beyond::Point3 point_at_parameter(float t) const noexcept
  {
    assert(t >= 0);
    return origin + t * direction;
  }
};

} // namespace lesty

#endif // LESTY_RAY_HPP
