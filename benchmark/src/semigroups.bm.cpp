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

// This file contains some benchmarks for libsemigroups/src/semigroups.cc.
//
// Numbers in a suffix of a function name refer to the number of the example in
// libsemigroups/test/semigroups.test.cc

#include <benchmark/benchmark.h>
#include <libsemigroups/semigroups.h>

using namespace libsemigroups;

typedef std::vector<std::vector<u_int32_t>> PBR_Input;

// The first few examples cover all different types of Element's and benchmark
// the performance of enumerating many small semigroups.

static void BM_size_small_trans_01(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<Element*> gens = {new Transformation<u_int16_t>({0, 1, 0}),
                                  new Transformation<u_int16_t>({0, 1, 2})};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);

    S.size();
  }
}

BENCHMARK(BM_size_small_trans_01)->MinTime(1)->Unit(benchmark::kMicrosecond);

static void BM_size_small_pperm_02(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<Element*> gens
        = {new PartialPerm<u_int16_t>(
               {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 10),
           new PartialPerm<u_int16_t>({4, 5, 0}, {10, 0, 1}, 10)};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);

    S.size();
  }
}

BENCHMARK(BM_size_small_pperm_02)->MinTime(1)->Unit(benchmark::kMicrosecond);

static void BM_size_small_bipart_03(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<Element*> gens
        = {new Bipartition(
               {0, 1, 2, 1, 0, 2, 1, 0, 2, 2, 0, 0, 2, 0, 3, 4, 4, 1, 3, 0}),
           new Bipartition(
               {0, 1, 1, 1, 1, 2, 3, 2, 4, 5, 5, 2, 4, 2, 1, 1, 1, 2, 3, 2}),
           new Bipartition(
               {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0})};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);
    S.size();
  }
}

BENCHMARK(BM_size_small_bipart_03)->MinTime(1)->Unit(benchmark::kMicrosecond);

static void BM_size_small_bmat_04(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<Element*> gens
        = {new BooleanMat({{1, 0, 1}, {0, 1, 0}, {0, 1, 0}}),
           new BooleanMat({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}),
           new BooleanMat({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}})};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);
    S.size();
  }
}

BENCHMARK(BM_size_small_bmat_04)->MinTime(1)->Unit(benchmark::kMicrosecond);

static void BM_size_small_proj_max_plus_05(benchmark::State& state) {
  while (state.KeepRunning()) {
    Semiring* sr = new MaxPlusSemiring();
    auto x = new ProjectiveMaxPlusMatrix({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, sr);
    auto id = x->identity();
    x->really_delete();
    delete x;
    Semigroup S = Semigroup({id});
    S.size();
    id->really_delete();
    delete id;
    delete sr;
  }
}

BENCHMARK(BM_size_small_proj_max_plus_05)
    ->MinTime(1)
    ->Unit(benchmark::kMicrosecond);

static void BM_size_small_int_mat_06(benchmark::State& state) {
  while (state.KeepRunning()) {
    Semiring*             sr = new Integers();
    std::vector<Element*> gens
        = {new MatrixOverSemiring({{0, 0}, {0, 1}}, sr),
           new MatrixOverSemiring({{0, 1}, {-1, 0}}, sr)};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);
    S.size();
    delete sr;
  }
}

BENCHMARK(BM_size_small_int_mat_06)->MinTime(1)->Unit(benchmark::kMicrosecond);

static void BM_size_small_max_plus_07(benchmark::State& state) {
  while (state.KeepRunning()) {
    Semiring*             sr = new MaxPlusSemiring();
    std::vector<Element*> gens
        = {new MatrixOverSemiring({{0, -4}, {-4, -1}}, sr),
           new MatrixOverSemiring({{0, -3}, {-3, -1}}, sr)};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);
    S.size();
    delete sr;
  }
}

BENCHMARK(BM_size_small_max_plus_07)->MinTime(1)->Unit(benchmark::kMicrosecond);

static void BM_size_small_min_plus_08(benchmark::State& state) {
  while (state.KeepRunning()) {
    Semiring*             sr = new MinPlusSemiring();
    std::vector<Element*> gens
        = {new MatrixOverSemiring({{1, 0}, {0, Semiring::INFTY}}, sr)};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);
    S.size();
    delete sr;
  }
}

BENCHMARK(BM_size_small_min_plus_08)->MinTime(1)->Unit(benchmark::kMicrosecond);

static void BM_size_small_trop_max_plus_09(benchmark::State& state) {
  while (state.KeepRunning()) {
    Semiring*             sr = new TropicalMaxPlusSemiring(33);
    std::vector<Element*> gens
        = {new MatrixOverSemiring({{22, 21, 0}, {10, 0, 0}, {1, 32, 1}}, sr),
           new MatrixOverSemiring({{0, 0, 0}, {0, 1, 0}, {1, 1, 0}}, sr)};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);
    S.size();
    delete sr;
  }
}

BENCHMARK(BM_size_small_trop_max_plus_09)
    ->MinTime(1)
    ->Unit(benchmark::kMicrosecond);

static void BM_size_small_trop_min_plus_10(benchmark::State& state) {
  while (state.KeepRunning()) {
    Semiring*             sr = new TropicalMinPlusSemiring(11);
    std::vector<Element*> gens
        = {new MatrixOverSemiring({{2, 1, 0}, {10, 0, 0}, {1, 2, 1}}, sr),
           new MatrixOverSemiring({{10, 0, 0}, {0, 1, 0}, {1, 1, 0}}, sr)};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);
    S.size();
    delete sr;
  }
}

BENCHMARK(BM_size_small_trop_min_plus_10)
    ->MinTime(1)
    ->Unit(benchmark::kMicrosecond);

static void BM_size_small_nat_mat_11(benchmark::State& state) {
  while (state.KeepRunning()) {
    Semiring*             sr = new NaturalSemiring(11, 3);
    std::vector<Element*> gens
        = {new MatrixOverSemiring({{2, 1, 0}, {10, 0, 0}, {1, 2, 1}}, sr),
           new MatrixOverSemiring({{10, 0, 0}, {0, 1, 0}, {1, 1, 0}}, sr)};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);
    S.size();
    delete sr;
  }
}

BENCHMARK(BM_size_small_nat_mat_11)->MinTime(1)->Unit(benchmark::kMicrosecond);

static void BM_size_small_pbr_12(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<Element*> gens
        = {new PBR(new PBR_Input({{1}, {4}, {3}, {1}, {0, 2}, {0, 3, 4, 5}})),
           new PBR(new PBR_Input(
               {{1, 2}, {0, 1}, {0, 2, 3}, {0, 1, 2}, {3}, {0, 3, 4, 5}}))};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);

    S.size();
  }
}

BENCHMARK(BM_size_small_pbr_12)->MinTime(1)->Unit(benchmark::kMicrosecond);

///////////////////////////////////////////////////////////////////////////////
// The following benchmark the performance of computing the size of specific
// large examples, at least one for each type of Element object.
///////////////////////////////////////////////////////////////////////////////

