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

// This file contains the implementations of functions for generating elements
// in the free monoid over an alphabet with a given number of letters up to a
// given length in short-lex order.

// WISLO = Words In Short-Lex Order

#include "libsemigroups/wislo.hpp"

#include <cstddef>  // for size_t

#include "libsemigroups/types.hpp"  // for word_type

namespace libsemigroups {

  const_wislo_iterator::~const_wislo_iterator() = default;

  const_wislo_iterator::const_wislo_iterator(const_wislo_iterator const&)
      = default;

  const_wislo_iterator cbegin_wislo(size_t      n,
                                    word_type&& first,
                                    word_type&& last) {
    if (!shortlex_compare(
            first.cbegin(), first.cend(), last.cbegin(), last.cend())) {
      return cend_wislo(n, std::move(first), std::move(last));
    }
    return const_wislo_iterator(n, std::move(first), std::move(last));
  }

  const_wislo_iterator cbegin_wislo(size_t           n,
                                    word_type const& first,
                                    word_type const& last) {
    return cbegin_wislo(n, word_type(first), word_type(last));
  }

  const_wislo_iterator cend_wislo(size_t n, word_type&&, word_type&& last) {
    return const_wislo_iterator(n, word_type(last), std::move(last));
  }

  const_wislo_iterator cend_wislo(size_t n,
                                  word_type const&,
                                  word_type const& last) {
    return cend_wislo(n, word_type(), word_type(last));
  }
}  // namespace libsemigroups
