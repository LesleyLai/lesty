#include "bounding_volume_hierarchy.hpp"

#include <algorithm>
#include <cassert>
#include <limits>
#include <random>

// A dummy object that you cannot hit
namespace {
struct Never_hitable : lesty::Hitable {
  [[nodiscard]] auto bounding_box() const -> beyond::AABB3 override
  {
    constexpr auto min = std::numeric_limits<float>::min();
    constexpr auto max = std::numeric_limits<float>::max();

    // Returns a bounding box that do not bound to anything
    return beyond::AABB3{
        {max, max, max}, {min, min, min}, beyond::AABB3::unchecked_tag};
  }

  [[nodiscard]] auto intersection_with(const beyond::Ray& /*r*/,
                                       float /*t_min*/, float /*t_max*/) const
      -> beyond::optional<lesty::HitRecord> override
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

  const auto sort_by = [&](auto axis) {
    std::sort(begin, end,
              [axis](const std::unique_ptr<Hitable>& lhs,
                     const std::unique_ptr<Hitable>& rhs) {
                return lhs->bounding_box().min().*axis <
                       rhs->bounding_box().min().*axis;
              });
  };

  std::uniform_int_distribution<int> dis(0, 2);
  int axis = dis(gen);
  switch (axis) {
  case 0:
    sort_by(&beyond::Point3::x);
    break;
  case 1:
    sort_by(&beyond::Point3::y);
    break;
  default:
    sort_by(&beyond::Point3::z);
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

  box_ = merge(left_->bounding_box(), right_->bounding_box());
}

[[nodiscard]] auto BVH_node::intersection_with(const beyond::Ray& r,
                                               float t_min,
                                               float t_max) const noexcept
    -> beyond::optional<HitRecord>
{
  assert(left_ != nullptr && right_ != nullptr);

  if (!box_.is_intersect_with(r, t_min, t_max)) {
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