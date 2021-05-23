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

// This file contains the implementation of functionality for generating words
// in the free monoid over an alphabet with a given number of letters between a
// given pair of words.
//
// WILO = Words In Lexicographic Order

#include "libsemigroups/wilo.hpp"

#include <cstddef>  // for size_t

#include "libsemigroups/types.hpp"  // for word_type

namespace libsemigroups {

  const_wilo_iterator::~const_wilo_iterator() = default;

  const_wilo_iterator::const_wilo_iterator(const_wilo_iterator const&)
      = default;

  const_wilo_iterator cbegin_wilo(size_t      n,
                                  size_t      upper_bound,
                                  word_type&& first,
                                  word_type&& last) {
    if (!lexicographical_compare(
            first.cbegin(), first.cend(), last.cbegin(), last.cend())) {
      return cend_wilo(n, upper_bound, std::move(first), std::move(last));
    }
    if (first.size() >= upper_bound) {
      return ++const_wilo_iterator(
          n, upper_bound, std::move(first), std::move(last));
    }
    return const_wilo_iterator(
        n, upper_bound, std::move(first), std::move(last));
  }

  const_wilo_iterator cbegin_wilo(size_t           n,
                                  size_t           upper_bound,
                                  word_type const& first,
                                  word_type const& last) {
    return cbegin_wilo(n, upper_bound, word_type(first), word_type(last));
  }

  const_wilo_iterator
  cend_wilo(size_t n, size_t upper_bound, word_type&&, word_type&& last) {
    return const_wilo_iterator(
        n, upper_bound, word_type(last), std::move(last));
  }

  const_wilo_iterator cend_wilo(size_t n,
                                size_t upper_bound,
                                word_type const&,
                                word_type const& last) {
    return cend_wilo(n, upper_bound, word_type(), word_type(last));
  }
}  // namespace libsemigroups
