//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 Florent Hivert
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

#ifndef LIBSEMIGROUPS_TESTS_FPSEMI_EXAMPLES_HPP_
#define LIBSEMIGROUPS_TESTS_FPSEMI_EXAMPLES_HPP_

#include "types.hpp"

namespace libsemigroups {
  template <typename T>
  std::vector<T> concat(std::vector<T> lhs, const std::vector<T>& rhs) {
    lhs.insert(lhs.end(), rhs.begin(), rhs.end());
    return lhs;
  }

  std::vector<relation_type> RookMonoid(size_t l, int q);
  std::vector<relation_type> RennerCommonTypeBMonoid(size_t l, int q);
  std::vector<relation_type> RennerTypeBMonoid(size_t l, int q);
  std::vector<relation_type> EGTypeBMonoid(size_t l, int q);
  std::vector<relation_type> RennerCommonTypeDMonoid(size_t l, int q);
  std::vector<relation_type> EGTypeDMonoid(size_t l, int q);
  std::vector<size_t>        max_elt_D(size_t i, int g);
  std::vector<size_t>        max_elt_B(size_t i);
  std::vector<relation_type> RennerTypeDMonoid(size_t l, int q);
  std::vector<relation_type> Stell(size_t l);

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_TESTS_FPSEMI_EXAMPLES_HPP_
