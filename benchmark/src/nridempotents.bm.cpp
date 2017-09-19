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

// This file contains some benchmarks for the nridempotents method for the
// Semigroup class defined in libsemigroups/src/semigroups.cc.
//
// Numbers in a suffix of a function name refer to the number of the example in
// libsemigroups/test/semigroups.test.cc

#include <benchmark/benchmark.h>
#include <libsemigroups/semigroups.h>

#include "examples.h"

using namespace libsemigroups;

#define BM_NRIDEMPOTENTS_THREADS(name, gens, n, m)                             \
  static void name(benchmark::State& state) {                                  \
    while (state.KeepRunning()) {                                              \
      Semigroup S(gens);                                                       \
      really_delete_cont(gens);                                                \
      S.reserve(n);                                                            \
      if (S.size() != n) {                                                     \
        throw std::runtime_error("wrong reserve size, expected "               \
                                 + std::to_string(S.size())                    \
                                 + ", got "                                    \
                                 + std::to_string(n));                         \
      }                                                                        \
      S.set_max_threads(state.range(0));                                       \
      auto start = std::chrono::high_resolution_clock::now();                  \
      S.nridempotents();                                                       \
      auto end = std::chrono::high_resolution_clock::now();                    \
      auto elapsed_seconds                                                     \
          = std::chrono::duration_cast<std::chrono::duration<double>>(         \
              end - start);                                                    \
      state.SetIterationTime(elapsed_seconds.count());                         \
      if (S.nridempotents() != m) {                                            \
        throw std::runtime_error(                                              \
            "wrong number of idempotents found, expected " + std::to_string(m) \
            + ", got "                                                         \
            + std::to_string(S.nridempotents()));                              \
      }                                                                        \
    }                                                                          \
  }                                                                            \
  BENCHMARK(name)                                                              \
      ->Repetitions(1)                                                         \
      ->MinTime(0.100)                                                         \
      ->Unit(benchmark::kMillisecond)                                          \
      ->UseManualTime()                                                        \
      ->RangeMultiplier(2)                                                     \
      ->Range(1, std::thread::hardware_concurrency())

#define BM_NRIDEMPOTENTS_NO_THREADS(name, gens, n, m)                          \
  static void name(benchmark::State& state) {                                  \
    while (state.KeepRunning()) {                                              \
      Semigroup S(gens);                                                       \
      really_delete_cont(gens);                                                \
      S.reserve(n);                                                            \
      if (S.size() != n) {                                                     \
        throw std::runtime_error("wrong reserve size, expected "               \
                                 + std::to_string(S.size())                    \
                                 + ", got "                                    \
                                 + std::to_string(n));                         \
      }                                                                        \
      auto start = std::chrono::high_resolution_clock::now();                  \
      S.nridempotents();                                                       \
      auto end = std::chrono::high_resolution_clock::now();                    \
      auto elapsed_seconds                                                     \
          = std::chrono::duration_cast<std::chrono::duration<double>>(         \
              end - start);                                                    \
      state.SetIterationTime(elapsed_seconds.count());                         \
      if (S.nridempotents() != m) {                                            \
        throw std::runtime_error(                                              \
            "wrong number of idempotents found, expected " + std::to_string(m) \
            + ", got "                                                         \
            + std::to_string(S.nridempotents()));                              \
      }                                                                        \
    }                                                                          \
  }                                                                            \
  BENCHMARK(name)                                                              \
      ->Repetitions(1)                                                         \
      ->MinTime(0.100)                                                         \
      ->Unit(benchmark::kMillisecond)                                          \
      ->UseManualTime()

Semiring<int64_t>* sr1 = new TropicalMinPlusSemiring(11);
BM_NRIDEMPOTENTS_NO_THREADS(
    BM_nridempotents_10,
    std::vector<Element*>(
        {new MatrixOverSemiring<int64_t>({{2, 1, 0}, {10, 0, 0}, {1, 2, 1}},
                                         sr1),
         new MatrixOverSemiring<int64_t>({{10, 0, 0}, {0, 1, 0}, {1, 1, 0}},
                                         sr1)}),
    1039,
    5);

