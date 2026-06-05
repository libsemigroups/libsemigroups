//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 James D. Mitchell
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

// This file contains implementations of specializations of the class adapters
// in adapters.hpp for the element types in HPCombi.

#include "libsemigroups/max-plus-trunc-mat.hpp"

namespace libsemigroups {

  template <>
  void Konieczny<MaxPlusTruncMat<>>::throw_if_bad_element(
      typename Konieczny<MaxPlusTruncMat<>>::const_reference x) const {
    if (number_of_generators() > 0
        && matrix::threshold(x) != matrix::threshold(generator(0))) {
      LIBSEMIGROUPS_EXCEPTION(
          "the matrix has threshold {} but should have threshold {}",
          matrix::threshold(x),
          matrix::threshold(generator(0)));
    }
  }
}  // namespace libsemigroups
