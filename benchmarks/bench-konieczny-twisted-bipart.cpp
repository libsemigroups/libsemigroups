//
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
#include <string>   // for to_string

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for TEST_CASE, BENCHMARK, REQUIRE

#include "libsemigroups/froidure-pin.hpp"    // for FroidurePin
#include "libsemigroups/konieczny.hpp"       // for Konieczny
#include "libsemigroups/twisted-bipart.hpp"  // for Lambda, Rho, etc.

namespace libsemigroups {

  std::vector<TwistedBipartition> brauer_gens_twisted(size_t thresh) {
    std::vector<TwistedBipartition> gens = {
        make<TwistedBipartition>(
            {{1, -1}, {2, -2}, {3, -3}, {4, -4}, {5, -5}, {6, -6}}, thresh, 0),
        make<TwistedBipartition>(
            {{1, -2}, {2, -3}, {3, -4}, {4, -5}, {5, -6}, {6, -1}}, thresh, 0),
        make<TwistedBipartition>(
            {{1, -2}, {2, -1}, {3, -3}, {4, -4}, {5, -5}, {6, -6}}, thresh, 0),
        make<TwistedBipartition>(
            {{1, 2}, {-1, -2}, {3, -3}, {4, -4}, {5, -5}, {6, -6}}, thresh, 0)};
    return gens;
  }

  std::vector<TwistedBipartition> partition_gens_twisted(size_t thresh) {
    std::vector<TwistedBipartition> gens = {
        make<TwistedBipartition>(
            {{1}, {-1}, {2, -2}, {3, -3}, {4, -4}, {5, -5}, {6, -6}},
            thresh,
            0),
        make<TwistedBipartition>(
            {{1, -2}, {2, -3}, {3, -4}, {4, -5}, {5, -6}, {6, -1}}, thresh, 0),
        make<TwistedBipartition>(
            {{1, -2}, {2, -1}, {3, -3}, {4, -4}, {5, -5}, {6, -6}}, thresh, 0),
        make<TwistedBipartition>(
            {{1, 2, -1, -2}, {3, -3}, {4, -4}, {5, -5}, {6, -6}}, thresh, 0)};

    return gens;
  }

  ////////////////////////////////////////////////////////////////////////
  // The actual benchmarks for Konieczny<TwistedBipartition>
  ////////////////////////////////////////////////////////////////////////
  TEST_CASE("Example 1: brauer gens twisted - deg 6, thresh 0 to 50",
            "[000][standard]") {
    auto rg = ReportGuard(false);

    for (size_t i = 0; i < 51; i += 5) {
      auto gens = brauer_gens_twisted(i);
      BENCHMARK("Konieczny: threshold " + std::to_string(i)) {
        Konieczny S = make<Konieczny>(gens);
        S.run();
      };
    }
    for (size_t i = 0; i < 51; i += 5) {
      auto gens = brauer_gens_twisted(i);
      BENCHMARK("FroidurePin: threshold " + std::to_string(i)) {
        FroidurePin S = make<FroidurePin>(gens);
        S.run();
      };
    }
  }

  TEST_CASE("Example 2: partition gens twisted - deg 6, thresh 1 to 10",
            "[001][standard]") {
    // auto rg = ReportGuard(false);

    for (size_t i = 0; i < 51; i += 10) {
      auto gens = partition_gens_twisted(i);
      BENCHMARK("Konieczny: threshold " + std::to_string(i)) {
        Konieczny S = make<Konieczny>(gens);
        S.run();
      };
    }

    /*
    for (size_t i = 40; i < 41; i += 10) {
      auto gens = partition_gens_twisted(i);
      BENCHMARK("FroidurePin: threshold " + std::to_string(i)) {
        FroidurePin S = make<FroidurePin>(gens);
        S.run();
      };
    }
    */
  }

}  // namespace libsemigroups
