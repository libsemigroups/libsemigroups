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
#include "libsemigroups/element.hpp"           // for BooleanMat
#include "libsemigroups/konieczny.hpp"         // for Konieczny
#include "test-konieczny-booleanmat-data.hpp"  // for clark_gens

namespace libsemigroups {

  constexpr bool REPORT = false;

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "018",
                          "non-pointer BooleanMat",
                          "[quick][boolmat][booleanmat]") {
    auto                    rg = ReportGuard(REPORT);
    std::vector<BooleanMat> gens
        = {BooleanMat({0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0}),
           BooleanMat({0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1}),
           BooleanMat({0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1})};

    Konieczny<BooleanMat> S(gens);
    REQUIRE(S.size() == 26);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "019",
                          "non-pointer BooleanMat",
                          "[quick][boolmat][booleanmat][no-valgrind]") {
    auto                    rg   = ReportGuard(REPORT);
    std::vector<BooleanMat> gens = {
        BooleanMat({{1, 0, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}, {0, 1, 0, 0}}),
        BooleanMat({{1, 0, 0, 1}, {1, 0, 0, 1}, {1, 1, 1, 1}, {0, 1, 1, 0}}),
        BooleanMat({{1, 0, 1, 0}, {1, 0, 1, 1}, {0, 0, 1, 1}, {0, 1, 0, 1}}),
        BooleanMat({{0, 0, 0, 0}, {0, 1, 0, 1}, {1, 1, 1, 0}, {1, 0, 0, 1}}),
        BooleanMat({{0, 0, 0, 1}, {0, 0, 1, 0}, {1, 0, 0, 1}, {1, 1, 0, 0}})};

    Konieczny<BooleanMat> S(gens);
    REQUIRE(S.size() == 415);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "020",
                          "generators from Sean Clark",
                          "[extreme]") {
    auto                  rg = ReportGuard(true);
    Konieczny<BooleanMat> S(konieczny_data::clark_gens);
    S.run();
    REQUIRE(S.size() == 248017);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "021",
                          "exceptions",
                          "[quick][booleanmat][boolmat]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(
        Konieczny<BooleanMat>(
            {BooleanMat(
                 {{1, 0, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}, {0, 1, 0, 0}}),
             BooleanMat({{1, 0, 0}, {1, 0, 0}, {1, 1, 1}})}),
        LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "022",
                          "code coverage",
                          "[quick][boolmat][booleanmat][no-valgrind]") {
    auto                    rg = ReportGuard(REPORT);
    std::vector<BooleanMat> gens
        = {BooleanMat({0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0,
                       1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1}),
           BooleanMat({0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0,
                       0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1}),
           BooleanMat({0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1,
                       1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0}),
           BooleanMat({0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1,
                       0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1})};

    Konieczny<BooleanMat> S(gens);
    REQUIRE(S.size() == 513);
  }
}  // namespace libsemigroups