static void BM_size_no_reserve_62(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<Element*> gens
        = {new Transformation<uint_fast8_t>({1, 7, 2, 6, 0, 4, 1, 5}),
           new Transformation<uint_fast8_t>({2, 4, 6, 1, 4, 5, 2, 7}),
           new Transformation<uint_fast8_t>({3, 0, 7, 2, 4, 6, 2, 4}),
           new Transformation<uint_fast8_t>({3, 2, 3, 4, 5, 3, 0, 1}),
           new Transformation<uint_fast8_t>({4, 3, 7, 7, 4, 5, 0, 4}),
           new Transformation<uint_fast8_t>({5, 6, 3, 0, 3, 0, 5, 1}),
           new Transformation<uint_fast8_t>({6, 0, 1, 1, 1, 6, 3, 4}),
           new Transformation<uint_fast8_t>({7, 7, 4, 0, 6, 4, 1, 7})};

    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);

    auto start = std::chrono::high_resolution_clock::now();
    S.size();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
  }
}

BENCHMARK(BM_size_no_reserve_62)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(10)
    ->UseManualTime();

static void BM_size_reserve_62(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<Element*> gens
        = {new Transformation<uint_fast8_t>({1, 7, 2, 6, 0, 4, 1, 5}),
           new Transformation<uint_fast8_t>({2, 4, 6, 1, 4, 5, 2, 7}),
           new Transformation<uint_fast8_t>({3, 0, 7, 2, 4, 6, 2, 4}),
           new Transformation<uint_fast8_t>({3, 2, 3, 4, 5, 3, 0, 1}),
           new Transformation<uint_fast8_t>({4, 3, 7, 7, 4, 5, 0, 4}),
           new Transformation<uint_fast8_t>({5, 6, 3, 0, 3, 0, 5, 1}),
           new Transformation<uint_fast8_t>({6, 0, 1, 1, 1, 6, 3, 4}),
           new Transformation<uint_fast8_t>({7, 7, 4, 0, 6, 4, 1, 7})};

    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);

    auto start = std::chrono::high_resolution_clock::now();
    S.reserve(597369);
    S.size();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
  }
}

BENCHMARK(BM_size_reserve_62)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(10)
    ->UseManualTime();

static void BM_size_no_reserve_full_trans_8(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<Element*> gens
        = {new Transformation<uint_fast8_t>({1, 2, 3, 4, 5, 6, 7, 0}),
           new Transformation<uint_fast8_t>({1, 0, 2, 3, 4, 5, 6, 7}),
           new Transformation<uint_fast8_t>({0, 1, 2, 3, 4, 5, 6, 0})};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);
    auto start = std::chrono::high_resolution_clock::now();
    S.size();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
  }
}

BENCHMARK(BM_size_no_reserve_full_trans_8)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(1)
    ->UseManualTime();

static void BM_size_reserve_full_trans_8(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<Element*> gens
        = {new Transformation<uint_fast8_t>({1, 2, 3, 4, 5, 6, 7, 0}),
           new Transformation<uint_fast8_t>({1, 0, 2, 3, 4, 5, 6, 7}),
           new Transformation<uint_fast8_t>({0, 1, 2, 3, 4, 5, 6, 0})};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);
    auto start = std::chrono::high_resolution_clock::now();
    S.reserve(std::pow(8, 8));
    S.size();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
  }
}

BENCHMARK(BM_size_reserve_full_trans_8)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(1)
    ->UseManualTime();

static void BM_size_no_reserve_uppertri_6(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<Element*> gens = {new BooleanMat({{1, 1, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 1, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 1, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 1, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 1},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 1, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 1, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 1, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 1},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 1, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 1, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 1},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 1, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 1},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 1},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{0, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0}})};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);
    auto start = std::chrono::high_resolution_clock::now();
    S.size();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
  }
}

BENCHMARK(BM_size_no_reserve_uppertri_6)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(1)
    ->UseManualTime();

static void BM_size_reserve_uppertri_6(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<Element*> gens = {new BooleanMat({{1, 1, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 1, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 1, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 1, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 1},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 1, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 1, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 1, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 1},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 1, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 1, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 1},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 1, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 1},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 1},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{0, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0}})};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);
    auto start = std::chrono::high_resolution_clock::now();
    S.reserve(2097152);
    S.size();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
  }
}

BENCHMARK(BM_size_reserve_uppertri_6)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(1)
    ->UseManualTime();

static void BM_size_no_reserve_unitri_7(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<Element*> gens = {new BooleanMat({{1, 1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 1, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 1, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 1, 0, 0},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 1, 0},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 1},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 0},
                                                  {0, 1, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 1, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 1, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 1, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0, 1},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 1, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 1, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 1},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 1, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 1},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 1, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 1},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 1},
                                                  {0, 0, 0, 0, 0, 0, 1}})};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);
    auto start = std::chrono::high_resolution_clock::now();
    S.size();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
  }
}

BENCHMARK(BM_size_no_reserve_unitri_7)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(1)
    ->UseManualTime();

static void BM_size_reserve_unitri_7(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<Element*> gens = {new BooleanMat({{1, 1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 1, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 1, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 1, 0, 0},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 1, 0},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 1},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 0},
                                                  {0, 1, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 1, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 1, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 1, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0, 1},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 1, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 1, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 1},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 1, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 1},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 1, 0},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 1},
                                                  {0, 0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 0, 1, 1},
                                                  {0, 0, 0, 0, 0, 0, 1}})};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);
    auto start = std::chrono::high_resolution_clock::now();
    S.reserve(2097152);
    S.size();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
  }
}

BENCHMARK(BM_size_reserve_unitri_7)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(1)
    ->UseManualTime();

static void BM_size_no_reserve_gossip_6(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<Element*> gens = {new BooleanMat({{1, 1, 0, 0, 0, 0},
                                                  {1, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 1, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {1, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 1, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {1, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 1, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {1, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 1},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {1, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 1, 0, 0, 0},
                                                  {0, 1, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 1, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 1, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 1, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 1, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 1},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 1, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 1, 0, 0},
                                                  {0, 0, 1, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 1, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 1, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 1},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 1, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 1, 0},
                                                  {0, 0, 0, 1, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 1},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 1, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 1},
                                                  {0, 0, 0, 0, 1, 1}})};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);
    auto start = std::chrono::high_resolution_clock::now();
    S.size();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
  }
}

BENCHMARK(BM_size_no_reserve_gossip_6)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(1)
    ->UseManualTime();

static void BM_size_reserve_gossip_6(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<Element*> gens = {new BooleanMat({{1, 1, 0, 0, 0, 0},
                                                  {1, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 1, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {1, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 1, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {1, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 1, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {1, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 1},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {1, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 1, 0, 0, 0},
                                                  {0, 1, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 1, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 1, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 1, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 1, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 1},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 1, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 1, 0, 0},
                                                  {0, 0, 1, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 1, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 1, 0, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 1},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 1, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 1, 0},
                                                  {0, 0, 0, 1, 1, 0},
                                                  {0, 0, 0, 0, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 1},
                                                  {0, 0, 0, 0, 1, 0},
                                                  {0, 0, 0, 1, 0, 1}}),
                                  new BooleanMat({{1, 0, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0, 0},
                                                  {0, 0, 0, 1, 0, 0},
                                                  {0, 0, 0, 0, 1, 1},
                                                  {0, 0, 0, 0, 1, 1}})};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);
    auto start = std::chrono::high_resolution_clock::now();
    S.reserve(1092473);
    S.size();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
  }
}

