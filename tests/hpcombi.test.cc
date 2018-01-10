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

#include "../extern/HPCombi/include/perm16.hpp"
#include "../src/semigroups.h"
#include "catch.hpp"

using namespace libsemigroups;
using namespace HPCombi;

TEST_CASE("HPCombi 01: Transf16", "[quick][hpcombi][finite][01]") {
  Semigroup<Transf16, std::hash<Transf16>, std::equal_to<Transf16>> S(
      {Transf16({1, 2, 0})});
  S.set_report(true);
  REQUIRE(S.size() == 3);
  REQUIRE(S.nridempotents() == 1);
}

TEST_CASE("HPCombi 02: Transf16", "[quick][hpcombi][finite][02]") {
  Semigroup<Transf16, std::hash<Transf16>, std::equal_to<Transf16>> S(
      {Transf16({1, 7, 2, 6, 0, 4, 1, 5}),
       Transf16({2, 4, 6, 1, 4, 5, 2, 7}),
       Transf16({3, 0, 7, 2, 4, 6, 2, 4}),
       Transf16({3, 2, 3, 4, 5, 3, 0, 1}),
       Transf16({4, 3, 7, 7, 4, 5, 0, 4}),
       Transf16({5, 6, 3, 0, 3, 0, 5, 1}),
       Transf16({6, 0, 1, 1, 1, 6, 3, 4}),
       Transf16({7, 7, 4, 0, 6, 4, 1, 7})});
  S.reserve(597369 * 2);
  S.set_report(false);
  REQUIRE(S.size() == 597369);
}

TEST_CASE("HPCombi 03: full transformation monoid 8",
          "[extreme][hpcombi][finite][03]") {
  Semigroup<Transf16, std::hash<Transf16>, std::equal_to<Transf16>> S(
      {Transf16({1, 2, 3, 4, 5, 6, 7, 0}),
       Transf16({1, 0, 2, 3, 4, 5, 6, 7}),
       Transf16({0, 1, 2, 3, 4, 5, 6, 0})});
  S.reserve(std::pow(8, 8));
  S.set_report(true);
  REQUIRE(S.size() == 16777216);
  S.set_report(false);
}
