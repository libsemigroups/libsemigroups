// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020 Finn Smith
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

#include "bmat-data.hpp"          // for clark_gens
#include "catch_amalgamated.hpp"  // for REQUIRE
#include "test-main.hpp"          // FOR LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/bmat-adapters.hpp"  // for BMat adapters
#include "libsemigroups/konieczny.hpp"      // for Konieczny
#include "libsemigroups/matrix.hpp"         // for BMat

namespace libsemigroups {

  constexpr bool REPORT = false;

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("Konieczny",
                                   "000",
                                   "4x4 boolean matrix semigroup (size 26)",
                                   "[quick][bmat]",
                                   BMat<>,
                                   BMat<4>) {
    auto                  rg = ReportGuard(REPORT);
    std::vector<TestType> gens
        = {TestType({{0, 1, 0, 1}, {1, 0, 0, 0}, {0, 1, 1, 1}, {0, 1, 1, 0}}),
           TestType({{0, 1, 1, 1}, {1, 1, 0, 0}, {0, 0, 0, 0}, {1, 1, 1, 1}}),
           TestType({{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 1, 1, 1}, {1, 1, 1, 1}})};

    Konieczny<TestType> S(gens);
    REQUIRE(S.size() == 26);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("Konieczny",
                                   "002",
                                   "4x4 boolean matrix semigroup (size 415)",
                                   "[quick][bmat][no-valgrind]",
                                   BMat<>,
                                   BMat<4>) {
    auto                  rg = ReportGuard(REPORT);
    std::vector<TestType> gens
        = {TestType({{1, 0, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}, {0, 1, 0, 0}}),
           TestType({{1, 0, 0, 1}, {1, 0, 0, 1}, {1, 1, 1, 1}, {0, 1, 1, 0}}),
           TestType({{1, 0, 1, 0}, {1, 0, 1, 1}, {0, 0, 1, 1}, {0, 1, 0, 1}}),
           TestType({{0, 0, 0, 0}, {0, 1, 0, 1}, {1, 1, 1, 0}, {1, 0, 0, 1}}),
           TestType({{0, 0, 0, 1}, {0, 0, 1, 0}, {1, 0, 0, 1}, {1, 1, 0, 0}})};

    Konieczny<TestType> S(gens);
    REQUIRE(S.size() == 415);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE(
      "Konieczny",
      "004",
      "40x40 boolean matrix semigroup (size 248'017)",
      "[extreme][bmat]",
      BMat<40>,
      BMat<>) {
    auto                rg = ReportGuard(true);
    Konieczny<TestType> S;
    for (auto const& v : konieczny_data::clark_gens) {
      S.add_generator(TestType(v));
    }
    REQUIRE(S.generator(0).number_of_rows() == 40);
    S.run();
    REQUIRE(S.size() == 248'017);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny", "006", "exceptions", "[quick][bmat]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(
        Konieczny<BMat<>>(
            {BMat<>({{1, 0, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}, {0, 1, 0, 0}}),
             BMat<>({{1, 0, 0}, {1, 0, 0}, {1, 1, 1}})}),
        LibsemigroupsException);
    // This doesn't throw when using BMat<4>, so there's no test for that
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("Konieczny",
                                   "007",
                                   "5x5 boolean matrix semigroup (size 513)",
                                   "[quick][bmat][no-valgrind]",
                                   BMat<>,
                                   BMat<5>) {
    auto                  rg   = ReportGuard(REPORT);
    std::vector<TestType> gens = {TestType({{0, 1, 1, 1, 0},
                                            {0, 0, 1, 0, 0},
                                            {1, 0, 0, 1, 0},
                                            {1, 1, 1, 0, 0},
                                            {0, 1, 1, 1, 1}}),
                                  TestType({{0, 0, 0, 1, 0},
                                            {0, 0, 1, 0, 0},
                                            {1, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0},
                                            {0, 1, 0, 1, 1}}),
                                  TestType({{0, 0, 0, 1, 0},
                                            {1, 1, 0, 0, 0},
                                            {0, 0, 1, 1, 1},
                                            {1, 1, 0, 0, 1},
                                            {0, 0, 1, 1, 0}}),
                                  TestType({{0, 1, 0, 0, 1},
                                            {0, 0, 1, 0, 1},
                                            {1, 0, 1, 0, 0},
                                            {0, 1, 1, 1, 0},
                                            {1, 0, 0, 0, 1}})};

    Konieczny<TestType> S(gens);
    REQUIRE(S.size() == 513);
  }
}  // namespace libsemigroups
