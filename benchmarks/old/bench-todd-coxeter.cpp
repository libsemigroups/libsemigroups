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

// This file contains some benchmarks for:
// - libsemigroups/include/todd-coxeter.hpp
// - libsemigroups/src/todd-coxeter.cpp

#include <benchmark/benchmark.h>

#include "bench-main.hpp"
#include "todd-coxeter.hpp"

using congruence_kind              = libsemigroups::congruence_kind;
congruence_kind constexpr TWOSIDED = congruence_kind::twosided;

void BM_todd_coxeter_002(benchmark::State& st) {
  using ToddCoxeter = libsemigroups::congruence::ToddCoxeter;
  auto rg           = libsemigroups::ReportGuard();

  ToddCoxeter tc(twosided);
  tc.set_nr_generators(4);
  tc.add_pair({0, 0}, {0});
  tc.add_pair({1, 0}, {1});
  tc.add_pair({0, 1}, {1});
  tc.add_pair({2, 0}, {2});
  tc.add_pair({0, 2}, {2});
  tc.add_pair({3, 0}, {3});
  tc.add_pair({0, 3}, {3});
  tc.add_pair({1, 1}, {0});
  tc.add_pair({2, 3}, {0});
  tc.add_pair({2, 2, 2}, {0});
  tc.add_pair({1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2}, {0});
  tc.add_pair({1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3,
               1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3},
              {0});
  tc.nr_classes();

  // auto& S = tc.quotient_froidure_pin();
  // S.size();
  // S.nr_idempotents();
}

BENCHMARK_MAIN();

BENCHMARK(BM_todd_coxeter_002)->Unit(benchmark::kMillisecond);
