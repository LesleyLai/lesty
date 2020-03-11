#ifndef LESTY_AXIS_ALIGNED_RECT_HPP
#define LESTY_AXIS_ALIGNED_RECT_HPP

#include <beyond/core/math/vector.hpp>

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

  [[nodiscard]] AABB bounding_box() const override
  {
    return AABB{{min, z - 0.0001f}, {max, z + 0.0001f}};
  }

  [[nodiscard]] Maybe_hit_t intersection_with(const Ray& r, float t_min,
                                              float t_max) const override;

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

  [[nodiscard]] AABB bounding_box() const override
  {
    return AABB{{min.x, y - 0.0001f, min.y}, {max.x, y + 0.0001f, max.y}};
  }

  [[nodiscard]] Maybe_hit_t intersection_with(const Ray& r, float t_min,
                                              float t_max) const override;
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

  [[nodiscard]] AABB bounding_box() const override
  {
    return AABB{{x - 0.0001f, min.x, min.y}, {x + 0.0001f, max.x, max.y}};
  }

  [[nodiscard]] Maybe_hit_t intersection_with(const Ray& r, float t_min,
                                              float t_max) const override;
};

} // namespace lesty

#endif // LESTY_AXIS_ALIGNED_RECT_HPP
