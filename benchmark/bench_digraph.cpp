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

// This file contains some benchmarks for libsemigroups/include/digraph.hpp.

#include <benchmark/benchmark.h>

#include "digraph.hpp"

static void BM_DIGRAPH_CONSTRUCTOR(benchmark::State& state) {
  while (state.KeepRunning()) {
    libsemigroups::ActionDigraph<size_t> g(10, 10);
  }
}

BENCHMARK(BM_DIGRAPH_CONSTRUCTOR);

BENCHMARK_MAIN();
