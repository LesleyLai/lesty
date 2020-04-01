#ifndef LESTY_AXIS_ALIGNED_RECT_HPP
#define LESTY_AXIS_ALIGNED_RECT_HPP

#include <beyond/math/vector.hpp>

#include "hitable.hpp"
#include "material.hpp"

namespace lesty {

enum class NormalDirection { Positive, Negetive };

struct Rect_XY : Hitable {
  beyond::Point2 min;
  beyond::Point2 max;
  float z;
  NormalDirection direction;

  Rect_XY(beyond::Point2 in_min, beyond::Point2 in_max, float in_z,
          const Material& mat, NormalDirection dir = NormalDirection::Positive)
      : min{in_min}, max{in_max}, z{in_z}, direction{dir}, material{&mat}
  {
  }

  [[nodiscard]] auto bounding_box() const -> beyond::AABB3 override
  {
    return beyond::AABB3{{min, z - 0.0001f}, {max, z + 0.0001f}};
  }

  [[nodiscard]] auto intersection_with(const beyond::Ray& r, float t_min,
                                       float t_max) const
      -> beyond::optional<HitRecord> override;

  const Material* const material;
};

struct Rect_XZ : Hitable {
  beyond::Point2 min;
  beyond::Point2 max;
  float y;
  NormalDirection direction;
  const Material* const material;

  Rect_XZ(beyond::Point2 in_min, beyond::Point2 in_max, float in_y,
          const Material& mat, NormalDirection dir = NormalDirection::Positive)
      : min{in_min}, max{in_max}, y{in_y}, direction{dir}, material{&mat}
  {
  }

  [[nodiscard]] auto bounding_box() const -> beyond::AABB3 override
  {
    return beyond::AABB3{{min.x, y - 0.0001f, min.y},
                         {max.x, y + 0.0001f, max.y}};
  }

  [[nodiscard]] auto intersection_with(const beyond::Ray& r, float t_min,
                                       float t_max) const
      -> beyond::optional<HitRecord> override;
};

struct Rect_YZ : Hitable {
  beyond::Point2 min;
  beyond::Point2 max;
  float x;
  NormalDirection direction;
  const Material* const material;

  Rect_YZ(beyond::Point2 in_min, beyond::Point2 in_max, float in_x,
          const Material& mat, NormalDirection dir = NormalDirection::Positive)
      : min{in_min}, max{in_max}, x{in_x}, direction{dir}, material{&mat}
  {
  }

  [[nodiscard]] auto bounding_box() const -> beyond::AABB3 override
  {
    return beyond::AABB3{{x - 0.0001f, min.x, min.y},
                         {x + 0.0001f, max.x, max.y}};
  }

  [[nodiscard]] auto intersection_with(const beyond::Ray& r, float t_min,
                                       float t_max) const
      -> beyond::optional<HitRecord> override;
};

} // namespace lesty

#endif // LESTY_AXIS_ALIGNED_RECT_HPP
