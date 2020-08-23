//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020 James D. Mitchell
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

#include "libsemigroups/timer.hpp"

#include <chrono>   // for nanoseconds, duration_cast, minutes, operator>
#include <cstddef>  // for size_t
#include <string>   // for string

#include "libsemigroups/string.hpp"  // for to_string

namespace libsemigroups {
  namespace detail {
    namespace {
      template <typename T>
      bool string_it(std::string&              str,
                     std::chrono::nanoseconds& elapsed,
                     std::string               unit,
                     size_t                    threshold) {
        T x = std::chrono::duration_cast<T>(elapsed);
        if (x > T(threshold)) {
          str += detail::to_string(x.count()) + unit;
          elapsed -= std::chrono::nanoseconds(x);
          return true;
        }
        return false;
      }
    }  // namespace
    // String containing the somewhat human readable amount of time, this is
    // primarily intended for testing purposes
    std::string Timer::string(std::chrono::nanoseconds elapsed) {
      std::string out;
      if (string_it<std::chrono::hours>(out, elapsed, "h", 0)) {
        string_it<std::chrono::minutes>(out, elapsed, "m", 0);
        return out;
      } else if (string_it<std::chrono::minutes>(out, elapsed, "m", 0)) {
        string_it<std::chrono::seconds>(out, elapsed, "s", 0);
        return out;
      } else if (string_it<std::chrono::milliseconds>(out, elapsed, "ms", 9)) {
        return out;
      } else if (string_it<std::chrono::microseconds>(
                     out, elapsed, "\u03BCs", 9)) {
        return out;
      } else {
        string_it<std::chrono::nanoseconds>(out, elapsed, "ns", 0);
        return out;
      }
    }
  }  // namespace detail
}  // namespace libsemigroups
