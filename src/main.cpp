#include <fmt/format.h>

#include "dummy.hpp"

auto main() -> int
{
  fmt::print("fib(10) = {}\n", fib(10));
}
