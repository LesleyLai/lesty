#ifndef LESTY_AXIS_ALIGNED_RECT_HPP
#define LESTY_AXIS_ALIGNED_RECT_HPP

#include "hitable.hpp"
#include "material.hpp"

#include <beyond/core/math/vector.hpp>

namespace lesty {

enum class Normal_Direction { Positive, Negetive };

struct Rect_XY : Hitable {
  beyond::Point2f min;
  beyond::Point2f max;
  float z;
  Normal_Direction direction;

  Rect_XY(beyond::Point2f in_min, beyond::Point2f in_max, float in_z,
          const Material& mat,
          Normal_Direction dir = Normal_Direction::Positive)
      : min{in_min}, max{in_max}, z{in_z}, direction{dir}, material{&mat}
  {
  }

  std::optional<AABB> bounding_box() const noexcept override
  {
    return AABB{{min, z - 0.0001f}, {max, z + 0.0001f}};
  }

  Maybe_hit_t intersect_at(const Ray& r, float t_min, float t_max) const
      noexcept override;

  const Material* const material;
};

struct Rect_XZ : Hitable {
  beyond::Point2f min;
  beyond::Point2f max;
  float y;
  Normal_Direction direction;
  const Material* const material;

  Rect_XZ(beyond::Point2f in_min, beyond::Point2f in_max, float in_y,
          const Material& mat,
          Normal_Direction dir = Normal_Direction::Positive)
      : min{in_min}, max{in_max}, y{in_y}, direction{dir}, material{&mat}
  {
  }

  std::optional<AABB> bounding_box() const noexcept override
  {
    return AABB{{min.x, y - 0.0001f, min.y}, {max.x, y + 0.0001f, max.y}};
  }

  Maybe_hit_t intersect_at(const Ray& r, float t_min, float t_max) const
      noexcept override;
};

struct Rect_YZ : Hitable {
  beyond::Point2f min;
  beyond::Point2f max;
  float x;
  Normal_Direction direction;
  const Material* const material;

  Rect_YZ(beyond::Point2f in_min, beyond::Point2f in_max, float in_x,
          const Material& mat,
          Normal_Direction dir = Normal_Direction::Positive)
      : min{in_min}, max{in_max}, x{in_x}, direction{dir}, material{&mat}
  {
  }

  std::optional<AABB> bounding_box() const noexcept override
  {
    return AABB{{x - 0.0001f, min.x, min.y}, {x + 0.0001f, max.x, max.y}};
  }

  Maybe_hit_t intersect_at(const Ray& r, float t_min, float t_max) const
      noexcept override;
};

} // namespace lesty

#endif // LESTY_AXIS_ALIGNED_RECT_HPP