// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 Finn Smith
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

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for REQUIRE
#include "test-main.hpp"                       // FOR LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/bipart.hpp"     // for TwistedBipartition
#include "libsemigroups/konieczny.hpp"  // for Konieczny

namespace libsemigroups {

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "900",
                          "small TwistedBipartition example",
                          "[quick][konieczny]") {
    auto rg = ReportGuard(true);

    auto x = make<TwistedBipartition>({{1, 5, 8, -1, -2, -4, -10},
                                       {2, 4, 7, -8},
                                       {3, 6, 9, 10, -3},
                                       {-5, -9},
                                       {-6, -7}},
                                      1,
                                      0);
    auto y = make<TwistedBipartition>({{1},
                                       {2, 3, 4, 5, -5, -6, -7},
                                       {6, 8, -2, -4, -8, -10},
                                       {7, -9},
                                       {9, -3},
                                       {10, -1}},
                                      1,
                                      0);
    auto z
        = make<TwistedBipartition>({{1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
                                    {-1, -2, -3, -4, -5, -6, -7, -8, -9, -10}},
                                   1,
                                   0);

    Konieczny S = make<Konieczny>({x, y, z});
    REQUIRE(S.size() == 23);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "901",
                          "twisted Brauer monoid, degree 6 threshold 1",
                          "[quick][konieczny]") {
    auto rg = ReportGuard(true);

    auto x = make<TwistedBipartition>(
        {{1, -1}, {2, -2}, {3, -3}, {4, -4}, {5, -5}, {6, -6}}, 1, 0);
    auto y = make<TwistedBipartition>(
        {{1, -2}, {2, -3}, {3, -4}, {4, -5}, {5, -6}, {6, -1}}, 1, 0);
    auto z = make<TwistedBipartition>(
        {{1, -2}, {2, -1}, {3, -3}, {4, -4}, {5, -5}, {6, -6}}, 1, 0);
    auto t = make<TwistedBipartition>(
        {{1, 2}, {-1, -2}, {3, -3}, {4, -4}, {5, -5}, {6, -6}}, 1, 0);

    Konieczny S = make<Konieczny>({x, y, z, t});

    S.run();

    REQUIRE(S.number_of_D_classes() == 8);
    REQUIRE(S.size() == 20071);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "903",
                          "twisted Brauer gens, degree 6 threshold 1",
                          "[quick][konieczny]") {
    auto rg = ReportGuard(true);

    auto x = make<TwistedBipartition>(
        {{1, -1}, {2, -2}, {3, -3}, {4, -4}, {5, -5}, {6, -6}}, 50, 0);
    auto y = make<TwistedBipartition>(
        {{1, -2}, {2, -3}, {3, -4}, {4, -5}, {5, -6}, {6, -1}}, 50, 0);
    auto z = make<TwistedBipartition>(
        {{1, -2}, {2, -1}, {3, -3}, {4, -4}, {5, -5}, {6, -6}}, 50, 0);
    auto t = make<TwistedBipartition>(
        {{1, 2}, {-1, -2}, {3, -3}, {4, -4}, {5, -5}, {6, -6}}, 50, 0);

    Konieczny S = make<Konieczny>({x, y, z, t});

    S.run();

    REQUIRE(S.size() == 494146);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "904",
                          "twisted Partition gens, degree 6 threshold 10",
                          "[standard][konieczny]") {
    auto rg = ReportGuard(true);

    auto x = make<TwistedBipartition>(
        {{1}, {-1}, {2, -2}, {3, -3}, {4, -4}, {5, -5}, {6, -6}}, 10, 0);
    auto y = make<TwistedBipartition>(
        {{1, -2}, {2, -3}, {3, -4}, {4, -5}, {5, -6}, {6, -1}}, 10, 0);
    auto z = make<TwistedBipartition>(
        {{1, -2}, {2, -1}, {3, -3}, {4, -4}, {5, -5}, {6, -6}}, 10, 0);
    auto t = make<TwistedBipartition>(
        {{1, 2, -1, -2}, {3, -3}, {4, -4}, {5, -5}, {6, -6}}, 10, 0);

    Konieczny S = make<Konieczny>({x, y, z, t});

    S.run();

    REQUIRE(S.size() == 46342368);
  }
}  // namespace libsemigroups
