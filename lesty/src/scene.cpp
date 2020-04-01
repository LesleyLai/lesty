#include <limits>

#include "scene.hpp"

namespace lesty {

/**
 * @param r The ray to check intersection
 * @return std::nullopt if the ray do not intersect with the sphere,
 * a HitRecord struct if the ray intersects the sphere
 */

auto Scene::intersect_at(const beyond::Ray& r) const
    -> beyond::optional<HitRecord>
{
  assert(aggregate_ != nullptr);
  return aggregate_->intersection_with(r, 0.001f,
                                       std::numeric_limits<float>::infinity());
}

} // namespace lesty