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

#include <cstddef>  // for size_t
#include <vector>   // for vector

#include "catch.hpp"                       // for LIBSEMIGROUPS_TEST_CASE
#include "libsemigroups/element.hpp"       // for BooleanMat
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin<>::element_index_type
#include "libsemigroups/semiring.hpp"      // for BooleanSemiring
#include "test-main.hpp"

namespace libsemigroups {
  // Forward declaration
  struct LibsemigroupsException;

  bool constexpr REPORT = false;

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "016",
                          "non-pointer BooleanMat",
                          "[quick][froidure-pin][boolmat][booleanmat]") {
    std::vector<BooleanMat> gens
        = {BooleanMat({0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0}),
           BooleanMat({0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1}),
           BooleanMat({0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1})};

    FroidurePin<BooleanMat> S(gens);

    S.reserve(26);
    auto rg = ReportGuard(REPORT);

    REQUIRE(S.size() == 26);
    REQUIRE(S.nr_idempotents() == 4);
    size_t pos = 0;

    for (auto it = S.cbegin(); it < S.cend(); ++it) {
      REQUIRE(S.position(*it) == pos);
      pos++;
    }

    S.add_generators(
        {BooleanMat({1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0})});
    REQUIRE(S.size() == 29);
    S.closure({BooleanMat({1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0})});
    REQUIRE(S.size() == 29);
    REQUIRE(S.minimal_factorisation(
                BooleanMat({1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0})
                * BooleanMat({0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0}))
            == word_type({3, 0}));
    REQUIRE(S.minimal_factorisation(28) == word_type({3, 0}));
    REQUIRE(
        S.at(28)
        == BooleanMat({1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0})
               * BooleanMat({0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0}));
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
}  // namespace libsemigroups
