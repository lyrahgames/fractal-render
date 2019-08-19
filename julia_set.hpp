#pragma once

#include <complex>

struct julia_set {
  using complex = std::complex<float>;

  static constexpr int max_iteration = 1 << 10;

  int iteration(complex z) const noexcept {
    int it = 0;
    for (; (norm(z) < 4) && (it < max_iteration); ++it) z = z * z + coeff;
    return it;
  }

  float distance(complex z) const noexcept {
    complex dz{1, 0};
    int it = 0;
    for (; it < max_iteration; ++it) {
      dz = 2.0f * z * dz;
      z = z * z + coeff;
      if (norm(z) > 1e12f) break;
    }
    const auto tmp = abs(z);
    if (tmp < 2) return 0.0f;
    return tmp * log(tmp) / abs(dz);
  }

  complex coeff;
};