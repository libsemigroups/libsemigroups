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

// This file contains some benchmarks for libsemigroups/include/digraph.hpp.

#include <iostream>

#include <benchmark/benchmark.h>

#include "bench-main.hpp"
#include "digraph.hpp"

using libsemigroups::ActionDigraph;

template <typename TIntType>
std::vector<ActionDigraph<TIntType>> make_sample(size_t nr,
                                                 size_t nr_nodes,
                                                 size_t nr_edges) {
  std::mt19937                         mt;
  std::vector<ActionDigraph<TIntType>> res;
  for (size_t i = 0; i < nr; ++i) {
    res.push_back(ActionDigraph<TIntType>::random(nr_nodes, nr_edges, mt));
  }
  return res;
}

void cycle(ActionDigraph<size_t>& digraph, size_t n) {
  size_t old_nodes = digraph.nr_nodes();
  digraph.add_nodes(n);
  for (size_t i = old_nodes; i < digraph.nr_nodes() - 1; ++i) {
    digraph.add_edge(i, i + 1, 0);
  }
  digraph.add_edge(digraph.nr_nodes() - 1, old_nodes, 0);
}

ActionDigraph<size_t> cycle(size_t n) {
  ActionDigraph<size_t> g(0, 1);
  cycle(g, n);
  return g;
}

ActionDigraph<size_t> cycles(size_t cycle_length, size_t nr) {
  ActionDigraph<size_t> g(0, 1);
  for (size_t i = 0; i < nr; ++i) {
    cycle(g, cycle_length);
  }
  return g;
}

std::vector<ActionDigraph<size_t>> sample_10_10_size_t
    = make_sample<size_t>(1000, 10, 10);
std::vector<ActionDigraph<size_t>> sample_100_10_size_t
    = make_sample<size_t>(1000, 100, 10);
std::vector<ActionDigraph<size_t>> sample_1000_10_size_t
    = make_sample<size_t>(1000, 1000, 10);

std::vector<ActionDigraph<size_t>> sample_100_20_size_t
    = make_sample<size_t>(1000, 100, 20);
std::vector<ActionDigraph<size_t>> sample_100_30_size_t
    = make_sample<size_t>(1000, 100, 30);

std::vector<ActionDigraph<u_int16_t>> sample_10_10_u_int16_t
    = make_sample<u_int16_t>(1000, 10, 10);
std::vector<ActionDigraph<u_int16_t>> sample_100_10_u_int16_t
    = make_sample<u_int16_t>(1000, 100, 10);
std::vector<ActionDigraph<u_int16_t>> sample_1000_10_u_int16_t
    = make_sample<u_int16_t>(1000, 1000, 10);

std::vector<ActionDigraph<size_t>> sample_cycles_10_10_size_t(1000,
                                                              cycles(10, 10));
std::vector<ActionDigraph<size_t>> sample_cycles_100_10_size_t(1000,
                                                               cycles(100, 10));
std::vector<ActionDigraph<size_t>> sample_cycles_1000_10_size_t(1000,
                                                                cycles(1000,
                                                                       10));

