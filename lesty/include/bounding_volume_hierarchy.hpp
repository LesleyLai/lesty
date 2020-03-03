#ifndef LESTY_BOUNDING_VOLUME_HIERARCHY_HPP
#define LESTY_BOUNDING_VOLUME_HIERARCHY_HPP

#include <memory>
#include <vector>

#include "aabb.hpp"
#include "hitable.hpp"

namespace lesty {

using Object_iterator = std::vector<std::unique_ptr<Hitable>>::iterator;

class BVH_node : public Hitable {
public:
  BVH_node(const Object_iterator& begin, const Object_iterator& end) noexcept;

  std::optional<AABB> bounding_box() const noexcept override
  {
    return box_;
  }

  Maybe_hit_t intersect_at(const Ray& r, float t_min, float t_max) const
      noexcept override;

private:
  std::unique_ptr<const Hitable> left_ = nullptr;
  std::unique_ptr<const Hitable> right_ = nullptr;
  AABB box_;
};

} // namespace lesty

#endif // LESTY_BOUNDING_VOLUME_HIERARCHY_HPP
