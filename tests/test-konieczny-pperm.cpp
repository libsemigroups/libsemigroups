
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

#include "catch.hpp"      // for REQUIRE
#include "test-main.hpp"  // FOR LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/konieczny.hpp"  // for Konieczny
#include "libsemigroups/transf.hpp"     // for PPerm

namespace libsemigroups {

  constexpr bool REPORT = false;

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "028",
                          "partial perm",
                          "[quick][pperm][no-valgrind]") {
    auto                             rg = ReportGuard(REPORT);
    std::vector<LeastPPerm<9>> const gens
        = {LeastPPerm<9>({0, 2, 3, 7}, {1, 6, 7, 3}, 9),
           LeastPPerm<9>({0, 1, 2, 3, 4, 7}, {6, 5, 8, 0, 2, 1}, 9),
           LeastPPerm<9>({0, 1, 2, 3, 4, 5, 6, 8}, {1, 7, 2, 6, 0, 4, 8, 5}, 9),
           LeastPPerm<9>({0, 1, 2, 3, 5, 6, 8}, {2, 4, 6, 1, 5, 8, 7}, 9),
           LeastPPerm<9>({0, 1, 2, 3, 5, 8}, {7, 3, 6, 4, 2, 5}, 9)};
    Konieczny<LeastPPerm<9>> S(gens);
    for (auto x : gens) {
      REQUIRE(S.contains(x));
      REQUIRE(S.D_class_of_element(x).contains(x));
    }
    REQUIRE(!S.contains(
        LeastPPerm<9>({0, 1, 2, 3, 4, 5, 6, 7}, {1, 7, 2, 6, 0, 4, 8, 5}, 9)));
    REQUIRE(!S.contains(
        LeastPPerm<9>({0, 1, 2, 3, 4, 5, 6, 7}, {0, 1, 2, 3, 4, 5, 6, 8}, 9)));
    REQUIRE(S.contains(LeastPPerm<9>({}, {}, 9)));
    REQUIRE(S.size() == 21033);
    REQUIRE(S.number_of_D_classes() == 3242);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "029",
                          "symmetric inverse monoid n = 8",
                          "[quick][pperm][no-valgrind]") {
    auto                     rg = ReportGuard(REPORT);
    Konieczny<LeastPPerm<8>> S(
        {LeastPPerm<8>({0, 1, 2, 3, 4, 5, 6, 7}, {0, 1, 2, 3, 4, 5, 6, 7}, 8),
         LeastPPerm<8>({0, 1, 2, 3, 4, 5, 6, 7}, {1, 2, 3, 4, 5, 6, 7, 0}, 8),
         LeastPPerm<8>({0, 1, 2, 3, 4, 5, 6, 7}, {1, 0, 2, 3, 4, 5, 6, 7}, 8),
         LeastPPerm<8>({1, 2, 3, 4, 5, 6, 7}, {0, 1, 2, 3, 4, 5, 6}, 8),
         LeastPPerm<8>({0, 1, 2, 3, 4, 5, 6}, {1, 2, 3, 4, 5, 6, 7}, 8)});
    REQUIRE(S.size() == 1441729);
    REQUIRE(std::distance(S.cbegin_D_classes(), S.cend_D_classes()) == 9);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "030",
                          "exceptions",
                          "[quick][pperm][no-valgrind]") {
    auto rg          = ReportGuard(REPORT);
    using value_type = typename PPerm<>::value_type;
    std::vector<value_type> v(65, 0);
    std::iota(v.begin(), v.end(), 0);
    REQUIRE_THROWS_AS(Konieczny<PPerm<>>({PPerm<>(v)}), LibsemigroupsException);
    std::vector<PPerm<>> const gens
        = {PPerm<>({0, 2, 3, 7}, {1, 6, 7, 3}, 9),
           PPerm<>({0, 1, 2, 3, 4, 7}, {6, 5, 8, 0, 2, 1}, 9),
           PPerm<>({0, 1, 2, 3, 4, 5, 6, 8}, {1, 7, 2, 6, 0, 4, 8, 5}, 9),
           PPerm<>({0, 1, 2, 3, 5, 6, 8}, {2, 4, 6, 1, 5, 8, 7}, 9),
           PPerm<>({0, 1, 2, 3, 5, 8}, {7, 3, 6, 4, 2, 5}, 9)};

    Konieczny<PPerm<>> S(gens);
    REQUIRE_NOTHROW(S.add_generators(gens.begin(), gens.begin() + 2));
    REQUIRE_THROWS_AS(S.add_generator(PPerm<>({0, 1, 2, 3, 4, 5, 6, 8, 9},
                                              {1, 7, 2, 6, 0, 4, 8, 5, 9},
                                              10)),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(
        S.add_generators(std::vector<PPerm<>>(
            {PPerm<>({0, 1, 2, 3, 5, 6, 8}, {2, 4, 6, 1, 5, 8, 7}, 9),
             PPerm<>({0, 1, 2, 3, 4, 5, 6, 8, 9},
                     {1, 7, 2, 6, 0, 4, 8, 5, 9},
                     10)})),
        LibsemigroupsException);
    REQUIRE(S.size() == 21033);
    REQUIRE_THROWS_AS(S.add_generators(gens.begin(), gens.begin() + 2),
                      LibsemigroupsException);
  }
}  // namespace libsemigroups
