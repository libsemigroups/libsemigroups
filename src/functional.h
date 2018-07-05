//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_SRC_FUNCTIONAL_H_
#define LIBSEMIGROUPS_SRC_FUNCTIONAL_H_

#include <functional>

namespace libsemigroups {
  template <typename TValueType, typename = void> struct hash {
    size_t operator()(TValueType const& x) const {
      return std::hash<TValueType>()(x);
    }
  };

  template <typename TValueType, typename = void> struct equal_to {
    size_t operator()(TValueType const& x, TValueType const& y) const {
      return std::equal_to<TValueType>()(x, y);
    }
  };

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_SRC_FUNCTIONAL_H_
