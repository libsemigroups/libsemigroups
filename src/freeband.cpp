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

#include "libsemigroups/freeband.hpp"

#include <algorithm>  // for max_element
#include <iostream>   // for cout
#include <vector>     // for vector

#include "libsemigroups/constants.hpp"
#include "libsemigroups/libsemigroups-debug.hpp"
#include "libsemigroups/string.hpp"

namespace libsemigroups {
  namespace {
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

  }  // namespace

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

  using level_edges_type = std::vector<std::vector<size_t>>;
  using left_type        = std::vector<size_t>;
  using right_type       = std::vector<size_t>;

  level_edges_type level_edges(word_type const&           w,
                               size_t const               k,
                               std::vector<size_t> const& rdx,
                               right_type const&          rightk,
                               left_type const&           leftk,
                               right_type const&          rightm,
                               left_type const&           leftm) {
    size_t const     n = w.size();
    level_edges_type outl;
    level_edges_type outr;

    if (k == 1) {
      for (size_t i = 0; i < n; ++i) {
        outl.push_back({0, w.at(i), w.at(i), 0});
        outr.push_back({0, w.at(i), w.at(i), 0});
      }
    } else {
      for (size_t i = 0; i < n; ++i) {
        if (rightk.at(i) != UNDEFINED) {
          outr.push_back({rdx.at(i),
                          w.at(rightm.at(i) + 1),
                          w.at(leftm.at(rightk.at(i)) - 1),
                          rdx.at(rightk.at(i) + n)});
        } else {
          outr.push_back({UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED});
        }
        if (leftk.at(i) != UNDEFINED) {
          outl.push_back({rdx.at(leftk.at(i)),
                          w.at(rightm.at(leftk.at(i)) + 1),
                          w.at(leftm.at(i) - 1),
                          rdx.at(i + n)});
        } else {
          outl.push_back({UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED});
        }
      }
    }
    outr.insert(outr.end(), outl.cbegin(), outl.cend());
    return outr;
  }

  bool freeband_equal_to(word_type& x, word_type& y) {
    if (x == y) {
      return true;
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

    level_edges_type    lvldgs(xy.size(), {0, 0, 0, 0});
    std::vector<size_t> rdx(xy.size(), 0);

    left_type  leftk;
    right_type rightk;
    left_type  leftm;
    right_type rightm;

    for (size_t k = 1; k < N; ++k) {
      std::swap(rightm, rightk);
      std::swap(leftm, leftk);

      rightk = right(xy.begin(), xy.end(), k);
      leftk  = left(xy.begin(), xy.end(), k);
      lvldgs = level_edges(xy, k, rdx, rightk, leftk, rightm, leftm);
      rdx    = radix_sort(lvldgs, N + 1);
    }

    return rdx[0] == rdx[x.size() + 1];
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

}  // namespace libsemigroups
