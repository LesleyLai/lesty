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
struct Ray;
struct Color;

class PathTracingRenderer : public Renderer {
  size_t width_;
  size_t height_;
  size_t sample_per_pixel_;
  Camera camera_;

public:
  PathTracingRenderer(size_t width, size_t height, size_t sample_per_pixel,
                      Camera camera)
      : width_{width}, height_{height},
        sample_per_pixel_{sample_per_pixel}, camera_{std::move(camera)}
  {
  }

private:
  [[nodiscard]] auto render_impl(const Scene& scene) -> Image;
};

} // namespace lesty

#endif // LESTY_PATH_TRACING_RENDERER_HPP
