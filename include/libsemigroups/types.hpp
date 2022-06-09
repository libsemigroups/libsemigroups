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

#ifndef LIBSEMIGROUPS_TYPES_HPP_
#define LIBSEMIGROUPS_TYPES_HPP_

#include <cstddef>  // for size_t

#include <cstdint>      // for uint8_t, uint16_t, uint32_t, uint64_t
#include <type_traits>  // for conditional
#include <utility>      // for pair
#include <vector>       // for vector

namespace libsemigroups {
  //! The values in this enum can be used to indicate a result is true, false,
  //! or not currently knowable.
  enum class tril {
    //! Value representing false.
    FALSE = 0,
    //! Value representing true.
    TRUE = 1,
    //! Value representing unknown (either true or false).
    unknown = 2
  };

  //! The values in this enum can be used to indicate that a congruence should
  //! be 2-sided, left, or right.
  enum class congruence_kind { left = 0, right = 1, twosided = 2 };

  //! Provides a type giving the smallest unsigned integer type capable of
  //! representing the template \c N.
  //!
  //! The member \c type contains the smallest (in terms of
  //! memory required) unsigned integer type which can represent the
  //! non-negative integer \c N.
  template <size_t N>
  struct SmallestInteger {
    //! The smallest (in terms of memory required) unsigned integer type which
    //! can represent \c N.
    using type = std::conditional_t<
        N >= 0x100000000,
        uint64_t,
        std::conditional_t<N >= 0x10000,
                           uint32_t,
                           std::conditional_t<N >= 0x100, uint16_t, uint8_t>>>;
  };

  //! Type for the index of a generator of a semigroup.
  using letter_type = size_t;

  //! Type for a word over the generators of a semigroup.
  using word_type = std::vector<letter_type>;

  //! Type for a pair of word_type (a *relation*) of a semigroup.
  using relation_type = std::pair<word_type, word_type>;
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_TYPES_HPP_
