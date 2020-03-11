#include "triangle.hpp"

namespace lesty {

[[nodiscard]] auto Triangle::intersect_at(const Ray& r, float t_min,
                                          float t_max) const
    -> std::optional<HitRecord>
{

  const auto vd = r.direction;
  const auto ve = r.origin;
  const auto va = vertices[0];
  const auto vb = vertices[1];
  const auto vc = vertices[2];

  const auto a = va.x - vb.x;
  const auto b = va.y - vb.y;
  const auto c = va.z - vb.z;
  const auto d = va.x - vc.x;
  const auto e = va.y - vc.y;
  const auto f = va.z - vc.z;
  const auto g = vd.x;
  const auto h = vd.y;
  const auto i = vd.z;
  const auto j = va.x - ve.x;
  const auto k = va.y - ve.y;
  const auto l = va.z - ve.z;

  const auto ei_hf = e * i - h * f;
  const auto gf_di = g * f - d * i;
  const auto dh_eg = d * h - e * g;
  const auto ak_jb = a * k - j * b;
  const auto jc_al = j * c - a * l;
  const auto bl_kc = b * l - k * c;

  const auto M = a * ei_hf + b * gf_di + c * dh_eg;

  // compute t
  const auto t = -(f * ak_jb + e * jc_al + d * bl_kc) / M;
  if ((t < t_min) || (t > t_max))
    return {};

  // compute gamma
  const auto gamma = (i * ak_jb + h * jc_al + g * bl_kc) / M;
  if ((gamma < 0) || (gamma > 1))
    return {};

  // compute beta
  const auto beta = (j * ei_hf + k * gf_di + l * dh_eg) / M;
  if ((beta < 0) || (beta > (1 - gamma)))
    return {};

  const auto n = normal();
  return HitRecord{t, r(t), n, material};
}

} // namespace lesty