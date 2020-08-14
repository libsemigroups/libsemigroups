
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

#include "catch.hpp"      // for REQUIRE
#include "test-main.hpp"  // FOR LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/element-adapters.hpp"  // for Degree etc
#include "libsemigroups/element-helper.hpp"    // for PPermHelper
#include "libsemigroups/element.hpp"           // for PartialPerm
#include "libsemigroups/konieczny.hpp"         // for Konieczny

namespace libsemigroups {

  constexpr bool REPORT = false;

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "023",
                          "partial perm",
                          "[quick][partialperm]") {
    using PPerm                 = typename PPermHelper<9>::type;
    auto                     rg = ReportGuard(REPORT);
    const std::vector<PPerm> gens
        = {PPerm({0, 2, 3, 7}, {1, 6, 7, 3}, 9),
           PPerm({0, 1, 2, 3, 4, 7}, {6, 5, 8, 0, 2, 1}, 9),
           PPerm({0, 1, 2, 3, 4, 5, 6, 8}, {1, 7, 2, 6, 0, 4, 8, 5}, 9),
           PPerm({0, 1, 2, 3, 5, 6, 8}, {2, 4, 6, 1, 5, 8, 7}, 9),
           PPerm({0, 1, 2, 3, 5, 8}, {7, 3, 6, 4, 2, 5}, 9)};
    Konieczny<PPerm> S(gens);
    for (auto x : gens) {
      REQUIRE(S.contains(x));
      REQUIRE(S.D_class_of_element(x).contains(x));
    }
    REQUIRE(!S.contains(
        PPerm({0, 1, 2, 3, 4, 5, 6, 7}, {1, 7, 2, 6, 0, 4, 8, 5}, 9)));
    REQUIRE(!S.contains(
        PPerm({0, 1, 2, 3, 4, 5, 6, 7}, {0, 1, 2, 3, 4, 5, 6, 8}, 9)));
    REQUIRE(S.contains(PPerm({}, {}, 9)));
    REQUIRE(S.size() == 21033);
    REQUIRE(S.number_of_D_classes() == 3242);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "024",
                          "symmetric inverse monoid n = 8",
                          "[quick][partialperm][no-valgrind]") {
    using PPerm         = typename PPermHelper<8>::type;
    auto             rg = ReportGuard(REPORT);
    Konieczny<PPerm> S(
        {PPerm({0, 1, 2, 3, 4, 5, 6, 7}, {0, 1, 2, 3, 4, 5, 6, 7}, 8),
         PPerm({0, 1, 2, 3, 4, 5, 6, 7}, {1, 2, 3, 4, 5, 6, 7, 0}, 8),
         PPerm({0, 1, 2, 3, 4, 5, 6, 7}, {1, 0, 2, 3, 4, 5, 6, 7}, 8),
         PPerm({1, 2, 3, 4, 5, 6, 7}, {0, 1, 2, 3, 4, 5, 6}, 8),
         PPerm({0, 1, 2, 3, 4, 5, 6}, {1, 2, 3, 4, 5, 6, 7}, 8)});
    REQUIRE(S.size() == 1441729);
    REQUIRE(std::distance(S.cbegin_D_classes(), S.cend_D_classes()) == 9);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "025",
                          "exceptions",
                          "[quick][partialperm]") {
    auto                rg = ReportGuard(REPORT);
    std::vector<size_t> v(65, 0);
    std::iota(v.begin(), v.end(), 0);
    REQUIRE_THROWS_AS(Konieczny<PartialPerm<size_t>>({PartialPerm<size_t>(v)}),
                      LibsemigroupsException);
    const std::vector<PartialPerm<size_t>> gens
        = {PartialPerm<size_t>({0, 2, 3, 7}, {1, 6, 7, 3}, 9),
           PartialPerm<size_t>({0, 1, 2, 3, 4, 7}, {6, 5, 8, 0, 2, 1}, 9),
           PartialPerm<size_t>(
               {0, 1, 2, 3, 4, 5, 6, 8}, {1, 7, 2, 6, 0, 4, 8, 5}, 9),
           PartialPerm<size_t>({0, 1, 2, 3, 5, 6, 8}, {2, 4, 6, 1, 5, 8, 7}, 9),
           PartialPerm<size_t>({0, 1, 2, 3, 5, 8}, {7, 3, 6, 4, 2, 5}, 9)};

    Konieczny<PartialPerm<size_t>> S(gens);
    REQUIRE_NOTHROW(S.add_generators(gens.begin(), gens.begin() + 2));
    REQUIRE_THROWS_AS(
        S.add_generator(PartialPerm<size_t>(
            {0, 1, 2, 3, 4, 5, 6, 8, 9}, {1, 7, 2, 6, 0, 4, 8, 5, 9}, 10)),
        LibsemigroupsException);
    REQUIRE_THROWS_AS(S.add_generators(std::vector<PartialPerm<size_t>>(
                          {PartialPerm<size_t>(
                               {0, 1, 2, 3, 5, 6, 8}, {2, 4, 6, 1, 5, 8, 7}, 9),
                           PartialPerm<size_t>({0, 1, 2, 3, 4, 5, 6, 8, 9},
                                               {1, 7, 2, 6, 0, 4, 8, 5, 9},
                                               10)})),
                      LibsemigroupsException);
    REQUIRE(S.size() == 21033);
    REQUIRE_THROWS_AS(S.add_generators(gens.begin(), gens.begin() + 2),
                      LibsemigroupsException);
  }
}  // namespace libsemigroups
