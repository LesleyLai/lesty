#pragma once

#ifndef LESTY_RENDERER_HPP
#define LESTY_RENDERER_HPP

#include <functional>
#include <memory>

#include "camera.hpp"
#include "image.hpp"
#include "tile.hpp"

namespace lesty {

struct Options {
  std::size_t spp;
  std::size_t width;
  std::size_t height;
  std::string input_filename;
  std::string output_filename;
};

class Scene;

// A tile is a 32X32 pixel area of an image
constexpr size_t tile_size = 32;

class Renderer {
  size_t width_ = 0;
  size_t height_ = 0;
  size_t sample_per_pixel_ = 0;
  Camera camera_;

  std::function<void(double progress)> set_progress_;

public:
  enum class Type { path };

  Renderer(size_t width, size_t height, size_t sample_per_pixel, Camera camera)
      : width_{width}, height_{height},
        sample_per_pixel_{sample_per_pixel}, camera_{std::move(camera)}
  {
  }

  virtual ~Renderer() = default;
  Renderer(const Renderer&) = delete;
  auto operator=(const Renderer&) & -> Renderer& = delete;
  Renderer(Renderer&&) = delete;
  auto operator=(Renderer&&) & -> Renderer& = delete;

  /// @brief Render the scene to an image
  [[nodiscard]] auto render(const Scene& scene) -> Image;

  /**
   * @brief Sets a callback function that gets invoked when the integrator make
   * progress
   *
   * The argument for that function is the percentage of the total progression.
   *
   * @warning The function func must be thread safe
   */
  template <class Func> auto set_progress_callback(Func&& set_progress) -> void
  {
    set_progress_ = std::forward<Func>(set_progress);
  }

  auto set_progress(double progress) -> void
  {
    if (set_progress_) {
      set_progress_(progress);
    }
  }

  [[nodiscard]] auto width() const -> size_t
  {
    return width_;
  }
  [[nodiscard]] auto height() const -> size_t
  {
    return height_;
  }
  [[nodiscard]] auto sample_per_pixel() const -> size_t
  {
    return sample_per_pixel_;
  }
  [[nodiscard]] auto camera() const -> const Camera&
  {
    return camera_;
  }

private:
  virtual auto render_tile(const TileDesc& tile_desc, const Scene& scene)
      -> Tile = 0;
};

[[nodiscard]] auto create_renderers(Renderer::Type type, const Options& options)
    -> std::unique_ptr<Renderer>;

} // namespace lesty

#endif // LESTY_RENDERER_HPP