BENCHMARK(BM_size_reserve_gossip_6)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(1)
    ->UseManualTime();

static void BM_size_no_reserve_reg_bool_4(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<Element*> gens
        = {new BooleanMat(
               {{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           new BooleanMat(
               {{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           new BooleanMat(
               {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
           new BooleanMat(
               {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);
    auto start = std::chrono::high_resolution_clock::now();
    S.size();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
  }
}

BENCHMARK(BM_size_no_reserve_reg_bool_4)
    ->Unit(benchmark::kMillisecond)
    ->MinTime(1)
    ->UseManualTime();

static void BM_size_reserve_reg_bool_4(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<Element*> gens
        = {new BooleanMat(
               {{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           new BooleanMat(
               {{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           new BooleanMat(
               {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
           new BooleanMat(
               {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);
    auto start = std::chrono::high_resolution_clock::now();
    S.reserve(65535);
    S.size();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
  }
}

BENCHMARK(BM_size_reserve_reg_bool_4)
    ->Unit(benchmark::kMillisecond)
    ->MinTime(1)
    ->UseManualTime();

static void BM_size_no_reserve_symm_inv_8(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<Element*> gens
        = {new PartialPerm<uint_fast8_t>(
               {0, 1, 2, 3, 4, 5, 6, 7}, {1, 2, 3, 4, 5, 6, 7, 0}, 8),
           new PartialPerm<uint_fast8_t>(
               {0, 1, 2, 3, 4, 5, 6, 7}, {1, 0, 2, 3, 4, 5, 6, 7}, 8),
           new PartialPerm<uint_fast8_t>(
               {1, 2, 3, 4, 5, 6, 7}, {0, 1, 2, 3, 4, 5, 6}, 8),
           new PartialPerm<uint_fast8_t>(
               {0, 1, 2, 3, 4, 5, 6}, {1, 2, 3, 4, 5, 6, 7}, 8)};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);
    auto start = std::chrono::high_resolution_clock::now();
    S.size();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
  }
}

BENCHMARK(BM_size_no_reserve_symm_inv_8)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(4)
    ->UseManualTime();

static void BM_size_reserve_symm_inv_8(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<Element*> gens
        = {new PartialPerm<uint_fast8_t>(
               {0, 1, 2, 3, 4, 5, 6, 7}, {1, 2, 3, 4, 5, 6, 7, 0}, 8),
           new PartialPerm<uint_fast8_t>(
               {0, 1, 2, 3, 4, 5, 6, 7}, {1, 0, 2, 3, 4, 5, 6, 7}, 8),
           new PartialPerm<uint_fast8_t>(
               {1, 2, 3, 4, 5, 6, 7}, {0, 1, 2, 3, 4, 5, 6}, 8),
           new PartialPerm<uint_fast8_t>(
               {0, 1, 2, 3, 4, 5, 6}, {1, 2, 3, 4, 5, 6, 7}, 8)};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);
    auto start = std::chrono::high_resolution_clock::now();
    S.reserve(1441729);
    S.size();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
  }
}

BENCHMARK(BM_size_reserve_symm_inv_8)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(4)
    ->UseManualTime();

static void BM_size_no_reserve_partition_6(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<Element*> gens
        = {new Bipartition({0, 1, 2, 3, 4, 5, 5, 0, 1, 2, 3, 4}),
           new Bipartition({0, 1, 2, 3, 4, 5, 1, 0, 2, 3, 4, 5}),
           new Bipartition({0, 1, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5}),
           new Bipartition({0, 0, 1, 2, 3, 4, 0, 0, 1, 2, 3, 4})};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);
    auto start = std::chrono::high_resolution_clock::now();
    S.size();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
  }
}

BENCHMARK(BM_size_no_reserve_partition_6)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(1)
    ->UseManualTime();

static void BM_size_reserve_partition_6(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<Element*> gens
        = {new Bipartition({0, 1, 2, 3, 4, 5, 5, 0, 1, 2, 3, 4}),
           new Bipartition({0, 1, 2, 3, 4, 5, 1, 0, 2, 3, 4, 5}),
           new Bipartition({0, 1, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5}),
           new Bipartition({0, 0, 1, 2, 3, 4, 0, 0, 1, 2, 3, 4})};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);
    auto start = std::chrono::high_resolution_clock::now();
    S.reserve(4213597);
    S.size();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
  }
}

BENCHMARK(BM_size_reserve_partition_6)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(1)
    ->UseManualTime();

static void BM_size_no_reserve_proj_max_plus(benchmark::State& state) {
  while (state.KeepRunning()) {
    Semiring* sr = new MaxPlusSemiring();

    std::vector<Element*> gens
        = {new ProjectiveMaxPlusMatrix({{Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY},
                                        {0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY}},
                                       sr),
           new ProjectiveMaxPlusMatrix({{Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0}},
                                       sr),
           new ProjectiveMaxPlusMatrix({{Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY}},
                                       sr),
           new ProjectiveMaxPlusMatrix({{Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0},
                                        {0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0},
                                        {0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY}},
                                       sr),
           new ProjectiveMaxPlusMatrix({{Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY},
                                        {0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY}},
                                       sr)};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);

    auto start = std::chrono::high_resolution_clock::now();
    S.size();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
    delete sr;
  }
}

BENCHMARK(BM_size_no_reserve_proj_max_plus)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(1)
    ->UseManualTime();

static void BM_size_reserve_proj_max_plus(benchmark::State& state) {
  while (state.KeepRunning()) {
    Semiring* sr = new MaxPlusSemiring();

    std::vector<Element*> gens
        = {new ProjectiveMaxPlusMatrix({{Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY},
                                        {0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY}},
                                       sr),
           new ProjectiveMaxPlusMatrix({{Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0}},
                                       sr),
           new ProjectiveMaxPlusMatrix({{Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY}},
                                       sr),
           new ProjectiveMaxPlusMatrix({{Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0},
                                        {0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0},
                                        {0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY}},
                                       sr),
           new ProjectiveMaxPlusMatrix({{Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY},
                                        {0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY},
                                        {Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY,
                                         0,
                                         Semiring::MINUS_INFTY,
                                         Semiring::MINUS_INFTY}},
                                       sr)};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);

    auto start = std::chrono::high_resolution_clock::now();
    S.reserve(242931);
    S.size();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
    delete sr;
  }
}

BENCHMARK(BM_size_reserve_proj_max_plus)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(1)
    ->UseManualTime();

static void BM_size_no_reserve_int_mat(benchmark::State& state) {
  while (state.KeepRunning()) {
    Semiring*             sr = new Integers();
    std::vector<Element*> gens
        = {new MatrixOverSemiring({{2, 0, -1, 0, 0, 1, -3, 1, 0, 0},
                                   {0, 0, 1, 0, 1, -1, 0, -1, 0, 0},
                                   {2, -1, 0, 2, -2, 0, 3, 0, -1, 1},
                                   {-3, 0, 1, 0, -1, 3, 1, 0, -1, 0},
                                   {0, 0, -1, 2, 4, 0, 0, 4, 0, -1},
                                   {0, 1, 2, 2, 1, 4, -1, 2, 0, 0},
                                   {-1, -1, 0, 0, 0, 0, -1, 0, 0, 2},
                                   {1, 0, 0, -1, -3, -4, 0, 0, 2, 1},
                                   {-2, 0, 0, 0, 0, 0, -2, 2, 0, 3},
                                   {1, 4, -1, 0, 3, 0, 3, 1, -3, 0}},
                                  sr),
           new MatrixOverSemiring({{2, 1, 0, 0, -3, 2, -1, 0, 0, 5},
                                   {3, 3, -2, 1, 0, 0, 5, 1, 4, 1},
                                   {1, 0, 2, 0, -1, 0, 2, -1, 0, 3},
                                   {4, -1, 2, 0, 0, 0, 1, 0, 0, 2},
                                   {0, 0, 1, 2, -1, 1, 2, 0, 3, 0},
                                   {-1, -2, -1, 0, 0, -1, 2, 1, 0, 1},
                                   {3, 0, 0, 2, 2, -3, -1, 0, -1, -2},
                                   {2, 3, 1, 2, -2, 1, 1, 0, 1, 2},
                                   {0, 2, 2, -1, 2, 4, 0, 6, 2, -2},
                                   {3, -3, 3, 0, 0, 1, 0, 0, -1, 0}},
                                  sr),
           new MatrixOverSemiring({{1, -1, -1, 2, 2, 2, 0, 0, 0, -1},
                                   {-1, 0, 0, 2, 0, 4, -2, 1, 0, 3},
                                   {-1, 0, 0, 1, -2, 2, 0, -1, 3, 2},
                                   {1, 0, 0, -1, -1, -1, 4, -3, -3, 0},
                                   {3, -1, 0, -2, 2, 1, 2, 0, -1, 2},
                                   {3, 0, 3, -3, 1, -3, 4, 0, 3, 5},
                                   {0, -2, 5, -1, 0, 0, 3, 0, 0, -2},
                                   {0, -1, -2, -2, 1, 5, -1, -3, 2, 2},
                                   {0, 2, -3, 0, -2, 0, -1, -1, 6, 0},
                                   {1, 3, -2, 0, 0, 2, 2, 0, 0, 4}},
                                  sr),
           new MatrixOverSemiring({{1, 0, 0, 1, 0, 3, 0, 0, 4, 2},
                                   {0, -1, 1, 3, 2, 1, 0, 1, 4, 0},
                                   {1, 4, 0, -5, 4, 2, 2, 0, -3, 2},
                                   {0, -1, 3, 1, 1, 0, -1, 2, 3, 1},
                                   {0, 0, -1, 0, 2, -1, 0, -3, 0, 0},
                                   {0, 5, 0, -3, -4, 0, 1, 1, 2, 0},
                                   {-1, 0, 3, 0, 0, 2, -2, -2, 0, 1},
                                   {0, -4, -1, 0, -1, 0, 4, 0, 1, -1},
                                   {-2, -1, 0, 0, 0, 0, -2, 1, -2, -1},
                                   {2, -4, 3, 1, -1, 3, -1, 0, -1, 0}},
                                  sr)};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);

    auto start = std::chrono::high_resolution_clock::now();
    S.enumerate(300000);  // This is probably infinite we stop at 300000
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
    delete sr;
  }
}

BENCHMARK(BM_size_no_reserve_int_mat)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(1)
    ->UseManualTime();

static void BM_size_reserve_int_mat(benchmark::State& state) {
  while (state.KeepRunning()) {
    Semiring*             sr = new Integers();
    std::vector<Element*> gens
        = {new MatrixOverSemiring({{2, 0, -1, 0, 0, 1, -3, 1, 0, 0},
                                   {0, 0, 1, 0, 1, -1, 0, -1, 0, 0},
                                   {2, -1, 0, 2, -2, 0, 3, 0, -1, 1},
                                   {-3, 0, 1, 0, -1, 3, 1, 0, -1, 0},
                                   {0, 0, -1, 2, 4, 0, 0, 4, 0, -1},
                                   {0, 1, 2, 2, 1, 4, -1, 2, 0, 0},
                                   {-1, -1, 0, 0, 0, 0, -1, 0, 0, 2},
                                   {1, 0, 0, -1, -3, -4, 0, 0, 2, 1},
                                   {-2, 0, 0, 0, 0, 0, -2, 2, 0, 3},
                                   {1, 4, -1, 0, 3, 0, 3, 1, -3, 0}},
                                  sr),
           new MatrixOverSemiring({{2, 1, 0, 0, -3, 2, -1, 0, 0, 5},
                                   {3, 3, -2, 1, 0, 0, 5, 1, 4, 1},
                                   {1, 0, 2, 0, -1, 0, 2, -1, 0, 3},
                                   {4, -1, 2, 0, 0, 0, 1, 0, 0, 2},
                                   {0, 0, 1, 2, -1, 1, 2, 0, 3, 0},
                                   {-1, -2, -1, 0, 0, -1, 2, 1, 0, 1},
                                   {3, 0, 0, 2, 2, -3, -1, 0, -1, -2},
                                   {2, 3, 1, 2, -2, 1, 1, 0, 1, 2},
                                   {0, 2, 2, -1, 2, 4, 0, 6, 2, -2},
                                   {3, -3, 3, 0, 0, 1, 0, 0, -1, 0}},
                                  sr),
           new MatrixOverSemiring({{1, -1, -1, 2, 2, 2, 0, 0, 0, -1},
                                   {-1, 0, 0, 2, 0, 4, -2, 1, 0, 3},
                                   {-1, 0, 0, 1, -2, 2, 0, -1, 3, 2},
                                   {1, 0, 0, -1, -1, -1, 4, -3, -3, 0},
                                   {3, -1, 0, -2, 2, 1, 2, 0, -1, 2},
                                   {3, 0, 3, -3, 1, -3, 4, 0, 3, 5},
                                   {0, -2, 5, -1, 0, 0, 3, 0, 0, -2},
                                   {0, -1, -2, -2, 1, 5, -1, -3, 2, 2},
                                   {0, 2, -3, 0, -2, 0, -1, -1, 6, 0},
                                   {1, 3, -2, 0, 0, 2, 2, 0, 0, 4}},
                                  sr),
           new MatrixOverSemiring({{1, 0, 0, 1, 0, 3, 0, 0, 4, 2},
                                   {0, -1, 1, 3, 2, 1, 0, 1, 4, 0},
                                   {1, 4, 0, -5, 4, 2, 2, 0, -3, 2},
                                   {0, -1, 3, 1, 1, 0, -1, 2, 3, 1},
                                   {0, 0, -1, 0, 2, -1, 0, -3, 0, 0},
                                   {0, 5, 0, -3, -4, 0, 1, 1, 2, 0},
                                   {-1, 0, 3, 0, 0, 2, -2, -2, 0, 1},
                                   {0, -4, -1, 0, -1, 0, 4, 0, 1, -1},
                                   {-2, -1, 0, 0, 0, 0, -2, 1, -2, -1},
                                   {2, -4, 3, 1, -1, 3, -1, 0, -1, 0}},
                                  sr)};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);

    auto start = std::chrono::high_resolution_clock::now();
    S.reserve(320000);
    S.enumerate(300000);  // This is probably infinite we stop at 300000
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
    delete sr;
  }
}

BENCHMARK(BM_size_reserve_int_mat)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(1)
    ->UseManualTime();

static void BM_size_no_reserve_max_plus(benchmark::State& state) {
  while (state.KeepRunning()) {
    Semiring*             sr   = new MaxPlusSemiring();
    std::vector<Element*> gens = {
        new MatrixOverSemiring({{-1, 0, 2, 7, 5, 0, -1, -3},
                                {4,
                                 Semiring::MINUS_INFTY,
                                 2,
                                 0,
                                 -1,
                                 Semiring::MINUS_INFTY,
                                 Semiring::MINUS_INFTY,
                                 4},
                                {0, 0, Semiring::MINUS_INFTY, 1, 2, 2, 0, -2},
                                {2,
                                 Semiring::MINUS_INFTY,
                                 -1,
                                 0,
                                 Semiring::MINUS_INFTY,
                                 Semiring::MINUS_INFTY,
                                 3,
                                 0},
                                {2,
                                 Semiring::MINUS_INFTY,
                                 -1,
                                 -2,
                                 -2,
                                 1,
                                 2,
                                 Semiring::MINUS_INFTY},
                                {0,
                                 0,
                                 -2,
                                 -3,
                                 Semiring::MINUS_INFTY,
                                 Semiring::MINUS_INFTY,
                                 -1,
                                 0},
                                {0,
                                 2,
                                 Semiring::MINUS_INFTY,
                                 Semiring::MINUS_INFTY,
                                 Semiring::MINUS_INFTY,
                                 1,
                                 2,
                                 0},
                                {Semiring::MINUS_INFTY,
                                 -2,
                                 2,
                                 -1,
                                 3,
                                 1,
                                 2,
                                 Semiring::MINUS_INFTY}},
                               sr),
        new MatrixOverSemiring(
            {{Semiring::MINUS_INFTY,
              0,
              2,
              1,
              Semiring::MINUS_INFTY,
              0,
              0,
              Semiring::MINUS_INFTY},
             {-3,
              Semiring::MINUS_INFTY,
              8,
              0,
              -1,
              -3,
              -1,
              Semiring::MINUS_INFTY},
             {-1, 0, -1, 3, -1, 0, Semiring::MINUS_INFTY, 2},
             {Semiring::MINUS_INFTY,
              1,
              Semiring::MINUS_INFTY,
              3,
              -1,
              Semiring::MINUS_INFTY,
              0,
              5},
             {3, Semiring::MINUS_INFTY, -1, 1, 2, 1, -1, Semiring::MINUS_INFTY},
             {-2,
              -2,
              0,
              Semiring::MINUS_INFTY,
              0,
              Semiring::MINUS_INFTY,
              -1,
              3},
             {2, -4, -3, 4, 1, 1, -2, Semiring::MINUS_INFTY},
             {-4, 1, -5, 2, 2, 1, -2, 2}},
            sr),
        new MatrixOverSemiring(
            {{Semiring::MINUS_INFTY,
              Semiring::MINUS_INFTY,
              0,
              3,
              Semiring::MINUS_INFTY,
              -2,
              -3,
              1},
             {-1,
              Semiring::MINUS_INFTY,
              Semiring::MINUS_INFTY,
              0,
              4,
              1,
              Semiring::MINUS_INFTY,
              4},
             {0, 2, -1, 0, -2, -2, 2, 0},
             {Semiring::MINUS_INFTY, -1, 3, 2, 0, Semiring::MINUS_INFTY, -2, 4},
             {Semiring::MINUS_INFTY,
              Semiring::MINUS_INFTY,
              0,
              Semiring::MINUS_INFTY,
              -1,
              3,
              3,
              -4},
             {Semiring::MINUS_INFTY,
              -5,
              Semiring::MINUS_INFTY,
              2,
              0,
              Semiring::MINUS_INFTY,
              1,
              1},
             {Semiring::MINUS_INFTY,
              1,
              -4,
              Semiring::MINUS_INFTY,
              0,
              Semiring::MINUS_INFTY,
              -3,
              -3},
             {1, -1, 2, 2, 0, Semiring::MINUS_INFTY, Semiring::MINUS_INFTY, 1}},
            sr),
        new MatrixOverSemiring(
            {{-1, -2, 0, -1, Semiring::MINUS_INFTY, 3, -1, 3},
             {1,
              4,
              -2,
              Semiring::MINUS_INFTY,
              4,
              4,
              Semiring::MINUS_INFTY,
              Semiring::MINUS_INFTY},
             {-1,
              -2,
              Semiring::MINUS_INFTY,
              1,
              2,
              1,
              Semiring::MINUS_INFTY,
              -1},
             {-1,
              -1,
              Semiring::MINUS_INFTY,
              -1,
              Semiring::MINUS_INFTY,
              Semiring::MINUS_INFTY,
              2,
              1},
             {Semiring::MINUS_INFTY, 0, 2, 0, 2, 3, 4, 0},
             {Semiring::MINUS_INFTY, -3, 2, 2, Semiring::MINUS_INFTY, 3, 2, 3},
             {-3, 5, Semiring::MINUS_INFTY, Semiring::MINUS_INFTY, 1, 0, 2, -3},
             {-1, 0, 4, 1, 0, 2, 5, 2}},
            sr)};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);

    auto start = std::chrono::high_resolution_clock::now();
    S.enumerate(300000);  // This is probably infinite we stop at 300000
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
  }
}

BENCHMARK(BM_size_no_reserve_max_plus)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(1)
    ->UseManualTime();

static void BM_size_reserve_max_plus(benchmark::State& state) {
  while (state.KeepRunning()) {
    Semiring*             sr   = new MaxPlusSemiring();
    std::vector<Element*> gens = {
        new MatrixOverSemiring({{-1, 0, 2, 7, 5, 0, -1, -3},
                                {4,
                                 Semiring::MINUS_INFTY,
                                 2,
                                 0,
                                 -1,
                                 Semiring::MINUS_INFTY,
                                 Semiring::MINUS_INFTY,
                                 4},
                                {0, 0, Semiring::MINUS_INFTY, 1, 2, 2, 0, -2},
                                {2,
                                 Semiring::MINUS_INFTY,
                                 -1,
                                 0,
                                 Semiring::MINUS_INFTY,
                                 Semiring::MINUS_INFTY,
                                 3,
                                 0},
                                {2,
                                 Semiring::MINUS_INFTY,
                                 -1,
                                 -2,
                                 -2,
                                 1,
                                 2,
                                 Semiring::MINUS_INFTY},
                                {0,
                                 0,
                                 -2,
                                 -3,
                                 Semiring::MINUS_INFTY,
                                 Semiring::MINUS_INFTY,
                                 -1,
                                 0},
                                {0,
                                 2,
                                 Semiring::MINUS_INFTY,
                                 Semiring::MINUS_INFTY,
                                 Semiring::MINUS_INFTY,
                                 1,
                                 2,
                                 0},
                                {Semiring::MINUS_INFTY,
                                 -2,
                                 2,
                                 -1,
                                 3,
                                 1,
                                 2,
                                 Semiring::MINUS_INFTY}},
                               sr),
        new MatrixOverSemiring(
            {{Semiring::MINUS_INFTY,
              0,
              2,
              1,
              Semiring::MINUS_INFTY,
              0,
              0,
              Semiring::MINUS_INFTY},
             {-3,
              Semiring::MINUS_INFTY,
              8,
              0,
              -1,
              -3,
              -1,
              Semiring::MINUS_INFTY},
             {-1, 0, -1, 3, -1, 0, Semiring::MINUS_INFTY, 2},
             {Semiring::MINUS_INFTY,
              1,
              Semiring::MINUS_INFTY,
              3,
              -1,
              Semiring::MINUS_INFTY,
              0,
              5},
             {3, Semiring::MINUS_INFTY, -1, 1, 2, 1, -1, Semiring::MINUS_INFTY},
             {-2,
              -2,
              0,
              Semiring::MINUS_INFTY,
              0,
              Semiring::MINUS_INFTY,
              -1,
              3},
             {2, -4, -3, 4, 1, 1, -2, Semiring::MINUS_INFTY},
             {-4, 1, -5, 2, 2, 1, -2, 2}},
            sr),
        new MatrixOverSemiring(
            {{Semiring::MINUS_INFTY,
              Semiring::MINUS_INFTY,
              0,
              3,
              Semiring::MINUS_INFTY,
              -2,
              -3,
              1},
             {-1,
              Semiring::MINUS_INFTY,
              Semiring::MINUS_INFTY,
              0,
              4,
              1,
              Semiring::MINUS_INFTY,
              4},
             {0, 2, -1, 0, -2, -2, 2, 0},
             {Semiring::MINUS_INFTY, -1, 3, 2, 0, Semiring::MINUS_INFTY, -2, 4},
             {Semiring::MINUS_INFTY,
              Semiring::MINUS_INFTY,
              0,
              Semiring::MINUS_INFTY,
              -1,
              3,
              3,
              -4},
             {Semiring::MINUS_INFTY,
              -5,
              Semiring::MINUS_INFTY,
              2,
              0,
              Semiring::MINUS_INFTY,
              1,
              1},
             {Semiring::MINUS_INFTY,
              1,
              -4,
              Semiring::MINUS_INFTY,
              0,
              Semiring::MINUS_INFTY,
              -3,
              -3},
             {1, -1, 2, 2, 0, Semiring::MINUS_INFTY, Semiring::MINUS_INFTY, 1}},
            sr),
        new MatrixOverSemiring(
            {{-1, -2, 0, -1, Semiring::MINUS_INFTY, 3, -1, 3},
             {1,
              4,
              -2,
              Semiring::MINUS_INFTY,
              4,
              4,
              Semiring::MINUS_INFTY,
              Semiring::MINUS_INFTY},
             {-1,
              -2,
              Semiring::MINUS_INFTY,
              1,
              2,
              1,
              Semiring::MINUS_INFTY,
              -1},
             {-1,
              -1,
              Semiring::MINUS_INFTY,
              -1,
              Semiring::MINUS_INFTY,
              Semiring::MINUS_INFTY,
              2,
              1},
             {Semiring::MINUS_INFTY, 0, 2, 0, 2, 3, 4, 0},
             {Semiring::MINUS_INFTY, -3, 2, 2, Semiring::MINUS_INFTY, 3, 2, 3},
             {-3, 5, Semiring::MINUS_INFTY, Semiring::MINUS_INFTY, 1, 0, 2, -3},
             {-1, 0, 4, 1, 0, 2, 5, 2}},
            sr)};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);

    auto start = std::chrono::high_resolution_clock::now();
    S.reserve(320000);
    S.enumerate(300000);  // This is probably infinite we stop at 300000
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
    delete sr;
  }
}

BENCHMARK(BM_size_reserve_max_plus)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(1)
    ->UseManualTime();

static void BM_size_no_reserve_min_plus(benchmark::State& state) {
  while (state.KeepRunning()) {
    Semiring*             sr   = new MinPlusSemiring();
    std::vector<Element*> gens = {
        new MatrixOverSemiring(
            {{-1, 0, 2, 7, 5, 0, -1, -3},
             {4,
              Semiring::INFTY,
              2,
              0,
              -1,
              Semiring::INFTY,
              Semiring::INFTY,
              4},
             {0, 0, Semiring::INFTY, 1, 2, 2, 0, -2},
             {2,
              Semiring::INFTY,
              -1,
              0,
              Semiring::INFTY,
              Semiring::INFTY,
              3,
              0},
             {2, Semiring::INFTY, -1, -2, -2, 1, 2, Semiring::INFTY},
             {0, 0, -2, -3, Semiring::INFTY, Semiring::INFTY, -1, 0},
             {0, 2, Semiring::INFTY, Semiring::INFTY, Semiring::INFTY, 1, 2, 0},
             {Semiring::INFTY, -2, 2, -1, 3, 1, 2, Semiring::INFTY}},
            sr),
        new MatrixOverSemiring(
            {{Semiring::INFTY, 0, 2, 1, Semiring::INFTY, 0, 0, Semiring::INFTY},
             {-3, Semiring::INFTY, 8, 0, -1, -3, -1, Semiring::INFTY},
             {-1, 0, -1, 3, -1, 0, Semiring::INFTY, 2},
             {Semiring::INFTY,
              1,
              Semiring::INFTY,
              3,
              -1,
              Semiring::INFTY,
              0,
              5},
             {3, Semiring::INFTY, -1, 1, 2, 1, -1, Semiring::INFTY},
             {-2, -2, 0, Semiring::INFTY, 0, Semiring::INFTY, -1, 3},
             {2, -4, -3, 4, 1, 1, -2, Semiring::INFTY},
             {-4, 1, -5, 2, 2, 1, -2, 2}},
            sr),
        new MatrixOverSemiring(
            {{Semiring::INFTY,
              Semiring::INFTY,
              0,
              3,
              Semiring::INFTY,
              -2,
              -3,
              1},
             {-1,
              Semiring::INFTY,
              Semiring::INFTY,
              0,
              4,
              1,
              Semiring::INFTY,
              4},
             {0, 2, -1, 0, -2, -2, 2, 0},
             {Semiring::INFTY, -1, 3, 2, 0, Semiring::INFTY, -2, 4},
             {Semiring::INFTY,
              Semiring::INFTY,
              0,
              Semiring::INFTY,
              -1,
              3,
              3,
              -4},
             {Semiring::INFTY,
              -5,
              Semiring::INFTY,
              2,
              0,
              Semiring::INFTY,
              1,
              1},
             {Semiring::INFTY,
              1,
              -4,
              Semiring::INFTY,
              0,
              Semiring::INFTY,
              -3,
              -3},
             {1, -1, 2, 2, 0, Semiring::INFTY, Semiring::INFTY, 1}},
            sr),
        new MatrixOverSemiring(
            {{-1, -2, 0, -1, Semiring::INFTY, 3, -1, 3},
             {1,
              4,
              -2,
              Semiring::INFTY,
              4,
              4,
              Semiring::INFTY,
              Semiring::INFTY},
             {-1, -2, Semiring::INFTY, 1, 2, 1, Semiring::INFTY, -1},
             {-1,
              -1,
              Semiring::INFTY,
              -1,
              Semiring::INFTY,
              Semiring::INFTY,
              2,
              1},
             {Semiring::INFTY, 0, 2, 0, 2, 3, 4, 0},
             {Semiring::INFTY, -3, 2, 2, Semiring::INFTY, 3, 2, 3},
             {-3, 5, Semiring::INFTY, Semiring::INFTY, 1, 0, 2, -3},
             {-1, 0, 4, 1, 0, 2, 5, 2}},
            sr)};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);

    auto start = std::chrono::high_resolution_clock::now();
    S.enumerate(300000);  // This is probably infinite we stop at 300000
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
    delete sr;
  }
}

BENCHMARK(BM_size_no_reserve_min_plus)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(1)
    ->UseManualTime();

static void BM_size_reserve_min_plus(benchmark::State& state) {
  while (state.KeepRunning()) {
    Semiring*             sr   = new MinPlusSemiring();
    std::vector<Element*> gens = {
        new MatrixOverSemiring(
            {{-1, 0, 2, 7, 5, 0, -1, -3},
             {4,
              Semiring::INFTY,
              2,
              0,
              -1,
              Semiring::INFTY,
              Semiring::INFTY,
              4},
             {0, 0, Semiring::INFTY, 1, 2, 2, 0, -2},
             {2,
              Semiring::INFTY,
              -1,
              0,
              Semiring::INFTY,
              Semiring::INFTY,
              3,
              0},
             {2, Semiring::INFTY, -1, -2, -2, 1, 2, Semiring::INFTY},
             {0, 0, -2, -3, Semiring::INFTY, Semiring::INFTY, -1, 0},
             {0, 2, Semiring::INFTY, Semiring::INFTY, Semiring::INFTY, 1, 2, 0},
             {Semiring::INFTY, -2, 2, -1, 3, 1, 2, Semiring::INFTY}},
            sr),
        new MatrixOverSemiring(
            {{Semiring::INFTY, 0, 2, 1, Semiring::INFTY, 0, 0, Semiring::INFTY},
             {-3, Semiring::INFTY, 8, 0, -1, -3, -1, Semiring::INFTY},
             {-1, 0, -1, 3, -1, 0, Semiring::INFTY, 2},
             {Semiring::INFTY,
              1,
              Semiring::INFTY,
              3,
              -1,
              Semiring::INFTY,
              0,
              5},
             {3, Semiring::INFTY, -1, 1, 2, 1, -1, Semiring::INFTY},
             {-2, -2, 0, Semiring::INFTY, 0, Semiring::INFTY, -1, 3},
             {2, -4, -3, 4, 1, 1, -2, Semiring::INFTY},
             {-4, 1, -5, 2, 2, 1, -2, 2}},
            sr),
        new MatrixOverSemiring(
            {{Semiring::INFTY,
              Semiring::INFTY,
              0,
              3,
              Semiring::INFTY,
              -2,
              -3,
              1},
             {-1,
              Semiring::INFTY,
              Semiring::INFTY,
              0,
              4,
              1,
              Semiring::INFTY,
              4},
             {0, 2, -1, 0, -2, -2, 2, 0},
             {Semiring::INFTY, -1, 3, 2, 0, Semiring::INFTY, -2, 4},
             {Semiring::INFTY,
              Semiring::INFTY,
              0,
              Semiring::INFTY,
              -1,
              3,
              3,
              -4},
             {Semiring::INFTY,
              -5,
              Semiring::INFTY,
              2,
              0,
              Semiring::INFTY,
              1,
              1},
             {Semiring::INFTY,
              1,
              -4,
              Semiring::INFTY,
              0,
              Semiring::INFTY,
              -3,
              -3},
             {1, -1, 2, 2, 0, Semiring::INFTY, Semiring::INFTY, 1}},
            sr),
        new MatrixOverSemiring(
            {{-1, -2, 0, -1, Semiring::INFTY, 3, -1, 3},
             {1,
              4,
              -2,
              Semiring::INFTY,
              4,
              4,
              Semiring::INFTY,
              Semiring::INFTY},
             {-1, -2, Semiring::INFTY, 1, 2, 1, Semiring::INFTY, -1},
             {-1,
              -1,
              Semiring::INFTY,
              -1,
              Semiring::INFTY,
              Semiring::INFTY,
              2,
              1},
             {Semiring::INFTY, 0, 2, 0, 2, 3, 4, 0},
             {Semiring::INFTY, -3, 2, 2, Semiring::INFTY, 3, 2, 3},
             {-3, 5, Semiring::INFTY, Semiring::INFTY, 1, 0, 2, -3},
             {-1, 0, 4, 1, 0, 2, 5, 2}},
            sr)};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);

    auto start = std::chrono::high_resolution_clock::now();
    S.reserve(320000);
    S.enumerate(300000);  // This is probably infinite we stop at 300000
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
    delete sr;
  }
}

BENCHMARK(BM_size_reserve_min_plus)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(1)
    ->UseManualTime();

static void BM_size_no_reserve_trop_max_plus(benchmark::State& state) {
  while (state.KeepRunning()) {
    Semiring*             sr = new TropicalMaxPlusSemiring(33);
    std::vector<Element*> gens
        = {new MatrixOverSemiring({{Semiring::MINUS_INFTY,
                                    0,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY},
                                   {Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    0,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY},
                                   {Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    0,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY},
                                   {Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    0,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY},
                                   {Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    0,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY},
                                   {Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    0,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY},
                                   {Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    0,
                                    Semiring::MINUS_INFTY},
                                   {Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    0},
                                   {0,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY}},
                                  sr),
           new MatrixOverSemiring({{Semiring::MINUS_INFTY,
                                    0,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY},
                                   {0,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY},
                                   {Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    0,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY},
                                   {Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    0,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY},
                                   {Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    0,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY},
                                   {Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    0,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY},
                                   {Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    0,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY},
                                   {Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    0,
                                    Semiring::MINUS_INFTY},
                                   {Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    0}},
                                  sr)};

    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);

    auto start = std::chrono::high_resolution_clock::now();
    S.size();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
    delete sr;
  }
}

BENCHMARK(BM_size_no_reserve_trop_max_plus)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(1)
    ->UseManualTime();

static void BM_size_reserve_trop_max_plus(benchmark::State& state) {
  while (state.KeepRunning()) {
    Semiring*             sr = new TropicalMaxPlusSemiring(33);
    std::vector<Element*> gens
        = {new MatrixOverSemiring({{Semiring::MINUS_INFTY,
                                    0,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY},
                                   {Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    0,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY},
                                   {Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    0,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY},
                                   {Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    0,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY},
                                   {Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    0,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY},
                                   {Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    0,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY},
                                   {Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    0,
                                    Semiring::MINUS_INFTY},
                                   {Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    0},
                                   {0,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY}},
                                  sr),
           new MatrixOverSemiring({{Semiring::MINUS_INFTY,
                                    0,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY},
                                   {0,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY},
                                   {Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    0,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY},
                                   {Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    0,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY},
                                   {Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    0,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY},
                                   {Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    0,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY},
                                   {Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    0,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY},
                                   {Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    0,
                                    Semiring::MINUS_INFTY},
                                   {Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    Semiring::MINUS_INFTY,
                                    0}},
                                  sr)};

    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);

    auto start = std::chrono::high_resolution_clock::now();
    S.reserve(362880);
    S.size();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
    delete sr;
  }
}

