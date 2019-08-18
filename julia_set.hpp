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
    for (int it = 0; it < max_iteration; ++it) {
      dz = 2.0f * z * dz;
      z = z * z + coeff;
      if (norm(z) > 1e5f) break;
    }
    const auto tmp = norm(z);
    return sqrt(tmp / norm(dz)) * 0.5f * log(tmp);
  }

  complex coeff;
};