LIBSEMIGROUPS_BENCHMARKS() {
  LIBSEMIGROUPS_BENCHMARK(
      "ActionDigraph<size_t>::nr_scc() 10 nodes, out-degree 10",
      nr_scc,
      sample_10_10_size_t)
      ->Unit(benchmark::kMicrosecond);
  LIBSEMIGROUPS_BENCHMARK(
      "ActionDigraph<size_t>::nr_scc() 100 nodes, out-degree 10",
      nr_scc,
      sample_100_10_size_t)
      ->Unit(benchmark::kMicrosecond);
  LIBSEMIGROUPS_BENCHMARK(
      "ActionDigraph<size_t>::nr_scc() 1000 nodes, out-degree 10",
      nr_scc,
      sample_1000_10_size_t)
      ->Unit(benchmark::kMicrosecond);
  LIBSEMIGROUPS_BENCHMARK(
      "ActionDigraph<u_int16_t>::nr_scc() 10 nodes, out-degree 10",
      nr_scc,
      sample_10_10_u_int16_t)
      ->Unit(benchmark::kMicrosecond);
  LIBSEMIGROUPS_BENCHMARK(
      "ActionDigraph<u_int16_t>::nr_scc() 100 nodes, out-degree 10",
      nr_scc,
      sample_100_10_u_int16_t)
      ->Unit(benchmark::kMicrosecond);
  LIBSEMIGROUPS_BENCHMARK(
      "ActionDigraph<u_int16_t>::nr_scc() 1000 nodes, out-degree 10",
      nr_scc,
      sample_1000_10_u_int16_t)
      ->Unit(benchmark::kMicrosecond);
  LIBSEMIGROUPS_BENCHMARK(
      "ActionDigraph<size_t>::nr_scc() 100 nodes, out-degree 10",
      nr_scc,
      sample_100_10_size_t)
      ->Unit(benchmark::kMicrosecond);
  LIBSEMIGROUPS_BENCHMARK(
      "ActionDigraph<size_t>::nr_scc() 100 nodes, out-degree 20",
      nr_scc,
      sample_100_20_size_t)
      ->Unit(benchmark::kMicrosecond);
  LIBSEMIGROUPS_BENCHMARK(
      "ActionDigraph<size_t>::nr_scc() 100 nodes, out-degree 30",
      nr_scc,
      sample_100_30_size_t)
      ->Unit(benchmark::kMicrosecond);
  LIBSEMIGROUPS_BENCHMARK(
      "ActionDigraph<size_t>::nr_scc() 10 disjoint cycles of length 10",
      nr_scc,
      sample_cycles_10_10_size_t)
      ->Unit(benchmark::kMicrosecond);
  LIBSEMIGROUPS_BENCHMARK(
      "ActionDigraph<size_t>::nr_scc() 100 disjoint cycles of length 10",
      nr_scc,
      sample_cycles_100_10_size_t)
      ->Unit(benchmark::kMicrosecond);
  LIBSEMIGROUPS_BENCHMARK(
      "ActionDigraph<size_t>::nr_scc() 1000 disjoint cycles of length 10",
      nr_scc,
      sample_cycles_1000_10_size_t)
      ->Unit(benchmark::kMicrosecond);
  LIBSEMIGROUPS_BENCHMARK(
      "ActionDigraph<size_t>::spanning_forest() 10 nodes, out-degree 10",
      spanning_forest,
      sample_10_10_size_t)
      ->Unit(benchmark::kMicrosecond);
  LIBSEMIGROUPS_BENCHMARK(
      "ActionDigraph<size_t>::spanning_forest() 100 nodes, out-degree 10",
      spanning_forest,
      sample_100_10_size_t)
      ->Unit(benchmark::kMicrosecond);
  LIBSEMIGROUPS_BENCHMARK(
      "ActionDigraph<size_t>::spanning_forest() 1000 nodes, out-degree 10",
      spanning_forest,
      sample_1000_10_size_t)
      ->Unit(benchmark::kMicrosecond);
  LIBSEMIGROUPS_BENCHMARK(
      "ActionDigraph<u_int16_t>::spanning_forest() 10 nodes, out-degree 10",
      spanning_forest,
      sample_10_10_u_int16_t)
      ->Unit(benchmark::kMicrosecond);
  LIBSEMIGROUPS_BENCHMARK(
      "ActionDigraph<u_int16_t>::spanning_forest() 100 nodes, out-degree 10",
      spanning_forest,
      sample_100_10_u_int16_t)
      ->Unit(benchmark::kMicrosecond);
  LIBSEMIGROUPS_BENCHMARK(
      "ActionDigraph<u_int16_t>::spanning_forest() 1000 nodes, out-degree 10",
      spanning_forest,
      sample_1000_10_u_int16_t)
      ->Unit(benchmark::kMicrosecond);
  LIBSEMIGROUPS_BENCHMARK(
      "ActionDigraph<size_t>::spanning_forest() 100 nodes, out-degree 10",
      spanning_forest,
      sample_100_10_size_t)
      ->Unit(benchmark::kMicrosecond);
  LIBSEMIGROUPS_BENCHMARK(
      "ActionDigraph<size_t>::spanning_forest() 100 nodes, out-degree 20",
      spanning_forest,
      sample_100_20_size_t)
      ->Unit(benchmark::kMicrosecond);
  LIBSEMIGROUPS_BENCHMARK(
      "ActionDigraph<size_t>::spanning_forest() 100 nodes, out-degree 30",
      spanning_forest,
      sample_100_30_size_t)
      ->Unit(benchmark::kMicrosecond);
  LIBSEMIGROUPS_BENCHMARK("ActionDigraph<size_t>::spanning_forest() 10 "
                          "disjoint cycles of length 10",
                          spanning_forest,
                          sample_cycles_10_10_size_t)
      ->Unit(benchmark::kMicrosecond);
  LIBSEMIGROUPS_BENCHMARK("ActionDigraph<size_t>::spanning_forest() 100 "
                          "disjoint cycles of length 10",
                          spanning_forest,
                          sample_cycles_100_10_size_t)
      ->Unit(benchmark::kMicrosecond);
  LIBSEMIGROUPS_BENCHMARK("ActionDigraph<size_t>::spanning_forest() 1000 "
                          "disjoint cycles of length 10",
                          spanning_forest,
                          sample_cycles_1000_10_size_t)
      ->Unit(benchmark::kMicrosecond);
  return 0;
}
LIBSEMIGROUPS_BENCHMARK_MAIN();
