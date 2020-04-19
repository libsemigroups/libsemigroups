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
#include "libsemigroups/element.hpp"       // for PBR
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin
#include "test-main.hpp"                   // for LIBSEMIGROUPS_TEST_CASE

namespace libsemigroups {
  struct LibsemigroupsException;
}

namespace libsemigroups {

  constexpr bool REPORT = false;

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "105",
                          "(pbrs)",
                          "[quick][froidure-pin][pbr]") {
    auto             rg   = ReportGuard(REPORT);
    std::vector<PBR> gens = {PBR({{5, 3},
                                  {5, 4, 3, 0, 1, 2},
                                  {5, 4, 3, 0, 2},
                                  {5, 3, 0, 1, 2},
                                  {5, 0, 2},
                                  {5, 4, 3, 1, 2}}),
                             PBR({{5, 4, 3, 0},
                                  {5, 4, 2},
                                  {5, 1, 2},
                                  {5, 4, 3, 2},
                                  {5, 4, 3, 2},
                                  {4, 1, 2}}),
                             PBR({{5, 4, 3, 0},
                                  {5, 4, 2},
                                  {5, 1, 2},
                                  {5, 4, 3, 2},
                                  {5, 4, 3, 2},
                                  {4, 1, 2}})};
    FroidurePin<PBR> S(gens);

    S.reserve(4);

    REQUIRE(S.size() == 4);
    REQUIRE(S.nr_idempotents() == 2);
    size_t pos = 0;

    for (auto it = S.cbegin(); it < S.cend(); ++it) {
      REQUIRE(S.position(*it) == pos);
      pos++;
    }
    S.add_generators({PBR({{5, 4, 3},
                           {5, 4, 2},
                           {4, 2, 1},
                           {5, 3, 0},
                           {5, 3, 2, 1},
                           {3, 1, 2}})});
    REQUIRE(S.size() == 6);
    S.closure({PBR({{5, 4, 3},
                    {5, 4, 2},
                    {4, 2, 1},
                    {5, 3, 0},
                    {5, 3, 2, 1},
                    {3, 1, 2}})});
    REQUIRE(S.size() == 6);
    REQUIRE(S.minimal_factorisation(PBR({{5, 3},
                                         {5, 4, 3, 0, 1, 2},
                                         {5, 4, 3, 0, 2},
                                         {5, 3, 0, 1, 2},
                                         {5, 0, 2},
                                         {5, 4, 3, 1, 2}})
                                    * PBR({{5, 4, 3},
                                           {5, 4, 2},
                                           {4, 2, 1},
                                           {5, 3, 0},
                                           {5, 3, 2, 1},
                                           {3, 1, 2}}))
            == word_type({0, 0}));
    REQUIRE(S.minimal_factorisation(5) == word_type({3, 3}));
    REQUIRE(S.at(5)
            == PBR({{5, 4, 3},
                    {5, 4, 2},
                    {4, 2, 1},
                    {5, 3, 0},
                    {5, 3, 2, 1},
                    {3, 1, 2}})
                   * PBR({{5, 4, 3},
                          {5, 4, 2},
                          {4, 2, 1},
                          {5, 3, 0},
                          {5, 3, 2, 1},
                          {3, 1, 2}}));
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
