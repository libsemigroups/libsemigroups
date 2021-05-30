//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020 James D. Mitchell
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

// This file contains a declaration of the function freeband_equal_to which
// implements the algorithm described in:
// "Efficient Testing of Equivalence of Words in a Free Idempotent Semigroup"
// by J. Radoszewski and W. Rytter
// in SOFSEM 2010: Theory and Practice of Computer Science. Jan. 2010,
// pp. 664-671
// doi: 10.1007/978-3-642-11266-9_55

#ifndef LIBSEMIGROUPS_FREEBAND_HPP_
#define LIBSEMIGROUPS_FREEBAND_HPP_

#include <algorithm>  // for max_element
#include <iostream>   // for cout
#include <vector>     // for vector

#include "libsemigroups/constants.hpp"
#include "libsemigroups/debug.hpp"
#include "types.hpp"  // word_type

namespace libsemigroups {

  bool freeband_equal_to(word_type&& x, word_type&& y);

  template <typename T>
  bool freeband_equal_to(T x, T y) {
    return freeband_equal_to(word_type(x), word_type(y));
  }

  template <typename T>
  bool freeband_equal_to(T first1, T last1, T first2, T last2) {
    return freeband_equal_to(word_type(first1, last1),
                             word_type(first2, last2));
  }

}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_FREEBAND_HPP_
