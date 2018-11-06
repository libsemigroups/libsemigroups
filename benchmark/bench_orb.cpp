//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 James D. Mitchell
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

// This file contains some benchmarks for libsemigroups/include/orb.hpp.

#include <iostream>

#include <benchmark/benchmark.h>

#include "bench_main.hpp"

#ifndef HPCOMBI_CONSTEXPR_FUN_ARGS
#define HPCOMBI_CONSTEXPR_FUN_ARGS
#endif

#include "element.hpp"
#include "orb.hpp"

using Perm = libsemigroups::Perm<10>::type;

void BM_orb_perm_on_tuples_vector(benchmark::State& st) {
  using Orb  = libsemigroups::
    Orb<Perm, std::vector<u_int8_t>, libsemigroups::on_tuples<Perm, u_int8_t>>;
  for (auto _ : st) {
    Orb o;
    o.add_seed({0, 1, 2, 3, 4});
    o.add_generator(Perm({1, 0, 2, 3, 4, 5, 6, 7, 8, 9}));
    o.add_generator(Perm({1, 2, 3, 4, 5, 6, 7, 8, 9, 0}));
    o.size();
  }
}

BENCHMARK(BM_orb_perm_on_tuples_vector)->Unit(benchmark::kMillisecond);

void BM_orb_perm_on_tuples_array(benchmark::State& st) {
  using Orb = libsemigroups::Orb<
      Perm,
      std::array<u_int8_t, 5>,
      libsemigroups::on_tuples<Perm, u_int8_t, std::array<u_int8_t, 5>>>;
  for (auto _ : st) {
    Orb o;
    o.add_seed({0, 1, 2, 3, 4});
    o.add_generator(Perm({1, 0, 2, 3, 4, 5, 6, 7, 8, 9}));
    o.add_generator(Perm({1, 2, 3, 4, 5, 6, 7, 8, 9, 0}));
    o.size();
  }
}

BENCHMARK(BM_orb_perm_on_tuples_array)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
