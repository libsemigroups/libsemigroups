// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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

#include <chrono>   // for nanoseconds, milliseconds, durati...
#include <sstream>  // for ostringstream
#include <string>   // for operator==
#include <thread>   // for sleep_for

#include "catch_amalgamated.hpp"  // for LIBSEMIGROUPS_TEST_CASE
#include "test-main.hpp"

#include "libsemigroups/detail/timer.hpp"  // for Timer, operator<<

namespace libsemigroups {
  namespace detail {
    LIBSEMIGROUPS_TEST_CASE("Timer",
                            "001",
                            "string method (1 argument)",
                            "[quick]") {
      Timer                    t;
      std::chrono::nanoseconds ns(1);
      REQUIRE(string_time(ns) == "1ns");
      ns *= 10;
      REQUIRE(string_time(ns) == "10ns");
      ns *= 10;
      REQUIRE(string_time(ns) == "100ns");
      ns *= 10;
      REQUIRE(string_time(ns) == "1µs");
      ns *= 10;
      REQUIRE(string_time(ns) == "10µs");
      ns *= 10;
      REQUIRE(string_time(ns) == "100µs");
      ns *= 10;
      REQUIRE(string_time(ns) == "1ms");
      ns *= 10;
      REQUIRE(string_time(ns) == "10ms");
      ns *= 10;
      REQUIRE(string_time(ns) == "100ms");
      ns *= 10;
      REQUIRE(string_time(ns) == "1.000s");
      ns *= 10;
      REQUIRE(string_time(ns) == "10.000s");
      ns *= 10;
      REQUIRE(string_time(ns) == "1min40s");
      ns *= 10;
      REQUIRE(string_time(ns) == "16min40s");
      ns *= 10;
      REQUIRE(string_time(ns) == "2h46min40s");
      ns *= 10;
      REQUIRE(string_time(ns) == "1d3h46min40s");
      ns *= 10;
      REQUIRE(string_time(ns) == "1w4d13h46min40s");
      ns *= 10;
      REQUIRE(string_time(ns) == "3mon3w3d10h19min22s");
      ns *= 10;
      REQUIRE(string_time(ns) == "3y2mon19h20min52s");
    }

    LIBSEMIGROUPS_TEST_CASE("Timer",
                            "002",
                            "string method (0 arguments)",
                            "[quick]") {
      Timer t;
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      t.string();  // Can't really test this
    }

    LIBSEMIGROUPS_TEST_CASE("Timer", "003", "reset/elapsed method", "[quick]") {
      Timer t;
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      auto e
          = std::chrono::duration_cast<std::chrono::milliseconds>(t.elapsed());
      REQUIRE(e.count() > 0);
      REQUIRE(e.count() < 1000);
      t.reset();
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      e = std::chrono::duration_cast<std::chrono::milliseconds>(t.elapsed());
      REQUIRE(e.count() > 0);
      REQUIRE(e.count() < 1000);
    }

    LIBSEMIGROUPS_TEST_CASE("Timer", "004", "operator<<", "[quick]") {
      std::ostringstream os;
      Timer              t;
      os << t;
    }
  }  // namespace detail
}  // namespace libsemigroups
