//
// Semigroups++ - C/C++ library for computing with semigroups and monoids
// Copyright (C) 2016 James D. Mitchell
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

#include "../semiring.h"
#include "catch.hpp"

using namespace libsemigroups;

TEST_CASE("Semiring 01: threshold/period [undefined]", "[quick][semiring]") {
  Semiring* sr = new MaxPlusSemiring();

  REQUIRE(sr->threshold() == -1);  // UNDEFINED
  REQUIRE(sr->period() == -1);     // UNDEFINED
  delete sr;
}

TEST_CASE("Semiring 02: threshold/period [NaturalSemiring]",
          "[quick][semiring") {
  Semiring* sr = new NaturalSemiring(10, 314);

  REQUIRE(sr->threshold() == 10);
  REQUIRE(sr->period() == 314);
  delete sr;
}
