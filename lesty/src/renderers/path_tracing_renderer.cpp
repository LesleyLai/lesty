#include "path_tracing_renderer.hpp"

#include <future>
#include <random>

#include "camera.hpp"
#include "color.hpp"
#include "image.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "scene.hpp"

namespace lesty {

[[nodiscard]] auto trace(const Scene& scene, const Ray& ray,
                         size_t depth = 0) noexcept -> Color
{
  constexpr size_t max_depth = 100;

  // depth exceed some threshold
  if (depth >= max_depth) {
    return Color{}; // return black
  }

  if (auto hit = scene.intersect_at(ray)) {
    auto material = hit->material;
    auto ref = material->scatter(ray, *hit);
    const auto emitted = material->emitted();
    if (ref) {
      return emitted + material->albedo() * trace(scene, *ref, depth + 1);
    }
    return emitted;
  }

  // Returns black if ray does not hit any object
  return Color{};
}

auto PathTracingRenderer::render_tile(const TileDesc& tile_desc,
                                      const Scene& scene) -> Tile
{
  const auto f_width = static_cast<float>(width());
  const auto f_height = static_cast<float>(height());
  const auto spp = sample_per_pixel();

  Tile tile(tile_desc);

  for (size_t j = 0; j < tile_desc.height; ++j) {
    const auto f_y = static_cast<float>(tile_desc.start_y + j);
    for (size_t i = 0; i < tile_desc.width; ++i) {
      const auto f_x = static_cast<float>(tile_desc.start_x + i);

      Color c;
      thread_local std::mt19937 gen = std::mt19937{std::random_device{}()};
      std::uniform_real_distribution<float> dis(0.0, 1.0);
      for (size_t sample = 0; sample < spp; ++sample) {
        const auto u = (f_x + dis(gen)) / f_width;
        const auto v = (f_y + dis(gen)) / f_height;

        const auto r = camera().get_ray(Camera_sample{{u, v}});
        c += trace(scene, r);
      }
      c /= static_cast<float>(spp);
      tile.at(i, j) = c;
    }
  }
  return tile;
}

} // namespace lesty