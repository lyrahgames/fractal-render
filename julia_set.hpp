#pragma once

#include <complex>

struct julia_set {
  using complex = std::complex<float>;

  static constexpr int max_iteration = 1 << 10;

  int iteration(complex z) const noexcept {
    constexpr int max_iteration = 1 << 10;
    int it = 0;
    for (; (norm(z) < 4) && (it < max_iteration); ++it)
      z = z * z + z * z * z + coeff;
    return it;
  }

  complex coeff;
};