#include "pathtracer.hpp"

#include <future>
#include <random>

#include "camera.hpp"
#include "color.hpp"
#include "image.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "scene.hpp"
#include "tile.hpp"

namespace lesty {

Color trace(const Scene& scene, const Ray& ray, size_t depth = 0) noexcept
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

struct PixelData {
  size_t x{};
  size_t y{};
  Color c;
};

constexpr size_t tile_size = 32;
Path_tracer::Path_tracer()
    : progress_bar_{
          indicators::option::BarWidth{50},
          indicators::option::Start{"["},
          indicators::option::Fill{"="},
          indicators::option::Lead{">"},
          indicators::option::Remainder{" "},
          indicators::option::End{"]"},
          indicators::option::PostfixText{"Rendering"},
          indicators::option::ForegroundColor{indicators::Color::green}}
{
}

void Path_tracer::run(const Scene& scene, const Camera& camera, Image& image,
                      size_t sample_per_pixel)
{
  const auto width = image.width(), height = image.height();

  std::vector<std::future<Tile>> results;

  std::atomic<std::size_t> progress_tick = 0;
  const std::size_t tile_count = height * width / tile_size / tile_size + 1;

  for (size_t y = 0; y < height; y += tile_size) {
    for (size_t x = 0; x < width; x += tile_size) {
      results.push_back(
          std::async(std::launch::async, [this, &progress_tick, tile_count, x,
                                          y, sample_per_pixel, width, height,
                                          &scene, &camera] {
            const size_t end_x = std::min(x + tile_size, width);
            const size_t end_y = std::min(y + tile_size, height);
            assert(x < end_x && y < end_y);
            Tile tile{x, y, end_x - x, end_y - y};

            for (size_t j = 0; j < tile.height(); ++j) {
              for (size_t i = 0; i < tile.width(); ++i) {

                Color c;
                thread_local std::mt19937 gen =
                    std::mt19937{std::random_device{}()};
                std::uniform_real_distribution<float> dis(0.0, 1.0);
                for (size_t sample = 0; sample < sample_per_pixel; ++sample) {
                  const auto u = (static_cast<float>(x + i) + dis(gen)) /
                                 static_cast<float>(width);
                  const auto v = (static_cast<float>(y + j) + dis(gen)) /
                                 static_cast<float>(height);

                  const auto r = camera.get_ray(Camera_sample{{u, v}});
                  c += trace(scene, r);
                }
                c /= static_cast<float>(sample_per_pixel);
                tile.at(i, j) = c;
              }
            }

            ++progress_tick;
            progress_bar_.set_progress(
                static_cast<size_t>(static_cast<double>(progress_tick.load()) /
                                    static_cast<double>(tile_count) * 100.));

            return tile;
          }));
    }
  }

  for (auto& result : results) {
    result.wait();
    const auto tile = result.get();

    for (size_t j = 0; j < tile.height(); ++j) {
      for (size_t i = 0; i < tile.width(); ++i) {
        image.color_at(tile.startX() + i, tile.startY() + j) = tile.at(i, j);
      }
    }
  }
}

} // namespace lesty