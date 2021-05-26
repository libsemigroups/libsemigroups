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
#include "libsemigroups/debug.hpp"
#include "types.hpp"  // word_type

namespace libsemigroups {

  bool freeband_equal_to(word_type& x, word_type& y);

  bool freeband_equal_to(std::initializer_list<size_t> x,
                         std::initializer_list<size_t> y) {
    word_type xx(x);
    word_type yy(y);
    return freeband_equal_to(xx, yy);
  }

}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_FREEBAND_HPP_
