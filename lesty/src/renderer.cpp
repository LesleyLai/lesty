

#include "renderer.hpp"
#include "renderers/path_tracing_renderer.hpp"
#include "tile.hpp"

#include "scene.hpp"

#include <beyond/core/utils/assert.hpp>
#include <future>
#include <random>

namespace lesty {

[[nodiscard]] auto trace(const Scene& scene, const Ray& ray,
                         size_t depth = 0) noexcept -> Color;

auto Renderer::render(const Scene& scene) -> Image
{
  const auto width = width_;
  const auto height = height_;

  std::vector<std::future<Tile>> results;

  std::atomic<std::size_t> progress_tick = 0;
  const std::size_t tile_count = height_ * width_ / tile_size / tile_size + 1;
  auto tick_progress = [this, &progress_tick, tile_count]() {
    ++progress_tick;
    set_progress(static_cast<size_t>(static_cast<double>(progress_tick.load()) /
                                     static_cast<double>(tile_count) * 100.));
  };

  for (size_t y = 0; y < height_; y += tile_size) {
    for (size_t x = 0; x < width_; x += tile_size) {
      const size_t end_x = std::min(x + tile_size, width);
      const size_t end_y = std::min(y + tile_size, height);
      assert(x < end_x && y < end_y);
      TileDesc tile_desc{x, y, end_x - x, end_y - y};

      results.push_back(std::async(std::launch::async,
                                   [this, tile_desc, &scene, &tick_progress] {
                                     auto tile = render_tile(tile_desc, scene);
                                     tick_progress();
                                     return tile;
                                   }));
    }
  }

  Image image(width, height);
  for (auto& result : results) {
    result.wait();
    const auto tile = result.get();

    for (size_t j = 0; j < tile.height(); ++j) {
      for (size_t i = 0; i < tile.width(); ++i) {
        image.color_at(tile.start_x() + i, tile.start_y() + j) = tile.at(i, j);
      }
    }
  }
  return image;
} // namespace lesty

auto create_renderers(Renderer::Type type, const Options& options)
    -> std::unique_ptr<Renderer>
{
  using namespace beyond::literals;

  const auto aspect_ratio =
      static_cast<float>(options.width) / static_cast<float>(options.height);

  switch (type) {
  case Renderer::Type::path:
    return std::make_unique<PathTracingRenderer>(options.width, options.height,
                                                 options.spp,
                                                 Camera{{278, 278, -800},
                                                        {278, 278, 0},
                                                        {0, 1, 0},
                                                        40.0_deg,
                                                        aspect_ratio});
  default:
    BEYOND_UNREACHABLE();
  }
}

} // namespace lesty