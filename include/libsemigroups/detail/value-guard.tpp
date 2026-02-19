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

namespace libsemigroups::detail {

  template <typename Thing>
  ValueGuard<Thing>::ValueGuard(Thing& value)
      : _value_old(), _value_ref(value) {
    if constexpr (is_specialization_of_v<Thing, std::atomic>) {
      _value_old = value.load();
    } else {
      _value_old = value;
    }
  }

  template <typename Thing>
  ValueGuard<Thing>::~ValueGuard() {
    if constexpr (is_specialization_of_v<Thing, std::atomic>) {
      _value_ref = _value_old.load();
    } else {
      _value_ref = _value_old;
    }
  }
}  // namespace libsemigroups::detail
