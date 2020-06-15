//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020 James D. Mitchell
//                    + TODO the other guys
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

#ifndef LIBSEMIGROUPS_FREEBAND_HPP_
#define LIBSEMIGROUPS_FREEBAND_HPP_

#include <algorithm>  // for max_element
#include <iostream>   // for cout
#include <vector>     // for vector

#include "libsemigroups/constants.hpp"
#include "libsemigroups/libsemigroups-debug.hpp"
#include "types.hpp" // word_type

namespace libsemigroups {

  bool freeband_equal_to(word_type& x, word_type& y);

  bool freeband_equal_to(std::initializer_list<size_t> x,
                         std::initializer_list<size_t> y) {
    word_type xx(x);
    word_type yy(y);
    return freeband_equal_to(xx, yy);
  }

  void standardize(word_type& x);

    template <typename T>
    word_type right(T const first, T const last, size_t const k) {
      // LIBSEMIGROUPS_ASSERT(is_standardized(first, last));
      word_type out;
      if (std::distance(first, last) == 0) {
        return out;
      }
      T                   j            = first;
      size_t              content_size = 0;
      size_t const        N = *std::max_element(first, last) + 1;
      std::vector<size_t> multiplicity(N + 1, 0);
      for (auto i = first; i != last; ++i) {
        if (i != first) {
          LIBSEMIGROUPS_ASSERT(multiplicity[*(i- 1)] != 0);
          --multiplicity[*(i - 1)];
          if (multiplicity[*(i - 1)] == 0) {
            --content_size;
          }
        }
        while (j < last
              && (multiplicity[*j] != 0 || content_size < k)) {
          if (multiplicity[*j] == 0) {
            ++content_size;
          }
          ++multiplicity[*j];
          ++j;
        }
        out.push_back(content_size == k ? size_t(std::distance(first, j)) - 1
                                        : UNDEFINED);
      }
      return out;
    }

    // Reverses and corrects output of "right" to "left"
    word_type reverse(word_type&& out) {
      std::reverse(out.begin(), out.end());
      for (auto& x : out) {
        if (x != UNDEFINED) {
          x = out.size() - x - 1;
        }
      }
      return std::move(out);
    }


}  // namespace libsemigroups

#endif
