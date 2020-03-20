#ifndef LESTY_SCENE_HPP
#define LESTY_SCENE_HPP

#include <memory>
#include <type_traits>
#include <vector>

#include "camera.hpp"
#include "hitable.hpp"
#include "material.hpp"

namespace lesty {

/**
 * @brief The Scene class represent the scene to be rendered by Path tracer
 */
class Scene {
public:
  /**
   * @brief Constructs a Scene object
   * @param aggregate The combination of all objects in the scene
   * @param materials Ownership of all materials used for the scene
   */
  Scene(std::unique_ptr<Hitable>&& aggregate,
        std::vector<std::unique_ptr<Material>>&& materials) noexcept
      : aggregate_{std::move(aggregate)}, materials_{std::move(materials)}
  {
  }

  /**
   * @brief Returns the hit record at the closet hit point
   */
  [[nodiscard]] auto intersect_at(const Ray& r) const
      -> std::optional<HitRecord>;

private:
  std::unique_ptr<const Hitable> aggregate_ = nullptr;
  std::vector<std::unique_ptr<Material>> materials_;
};

} // namespace lesty

#endif // LESTY_SCENE_HPP
