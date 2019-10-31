//
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

#include "catch.hpp"     // for LIBSEMIGROUPS_TEST_CASE
#include "semiring.hpp"  // for BooleanSemiring, NaturalSemiring
#include "test-main.hpp"

namespace libsemigroups {

  LIBSEMIGROUPS_TEST_CASE("Semiring",
                          "001",
                          "threshold/period [NaturalSemiring]",
                          "[quick]") {
    NaturalSemiring* sr = new NaturalSemiring(10, 314);

    REQUIRE(sr->threshold() == 10);
    REQUIRE(sr->period() == 314);
    delete sr;
  }

  LIBSEMIGROUPS_TEST_CASE("Semiring",
                          "002",
                          "threshold 0 [NaturalSemiring]",
                          "[quick]") {
    NaturalSemiring* sr = new NaturalSemiring(0, 7);

    REQUIRE(sr->threshold() == 0);
    REQUIRE(sr->period() == 7);
    REQUIRE(sr->plus(5, 6) == 4);
    REQUIRE(sr->prod(5, 6) == 2);

    delete sr;
  }

  LIBSEMIGROUPS_TEST_CASE("Semiring", "003", "boolean semiring", "[quick]") {
    BooleanSemiring* sr = new BooleanSemiring();

    REQUIRE(sr->one());
    REQUIRE(!sr->zero());
    REQUIRE(sr->prod(true, true));
    REQUIRE(!sr->prod(true, false));
    REQUIRE(!sr->prod(false, true));
    REQUIRE(!sr->prod(false, false));
    REQUIRE(sr->plus(true, true));
    REQUIRE(sr->plus(true, false));
    REQUIRE(sr->plus(false, true));
    REQUIRE(!sr->plus(false, false));

    delete sr;
  }
}  // namespace libsemigroups
