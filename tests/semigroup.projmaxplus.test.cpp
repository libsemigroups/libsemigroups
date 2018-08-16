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

#include "catch.hpp"
#include "element.hpp"
#include "semigroup.hpp"

#define SEMIGROUPS_REPORT false

using namespace libsemigroups;

TEST_CASE("Semigroup of ProjectiveMaxPlusMatrices 01: non-pointer "
          "ProjectiveMaxPlusMatrix",
          "[quick][semigroup][matrix][finite][01]") {
  Semiring<int64_t>*                   sr = new MaxPlusSemiring();
  std::vector<ProjectiveMaxPlusMatrix> gens
      = {ProjectiveMaxPlusMatrix({0, 1, 2, 3, 4, 1, 2, 1, 1}, sr),
         ProjectiveMaxPlusMatrix({0, 1, 1, 1, 1, 1, 0, 0, 0}, sr),
         ProjectiveMaxPlusMatrix({0, 1, 1, 0, 0, 1, 1, 0, 0}, sr)};
  Semigroup<ProjectiveMaxPlusMatrix> S(gens);

  S.reserve(142);
  REPORTER.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.size() == 142);
  REQUIRE(S.nr_idempotents() == 90);
  size_t pos = 0;

  for (auto it = S.cbegin(); it < S.cend(); ++it) {
    REQUIRE(S.position(*it) == pos);
    pos++;
  }

  S.add_generators({ProjectiveMaxPlusMatrix({1, 0, 0, 1, 0, 1, 0, 1, 0}, sr)});
  REQUIRE(S.size() == 223);
  S.closure({ProjectiveMaxPlusMatrix({1, 0, 0, 1, 0, 1, 0, 1, 0}, sr)});
  REQUIRE(S.size() == 223);
  REQUIRE(S.minimal_factorisation(
              ProjectiveMaxPlusMatrix({1, 0, 0, 1, 0, 1, 0, 1, 0}, sr)
              * ProjectiveMaxPlusMatrix({0, 1, 2, 3, 4, 1, 2, 1, 1}, sr))
          == word_type({3, 0}));
  REQUIRE_THROWS_AS(S.minimal_factorisation(1000000000),
                    LibsemigroupsException);
  pos = 0;
  for (auto it = S.cbegin_idempotents(); it < S.cend_idempotents(); ++it) {
    REQUIRE(*it * *it == *it);
    pos++;
  }
  REQUIRE(pos == S.nr_idempotents());
  for (auto it = S.cbegin_sorted() + 1; it < S.cend_sorted(); ++it) {
    REQUIRE(*(it - 1) < *it);
  }
  delete sr;
}
