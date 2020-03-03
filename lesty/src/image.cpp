#include <cmath>
#include <fstream>
#include <regex>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "image.hpp"

#include <cmath>

constexpr float to_gamma(float color)
{
  return std::pow(color, 1 / 2.2f);
}

using byte = unsigned char;
constexpr byte float_color_to_255(float color)
{
  return static_cast<byte>(255.99f * to_gamma(color));
}

namespace lesty {

Image::Image(size_t width, size_t height)
    : width_(width), height_(height), data_(width * height)
{
}

void Image::saveto(const std::string& filename) const
{
  std::regex png{R"(.*\.png$)"};
  if (!std::regex_match(filename, png)) {
    throw Unsupported_image_extension{filename.c_str()};
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
  stbi_write_png(filename.c_str(), width_, height_, 3,
                 reinterpret_cast<void*>(buffer.data()), width_ * 3);
}

} // namespace lesty