BENCHMARK(BM_size_reserve_trop_max_plus)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(1)
    ->UseManualTime();

static void BM_size_no_reserve_trop_min_plus(benchmark::State& state) {
  while (state.KeepRunning()) {
    Semiring*             sr   = new TropicalMinPlusSemiring(33);
    std::vector<Element*> gens = {new MatrixOverSemiring({{Semiring::INFTY,
                                                           0,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY},
                                                          {Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           0,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY},
                                                          {Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           0,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY},
                                                          {Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           0,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY},
                                                          {Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           0,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY},
                                                          {Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           0,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY},
                                                          {Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           0,
                                                           Semiring::INFTY},
                                                          {Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           0},
                                                          {0,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY}},
                                                         sr),
                                  new MatrixOverSemiring({{Semiring::INFTY,
                                                           0,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY},
                                                          {0,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY},
                                                          {Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           0,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY},
                                                          {Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           0,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY},
                                                          {Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           0,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY},
                                                          {Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           0,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY},
                                                          {Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           0,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY},
                                                          {Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           0,
                                                           Semiring::INFTY},
                                                          {Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           0}},
                                                         sr)};

    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);

    auto start = std::chrono::high_resolution_clock::now();
    S.size();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
    delete sr;
  }
}

BENCHMARK(BM_size_no_reserve_trop_min_plus)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(1)
    ->UseManualTime();

