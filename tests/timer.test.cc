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

#include "catch.hpp"

#include <chrono>
#include <iostream>
#include <thread>

#include "../src/timer.h"

using namespace libsemigroups;

TEST_CASE("Timer 01: string method (1 argument)", "[quick][timer][01]") {
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
  REQUIRE(t.string(ns) == "1m 40s");
  ns *= 10;
  REQUIRE(t.string(ns) == "16m 40s");
  ns *= 10;
  REQUIRE(t.string(ns) == "2h 46m");
  ns *= 10;
  REQUIRE(t.string(ns) == "27h 46m");
  ns *= 10;
  REQUIRE(t.string(ns) == "277h 46m");
}

TEST_CASE("Timer 02: string method (0 arguments)", "[quick][timer][02]") {
  Timer t;
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  t.string();  // Can't really test this
}

TEST_CASE("Timer 03: reset/elapsed method", "[quick][timer][03]") {
  Timer t;
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  auto e = std::chrono::duration_cast<std::chrono::milliseconds>(t.elapsed());
  REQUIRE(e.count() >= 10);
  REQUIRE(e.count() < 20);
  t.reset();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  e = std::chrono::duration_cast<std::chrono::milliseconds>(t.elapsed());
  REQUIRE(e.count() >= 10);
  REQUIRE(e.count() < 20);
}

TEST_CASE("Timer 04: operator<<", "[quick][timer][04]") {
  std::ostringstream os;
  Timer              t;
  os << t;
}
