#ifndef LESTY_PATH_TRACING_RENDERER_HPP
#define LESTY_PATH_TRACING_RENDERER_HPP

#include <cstddef>
#include <functional>
#include <random>

#include "camera.hpp"
#include "image.hpp"
#include "renderer.hpp"

namespace lesty {

class Scene;
struct Color;

class PathTracingRenderer : public Renderer {
public:
  PathTracingRenderer(size_t width, size_t height, size_t sample_per_pixel,
                      Camera camera)
      : Renderer(width, height, sample_per_pixel, camera)
  {
  }

private:
  auto render_to_tile_impl(Tile& tile, const Scene& scene, std::uint32_t spp)
      -> void override;
};

} // namespace lesty

#endif // LESTY_PATH_TRACING_RENDERER_HPP