static void BM_size_reserve_trop_min_plus(benchmark::State& state) {
  while (state.KeepRunning()) {
    Semiring*             sr   = new TropicalMinPlusSemiring(33);
    std::vector<Element*> gens = {new MatrixOverSemiring({{Semiring::INFTY,
                                                           0,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY},
                                                          {Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           0,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY},
                                                          {Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           0,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY},
                                                          {Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           0,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY},
                                                          {Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           0,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY},
                                                          {Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           0,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY},
                                                          {Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           0,
                                                           Semiring::INFTY},
                                                          {Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           0},
                                                          {0,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY}},
                                                         sr),
                                  new MatrixOverSemiring({{Semiring::INFTY,
                                                           0,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY},
                                                          {0,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY},
                                                          {Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           0,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY},
                                                          {Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           0,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY},
                                                          {Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           0,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY},
                                                          {Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           0,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY},
                                                          {Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           0,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY},
                                                          {Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           0,
                                                           Semiring::INFTY},
                                                          {Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           Semiring::INFTY,
                                                           0}},
                                                         sr)};

    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);

    auto start = std::chrono::high_resolution_clock::now();
    S.reserve(362880);
    S.size();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
    delete sr;
  }
}

BENCHMARK(BM_size_reserve_trop_min_plus)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(1)
    ->UseManualTime();