BM_NRIDEMPOTENTS_NO_THREADS(
    BM_nridempotents_62,
    std::vector<Element*>(
        {new Transformation<uint_fast8_t>({1, 7, 2, 6, 0, 4, 1, 5}),
         new Transformation<uint_fast8_t>({2, 4, 6, 1, 4, 5, 2, 7}),
         new Transformation<uint_fast8_t>({3, 0, 7, 2, 4, 6, 2, 4}),
         new Transformation<uint_fast8_t>({3, 2, 3, 4, 5, 3, 0, 1}),
         new Transformation<uint_fast8_t>({4, 3, 7, 7, 4, 5, 0, 4}),
         new Transformation<uint_fast8_t>({5, 6, 3, 0, 3, 0, 5, 1}),
         new Transformation<uint_fast8_t>({6, 0, 1, 1, 1, 6, 3, 4}),
         new Transformation<uint_fast8_t>({7, 7, 4, 0, 6, 4, 1, 7})}),
    597369,
    8194);

BM_NRIDEMPOTENTS_THREADS(
    BM_nridempotents_full_trans_8,
    std::vector<Element*>(
        {new Transformation<uint_fast8_t>({1, 2, 3, 4, 5, 6, 7, 0}),
         new Transformation<uint_fast8_t>({1, 0, 2, 3, 4, 5, 6, 7}),
         new Transformation<uint_fast8_t>({0, 1, 2, 3, 4, 5, 6, 0})}),
    std::pow(8, 8),
    41393);

Semiring<int64_t>* sr2 = new NaturalSemiring(0, 6);
BM_NRIDEMPOTENTS_THREADS(
    BM_nridempotents_nat_mat,
    std::vector<Element*>(
        {new MatrixOverSemiring<int64_t>({{0, 0, 1}, {0, 1, 0}, {1, 1, 0}},
                                         sr2),
         new MatrixOverSemiring<int64_t>({{0, 0, 1}, {0, 1, 0}, {2, 0, 0}},
                                         sr2),
         new MatrixOverSemiring<int64_t>({{0, 0, 1}, {0, 1, 1}, {1, 0, 0}},
                                         sr2),
         new MatrixOverSemiring<int64_t>({{0, 0, 1}, {0, 1, 0}, {3, 0, 0}},
                                         sr2)}),
    10077696,
    13688);

BM_NRIDEMPOTENTS_THREADS(BM_nridempotents_uppertri_6,
                         upper_triangular_boolean_mat(6),
                         2097152,
                         114433);

BM_NRIDEMPOTENTS_THREADS(BM_nridempotents_unitri_7,
                         uni_triangular_boolean_mat(7),
                         2097152,
                         96428);

BM_NRIDEMPOTENTS_THREADS(BM_nridempotents_gossip_6, gossip(6), 1092473, 203);

BM_NRIDEMPOTENTS_THREADS(
    BM_nridempotents_symm_inv_8,
    std::vector<Element*>(
        {new PartialPerm<uint_fast8_t>({0, 1, 2, 3, 4, 5, 6, 7},
                                       {1, 2, 3, 4, 5, 6, 7, 0},
                                       8),
         new PartialPerm<uint_fast8_t>({0, 1, 2, 3, 4, 5, 6, 7},
                                       {1, 0, 2, 3, 4, 5, 6, 7},
                                       8),
         new PartialPerm<uint_fast8_t>({1, 2, 3, 4, 5, 6, 7},
                                       {0, 1, 2, 3, 4, 5, 6},
                                       8),
         new PartialPerm<uint_fast8_t>({0, 1, 2, 3, 4, 5, 6},
                                       {1, 2, 3, 4, 5, 6, 7},
                                       8)}),
    1441729,
    256);

BM_NRIDEMPOTENTS_THREADS(
    BM_nridempotents_partition_6,
    std::vector<Element*>(
        {new Bipartition({0, 1, 2, 3, 4, 5, 5, 0, 1, 2, 3, 4}),
         new Bipartition({0, 1, 2, 3, 4, 5, 1, 0, 2, 3, 4, 5}),
         new Bipartition({0, 1, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5}),
         new Bipartition({0, 0, 1, 2, 3, 4, 0, 0, 1, 2, 3, 4})}),
    4213597,
    541254);

BENCHMARK_MAIN();
