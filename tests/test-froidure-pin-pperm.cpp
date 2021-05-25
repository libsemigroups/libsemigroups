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

#include "catch.hpp"      // for REQUIRE
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin
#include "libsemigroups/transf.hpp"        // for PPerm

namespace libsemigroups {
  // Forward declaration
  struct LibsemigroupsException;

  constexpr bool REPORT = false;

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<PPerm<>>",
                          "058",
                          "",
                          "[quick][froidure-pin][pperm]") {
    auto rg = ReportGuard(REPORT);

    FroidurePin<PPerm<>> S;
    S.add_generator(PPerm<>({0, 3, 4, 5}, {1, 0, 3, 2}, 6));
    S.add_generator(PPerm<>({1, 2, 3}, {0, 5, 2}, 6));
    S.add_generator(PPerm<>({0, 2, 3, 4, 5}, {5, 2, 3, 0, 1}, 6));

    S.reserve(102);

    REQUIRE(S.size() == 102);
    REQUIRE(S.number_of_idempotents() == 8);
    size_t pos = 0;

    for (auto it = S.cbegin(); it < S.cend(); ++it) {
      REQUIRE(S.position(*it) == pos);
      pos++;
    }

    S.add_generators({PPerm<>({0, 1, 2}, {3, 4, 5}, 6)});
    REQUIRE(S.size() == 396);
    S.closure({PPerm<>({0, 1, 2}, {3, 4, 5}, 6)});
    REQUIRE(S.size() == 396);
    REQUIRE(
        S.minimal_factorisation(PPerm<>({0, 1, 2}, {3, 4, 5}, 6)
                                * PPerm<>({0, 2, 3, 4, 5}, {5, 2, 3, 0, 1}, 6))
        == word_type({3, 2}));
    REQUIRE(S.minimal_factorisation(10) == word_type({2, 1}));
    REQUIRE(S.at(10) == PPerm<>({2, 3, 5}, {5, 2, 0}, 6));
    REQUIRE_THROWS_AS(S.minimal_factorisation(1000000000),
                      LibsemigroupsException);
    pos = 0;
    for (auto it = S.cbegin_idempotents(); it < S.cend_idempotents(); ++it) {
      REQUIRE(*it * *it == *it);
      pos++;
    }
    REQUIRE(pos == S.number_of_idempotents());
    for (auto it = S.cbegin_sorted() + 1; it < S.cend_sorted(); ++it) {
      REQUIRE(*(it - 1) < *it);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<PPerm<>>",
                          "059",
                          "",
                          "[quick][froidure-pin][pperm]") {
    auto                 rg = ReportGuard(REPORT);
    FroidurePin<PPerm<>> S;
    S.add_generator(PPerm<>({0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 11));
    S.add_generator(PPerm<>({4, 5, 0}, {10, 0, 1}, 11));

    REQUIRE(S.size() == 22);
    REQUIRE(S.degree() == 11);
    REQUIRE(S.number_of_idempotents() == 1);
    REQUIRE(S.number_of_generators() == 2);
    REQUIRE(S.number_of_rules() == 9);

    REQUIRE(S[0] == PPerm<>({0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 11));

    REQUIRE(S[1] == PPerm<>({4, 5, 0}, {10, 0, 1}, 11));

    auto x = PPerm<>({}, {}, 11);
    REQUIRE(S.position(x) == 10);
    REQUIRE(S.contains(x));

    x = PPerm<>({}, {}, 9);
    REQUIRE(S.position(x) == UNDEFINED);
    REQUIRE(!S.contains(x));

    x = PPerm<>({0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 11);
    REQUIRE(S.position(x) == 0);
    REQUIRE(S.contains(x));

    auto y = PPerm<>({4, 5, 0}, {10, 0, 1}, 11);
    REQUIRE(S.position(y) == 1);
    REQUIRE(S.contains(y));

    y.product_inplace(x, x);
    REQUIRE(S.position(y) == 2);
    REQUIRE(S.contains(y));

    REQUIRE(y == S[2]);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<PPerm<>>",
                          "060",
                          "exceptions: add_generator(s)",
                          "[quick][froidure-pin][pperm]") {
    FroidurePin<PPerm<>> S;
    S.add_generator(PPerm<>({0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 10));

    REQUIRE_THROWS_AS(S.add_generator(PPerm<>({4, 5, 0}, {10, 0, 1}, 11)),
                      LibsemigroupsException);

    FroidurePin<PPerm<>> U;
    U.add_generator(PPerm<>({0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 11));
    U.add_generator(PPerm<>({4, 5, 0}, {10, 0, 1}, 11));

    std::vector<PPerm<>> additional_gens_2_1
        = {PPerm<>({0, 1, 2, 3, 5, 6, 9}, {2, 7, 5, 1, 4, 3, 9}, 11),
           PPerm<>({2, 5, 1}, {6, 0, 3}, 11)};
    std::vector<PPerm<>> additional_gens_2_2
        = {PPerm<>({0, 1, 2, 3, 5, 6, 9}, {2, 7, 5, 1, 4, 3, 9}, 11),
           PPerm<>({2, 5, 1}, {6, 0, 3}, 12)};

    REQUIRE_NOTHROW(U.add_generators(additional_gens_2_1));
    REQUIRE_THROWS_AS(U.add_generators(additional_gens_2_2),
                      LibsemigroupsException);
  }
}  // namespace libsemigroups
