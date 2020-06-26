//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020 James D. Mitchell
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

// This entire file is only build if LIBSEMIGROUPS_ENABLE_EIGEN is not defined.

#include "libsemigroups/digraph.hpp"

namespace libsemigroups {
  namespace detail {
    std::vector<uint64_t> one(size_t const N) {
      std::vector<uint64_t> out(N * N, 0);
      for (size_t i = 0; i < N; ++i) {
        out[i * N + i] = 1;
      }
      return out;
    }

    void matrix_product_in_place(std::vector<uint64_t>&       xy,
                                 std::vector<uint64_t> const& x,
                                 std::vector<uint64_t> const& y,
                                 size_t const                 N) {
      for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N; ++j) {
          uint64_t v = 0;
          for (size_t k = 0; k < N; ++k) {
            v += x[i * N + k] * y[k * N + j];
          }
          xy[i * N + j] = v;
        }
      }
    }

    void pow(std::vector<uint64_t>& x, uint64_t e, size_t N) {
      if (e == 0) {
        x = one(N);
        return;
      } else if (e == 1) {
        return;
      }
      std::vector<uint64_t> y(x);
      std::vector<uint64_t> tmp(x.size(), 0);
      auto                  z = (e % 2 == 0 ? one(N) : x);

      while (e > 1) {
        matrix_product_in_place(tmp, y, y, N);
        std::swap(y, tmp);
        e /= 2;
        if (e % 2 == 1) {
          matrix_product_in_place(tmp, z, y, N);
          std::swap(z, tmp);
        }
      }
      x = z;
    }

  }  // namespace detail
}  // namespace libsemigroups
