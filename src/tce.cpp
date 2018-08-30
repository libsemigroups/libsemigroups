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

// TODO 1. smart pointer to _tc?

#include "tce.hpp"

#include "internal/libsemigroups-debug.hpp"  // for LIBSEMIGROUPS_ASSERT

#include "cong-base.hpp"     // for CongBase::class_index_type
#include "todd-coxeter.hpp"  // for congruence::ToddCoxeter

namespace libsemigroups {

  static_assert(
      std::is_same<TCE::class_index_type, CongBase::class_index_type>::value,
      "class_index_type is incorrect");

  TCE::TCE(congruence::ToddCoxeter* tc, class_index_type i) noexcept
      : _tc(tc), _index(i) {}

  TCE::TCE(congruence::ToddCoxeter& tc, class_index_type i) noexcept
      : TCE(&tc, i) {}

  bool TCE::operator==(TCE const& that) const noexcept {
    LIBSEMIGROUPS_ASSERT(_tc == that._tc);
    return _index == that._index;
  }

  bool TCE::operator<(TCE const& that) const noexcept {
    LIBSEMIGROUPS_ASSERT(_tc == that._tc);
    return _index < that._index;
  }

  // Only works when that is a generator!!
  TCE TCE::operator*(TCE const& that) const {
    // class_index_to_letter checks that "that" is really a generator
    return TCE(_tc,
               _tc->table(_index, _tc->class_index_to_letter(that._index)));
  }

  TCE TCE::one() const noexcept {
    return TCE(_tc, 0);
  }

  std::ostringstream& operator<<(std::ostringstream& os, TCE const& x) {
    os << "TCE(" << x._index << ")";
    return os;
  }

  std::ostream& operator<<(std::ostream& os, TCE const& tc) {
    os << to_string(tc);
    return os;
  }
}  // namespace libsemigroups
