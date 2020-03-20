#ifndef LESTY_PATHTRACER_HPP
#define LESTY_PATHTRACER_HPP

#include <cstddef>
#include <functional>
#include <random>

namespace lesty {

class Camera;
class Scene;
class Image;
struct Ray;
struct Color;

class Path_tracer {

public:
  Path_tracer();

  auto run(const Scene& scene, const Camera& camera, Image& image,
           size_t sample_per_pixel) -> void;

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

private:
  std::function<void(double progress)> set_progress_;
};

} // namespace lesty

#endif // LESTY_PATHTRACER_HPP
