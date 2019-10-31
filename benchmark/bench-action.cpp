//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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

// This file contains some benchmarks for libsemigroups/include/action.hpp.

#include <iostream>

#include <benchmark/benchmark.h>

#include "bench-main.hpp"

#ifndef HPCOMBI_CONSTEXPR_FUN_ARGS
#define HPCOMBI_CONSTEXPR_FUN_ARGS
#endif

#include "action.hpp"
#include "bmat8.hpp"
#include "element.hpp"

void BM_orb_perm_on_tuples_10_5_vector(benchmark::State& st) {
  using Perm = libsemigroups::Perm<10>::type;
  using Action  = libsemigroups::Action<Perm,
                                 std::vector<u_int8_t>,
                                 libsemigroups::on_tuples<Perm, u_int8_t>>;
  for (auto _ : st) {
    Action o;
    o.add_seed({0, 1, 2, 3, 4});
    o.add_generator(Perm({1, 0, 2, 3, 4, 5, 6, 7, 8, 9}));
    o.add_generator(Perm({1, 2, 3, 4, 5, 6, 7, 8, 9, 0}));
    o.size();
  }
}

BENCHMARK(BM_orb_perm_on_tuples_10_5_vector)->Unit(benchmark::kMillisecond);

void BM_orb_perm_on_tuples_10_5_array(benchmark::State& st) {
  using Perm = libsemigroups::Perm<10>::type;
  using Action  = libsemigroups::Action<
      Perm,
      std::array<u_int8_t, 5>,
      libsemigroups::on_tuples<Perm, u_int8_t, std::array<u_int8_t, 5>>>;
  for (auto _ : st) {
    Action o;
    o.add_seed({0, 1, 2, 3, 4});
    o.add_generator(Perm({1, 0, 2, 3, 4, 5, 6, 7, 8, 9}));
    o.add_generator(Perm({1, 2, 3, 4, 5, 6, 7, 8, 9, 0}));
    o.size();
  }
}

BENCHMARK(BM_orb_perm_on_tuples_10_5_array)->Unit(benchmark::kMillisecond);

void BM_orb_perm_on_tuples_15_5_vector(benchmark::State& st) {
  using Perm = libsemigroups::Perm<15>::type;
  using Action  = libsemigroups::Action<Perm,
                                 std::vector<u_int8_t>,
                                 libsemigroups::on_tuples<Perm, u_int8_t>>;
  for (auto _ : st) {
    Action o;
    o.add_seed({0, 1, 2, 3, 4});
    o.add_generator(Perm({1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}));
    o.add_generator(Perm({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0}));
    o.size();
  }
}

BENCHMARK(BM_orb_perm_on_tuples_15_5_vector)->Unit(benchmark::kMillisecond);

void BM_orb_perm_on_tuples_15_5_array(benchmark::State& st) {
  using Perm = libsemigroups::Perm<15>::type;
  using Action  = libsemigroups::Action<
      Perm,
      std::array<u_int8_t, 5>,
      libsemigroups::on_tuples<Perm, u_int8_t, std::array<u_int8_t, 5>>>;
  for (auto _ : st) {
    Action o;
    o.add_seed({0, 1, 2, 3, 4});
    o.add_generator(Perm({1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}));
    o.add_generator(Perm({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0}));
    o.size();
  }
}

BENCHMARK(BM_orb_perm_on_tuples_15_5_array)->Unit(benchmark::kMillisecond);

void BM_orb_bmat8_on_rows_regular_5(benchmark::State& st) {
  using BMat8        = libsemigroups::BMat8;
  using right_action = libsemigroups::right_action<BMat8, BMat8>;
  using row_orb_type = libsemigroups::Action<BMat8, BMat8, right_action>;
  for (auto _ : st) {
    row_orb_type o;

    o.add_seed(BMat8::one());
    o.add_generator(BMat8({{0, 1, 0, 0, 0},
                           {1, 0, 0, 0, 0},
                           {0, 0, 1, 0, 0},
                           {0, 0, 0, 1, 0},
                           {0, 0, 0, 0, 1}}));
    o.add_generator(BMat8({{0, 1, 0, 0, 0},
                           {0, 0, 1, 0, 0},
                           {0, 0, 0, 1, 0},
                           {0, 0, 0, 0, 1},
                           {1, 0, 0, 0, 0}}));
    o.add_generator(BMat8({{1, 0, 0, 0, 0},
                           {1, 1, 0, 0, 0},
                           {0, 0, 1, 0, 0},
                           {0, 0, 0, 1, 0},
                           {0, 0, 0, 0, 1}}));
    o.add_generator(BMat8({{0, 0, 0, 0, 0},
                           {0, 1, 0, 0, 0},
                           {0, 0, 1, 0, 0},
                           {0, 0, 0, 1, 0},
                           {0, 0, 0, 0, 1}}));
    o.enumerate();
  }
}