static void BM_size_no_reserve_nat_mat(benchmark::State& state) {
  while (state.KeepRunning()) {
    Semiring*             sr = new NaturalSemiring(0, 6);
    std::vector<Element*> gens
        = {new MatrixOverSemiring({{0, 0, 1}, {0, 1, 0}, {1, 1, 0}}, sr),
           new MatrixOverSemiring({{0, 0, 1}, {0, 1, 0}, {2, 0, 0}}, sr),
           new MatrixOverSemiring({{0, 0, 1}, {0, 1, 1}, {1, 0, 0}}, sr),
           new MatrixOverSemiring({{0, 0, 1}, {0, 1, 0}, {3, 0, 0}}, sr)};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);

    auto start = std::chrono::high_resolution_clock::now();
    S.size();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
    delete sr;
  }
}

BENCHMARK(BM_size_no_reserve_nat_mat)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(1)
    ->UseManualTime();

static void BM_size_reserve_nat_mat(benchmark::State& state) {
  while (state.KeepRunning()) {
    Semiring*             sr = new NaturalSemiring(0, 6);
    std::vector<Element*> gens
        = {new MatrixOverSemiring({{0, 0, 1}, {0, 1, 0}, {1, 1, 0}}, sr),
           new MatrixOverSemiring({{0, 0, 1}, {0, 1, 0}, {2, 0, 0}}, sr),
           new MatrixOverSemiring({{0, 0, 1}, {0, 1, 1}, {1, 0, 0}}, sr),
           new MatrixOverSemiring({{0, 0, 1}, {0, 1, 0}, {3, 0, 0}}, sr)};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);

    auto start = std::chrono::high_resolution_clock::now();
    S.reserve(10077696);
    S.size();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
    delete sr;
  }
}

