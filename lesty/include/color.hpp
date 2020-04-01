#ifndef LESTY_COLOR_HPP
#define LESTY_COLOR_HPP

#include <algorithm>

namespace lesty {

/**
 * \brief 24 bit float RGB color
 */
struct Color {
  /// Red component of the color.
  float r = 0;

  /// Green component of the color.
  float g = 0;

  /// Blue component of the color.
  float b = 0;

  /**
   * \brief Constructs a new black 24 bit RGB Color.
   */
  constexpr Color() noexcept = default;

  /**
   * \brief Constructs a new 24 bit float RGB Color with given r,g,b components.
   * \param red, green, blue
   */
  constexpr Color(float red, float green, float blue) noexcept
      : r{red}, g{green}, b{blue}
  {
  }

  constexpr auto operator*=(float rhs) noexcept -> Color&
  {
    r *= rhs;
    g *= rhs;
    b *= rhs;
    return *this;
  }

  constexpr auto operator/=(float rhs) noexcept -> Color&
  {
    r /= rhs;
    g /= rhs;
    b /= rhs;
    return *this;
  }

  constexpr auto operator*=(const Color& rhs) noexcept -> Color&
  {
    r *= rhs.r;
    g *= rhs.g;
    b *= rhs.b;
    return *this;
  }

  constexpr auto operator+=(const Color& rhs) noexcept -> Color&
  {
    r += rhs.r;
    g += rhs.g;
    b += rhs.b;
    return *this;
  }

  constexpr auto operator-=(const Color& rhs) noexcept -> Color&
  {
    r -= rhs.r;
    g -= rhs.g;
    b -= rhs.b;
    return *this;
  }

  /**
   * @brief Operator << of Color.
   * @tparam CharT  Type of character stream.
   * @tparam Traits  Traits for character type.
   * @param os The output stream
   * @param color The color to output.
   */
  template <class CharT, class Traits>
  inline friend auto operator<<(std::basic_ostream<CharT, Traits>& os,
                                const Color& color)
      -> std::basic_ostream<CharT, Traits>&
  {
    os << "Color(" << color.r << "," << color.g << "," << color.b << ")";
    return os;
  }

  /**
   * @brief Clamps the RGB values of color to [0, 1)
   */
  constexpr void clamp() noexcept
  {
    r = std::clamp(r, 0.0f, 1.0f);
    g = std::clamp(g, 0.0f, 1.0f);
    b = std::clamp(b, 0.0f, 1.0f);
  }
};

[[nodiscard]] constexpr auto operator==(const Color& lhs,
                                        const Color& rhs) noexcept -> bool
{
  return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b;
}

[[nodiscard]] constexpr auto operator+(const Color& lhs,
                                       const Color& rhs) noexcept -> Color
{
  return Color(lhs.r + rhs.r, lhs.g + rhs.g, lhs.b + rhs.b);
}

[[nodiscard]] constexpr auto operator-(const Color& lhs,
                                       const Color& rhs) noexcept -> Color
{
  return Color(lhs.r - rhs.r, lhs.g - rhs.g, lhs.b - rhs.b);
}

[[nodiscard]] constexpr auto operator*(const Color& lhs,
                                       const Color& rhs) noexcept -> Color
{
  return Color(lhs.r * rhs.r, lhs.g * rhs.g, lhs.b * rhs.b);
}

[[nodiscard]] constexpr auto operator*(const Color& c, float scalar) noexcept
    -> Color
{
  return Color(c.r * scalar, c.g * scalar, c.b * scalar);
}

[[nodiscard]] constexpr auto operator/(const Color& c, float scalar) noexcept
    -> Color
{
  return Color(c.r / scalar, c.g / scalar, c.b / scalar);
}

[[nodiscard]] constexpr auto operator*(float scalar, const Color& c) noexcept
    -> Color
{
  return Color(c.r * scalar, c.g * scalar, c.b * scalar);
}

} // namespace lesty

#endif // LESTY_COLOR_HPP
