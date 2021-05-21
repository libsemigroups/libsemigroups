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

#ifndef LIBSEMIGROUPS_BENCHMARKS_EXAMPLES_COMMON_HPP_
#define LIBSEMIGROUPS_BENCHMARKS_EXAMPLES_COMMON_HPP_

#include <cstddef>  // for size_t
#include <vector>   // for vector

#include "libsemigroups/adapters.hpp"

namespace libsemigroups {
  namespace detail {
    template <typename T>
    inline void hash_combine(size_t& s, T const& v) {
      Hash<T> h;
      s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
    }

    template <typename T>
    inline size_t hash_combine(std::vector<T> const& v) {
      size_t s = 0;
      for (auto const& x : v) {
        hash_combine(s, x);
      }
      return s;
    }

  }  // namespace detail
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_BENCHMARKS_EXAMPLES_COMMON_HPP_
