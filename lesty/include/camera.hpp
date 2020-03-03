#ifndef LESTY_CAMERA_HPP
#define LESTY_CAMERA_HPP

#include "ray.hpp"
#include <beyond/core/math/angle.hpp>
#include <beyond/core/math/vector.hpp>

namespace lesty {

/**
 * @brief The Camera_sample struct hold all the sample value for a camera to
 * generate a ray
 */
struct Camera_sample {
  beyond::Point2f film_pos;
};

class Camera {
public:
  /**
   * @brief Constructor of a camera
   * @param position The position of the camera
   * @param lookat
   * @param up Direction of up
   * @param fov Field of view of the camera
   * @param aspect Aspect ratio of the screen
   */
  Camera(beyond::Point3f position, beyond::Point3f lookat, beyond::Vector3f up,
         beyond::Radianf fov, float aspect) noexcept
  {
    const float half_height = std::tan(fov.value() / 2);
    const float half_width = aspect * half_height;

    origin_ = position;
    const auto w = normalize(position - lookat);
    const auto u = normalize(cross(up, w));
    const auto v = cross(w, u);

    lower_left_corner_ = origin_ - half_width * u - half_height * v - w;
    horizontal_ = 2 * half_width * u;
    vertical_ = 2 * half_height * v;
  }

  /**
   * @brief Generate a ray by uv coodinate
   */
  [[nodiscard]] auto get_ray(Camera_sample sample) const noexcept -> Ray
  {
    const float u = sample.film_pos.x;
    const float v = sample.film_pos.y;
    return Ray{origin_,
               lower_left_corner_ + u * horizontal_ + v * vertical_ - origin_};
  }

private:
  beyond::Point3f origin_{};
  beyond::Point3f lower_left_corner_{};
  beyond::Vector3f horizontal_{};
  beyond::Vector3f vertical_{};
};

} // namespace lesty

#endif // LESTY_CAMERA_HPP
