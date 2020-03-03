#ifndef LESTY_BOUNDING_VOLUME_HIERARCHY_HPP
#define LESTY_BOUNDING_VOLUME_HIERARCHY_HPP

#include <memory>
#include <vector>

#include "aabb.hpp"
#include "hitable.hpp"

namespace lesty {

using ObjectIterator = std::vector<std::unique_ptr<Hitable>>::iterator;

class BVH_node : public Hitable {
public:
  BVH_node(const ObjectIterator& begin, const ObjectIterator& end) noexcept;

  [[nodiscard]] auto bounding_box() const noexcept -> AABB override
  {
    return box_;
  }

  [[nodiscard]] auto intersect_at(const Ray& r, float t_min, float t_max) const
      noexcept -> std::optional<HitRecord> override;

private:
  std::unique_ptr<const Hitable> left_ = nullptr;
  std::unique_ptr<const Hitable> right_ = nullptr;
  AABB box_;
};

} // namespace lesty

#endif // LESTY_BOUNDING_VOLUME_HIERARCHY_HPP
