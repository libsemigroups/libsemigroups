// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 James D. Mitchell
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

// This file contains some benchmarks for the performance of the bipartition
// multiplication.

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS

#include "libsemigroups/bipart.hpp"        // for Bipartition
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin

namespace libsemigroups {

  TEST_CASE("Bipartition product_inplace", "[quick][001]") {
    FroidurePin<Bipartition> S;
    froidure_pin::add_generators(
        S,
        {Bipartition({{1, -2}, {2, -3}, {3, -4}, {4, -5}, {5, -1}}),
         Bipartition({{1, -2}, {2, -1}, {3, -3}, {4, -4}, {5, -5}}),
         Bipartition({{1}, {2, -2}, {3, -3}, {4, -4}, {5, -5}, {-1}}),
         Bipartition({{1, 2, -1, -2}, {3, -3}, {4, -4}, {5, -5}})});

    REQUIRE(S.size() == 115'975);

    Bipartition tmp(5);

    BENCHMARK("square every bipartition of degree 5") {
      for (auto const& x : S) {
        tmp.product_inplace_no_checks(x, x);
        REQUIRE(tmp.degree() == 5);
      }
    };
  }

  TEST_CASE("Bipartition product_inplace", "[quick][002]") {
    for (size_t N : {0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1'024, 2'048}) {
      Bipartition tmp(N);
      auto        x  = bipartition::random(N);
      auto        id = bipartition::one(x);
      BENCHMARK(fmt::format("2'000 products, degree {}", N).c_str()) {
        for (size_t i = 0; i < 1'000; ++i) {
          tmp.product_inplace_no_checks(x, id);
          REQUIRE(tmp == x);
          tmp.product_inplace_no_checks(x, x);
          REQUIRE(tmp.degree() == N);
        }
      };
    }
  }
}  // namespace libsemigroups
