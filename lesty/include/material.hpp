#ifndef LESTY_MATERIAL_HPP
#define LESTY_MATERIAL_HPP

#include <optional>
#include <variant>

#include <beyond/math/angle.hpp>

#include "beyond/geometry/ray.hpp"
#include "color.hpp"
#include "hitable.hpp"

namespace lesty {

class Material {
public:
  Material() noexcept = default;
  explicit Material(Color albedo) noexcept : albedo_{albedo} {}
  virtual ~Material() = default;

  /**
   * @brief scatter
   * @param ray_in Incident ray
   * @param record
   * @return scattered ray if the incident ray is not absorbed
   */
  [[nodiscard]] virtual auto scatter(const beyond::Ray& ray_in,
                                     const HitRecord& record) const
      -> std::optional<beyond::Ray> = 0;

  [[nodiscard]] virtual auto emitted() const -> Color
  {
    return Color{};
  }

  [[nodiscard]] constexpr auto albedo() const noexcept -> Color
  {
    return albedo_;
  }

private:
  Color albedo_{0.5f, 0.5f, 0.5f};
};

class Lambertian : public Material {
public:
  explicit Lambertian(Color albedo) noexcept : Material{albedo} {}

  [[nodiscard]] auto scatter(const beyond::Ray& ray_in,
                             const HitRecord& record) const
      -> std::optional<beyond::Ray> override;
};

class Metal : public Material {
public:
  Metal(Color albedo, float fuzzness) noexcept
      : Material{albedo}, fuzzness_{fuzzness}
  {
  }

  [[nodiscard]] auto scatter(const beyond::Ray& ray_in,
                             const HitRecord& record) const
      -> std::optional<beyond::Ray> override;

private:
  float fuzzness_;
};

class Dielectric : public Material {
public:
  Dielectric(Color albedo, float refractive_index) noexcept
      : Material{albedo}, refractive_index_{refractive_index}
  {
  }

  [[nodiscard]] auto scatter(const beyond::Ray& ray_in,
                             const HitRecord& record) const
      -> std::optional<beyond::Ray> override;

private:
  float refractive_index_;
};

class Emission : public Material {
public:
  explicit Emission(Color emit) noexcept : emit_(emit) {}

  [[nodiscard]] auto scatter(const beyond::Ray& ray_in,
                             const HitRecord& record) const
      -> std::optional<beyond::Ray> override;
  [[nodiscard]] auto emitted() const -> Color override;

private:
  Color emit_;
};

} // namespace lesty

#endif // LESTY_MATERIAL_HPP
