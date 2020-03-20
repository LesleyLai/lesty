#pragma once

#ifndef LESTY_RENDERER_HPP
#define LESTY_RENDERER_HPP

#include <functional>
#include <memory>

#include "camera.hpp"
#include "image.hpp"

namespace lesty {

struct Options {
  std::size_t spp;
  std::size_t width;
  std::size_t height;
  std::string input_filename;
  std::string output_filename;
};

class Scene;

class Renderer {
  std::function<void(double progress)> set_progress_;

public:
  enum class Type { path };

  virtual ~Renderer() = default;

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

private:
  [[nodiscard]] virtual auto render_impl(const Scene& scene) -> Image = 0;
};

[[nodiscard]] auto create_renderers(Renderer::Type type, const Options& options)
    -> std::unique_ptr<Renderer>;

} // namespace lesty

#endif // LESTY_RENDERER_HPP
