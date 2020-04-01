#ifndef LESTY_CAMERA_HPP
#define LESTY_CAMERA_HPP

#include "beyond/geometry/ray.hpp"
#include <beyond/math/angle.hpp>
#include <beyond/math/vector.hpp>

namespace lesty {

/**
 * @brief The Camera_sample struct hold all the sample value for a camera to
 * generate a ray
 */
struct Camera_sample {
  beyond::Point2 film_pos;
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
  Camera(const beyond::Point3& position, const beyond::Point3& lookat,
         const beyond::Vec3& up, const beyond::Radian& fov,
         float aspect) noexcept
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
  [[nodiscard]] auto get_ray(Camera_sample sample) const noexcept -> beyond::Ray
  {
    const float u = sample.film_pos.x;
    const float v = sample.film_pos.y;
    return beyond::Ray{origin_, lower_left_corner_ + u * horizontal_ +
                                    v * vertical_ - origin_};
  }

private:
  beyond::Point3 origin_{};
  beyond::Point3 lower_left_corner_{};
  beyond::Vec3 horizontal_{};
  beyond::Vec3 vertical_{};
};

} // namespace lesty

#endif // LESTY_CAMERA_HPP
