#ifndef LESTY_TILE_HPP
#define LESTY_TILE_HPP

#include <cassert>
#include <vector>

#include "color.hpp"

namespace lesty {

struct TileDesc {
  size_t start_x = 0;
  size_t start_y = 0;
  size_t width = 0;
  size_t height = 0;
};

struct Tile {
public:
  Tile() = default;

  explicit Tile(const TileDesc& desc)
      : start_x_{desc.start_x}, start_y_{desc.start_y}, width_{desc.width},
        height_{desc.height}
  {
    data_.resize(width_ * height_);
  }

  [[nodiscard]] auto at(size_t i, size_t j) const -> Color
  {
    assert(i < width_);
    assert(j < height_);
    return data_[j * width_ + i];
  }

  [[nodiscard]] auto at(size_t i, size_t j) -> Color&
  {
    assert(i < width_);
    assert(j < height_);
    return data_[j * width_ + i];
  }

  [[nodiscard]] auto height() const -> size_t
  {
    return height_;
  }
  [[nodiscard]] auto width() const -> size_t
  {
    return width_;
  }
  [[nodiscard]] auto start_x() const -> size_t
  {
    return start_x_;
  }
  [[nodiscard]] auto start_y() const -> size_t
  {
    return start_y_;
  }

private:
  size_t start_x_ = 0;
  size_t start_y_ = 0;
  size_t width_ = 0;
  size_t height_ = 0;
  std::vector<Color> data_{};
};

} // namespace lesty

#endif // LESTY_TILE_HPP
