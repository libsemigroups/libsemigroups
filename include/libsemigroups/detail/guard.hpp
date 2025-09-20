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

#ifndef LIBSEMIGROUPS_DETAIL_GUARD_HPP_
#define LIBSEMIGROUPS_DETAIL_GUARD_HPP_

#include "libsemigroups/is_specialization_of.hpp"
#include <atomic>  // for std::atomic

namespace libsemigroups {
  namespace detail {
    // A simple class for ensuring that a value is set back to its value at the
    // time of the creation of the Guard. Probably should only be used with POD
    // types for Thing.
    template <typename Thing>
    class Guard {
      Thing  _old_value;
      Thing& _value;

     public:
      Guard()                        = delete;
      Guard(Guard const&)            = delete;
      Guard(Guard&&)                 = delete;
      Guard& operator=(Guard const&) = delete;
      Guard& operator=(Guard&&)      = delete;

      Guard(Thing& value, Thing new_value) : _old_value(value), _value(value) {
        _value = new_value;
      }

      template <typename Value>
      Guard(Thing& value, Value new_value)
          : _old_value(value.load()), _value(value) {
        _value = new_value;
      }

      explicit Guard(Thing& value) : _old_value(value), _value(value) {}

      ~Guard() {
        if constexpr (is_specialization_of_v<Thing, std::atomic>) {
          _value = _old_value.load();
        } else {
          _value = _old_value;
        }
      }
    };

    template <typename Thing>
    Guard(Thing& value, typename Thing::value_type new_value) -> Guard<Thing>;

  }  // namespace detail
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_DETAIL_GUARD_HPP_
