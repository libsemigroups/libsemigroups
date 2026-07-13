//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 Joseph Edwards
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

// This file contains the declaration of a function that takes in 2d array of
// constraints and returns an integer solution, if such a solution exists.

#ifndef LIBSEMIGROUPS_DETAIL_WEIGHT_FINDING_HPP_
#define LIBSEMIGROUPS_DETAIL_WEIGHT_FINDING_HPP_

#include "libsemigroups/config.hpp"

#ifdef LIBSEMIGROUPS_ALGLIB_ENABLED

#include <optional>  // for std::optional
#include <vector>    // for std::vector

#include "containers.hpp"  // for DynamicArray2

namespace libsemigroups::detail {
  std::optional<std::vector<size_t>>
  get_weights(DynamicArray2<int> const& coefficients,
              std::vector<bool> const&  is_strict);
}

#endif  // LIBSEMIGROUPS_ALGLIB_ENABLED
#endif  // LIBSEMIGROUPS_DETAIL_WEIGHT_FINDING_HPP_
