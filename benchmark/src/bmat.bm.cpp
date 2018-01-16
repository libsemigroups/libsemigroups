//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2017 James D. Mitchell
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

// This file contains some benchmarks for libsemigroups/src/bmat.*.

#include <chrono>

#include <benchmark/benchmark.h>
#include <libsemigroups/bmat.h>

using namespace libsemigroups;

static void BM_BMat8_transpose_random_8(benchmark::State& state) {
  while (state.KeepRunning()) {
    BMat8 bm    = BMat8::random();
    auto  start = std::chrono::high_resolution_clock::now();
    bm.transpose();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
  }
}

BENCHMARK(BM_BMat8_transpose_random_8)->UseManualTime()->MinTime(1);

static void BM_BMat8_transpose_random_4(benchmark::State& state) {
  while (state.KeepRunning()) {
    BMat8 bm    = BMat8::random(4);
    auto  start = std::chrono::high_resolution_clock::now();
    bm.transpose();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
  }
}

BENCHMARK(BM_BMat8_transpose_random_4)->UseManualTime()->MinTime(1);

static void BM_BMat8_multiply(benchmark::State& state) {
  while (state.KeepRunning()) {
    BMat8 bm1   = BMat8::random();
    BMat8 bm2   = BMat8::random();
    auto  start = std::chrono::high_resolution_clock::now();
    BMat8 bm3   = bm1 * bm2;
    auto  end   = std::chrono::high_resolution_clock::now();
    auto  elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
  }
}

BENCHMARK(BM_BMat8_multiply)->UseManualTime()->MinTime(1);

BENCHMARK_MAIN();
