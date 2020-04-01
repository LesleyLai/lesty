#ifndef LESTY_IMAGE_HPP
#define LESTY_IMAGE_HPP

#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <beyond/utils/panic.hpp>

#include <fmt/format.h>

#include "color.hpp"

namespace lesty {

class Image {
public:
  Image(size_t width, size_t height);

  /**
   * @brief Save the image into a file
   * @param filename with extension
   *
   * Currently only support ppm as extension.
   */
  void saveto(const std::string& filename) const;

  [[nodiscard]] auto width() const -> size_t
  {
    return width_;
  }

  [[nodiscard]] auto height() const -> size_t
  {
    return height_;
  }

  [[nodiscard]] auto color_at(size_t x, size_t y) const -> Color
  {
    bound_checking(x, y);
    return data_[y * width_ + x];
  }

  auto color_at(size_t x, size_t y) -> Color&
  {
    bound_checking(x, y);
    return data_[y * width_ + x];
  }

private:
  void bound_checking(size_t x, size_t y) const
  {
    if (x >= width_ || y >= height_) {
      beyond::panic(fmt::format("Access image out of range\nat x: {}, y: {}\n "
                                "width: {}, height: {}\n",
                                x, y, width_, height_));
    }
  }

  size_t width_;
  size_t height_;
  std::vector<Color> data_;
};

} // namespace lesty

#endif // LESTY_IMAGE_HPP
