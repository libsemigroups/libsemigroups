//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023 James D. Mitchell
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

// This file contains TODO

#ifndef LIBSEMIGROUPS_RANGES_HPP_
#define LIBSEMIGROUPS_RANGES_HPP_

#include <algorithm>

namespace libsemigroups {

  template <typename Range, typename Compare>
  constexpr bool is_sorted(Range r, Compare comp) {
    if (!r.at_end()) {
      auto first = r.get();
      r.next();
      while (!r.at_end()) {
        auto second = r.get();
        if (comp(second, first)) {
          return false;
        }
        first = second;
        r.next();
      }
    }
    return true;
  }

  template <class Range>
  constexpr bool is_sorted(Range r) {
    return std::is_sorted(r, std::less<>());
  }

}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_RANGES_HPP_
