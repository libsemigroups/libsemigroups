//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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

// This file contains some functionality for generating elements in the free
// monoid over an alphabet with a given number of letters up to a given length
// in a particular order.

// TODO(later)
// 1. add the others (recursive path words) from test-todd-coxeter.cpp
// 2. add some documentation

#ifndef LIBSEMIGROUPS_INCLUDE_ORDER_HPP_
#define LIBSEMIGROUPS_INCLUDE_ORDER_HPP_

#include <cstddef>  // for size_t
#include <vector>   // for vector

#include "types.hpp"  // for word_type

namespace libsemigroups {
  std::vector<word_type> shortlex_words(size_t nr_gens, size_t len);

  template <typename T>
  struct LexicographicalCompare {
    bool operator()(T const& x, T const& y) {
      return std::lexicographical_compare(
          x.cbegin(), x.cend(), y.cbegin(), y.cend());
    }
  };

  template <typename T, typename S>
  bool shortlex_compare(T const first1,
                        T const last1,
                        S const first2,
                        S const last2) {
    return (last1 - first1) < (last2 - first2)
           || ((last1 - first1) == (last2 - first2)
               && std::lexicographical_compare(first1, last1, first2, last2));
  }

  template <typename T>
  bool shortlex_compare(T const& x, T const& y) {
    return shortlex_compare(x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  template <typename T>
  bool shortlex_compare(T* const x, T* const y) {
    return shortlex_compare(x->cbegin(), x->cend(), y->cbegin(), y->cend());
  }

  template <typename T>
  struct ShortLexCompare {
    bool operator()(T const& x, T const& y) {
      return shortlex_compare(x.cbegin(), x.cend(), y.cbegin(), y.cend());
    }
  };

  template <typename T, typename S>
  bool
  recursive_path_compare(T const first1, T last1, S const first2, S last2) {
    bool lastmoved = false;
    --last1;
    --last2;
    while (true) {
      if (last1 < first1) {
        if (last2 < first2) {
          return lastmoved;
        }
        return true;
      }
      if (last2 < first2) {
        return false;
      }
      if (*last1 == *last2) {
        last1--;
        last2--;
      } else if (*last1 < *last2) {
        last1--;
        lastmoved = false;
      } else if (*last2 < *last1) {
        last2--;
        lastmoved = true;
      }
    }
  }

  template <typename T>
  struct RecursivePathCompare {
    bool operator()(T const& x, T const& y) {
      return recursive_path_compare(x.cbegin(), x.cend(), y.cbegin(), y.cend());
    }
  };
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_INCLUDE_ORDER_HPP_
