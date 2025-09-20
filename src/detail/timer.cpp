//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-2025 James D. Mitchell
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

#include "libsemigroups/detail/fmt.hpp"

namespace libsemigroups {
  using namespace std::chrono;  // NOLINT(build/namespaces)

  // days, weeks, months, years are C++20, hence we declare them here
  using days   = std::chrono::duration<int, std::ratio<86400>>;
  using weeks  = std::chrono::duration<int, std::ratio<604800>>;
  using months = std::chrono::duration<int, std::ratio<2629746>>;
  using years  = std::chrono::duration<int, std::ratio<31556952>>;

  namespace detail {
    namespace {

      template <typename Unit>
      void append(std::string& s, Unit const& x) {
        s += fmt::format("{}", x);
      }

      void append(std::string& s, minutes const& x) {
        s += fmt::format("{}min", x.count());
      }

      void append(std::string& s, days const& x) {
        s += fmt::format("{}d", x.count());
      }

      void append(std::string& s, weeks const& x) {
        s += fmt::format("{}w", x.count());
      }

      void append(std::string& s, months const& x) {
        s += fmt::format("{}mon", x.count());
      }

      void append(std::string& s, years const& x) {
        s += fmt::format("{}y", x.count());
      }

      template <typename Unit>
      bool append_if_non_zero(std::string& s, nanoseconds& x) {
        auto y = duration_cast<Unit>(x);
        if (y.count() > 0) {
          x -= nanoseconds(y);
          append(s, y);
          return true;
        }
        return false;
      }
    }  // namespace

    // String containing the somewhat human readable amount of time, this is
    // primarily intended for testing purposes
    std::string string_time(nanoseconds x) {
      using units = std::tuple<years, months, weeks, days, hours, minutes>;

      if (x.count() == 0) {
        return "-";
      }
      std::string s;
      std::apply(
          [&](auto... args) {
            ((append_if_non_zero<decltype(args)>(s, x)), ...);
          },
          units{});

      if (!s.empty()) {
        append<seconds>(s, duration_cast<seconds>(x));
        return s;
      }
      auto y = duration_cast<seconds>(x);
      if (y.count() > 0) {
        auto x_double = static_cast<double>(x.count()) / 1'000'000'000;
        s += fmt::format("{:.3f}s", x_double);
      } else {
        append_if_non_zero<milliseconds>(s, x)
            || append_if_non_zero<microseconds>(s, x)
            || append_if_non_zero<nanoseconds>(s, x);
      }
      return s;
    }
  }  // namespace detail
}  // namespace libsemigroups
