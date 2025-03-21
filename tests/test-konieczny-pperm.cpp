
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-2025 Finn Smith
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

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for REQUIRE
#include "test-main.hpp"                       // FOR LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/konieczny.hpp"  // for Konieczny
#include "libsemigroups/transf.hpp"     // for PPerm

namespace libsemigroups {

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "026",
                          "partial perm",
                          "[quick][pperm][no-valgrind]") {
    auto      rg = ReportGuard(false);
    Konieczny S  = make<Konieczny>(
        {make<LeastPPerm<9>>({0, 2, 3, 7}, {1, 6, 7, 3}, 9),
          make<LeastPPerm<9>>({0, 1, 2, 3, 4, 7}, {6, 5, 8, 0, 2, 1}, 9),
          make<LeastPPerm<9>>(
             {0, 1, 2, 3, 4, 5, 6, 8}, {1, 7, 2, 6, 0, 4, 8, 5}, 9),
          make<LeastPPerm<9>>({0, 1, 2, 3, 5, 6, 8}, {2, 4, 6, 1, 5, 8, 7}, 9),
          make<LeastPPerm<9>>({0, 1, 2, 3, 5, 8}, {7, 3, 6, 4, 2, 5}, 9)});
    for (auto it = S.cbegin_generators(); it != S.cend_generators(); ++it) {
      REQUIRE(S.contains(*it));
      REQUIRE(S.D_class_of_element(*it).contains(*it));
    }
    REQUIRE(!S.contains(make<LeastPPerm<9>>(
        {0, 1, 2, 3, 4, 5, 6, 7}, {1, 7, 2, 6, 0, 4, 8, 5}, 9)));
    REQUIRE(!S.contains(make<LeastPPerm<9>>(
        {0, 1, 2, 3, 4, 5, 6, 7}, {0, 1, 2, 3, 4, 5, 6, 8}, 9)));
    REQUIRE(S.contains(make<LeastPPerm<9>>({}, {}, 9)));
    REQUIRE(S.size() == 21'033);
    REQUIRE(S.number_of_D_classes() == 3'242);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "027",
                          "symmetric inverse monoid n = 8",
                          "[quick][pperm][no-valgrind]") {
    auto      rg = ReportGuard(false);
    Konieczny S  = make<Konieczny>(
        {make<LeastPPerm<8>>(
             {0, 1, 2, 3, 4, 5, 6, 7}, {0, 1, 2, 3, 4, 5, 6, 7}, 8),
          make<LeastPPerm<8>>(
             {0, 1, 2, 3, 4, 5, 6, 7}, {1, 2, 3, 4, 5, 6, 7, 0}, 8),
          make<LeastPPerm<8>>(
             {0, 1, 2, 3, 4, 5, 6, 7}, {1, 0, 2, 3, 4, 5, 6, 7}, 8),
          make<LeastPPerm<8>>({1, 2, 3, 4, 5, 6, 7}, {0, 1, 2, 3, 4, 5, 6}, 8),
          // clang-format off
          make<LeastPPerm<8>>({0, 1, 2, 3, 4, 5, 6}, {1, 2, 3, 4, 5, 6, 7}, 8)
        });
    // clang-format on
    REQUIRE(S.size() == 1'441'729);
    REQUIRE(
        std::distance(S.cbegin_current_D_classes(), S.cend_current_D_classes())
        == 9);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "028",
                          "exceptions",
                          "[quick][pperm][no-valgrind]") {
    auto rg = ReportGuard(false);

    REQUIRE_THROWS_AS(make<Konieczny>({PPerm<>::one(65)}),
                      LibsemigroupsException);
    std::vector const gens
        = {make<PPerm<>>({0, 2, 3, 7}, {1, 6, 7, 3}, 9),
           make<PPerm<>>({0, 1, 2, 3, 4, 7}, {6, 5, 8, 0, 2, 1}, 9),
           make<PPerm<>>({0, 1, 2, 3, 4, 5, 6, 8}, {1, 7, 2, 6, 0, 4, 8, 5}, 9),
           make<PPerm<>>({0, 1, 2, 3, 5, 6, 8}, {2, 4, 6, 1, 5, 8, 7}, 9),
           make<PPerm<>>({0, 1, 2, 3, 5, 8}, {7, 3, 6, 4, 2, 5}, 9)};

    Konieczny S = make<Konieczny>(gens);
    REQUIRE_NOTHROW(S.add_generators(gens.begin(), gens.begin() + 2));
    REQUIRE_THROWS_AS(S.add_generator(make<PPerm<>>({0, 1, 2, 3, 4, 5, 6, 8, 9},
                                                    {1, 7, 2, 6, 0, 4, 8, 5, 9},
                                                    10)),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(
        konieczny::add_generators(
            S,
            {make<PPerm<>>({0, 1, 2, 3, 5, 6, 8}, {2, 4, 6, 1, 5, 8, 7}, 9),
             make<PPerm<>>({0, 1, 2, 3, 4, 5, 6, 8, 9},
                           {1, 7, 2, 6, 0, 4, 8, 5, 9},
                           10)}),
        LibsemigroupsException);
    REQUIRE(S.size() == 21'033);
    REQUIRE_THROWS_AS(S.add_generators(gens.begin(), gens.begin() + 2),
                      LibsemigroupsException);
  }
}  // namespace libsemigroups
