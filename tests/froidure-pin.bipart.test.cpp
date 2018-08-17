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
#include "froidure-pin.hpp"

#define SEMIGROUPS_REPORT false

using namespace libsemigroups;

TEST_CASE("FroidurePin of Bipartitions 01: non-pointer Bipartitions",
          "[quick][semigroup][bipart][finite][01]") {
  std::vector<Bipartition> gens = {
      Bipartition({0, 1, 2, 1, 0, 2, 1, 0, 2, 2, 0, 0, 2, 0, 3, 4, 4, 1, 3, 0}),
      Bipartition({0, 1, 1, 1, 1, 2, 3, 2, 4, 5, 5, 2, 4, 2, 1, 1, 1, 2, 3, 2}),
      Bipartition(
          {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0})};

  FroidurePin<Bipartition> S(gens);

  S.reserve(10);
  REPORTER.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.size() == 10);
  REQUIRE(S.nr_idempotents() == 6);

  size_t pos = 0;
  for (auto it = S.cbegin(); it < S.cend(); ++it) {
    REQUIRE(S.position(*it) == pos);
    pos++;
  }

  S.add_generators({Bipartition(
      {0, 1, 2, 1, 1, 3, 1, 4, 2, 3, 1, 0, 3, 2, 3, 5, 4, 1, 3, 0})});

  REQUIRE(S.size() == 21);
  S.closure({Bipartition(
      {0, 1, 2, 1, 1, 3, 1, 4, 2, 3, 1, 0, 3, 2, 3, 5, 4, 1, 3, 0})});
  REQUIRE(S.size() == 21);
  REQUIRE(S.minimal_factorisation(Bipartition({0, 1, 2, 1, 0, 2, 1, 0, 2, 2,
                                               0, 0, 2, 0, 3, 4, 4, 1, 3, 0})
                                  * Bipartition({0, 1, 2, 1, 1, 3, 1, 4, 2, 3,
                                                 1, 0, 3, 2, 3, 5, 4, 1, 3, 0})
                                  * Bipartition({0, 1, 1, 1, 1, 2, 3, 2, 4, 5,
                                                 5, 2, 4, 2, 1, 1, 1, 2, 3, 2}))
          == word_type({0, 3, 1}));
  REQUIRE(S.minimal_factorisation(11) == word_type({0, 3}));
  REQUIRE(S.at(11)
          == Bipartition(
                 {0, 1, 2, 1, 0, 2, 1, 0, 2, 2, 0, 0, 2, 0, 3, 4, 4, 1, 3, 0})
                 * Bipartition({0, 1, 2, 1, 1, 3, 1, 4, 2, 3,
                                1, 0, 3, 2, 3, 5, 4, 1, 3, 0}));
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
}
