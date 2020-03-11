#include "bounding_volume_hierarchy.hpp"

#include <algorithm>
#include <cassert>
#include <limits>
#include <random>

// A dummy object that you cannot hit
namespace {
struct Never_hitable : lesty::Hitable {
  auto bounding_box() const -> lesty::AABB override
  {
    constexpr auto min = std::numeric_limits<float>::min();
    constexpr auto max = std::numeric_limits<float>::max();

    // Returns a bounding box that do not bound to anything
    return lesty::AABB{
        {max, max, max}, {min, min, min}, lesty::AABB::unchecked_tag};
  }

  auto intersection_with(const lesty::Ray& /*r*/, float /*t_min*/,
                         float /*t_max*/) const
      -> std::optional<lesty::HitRecord> override
  {
    return {};
  }
};
} // anonymous namespace

namespace lesty {

BVH_node::BVH_node(const ObjectIterator& begin,
                   const ObjectIterator& end) noexcept
{
  thread_local std::mt19937 gen = std::mt19937{std::random_device{}()};
  std::uniform_int_distribution<int> dis(0, 2);
  int axis = dis(gen);
  if (axis == 0) {
    // Sort by x
    std::sort(begin, end,
              [](const std::unique_ptr<Hitable>& lhs,
                 const std::unique_ptr<Hitable>& rhs) {
                return lhs->bounding_box().min().x <
                       rhs->bounding_box().min().x;
              });
  } else if (axis == 1) {
    // Sort by y
    std::sort(begin, end,
              [](const std::unique_ptr<Hitable>& lhs,
                 const std::unique_ptr<Hitable>& rhs) {
                return lhs->bounding_box().min().y <
                       rhs->bounding_box().min().y;
              });
  } else {
    // Sort by z
    std::sort(begin, end,
              [](const std::unique_ptr<Hitable>& lhs,
                 const std::unique_ptr<Hitable>& rhs) {
                return lhs->bounding_box().min().z <
                       rhs->bounding_box().min().z;
              });
  }

  const auto size = end - begin;
  assert(size > 0);

  switch (size) {
  case 1:
    left_ = std::move(*begin);
    right_ = std::make_unique<Never_hitable>();
    break;
  case 2:
    left_ = std::move(*begin);
    right_ = std::move(*(begin + 1));
    break;
  default:
    left_ = std::make_unique<BVH_node>(begin, begin + size / 2);
    right_ = std::make_unique<BVH_node>(begin + size / 2, end);
  }

  box_ = aabb_union(left_->bounding_box(), right_->bounding_box());
}

auto BVH_node::intersection_with(const Ray& r, float t_min, float t_max) const
    noexcept -> std::optional<HitRecord>
{
  assert(left_ != nullptr && right_ != nullptr);

  if (!box_.hit(r, t_min, t_max)) {
    return {};
  }

  const auto hit_left_record = left_->intersection_with(r, t_min, t_max);
  const auto hit_right_record = right_->intersection_with(r, t_min, t_max);
  if (hit_left_record && hit_right_record) {
    if (hit_left_record->t < hit_right_record->t) {
      return hit_left_record;
    } else {
      return hit_right_record;
    }
  } else if (hit_left_record) {
    return hit_left_record;
  } else if (hit_right_record) {
    return hit_right_record;
  } else {
    return {};
  }
}

} // namespace lesty