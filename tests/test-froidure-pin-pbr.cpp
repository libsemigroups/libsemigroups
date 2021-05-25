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

#include "catch.hpp"  // for REQUIRE, AssertionHandler, REQUIRE_THROWS_AS
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin
#include "libsemigroups/pbr.hpp"           // for PBR
#include "test-main.hpp"                   // for LIBSEMIGROUPS_TEST_CASE

namespace libsemigroups {
  struct LibsemigroupsException;
}

namespace libsemigroups {

  constexpr bool REPORT = false;

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<PBR>",
                          "056",
                          "example 1",
                          "[quick][froidure-pin][pbr]") {
    auto             rg   = ReportGuard(REPORT);
    std::vector<PBR> gens = {PBR({{3, 5},
                                  {0, 1, 2, 3, 4, 5},
                                  {0, 2, 3, 4, 5},
                                  {0, 1, 2, 3, 5},
                                  {0, 2, 5},
                                  {1, 2, 3, 4, 5}}),
                             PBR({{0, 3, 4, 5},
                                  {2, 4, 5},
                                  {1, 2, 5},
                                  {2, 3, 4, 5},
                                  {2, 3, 4, 5},
                                  {1, 2, 4}}),
                             PBR({{0, 3, 4, 5},
                                  {2, 4, 5},
                                  {1, 2, 5},
                                  {2, 3, 4, 5},
                                  {2, 3, 4, 5},
                                  {1, 2, 4}})};
    FroidurePin<PBR> S(gens);

    S.reserve(4);

    REQUIRE(S.size() == 4);
    REQUIRE(S.number_of_idempotents() == 2);
    size_t pos = 0;

    for (auto it = S.cbegin(); it < S.cend(); ++it) {
      REQUIRE(S.position(*it) == pos);
      pos++;
    }
    S.add_generators({PBR({{3, 4, 5},
                           {2, 4, 5},
                           {1, 2, 4},
                           {0, 3, 5},
                           {1, 2, 3, 5},
                           {1, 2, 3}})});
    REQUIRE(S.size() == 6);
    S.closure({PBR({{3, 4, 5},
                    {2, 4, 5},
                    {1, 2, 4},
                    {0, 3, 5},
                    {1, 2, 3, 5},
                    {1, 2, 3}})});
    REQUIRE(S.size() == 6);
    REQUIRE(S.minimal_factorisation(PBR({{3, 5},
                                         {0, 1, 2, 3, 4, 5},
                                         {0, 2, 3, 4, 5},
                                         {0, 1, 2, 3, 5},
                                         {0, 2, 5},
                                         {1, 2, 3, 4, 5}})
                                    * PBR({{3, 4, 5},
                                           {2, 4, 5},
                                           {1, 2, 4},
                                           {0, 3, 5},
                                           {1, 2, 3, 5},
                                           {1, 2, 3}}))
            == word_type({0, 0}));
    REQUIRE(S.minimal_factorisation(5) == word_type({3, 3}));
    REQUIRE(S.at(5)
            == PBR({{3, 4, 5},
                    {2, 4, 5},
                    {1, 2, 4},
                    {0, 3, 5},
                    {1, 2, 3, 5},
                    {1, 2, 3}})
                   * PBR({{3, 4, 5},
                          {2, 4, 5},
                          {1, 2, 4},
                          {0, 3, 5},
                          {1, 2, 3, 5},
                          {1, 2, 3}}));
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

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<PBR>",
                          "057",
                          "example 2",
                          "[quick][froidure-pin][pbr]") {
    auto rg = ReportGuard(REPORT);

    FroidurePin<PBR> S;
    S.add_generator(PBR({{1}, {4}, {3}, {1}, {0, 2}, {0, 3, 4, 5}}));
    S.add_generator(
        PBR({{1, 2}, {0, 1}, {0, 2, 3}, {0, 1, 2}, {3}, {0, 3, 4, 5}}));

    REQUIRE(S.size() == 30);
    REQUIRE(S.degree() == 3);
    REQUIRE(S.number_of_idempotents() == 22);
    REQUIRE(S.number_of_generators() == 2);
    REQUIRE(S.number_of_rules() == 11);

    REQUIRE(S[0] == S.generator(0));
    REQUIRE(S.position(S.generator(0)) == 0);
    REQUIRE(S.contains(S.generator(0)));
    REQUIRE(S[1] == S.generator(1));
    REQUIRE(S.position(S.generator(1)) == 1);
    REQUIRE(S.contains(S.generator(1)));

    PBR x({{}, {}, {}, {}, {}, {}});
    REQUIRE(S.position(x) == UNDEFINED);
    REQUIRE(!S.contains(x));
    x.product_inplace(S.generator(1), S.generator(1));
    REQUIRE(S.position(x) == 5);
    REQUIRE(S.contains(x));
  }
}  // namespace libsemigroups
