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

// This file contains a declaration of the function freeband_equal_to which
// implements the algorithm described in:
// TODO

#ifndef LIBSEMIGROUPS_FREEBAND_HPP_
#define LIBSEMIGROUPS_FREEBAND_HPP_

#include <algorithm>  // for max_element
#include <iostream>   // for cout
#include <vector>     // for vector

#include "libsemigroups/constants.hpp"
#include "libsemigroups/libsemigroups-debug.hpp"
#include "types.hpp"  // word_type

namespace libsemigroups {

  bool freeband_equal_to(word_type& x, word_type& y);

  bool freeband_equal_to(std::initializer_list<size_t> x,
                         std::initializer_list<size_t> y) {
    word_type xx(x);
    word_type yy(y);
    return freeband_equal_to(xx, yy);
  }

  void standardize(word_type& x);

  // Returns a vector `out` of indices in [0, last - first) or UNDEFINED, where
  // [i, i + out[i]) has content exactly k, and `out[i]` is the minimum such
  // value.
  //
  // Complexity O(last - first)
  template <typename T>
  std::vector<size_t> right(T const first, T const last, size_t const k) {
    // TODO assertions
    word_type out;
    if (std::distance(first, last) == 0) {
      return out;
    }
    T                   j            = first;
    size_t              content_size = 0;
    size_t const        N            = *std::max_element(first, last) + 1;
    std::vector<size_t> multiplicity(N + 2, 0);
    for (auto i = first; i != last; ++i) {
      if (i != first) {
        LIBSEMIGROUPS_ASSERT(multiplicity[*(i - 1)] != 0);
        --multiplicity[*(i - 1)];
        if (multiplicity[*(i - 1)] == 0) {
          --content_size;
        }
      }
      while (j < last && (multiplicity[*j] != 0 || content_size < k)) {
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
  // i.e. reverse(right(last, first, k)) = left(first, last, k)
  void reverse(std::vector<size_t>& out) {
    std::reverse(out.begin(), out.end());
    for (auto& x : out) {
      if (x != UNDEFINED) {
        x = out.size() - x - 1;
      }
    }
  }

  template <typename T>
  std::vector<size_t> left(T const first, T const last, size_t const k) {
    auto out = right(
        std::reverse_iterator<T>(last), std::reverse_iterator<T>(first), k);
    reverse(out);
    return out;
  }

  // What types should we be using? I get that word_type is an alias to
  // std::vector<size_t>, but wouldn't using the vector be more useful,
  // since the output/input is not really a representation of a word.
  // Also the input i here will range from 0 to 3, so is it better
  // to assign it to a unsigned short or something?
  word_type count_sort(std::vector<word_type> const& level_edges,
                       word_type const&              index_list,
                       size_t                        i,
                       size_t                        radix) {
    // Could actually reuse an already existing count array
    word_type counts(radix + 1, 0);
    for (auto j : index_list) {
      if (level_edges[j][i] != UNDEFINED)
        counts[level_edges[j][i]]++;
      else
        counts[radix]++;
    }
    for (size_t j = 1; j < counts.size(); j++)
      counts[j] += counts[j - 1];
    // This can also be reused and doesnt even have to be initialized if we
    // reuse it.
    word_type result_index_list(index_list.size(), 0);
    for (auto j = index_list.rbegin(); j != index_list.rend(); j++) {
      if (level_edges[*j][i] != UNDEFINED) {
        counts[level_edges[*j][i]]--;
        result_index_list[counts[level_edges[*j][i]]] = *j;
      } else {
        counts[radix]--;
        result_index_list[counts[radix]] = *j;
      }
    }
    // Could also swap result_index_list with index_list!
    return result_index_list;
  }

  word_type radix_sort(std::vector<word_type> const& level_edges,
                       size_t                        alphabet_size) {
    // Can reuse this instead of initializing every time
    word_type index_list(level_edges.size(), 0);
    for (size_t j = 0; j < index_list.size(); j++)
      index_list[j] = j;

    index_list = count_sort(level_edges, index_list, 0, index_list.size());
    index_list = count_sort(level_edges, index_list, 1, alphabet_size);
    index_list = count_sort(level_edges, index_list, 2, alphabet_size);
    index_list = count_sort(level_edges, index_list, 3, index_list.size());

    // Reuse
    word_type result_index_list(index_list.size(), 0);
    size_t    c = 0;
    for (size_t j = 1; j < index_list.size(); j++) {
      if (level_edges[index_list[j]] != level_edges[index_list[j - 1]])
        c++;
      result_index_list[index_list[j]] = c;
    }
    result_index_list[index_list[0]] = 0;
    // Something something, swap instead
    return result_index_list;
  }

}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_FREEBAND_HPP_
