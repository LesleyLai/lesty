#include <optional>
#include <random>

#include <beyond/math/vector.hpp>
#include <beyond/random/generators/xorshift32.hpp>

#include "material.hpp"

namespace {
constexpr auto reflect(beyond::Vec3 v, beyond::Vec3 n) noexcept -> beyond::Vec3
{
  return v - 2 * dot(v, n) * n;
}

// Refraction by snell's law
auto refract(beyond::Vec3 v, beyond::Vec3 n, float ni_over_nt) noexcept
    -> std::optional<beyond::Vec3>
{
  const auto uv = v / v.length();
  float dt = dot(uv, n);
  float discriminant = 1 - ni_over_nt * ni_over_nt * (1 - dt * dt);
  if (discriminant > 0) {
    return ni_over_nt * (uv - n * dt) - n * std::sqrt(discriminant);
  }
  return std::nullopt;
}

auto random_in_unit_sphere() -> beyond::Vec3
{
  // Credit:
  // https://math.stackexchange.com/questions/87230/picking-random-points-in-the-volume-of-sphere-with-uniform-probability/87238#87238
  thread_local beyond::xorshift32 gen;
  thread_local std::uniform_real_distribution<float> uni(-1, 1);

  thread_local std::normal_distribution<float> normal(0, 1);

  beyond::Vec3 p{normal(gen), normal(gen), normal(gen)};
  p = normalize(p);

  const auto c = std::cbrt(uni(gen));
  return p * c;
}

// Reflectivity by Christophe Schlick
auto schlick(float cosine, float ref_idx) -> float
{
  float r0 = (1 - ref_idx) / (1 + ref_idx);
  r0 *= r0;
  return r0 + (1 - r0) * std::pow(1 - cosine, 5.f);
}

} // namespace

namespace lesty {

auto Lambertian::scatter(const beyond::Ray& /*ray_in*/,
                         const HitRecord& record) const
    -> std::optional<beyond::Ray>
{
  const auto target = record.point + record.normal + random_in_unit_sphere();
  return beyond::Ray{record.point, target - record.point};
}

auto Metal::scatter(const beyond::Ray& ray_in, const HitRecord& record) const
    -> std::optional<beyond::Ray>
{
  auto incident_dir = ray_in.direction / ray_in.direction.length();
  auto reflected = reflect(incident_dir, record.normal) +
                   fuzzness_ * random_in_unit_sphere();
  if (dot(reflected, record.normal) <= 0) {
    return std::nullopt;
  }

  return beyond::Ray{record.point, reflected};
}

auto Dielectric::scatter(const beyond::Ray& ray_in,
                         const HitRecord& record) const
    -> std::optional<beyond::Ray>
{
  beyond::Vec3 out_normal;
  float ni_over_nt;
  float cosine;
  if (dot(ray_in.direction, record.normal) > 0) {
    out_normal = -record.normal;
    ni_over_nt = refractive_index_;
    cosine = refractive_index_ * dot(ray_in.direction, record.normal) /
             ray_in.direction.length();
  } else {
    out_normal = record.normal;
    ni_over_nt = 1 / refractive_index_;
    cosine = -dot(ray_in.direction, record.normal) / ray_in.direction.length();
  }

  float reflection_prob = 1;

  auto refraction = refract(ray_in.direction, out_normal, ni_over_nt);
  if (refraction) {
    reflection_prob = schlick(cosine, refractive_index_);
  }

  std::uniform_real_distribution<float> dis(0, 1);
  thread_local beyond::xorshift32 gen;

  if (dis(gen) < reflection_prob) {
    auto incident_dir = ray_in.direction / ray_in.direction.length();
    auto reflection = reflect(incident_dir, record.normal);
    return beyond::Ray(record.point, reflection);
  }
  return beyond::Ray(record.point, *refraction);
}

auto Emission::scatter(const beyond::Ray& /*ray_in*/,
                       const HitRecord& /*record*/) const
    -> std::optional<beyond::Ray>
{
  return {};
}

auto Emission::emitted() const -> Color
{
  return emit_;
}

} // namespace lesty