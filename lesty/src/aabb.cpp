#include <ostream>

#include <fmt/format.h>

#include "aabb.hpp"

namespace lesty {

[[nodiscard]] auto to_string(const AABB& box) -> std::string
{
  return fmt::format("AABB(min: {}, max: {})", to_string(box.min()),
                     to_string(box.max()));
}

auto operator<<(std::ostream& os, const AABB& box) -> std::ostream&
{
  os << to_string(box);
  return os;
}

} // namespace lesty