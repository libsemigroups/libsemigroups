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

// The purpose of this file is to test the Race class.

#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE
#include "race.hpp"                 // for Race

namespace libsemigroups {
  struct LibsemigroupsException;

  constexpr bool REPORT = false;
  namespace internal {

    class TestRunner : public Runner {
     public:
      void run() {
        if (finished()) {
          return;
        }
        while (!stopped()) {
        }
        set_finished(true);
      }
    };

    LIBSEMIGROUPS_TEST_CASE("Race", "001", "run_for", "[quick]") {
      Race rc;
      rc.set_max_threads(1);
      rc.add_runner(new TestRunner());
      rc.run_for(std::chrono::milliseconds(10));
      rc.run_until([]() -> bool { return true; });
      REQUIRE(rc.winner() != nullptr);
    }

    LIBSEMIGROUPS_TEST_CASE("Race", "002", "run_until", "[quick]") {
      Race rc;
      rc.add_runner(new TestRunner());
      size_t nr  = 0;
      auto   foo = [&nr]() -> bool { return ++nr == 2; };
      rc.run_until(foo, std::chrono::milliseconds(10));
      REQUIRE(rc.winner() != nullptr);
    }

    LIBSEMIGROUPS_TEST_CASE("Race", "003", "exceptions", "[quick]") {
      Race rc;
      REQUIRE_THROWS_AS(rc.set_max_threads(0), LibsemigroupsException);
      REQUIRE_THROWS_AS(rc.run_for(std::chrono::milliseconds(10)),
                        LibsemigroupsException);
      REQUIRE_THROWS_AS(rc.run_until([]() -> bool { return true; }),
                        LibsemigroupsException);
      REQUIRE_THROWS_AS(rc.run(), LibsemigroupsException);

      rc.add_runner(new TestRunner());
      rc.run_for(std::chrono::milliseconds(10));
      REQUIRE(rc.winner() != nullptr);
      auto* tr = new TestRunner();
      REQUIRE_THROWS_AS(rc.add_runner(tr), LibsemigroupsException);
      delete tr;
    }
  }  // namespace internal
}  // namespace libsemigroups
