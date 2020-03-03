#ifndef LESTY_PATHTRACER_HPP
#define LESTY_PATHTRACER_HPP

#include <cstddef>
#include <random>

#include <indicators/progress_bar.hpp>

namespace lesty {

class Camera;
class Scene;
class Image;
struct Ray;
struct Color;

class Path_tracer {

public:
  Path_tracer();

  void run(const Scene& scene, const Camera& camera, Image& image,
           size_t sample_per_pixel);

private:
  indicators::ProgressBar progress_bar_{};
};

} // namespace lesty

#endif // LESTY_PATHTRACER_HPP
