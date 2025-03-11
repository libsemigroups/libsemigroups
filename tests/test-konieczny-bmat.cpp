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

#include "Catch2-3.7.1/catch_amalgamated.hpp"  // for REQUIRE
#include "bmat-data.hpp"                       // for clark_gens
#include "test-main.hpp"                       // FOR LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/bmat-adapters.hpp"  // for BMat adapters
#include "libsemigroups/konieczny.hpp"      // for Konieczny
#include "libsemigroups/matrix.hpp"         // for BMat

namespace libsemigroups {

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("Konieczny",
                                   "000",
                                   "4x4 boolean matrix semigroup (size 26)",
                                   "[quick][bmat]",
                                   BMat<>,
                                   BMat<4>) {
    auto rg = ReportGuard(false);

    Konieczny S = make<Konieczny>(
        {TestType({{0, 1, 0, 1}, {1, 0, 0, 0}, {0, 1, 1, 1}, {0, 1, 1, 0}}),
         TestType({{0, 1, 1, 1}, {1, 1, 0, 0}, {0, 0, 0, 0}, {1, 1, 1, 1}}),
         TestType({{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 1, 1, 1}, {1, 1, 1, 1}})});

    REQUIRE(S.size() == 26);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("Konieczny",
                                   "001",
                                   "4x4 boolean matrix semigroup (size 415)",
                                   "[quick][bmat][no-valgrind]",
                                   BMat<>,
                                   BMat<4>) {
    auto rg = ReportGuard(false);

    Konieczny S = make<Konieczny>(
        {TestType({{1, 0, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}, {0, 1, 0, 0}}),
         TestType({{1, 0, 0, 1}, {1, 0, 0, 1}, {1, 1, 1, 1}, {0, 1, 1, 0}}),
         TestType({{1, 0, 1, 0}, {1, 0, 1, 1}, {0, 0, 1, 1}, {0, 1, 0, 1}}),
         TestType({{0, 0, 0, 0}, {0, 1, 0, 1}, {1, 1, 1, 0}, {1, 0, 0, 1}}),
         TestType({{0, 0, 0, 1}, {0, 0, 1, 0}, {1, 0, 0, 1}, {1, 1, 0, 0}})});
    REQUIRE(S.size() == 415);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE(
      "Konieczny",
      "002",
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

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("Konieczny",
                                   "003",
                                   "exceptions",
                                   "[quick][bmat]",
                                   BMat<>,
                                   BMat<4>) {
    auto rg = ReportGuard(false);
    REQUIRE_THROWS_AS(
        make<Konieczny>(
            {make<TestType>(
                 {{1, 0, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}, {0, 1, 0, 0}}),
             make<TestType>({{1, 0, 0}, {1, 0, 0}, {1, 1, 1}})}),
        LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("Konieczny",
                                   "004",
                                   "5x5 boolean matrix semigroup (size 513)",
                                   "[quick][bmat][no-valgrind]",
                                   BMat<>,
                                   BMat<5>) {
    auto                  rg   = ReportGuard(false);
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

    Konieczny S = make<Konieczny>(gens);
    REQUIRE(S.size() == 513);
  }
}  // namespace libsemigroups
