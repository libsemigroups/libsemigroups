//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2025 James D. Mitchell
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

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for REQUIRE
#include "test-main.hpp"                       // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/froidure-pin.hpp"    // for FroidurePin
#include "libsemigroups/twisted-bipart.hpp"  // for Bipartition
#include "libsemigroups/word-range.hpp"      // for namespace literals

namespace libsemigroups {
  // Forward declaration
  struct LibsemigroupsException;

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<TwistedBipartition>",
                          "120",
                          "small example",
                          "[quick][froidure-pin][element]") {
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

    FroidurePin S = make<FroidurePin>({x, y, z});
    REQUIRE(S.size() == 23);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<TwistedBipartition>",
                          "121",
                          "small example 2",
                          "[quick][froidure-pin][element]") {
    auto x = make<TwistedBipartition>(
        {{1, 2, 3, -1}, {4, 5}, {6, -5, -6}, {-2, -3}, {-4}}, 10, 1);
    auto y = make<TwistedBipartition>(
        {{1, -1}, {2}, {3, 4, 5, -4}, {6, -5}, {-2}, {-3, -6}}, 10, 0);
    auto z = make<TwistedBipartition>(
        {{1, 2, -1, -2}, {3, -5}, {4, 5}, {6}, {-3}, {-4, -6}}, 10, 0);

    FroidurePin S = make<FroidurePin>({x, y, z});
    REQUIRE(S.size() == 236);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<TwistedBipartition>",
                          "122",
                          "twisted Brauer gens, deg 6 thresh 1",
                          "[quick][froidure-pin][element]") {
    auto rg = ReportGuard(true);

    auto x = make<TwistedBipartition>(
        {{1, -1}, {2, -2}, {3, -3}, {4, -4}, {5, -5}, {6, -6}}, 1, 0);
    auto y = make<TwistedBipartition>(
        {{1, -2}, {2, -3}, {3, -4}, {4, -5}, {5, -6}, {6, -1}}, 1, 0);
    auto z = make<TwistedBipartition>(
        {{1, -2}, {2, -1}, {3, -3}, {4, -4}, {5, -5}, {6, -6}}, 1, 0);
    auto t = make<TwistedBipartition>(
        {{1, 2}, {-1, -2}, {3, -3}, {4, -4}, {5, -5}, {6, -6}}, 1, 0);

    FroidurePin S = make<FroidurePin>({x, y, z, t});

    S.run();

    REQUIRE(S.size() == 20071);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<TwistedBipartition>",
                          "123",
                          "twisted brauer gens, deg 6 thresh 10",
                          "[standard][froidure-pin][element]") {
    auto rg = ReportGuard(true);

    auto x = make<TwistedBipartition>(
        {{1, -1}, {2, -2}, {3, -3}, {4, -4}, {5, -5}, {6, -6}}, 50, 0);
    auto y = make<TwistedBipartition>(
        {{1, -2}, {2, -3}, {3, -4}, {4, -5}, {5, -6}, {6, -1}}, 50, 0);
    auto z = make<TwistedBipartition>(
        {{1, -2}, {2, -1}, {3, -3}, {4, -4}, {5, -5}, {6, -6}}, 50, 0);
    auto t = make<TwistedBipartition>(
        {{1, 2}, {-1, -2}, {3, -3}, {4, -4}, {5, -5}, {6, -6}}, 50, 0);

    FroidurePin S = make<FroidurePin>({x, y, z, t});

    S.run();

    REQUIRE(S.size() == 494146);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<TwistedBipartition>",
                          "124",
                          "twisted partition gens, deg 6 thr 10",
                          "[extreme][froidure-pin][element]") {
    auto rg = ReportGuard(true);

    auto x = make<TwistedBipartition>(
        {{1}, {-1}, {2, -2}, {3, -3}, {4, -4}, {5, -5}, {6, -6}}, 10, 0);
    auto y = make<TwistedBipartition>(
        {{1, -2}, {2, -3}, {3, -4}, {4, -5}, {5, -6}, {6, -1}}, 10, 0);
    auto z = make<TwistedBipartition>(
        {{1, -2}, {2, -1}, {3, -3}, {4, -4}, {5, -5}, {6, -6}}, 10, 0);
    auto t = make<TwistedBipartition>(
        {{1, 2, -1, -2}, {3, -3}, {4, -4}, {5, -5}, {6, -6}}, 10, 0);

    FroidurePin S = make<FroidurePin>({x, y, z, t});

    S.run();

    REQUIRE(S.size() == 46342368);
  }

}  // namespace libsemigroups