BENCHMARK(BM_size_reserve_nat_mat)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(1)
    ->UseManualTime();

static void BM_size_no_reserve_full_pbr_2(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<Element*> gens
        = {new PBR(new PBR_Input({{}, {2}, {1}, {3, 0}})),
           new PBR(new PBR_Input({{3, 0}, {2}, {1}, {}})),
           new PBR(new PBR_Input({{2, 1}, {3}, {0}, {1}})),
           new PBR(new PBR_Input({{2}, {3}, {0}, {3, 1}})),
           new PBR(new PBR_Input({{3}, {1}, {0}, {1}})),
           new PBR(new PBR_Input({{3}, {2}, {0}, {0, 1}})),
           new PBR(new PBR_Input({{3}, {2}, {0}, {1}})),
           new PBR(new PBR_Input({{3}, {2}, {0}, {3}})),
           new PBR(new PBR_Input({{3}, {2}, {1}, {0}})),
           new PBR(new PBR_Input({{3}, {3, 2}, {0}, {1}}))};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);

    auto start = std::chrono::high_resolution_clock::now();
    S.size();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
  }
}

BENCHMARK(BM_size_no_reserve_full_pbr_2)
    ->Unit(benchmark::kMillisecond)
    ->MinTime(1)
    ->UseManualTime();

