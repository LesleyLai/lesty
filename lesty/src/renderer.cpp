#include <beyond/core/utils/assert.hpp>

#include "renderer.hpp"

#include "renderers/path_tracing_renderer.hpp"

namespace lesty {

auto Renderer::render(const Scene& scene) -> Image
{
  return render_impl(scene);
}

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