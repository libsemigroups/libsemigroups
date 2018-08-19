// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2017 James D. Mitchell
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

#include <chrono>
#include <iostream>
#include <thread>

#include "internal/timer.hpp"
#include "libsemigroups.tests.hpp"

namespace libsemigroups {

  LIBSEMIGROUPS_TEST_CASE("Timer",
                          "001",
                          "string method (1 argument)",
                          "[quick]") {
    Timer                    t;
    std::chrono::nanoseconds ns(1);
    REQUIRE(t.string(ns) == "1ns");
    ns *= 10;
    REQUIRE(t.string(ns) == "10ns");
    ns *= 10;
    REQUIRE(t.string(ns) == "100ns");
    ns *= 10;
    REQUIRE(t.string(ns) == "1000ns");
    ns *= 10;
    REQUIRE(t.string(ns) == "10μs");
    ns *= 10;
    REQUIRE(t.string(ns) == "100μs");
    ns *= 10;
    REQUIRE(t.string(ns) == "1000μs");
    ns *= 10;
    REQUIRE(t.string(ns) == "10ms");
    ns *= 10;
    REQUIRE(t.string(ns) == "100ms");
    ns *= 10;
    REQUIRE(t.string(ns) == "1000ms");
    ns *= 10;
    REQUIRE(t.string(ns) == "10000ms");
    ns *= 10;
    REQUIRE(t.string(ns) == "1m40s");
    ns *= 10;
    REQUIRE(t.string(ns) == "16m40s");
    ns *= 10;
    REQUIRE(t.string(ns) == "2h46m");
    ns *= 10;
    REQUIRE(t.string(ns) == "27h46m");
    ns *= 10;
    REQUIRE(t.string(ns) == "277h46m");
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
    auto e = std::chrono::duration_cast<std::chrono::milliseconds>(t.elapsed());
    REQUIRE(e.count() >= 5);
    REQUIRE(e.count() < 20);
    t.reset();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    e = std::chrono::duration_cast<std::chrono::milliseconds>(t.elapsed());
    REQUIRE(e.count() >= 5);
    REQUIRE(e.count() < 20);
  }

  LIBSEMIGROUPS_TEST_CASE("Timer", "004", "operator<<", "[quick]") {
    std::ostringstream os;
    Timer              t;
    os << t;
  }
}  // namespace libsemigroups
