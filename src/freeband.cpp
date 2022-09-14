//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021 James D. Mitchell
//                    Tom D. Conti-Leslie
//                    Murray T. Whyte
//                    Reinis Cirpons
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

#include "libsemigroups/freeband.hpp"

#include <algorithm>    // for max_element
#include <cstddef>      // for size_t
#include <iterator>     // for distance, reverse_iterator
#include <type_traits>  // for decay_t, is_same
#include <vector>       // for vector

#include "libsemigroups/constants.hpp"  // for UNDEFINED
#include "libsemigroups/debug.hpp"      // for LIBSEMIGROUPS_ASSERT

namespace libsemigroups {

  namespace {
    using level_edges_type = std::vector<std::vector<size_t>>;
    using left_type        = std::vector<size_t>;
    using right_type       = std::vector<size_t>;
    template <typename T>
    bool is_standardized(T first, T last) {
      size_t m = 0;
      for (auto it = first; it != last; ++it) {
        if (*it > m + 1) {
          return false;
        } else if (*it > m) {
          ++m;
        }
      }
      return true;
    }

    size_t content_size(word_type const& w) {
      // LIBSEMIGROUPS_ASSERT(is_standardized(w.cbegin(), w.cend()));
      return *std::max_element(w.cbegin(), w.cend()) + 1;
    }

    void standardize(word_type& x) {
      if (x.empty()) {
        return;
      }

      size_t              distinct_chars = 0;
      size_t const        N = *std::max_element(x.cbegin(), x.cend()) + 1;
      std::vector<size_t> lookup(N, N);
      lookup[x[0]] = 0;
      x[0]         = 0;
      for (size_t i = 1; i < x.size(); ++i) {
        if (lookup[x[i]] == N) {
          lookup[x[i]] = ++distinct_chars;
        }
        x[i] = lookup[x[i]];
      }
    }

    // Returns a vector `out` of indices in [0, last - first) or UNDEFINED,
    // where [i, i + out[i]) has content exactly k, and `out[i]` is the minimum
    // such value.
    //
    // Complexity O(last - first)

    template <typename T>
    void right(T first, T last, size_t const k, std::vector<size_t>& out) {
      // TODO(later) assertions
      static_assert(
          std::is_same<std::decay_t<T>,
                       typename word_type::const_iterator>::value
              || std::is_same<
                  std::decay_t<T>,
                  typename word_type::const_reverse_iterator>::value,
          "The template parameter must be a word_type::const_iterator or a "
          "word_type::const_reverse_iterator");

      out.clear();
      if (std::distance(first, last) == 0) {
        return;
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
    }

    template <typename T>
    void left(T first, T last, size_t const k, std::vector<size_t>& out) {
      right(std::reverse_iterator<T>(last),
            std::reverse_iterator<T>(first),
            k,
            out);
      std::reverse(out.begin(), out.end());
      for (auto& x : out) {
        if (x != UNDEFINED) {
          x = out.size() - x - 1;
        }
      }
    }

    // What types should we be using? I get that word_type is an alias to
    // std::vector<size_t>, but wouldn't using the vector be more useful,
    // since the output/input is not really a representation of a word.
    // Also the input i here will range from 0 to 3, so is it better
    // to assign it to a unsigned short or something?
    void count_sort(std::vector<word_type> const& level_edges,
                    word_type&                    index_list,
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
      std::swap(result_index_list, index_list);
    }

    void radix_sort(std::vector<word_type> const& level_edges,
                    size_t                        alphabet_size,
                    std::vector<size_t>&          result_index_list,
                    std::vector<size_t>&          index_list) {
      LIBSEMIGROUPS_ASSERT(result_index_list.size() == level_edges.size());
      LIBSEMIGROUPS_ASSERT(index_list.size() == level_edges.size());

      std::fill(result_index_list.begin(), result_index_list.end(), 0);
      std::fill(index_list.begin(), index_list.end(), 0);

      for (size_t j = 0; j < index_list.size(); j++) {
        index_list[j] = j;
      }

      count_sort(level_edges, index_list, 0, index_list.size());
      count_sort(level_edges, index_list, 1, alphabet_size);
      count_sort(level_edges, index_list, 2, alphabet_size);
      count_sort(level_edges, index_list, 3, index_list.size());

      size_t c = 0;
      for (size_t j = 1; j < index_list.size(); j++) {
        if (level_edges[index_list[j]] != level_edges[index_list[j - 1]])
          c++;
        result_index_list[index_list[j]] = c;
      }
      result_index_list[index_list[0]] = 0;
    }

    void level_edges(word_type const&           w,
                     size_t const               k,
                     std::vector<size_t> const& rdx,
                     right_type const&          rightk,
                     left_type const&           leftk,
                     right_type const&          rightm,
                     left_type const&           leftm,
                     level_edges_type&          out) {
      size_t const n = w.size();
      LIBSEMIGROUPS_ASSERT(out.size() == 2 * n);
      std::fill(
          out.begin(),
          out.end(),
          std::vector<size_t>({UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED}));

      if (k == 1) {
        for (size_t i = 0; i < n; ++i) {
          out[i]     = {0, w.at(i), w.at(i), 0};
          out[i + n] = {0, w.at(i), w.at(i), 0};
        }
        return;
      }

      for (size_t i = 0; i < n; ++i) {
        if (rightk.at(i) != UNDEFINED) {
          out[i] = {rdx.at(i),
                    w.at(rightm.at(i) + 1),
                    w.at(leftm.at(rightk.at(i)) - 1),
                    rdx.at(rightk.at(i) + n)};
        }
        if (leftk.at(i) != UNDEFINED) {
          out[i + n] = {rdx.at(leftk.at(i)),
                        w.at(rightm.at(leftk.at(i)) + 1),
                        w.at(leftm.at(i) - 1),
                        rdx.at(i + n)};
        }
      }
    }
  }  // namespace

  bool freeband_equal_to(word_type const& x, word_type const& y) {
    if (x == y) {
      return true;
    }
    if (x.empty() || y.empty()) {
      // FIXME: Code segfaults without this check, its possible there is some
      // unsafe memory access further in the code.
      return false;
    }
    size_t N = content_size(x);
    if (N != content_size(y)) {
      return false;
    }
    word_type xy(x);
    xy.push_back(N);
    xy.insert(xy.end(), y.cbegin(), y.cend());
    standardize(xy);
    LIBSEMIGROUPS_ASSERT(xy.at(x.size())
                         == *std::max_element(xy.cbegin(), xy.cend()));
    N = content_size(xy);

    level_edges_type    lvldgs(2 * xy.size(), {0, 0, 0, 0});
    std::vector<size_t> rdx(2 * xy.size(), 0);
    std::vector<size_t> ndx(2 * xy.size(), 0);

    left_type  leftk;
    right_type rightk;
    left_type  leftm;
    right_type rightm;

    for (size_t k = 1; k < N; ++k) {
      std::swap(rightm, rightk);
      std::swap(leftm, leftk);

      right(xy.cbegin(), xy.cend(), k, rightk);
      left(xy.cbegin(), xy.cend(), k, leftk);
      level_edges(xy, k, rdx, rightk, leftk, rightm, leftm, lvldgs);
      radix_sort(lvldgs, N + 1, rdx, ndx);
    }

    return rdx[0] == rdx[x.size() + 1];
  }
}  // namespace libsemigroups
