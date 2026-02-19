//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_DETAIL_VALUE_GUARD_HPP_
#define LIBSEMIGROUPS_DETAIL_VALUE_GUARD_HPP_

#include <atomic>  // for std::atomic

#include "libsemigroups/is_specialization_of.hpp"  // for is_specialization_of

namespace libsemigroups::detail {
  // A simple class for ensuring that a value is set back to its value at the
  // time of the creation of the ValueGuard. Probably should only be used with
  // POD types for Thing.
  template <typename Thing>
  class ValueGuard {
    Thing  _value_old;
    Thing& _value_ref;

   public:
    ValueGuard()                             = delete;
    ValueGuard(ValueGuard const&)            = delete;
    ValueGuard(ValueGuard&&)                 = delete;
    ValueGuard& operator=(ValueGuard const&) = delete;
    ValueGuard& operator=(ValueGuard&&)      = delete;

    explicit ValueGuard(Thing& value);
    ~ValueGuard();
  };
}  // namespace libsemigroups::detail

#include "value-guard.tpp"
#endif  // LIBSEMIGROUPS_DETAIL_VALUE_GUARD_HPP_
