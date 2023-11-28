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

#include "libsemigroups/detail/timer.hpp"

#include <chrono>  // for nanoseconds, duration_cast, minutes, operator>
#include <string>  // for string

#include <fmt/chrono.h>
#include <fmt/format.h>

namespace libsemigroups {
  using namespace std::chrono;

  namespace detail {
    namespace {

      template <typename T>
      bool string_time_incremental(std::string& result, nanoseconds& elapsed) {
        T x = duration_cast<T>(elapsed);
        if (x.count() > 0) {
          result += fmt::format("{}", x);
          elapsed -= nanoseconds(x);
          return true;
        }
        return false;
      }

      bool string_time_incremental(std::string& result,
                                   nanoseconds& elapsed,
                                   bool         use_float) {
        using seconds = seconds;
        seconds x     = duration_cast<seconds>(elapsed);
        if (x.count() > 0) {
          if (use_float) {
            double x_float
                = static_cast<double>(elapsed.count()) / 1'000'000'000;
            result += fmt::format("{:.3f}s", x_float);
          } else {
            result += fmt::format("{}", x);
          }
          elapsed -= nanoseconds(x);
          return true;
        }
        return false;
      }
    }  // namespace

    // String containing the somewhat human readable amount of time, this is
    // primarily intended for testing purposes
    std::string string_time(nanoseconds elapsed) {
      using detail::string_time_incremental;
      std::string out;
      // TODO add day, months etc
      if (string_time_incremental<hours>(out, elapsed)) {
        string_time_incremental<minutes>(out, elapsed);
        string_time_incremental(out, elapsed, false);
      } else if (string_time_incremental<minutes>(out, elapsed)) {
        string_time_incremental(out, elapsed, false);
      } else if (string_time_incremental(out, elapsed, true)) {
      } else if (string_time_incremental<milliseconds>(out, elapsed)) {
      } else if (string_time_incremental<microseconds>(out, elapsed)) {
      } else if (string_time_incremental<nanoseconds>(out, elapsed)) {
      }
      return out;
    }
  }  // namespace detail
}  // namespace libsemigroups
