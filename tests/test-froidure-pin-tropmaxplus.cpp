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
#include <cstdint>  // for int64_t
#include <vector>   // for vector

#include "catch.hpp"                           // for LIBSEMIGROUPS_TEST_CASE
#include "libsemigroups/bruidhinn-traits.hpp"  // for detail::BruidhinnTraits
#include "libsemigroups/element.hpp"           // for MatrixOverSemiring
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin, FroidurePin<>::eleme...
#include "libsemigroups/semiring.hpp"  // for TropicalMaxPlusSemiring, Semiring
#include "test-main.hpp"

namespace libsemigroups {
  // Forward declaration
  struct LibsemigroupsException;

  constexpr bool REPORT = false;

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "125",
                          "(tropical max-plus semiring matrices)",
                          "[quick][froidure-pin][tropmaxplus]") {
    Semiring<int64_t>* sr = new TropicalMaxPlusSemiring(9);
    std::vector<MatrixOverSemiring<int64_t>> gens
        = {MatrixOverSemiring<int64_t>({{1, 3}, {2, 1}}, sr),
           MatrixOverSemiring<int64_t>({{2, 1}, {4, 0}}, sr)};
    FroidurePin<MatrixOverSemiring<int64_t>> S
        = FroidurePin<MatrixOverSemiring<int64_t>>(gens);

    S.reserve(4);
    auto rg = ReportGuard(REPORT);

    REQUIRE(S.size() == 20);
    REQUIRE(S.nr_idempotents() == 1);
    size_t pos = 0;

    for (auto it = S.cbegin(); it < S.cend(); ++it) {
      REQUIRE(S.position(*it) == pos);
      pos++;
    }
    S.add_generators({MatrixOverSemiring<int64_t>({{1, 1}, {0, 2}}, sr)});
    REQUIRE(S.size() == 73);
    S.closure({MatrixOverSemiring<int64_t>({{1, 1}, {0, 2}}, sr)});
    REQUIRE(S.size() == 73);
    REQUIRE(S.minimal_factorisation(
                MatrixOverSemiring<int64_t>({{1, 1}, {0, 2}}, sr)
                * MatrixOverSemiring<int64_t>({{2, 1}, {4, 0}}, sr))
            == word_type({2, 1}));
    REQUIRE(S.minimal_factorisation(52) == word_type({0, 2, 2, 1}));
    REQUIRE(S.at(52) == MatrixOverSemiring<int64_t>({{9, 7}, {9, 5}}, sr));
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
}  // namespace libsemigroups