static void BM_size_reserve_full_pbr_2(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<Element*> gens
        = {new PBR(new PBR_Input({{}, {2}, {1}, {3, 0}})),
           new PBR(new PBR_Input({{3, 0}, {2}, {1}, {}})),
           new PBR(new PBR_Input({{2, 1}, {3}, {0}, {1}})),
           new PBR(new PBR_Input({{2}, {3}, {0}, {3, 1}})),
           new PBR(new PBR_Input({{3}, {1}, {0}, {1}})),
           new PBR(new PBR_Input({{3}, {2}, {0}, {0, 1}})),
           new PBR(new PBR_Input({{3}, {2}, {0}, {1}})),
           new PBR(new PBR_Input({{3}, {2}, {0}, {3}})),
           new PBR(new PBR_Input({{3}, {2}, {1}, {0}})),
           new PBR(new PBR_Input({{3}, {3, 2}, {0}, {1}}))};
    Semigroup S = Semigroup(gens);
    really_delete_cont(gens);

    auto start = std::chrono::high_resolution_clock::now();
    S.reserve(65536);
    S.size();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
  }
}

BENCHMARK(BM_size_reserve_full_pbr_2)
    ->Unit(benchmark::kMillisecond)
    ->MinTime(1)
    ->UseManualTime();

BENCHMARK_MAIN();
