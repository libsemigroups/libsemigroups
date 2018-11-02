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

// The purpose of this file is to test the Runner class.

#include "libsemigroups.tests.hpp"  // for LIBSEMIGROUPS_TEST_CASE
#include "runner.hpp"               // for Race

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

    LIBSEMIGROUPS_TEST_CASE("Runner", "001", "run_for", "[quick]") {
      TestRunner tr;
      tr.run_for(std::chrono::milliseconds(10));
      REQUIRE(tr.finished());
      REQUIRE(tr.stopped());
      REQUIRE(!tr.dead());
    }
  }  // namespace internal
}  // namespace libsemigroups
