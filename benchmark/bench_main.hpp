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

#ifndef LIBSEMIGROUPS_BENCHMARK_BENCH_MAIN_HPP_
#define LIBSEMIGROUPS_BENCHMARK_BENCH_MAIN_HPP_

#define LIBSEMIGROUPS_BENCHMARK(name, mem_fn, sample)           \
  benchmark::RegisterBenchmark(name, [](benchmark::State& st) { \
    for (auto _ : st) {                                         \
      for (auto elem : sample) {                                \
        benchmark::DoNotOptimize(elem.mem_fn());                \
      }                                                         \
    }                                                           \
  })

#define LIBSEMIGROUPS_BENCHMARK2(string, name)                   \
  size_t init_##name##_benchmark();                              \
  BENCHMARK(init_##name##_benchmark); \
  size_t init_##name##_benchmark(benchmark::State& st) {         \
    for (auto _ : st)

#define LIBSEMIGROUPS_BENCHMARKS() static int init_bench()

#define LIBSEMIGROUPS_BENCHMARK_MAIN() \
  auto dummy = {init_bench()};         \
  BENCHMARK_MAIN();

#endif  // LIBSEMIGROUPS_BENCHMARK_BENCH_MAIN_HPP_
