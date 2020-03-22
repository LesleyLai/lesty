#include <cmath>
#include <fstream>

#include <ctre.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "image.hpp"

#include <beyond/core/utils/panic.hpp>

namespace {

float to_gamma(float color)
{
  return std::pow(color, 1 / 2.2f);
}

using byte = unsigned char;
byte float_color_to_255(float color)
{
  return static_cast<byte>(255.99f * to_gamma(color));
}

} // namespace

namespace lesty {

Image::Image(size_t width, size_t height)
    : width_(width), height_(height), data_(width * height)
{
}

void Image::saveto(const std::string& filename) const
{
  static constexpr auto png_pattern = ctll::fixed_string{R"(.*\.png$)"};
  if (ctre::match<png_pattern>(filename)) {
    std::puts("Output to png file");
  } else {
    beyond::panic("Unsupported image extension");
  }

  std::vector<byte> buffer;
  buffer.reserve(data_.size() * 3);
  for (auto i = data_.crbegin(), end = data_.crend(); i != end; ++i) {
    byte red = float_color_to_255(std::sqrt(i->r));
    byte green = float_color_to_255(std::sqrt(i->g));
    byte blue = float_color_to_255(std::sqrt(i->b));
    // auto color: data_
    buffer.push_back(red);
    buffer.push_back(green);
    buffer.push_back(blue);
  }
  stbi_write_png(
      filename.c_str(), static_cast<int>(width_), static_cast<int>(height_), 3,
      reinterpret_cast<void*>(buffer.data()), static_cast<int>(width_ * 3));
}

} // namespace lesty