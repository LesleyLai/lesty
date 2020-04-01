#include <catch2/catch.hpp>

#include "image.hpp"

TEST_CASE("Image", "[Graphics]")
{
  using lesty::Color;
  using lesty::Image;

  Image img(200, 100);

  SECTION("Modify color at centain point")
  {
    Color red = {1, 0, 0};
    img.color_at(100, 50) = red;
    const Color& result = img.color_at(100, 50);

    REQUIRE(result.r == Approx(1));
    REQUIRE(result.g == Approx(0));
    REQUIRE(result.b == Approx(0));
  }
}