BENCHMARK(BM_orb_bmat8_on_rows_regular_5)->Unit(benchmark::kMillisecond);

void BM_orb_bmat8_on_rows_5(benchmark::State& st) {
  using BMat8        = libsemigroups::BMat8;
  using right_action = libsemigroups::right_action<BMat8, BMat8>;
  using row_orb_type = libsemigroups::Action<BMat8, BMat8, right_action>;
  for (auto _ : st) {
    row_orb_type o;

    o.add_seed(BMat8::one());
    o.add_generator(BMat8({{1, 0, 0, 0, 0},
                           {0, 1, 0, 0, 0},
                           {0, 0, 1, 0, 0},
                           {0, 0, 0, 1, 0},
                           {0, 0, 0, 0, 1}}));
    o.add_generator(BMat8({{0, 1, 0, 0, 0},
                           {0, 0, 1, 0, 0},
                           {0, 0, 0, 1, 0},
                           {0, 0, 0, 0, 1},
                           {1, 0, 0, 0, 0}}));
    o.add_generator(BMat8({{0, 1, 0, 0, 0},
                           {1, 0, 0, 0, 0},
                           {0, 0, 1, 0, 0},
                           {0, 0, 0, 1, 0},
                           {0, 0, 0, 0, 1}}));
    o.add_generator(BMat8({{1, 0, 0, 0, 0},
                           {0, 1, 0, 0, 0},
                           {0, 0, 1, 0, 0},
                           {0, 0, 0, 1, 0},
                           {1, 0, 0, 0, 1}}));
    o.add_generator(BMat8({{1, 1, 0, 0, 0},
                           {1, 0, 1, 0, 0},
                           {0, 1, 0, 1, 0},
                           {0, 0, 1, 1, 0},
                           {0, 0, 0, 0, 1}}));
    o.add_generator(BMat8({{1, 1, 0, 0, 0},
                           {1, 0, 1, 0, 0},
                           {0, 1, 1, 0, 0},
                           {0, 0, 0, 1, 0},
                           {0, 0, 0, 0, 1}}));
    o.add_generator(BMat8({{1, 1, 1, 0, 0},
                           {1, 0, 0, 1, 0},
                           {0, 1, 0, 1, 0},
                           {0, 0, 1, 1, 0},
                           {0, 0, 0, 0, 1}}));
    o.add_generator(BMat8({{1, 1, 0, 0, 0},
                           {1, 0, 1, 0, 0},
                           {0, 1, 0, 1, 0},
                           {0, 0, 1, 0, 1},
                           {0, 0, 0, 1, 1}}));
    o.add_generator(BMat8({{1, 1, 1, 1, 0},
                           {1, 0, 0, 0, 1},
                           {0, 1, 0, 0, 1},
                           {0, 0, 1, 0, 1},
                           {0, 0, 0, 1, 1}}));
    o.add_generator(BMat8({{1, 0, 0, 0, 0},
                           {0, 1, 0, 0, 0},
                           {0, 0, 1, 0, 0},
                           {0, 0, 0, 1, 0},
                           {0, 0, 0, 0, 0}}));
    o.add_generator(BMat8({{1, 1, 1, 0, 0},
                           {1, 0, 0, 1, 0},
                           {0, 1, 0, 1, 0},
                           {0, 0, 1, 0, 1},
                           {0, 0, 0, 1, 1}}));
    o.add_generator(BMat8({{1, 1, 1, 0, 0},
                           {1, 0, 0, 1, 0},
                           {1, 0, 0, 0, 1},
                           {0, 1, 0, 1, 0},
                           {0, 0, 1, 0, 1}}));
    o.add_generator(BMat8({{1, 1, 1, 0, 0},
                           {1, 0, 0, 1, 1},
                           {0, 1, 0, 1, 0},
                           {0, 1, 0, 0, 1},
                           {0, 0, 1, 1, 0}}));
    o.add_generator(BMat8({{1, 1, 1, 0, 0},
                           {1, 1, 0, 1, 0},
                           {1, 0, 0, 0, 1},
                           {0, 1, 0, 0, 1},
                           {0, 0, 1, 1, 1}}));
    o.enumerate();
  }
}

BENCHMARK(BM_orb_bmat8_on_rows_5)->Unit(benchmark::kMillisecond);
BENCHMARK_MAIN();
