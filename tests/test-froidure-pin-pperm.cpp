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
#include <cstdint>  // for uint16_t
#include <vector>   // for vector

#include "catch.hpp"                       // for LIBSEMIGROUPS_TEST_CASE
#include "libsemigroups/element.hpp"       // for PartialPerm
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin<>::element_index_type
#include "test-main.hpp"

namespace libsemigroups {
  // Forward declaration
  struct LibsemigroupsException;

  constexpr bool REPORT = false;

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "106",
                          "(partial perm)",
                          "[quick][froidure-pin][pperm]") {
    auto                               rg = ReportGuard(REPORT);
    std::vector<PartialPerm<uint16_t>> gens
        = {PartialPerm<uint16_t>({0, 3, 4, 5}, {1, 0, 3, 2}, 6),
           PartialPerm<uint16_t>({1, 2, 3}, {0, 5, 2}, 6),
           PartialPerm<uint16_t>({0, 2, 3, 4, 5}, {5, 2, 3, 0, 1}, 6)};

    FroidurePin<PartialPerm<uint16_t>> S(gens);

    S.reserve(102);

    REQUIRE(S.size() == 102);
    REQUIRE(S.nr_idempotents() == 8);
    size_t pos = 0;

    for (auto it = S.cbegin(); it < S.cend(); ++it) {
      REQUIRE(S.position(*it) == pos);
      pos++;
    }

    S.add_generators({PartialPerm<uint16_t>({0, 1, 2}, {3, 4, 5}, 6)});
    REQUIRE(S.size() == 396);
    S.closure({PartialPerm<uint16_t>({0, 1, 2}, {3, 4, 5}, 6)});
    REQUIRE(S.size() == 396);
    REQUIRE(S.minimal_factorisation(
                PartialPerm<uint16_t>({0, 1, 2}, {3, 4, 5}, 6)
                * PartialPerm<uint16_t>({0, 2, 3, 4, 5}, {5, 2, 3, 0, 1}, 6))
            == word_type({3, 2}));
    REQUIRE(S.minimal_factorisation(10) == word_type({2, 1}));
    REQUIRE(S.at(10) == PartialPerm<uint16_t>({2, 3, 5}, {5, 2, 0}, 6));
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
