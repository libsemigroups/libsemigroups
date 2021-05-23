//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020 James D. Mitchell
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

#include "bench-main.hpp"  // for LIBSEMIGROUPS_BENCHMARK
#include "catch.hpp"       // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS

#include "uf-old.hpp"  // for UFOld

#include "libsemigroups/string.hpp"  // for to_string
#include "libsemigroups/uf.hpp"      // for Duf and Suf

namespace libsemigroups {
  namespace detail {
    template <size_t N>
    void bench_uf() {
      std::default_random_engine         gen;
      std::uniform_int_distribution<int> dist(0, N - 1);
      static size_t                      M = 10;
      M                                    = N;
      BENCHMARK("Duf" + to_string(M)) {
        for (size_t j = 0; j < 10; ++j) {
          Duf<> uf(N);
          for (size_t i = 0; i < N / 5; ++i) {
            uf.unite(dist(gen), dist(gen));
          }
          REQUIRE(uf.number_of_blocks() > 1);
        }
      };

      BENCHMARK("Suf" + to_string(M)) {
        for (size_t j = 0; j < 10; ++j) {
          Suf<N> uf;
          for (size_t i = 0; i < N / 5; ++i) {
            uf.unite(dist(gen), dist(gen));
          }
          REQUIRE(uf.number_of_blocks() > 1);
        }
      };

      BENCHMARK("UFOld" + to_string(M)) {
        for (size_t j = 0; j < 10; ++j) {
          UFOld uf(N);
          for (size_t i = 0; i < N / 5; ++i) {
            uf.unite(dist(gen), dist(gen));
          }
          REQUIRE(uf.nr_blocks() > 1);
        }
      };
    }

    TEST_CASE("UFOld", "[quick]") {
      bench_uf<100>();
      bench_uf<1000>();
      bench_uf<10000>();
      bench_uf<100000>();
      bench_uf<1000000>();
    }
  }  // namespace detail
}  // namespace libsemigroups
