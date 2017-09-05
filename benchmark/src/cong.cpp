//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2017 Michael Torpey
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

// This file contains some benchmarks for libsemigroups/src/cong.cc

#include <benchmark/benchmark.h>
#include <libsemigroups/cong.h>

using namespace libsemigroups;

static void BM_Congruence_full_PBR_monoid(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<Element*> gens = {
        new PBR(new std::vector<std::vector<u_int32_t>>({{2}, {3}, {0}, {1}})),
        new PBR(
            new std::vector<std::vector<u_int32_t>>({{}, {2}, {1}, {0, 3}})),
        new PBR(
            new std::vector<std::vector<u_int32_t>>({{0, 3}, {2}, {1}, {}})),
        new PBR(
            new std::vector<std::vector<u_int32_t>>({{1, 2}, {3}, {0}, {1}})),
        new PBR(
            new std::vector<std::vector<u_int32_t>>({{2}, {3}, {0}, {1, 3}})),
        new PBR(new std::vector<std::vector<u_int32_t>>({{3}, {1}, {0}, {1}})),
        new PBR(
            new std::vector<std::vector<u_int32_t>>({{3}, {2}, {0}, {0, 1}})),
        new PBR(new std::vector<std::vector<u_int32_t>>({{3}, {2}, {0}, {1}})),
        new PBR(new std::vector<std::vector<u_int32_t>>({{3}, {2}, {0}, {3}})),
        new PBR(new std::vector<std::vector<u_int32_t>>({{3}, {2}, {1}, {0}})),
        new PBR(
            new std::vector<std::vector<u_int32_t>>({{3}, {2, 3}, {0}, {1}}))};

    Semigroup S = Semigroup(gens);
    S.set_report(false);
    really_delete_cont(gens);

    std::vector<relation_t> extra(
        {relation_t({7, 10, 9, 3, 6, 9, 4, 7, 9, 10},
                    {9, 3, 6, 6, 10, 9, 4, 7}),
         relation_t({8, 7, 5, 8, 9, 8}, {6, 3, 8, 6, 1, 2, 4})});
    Congruence cong("twosided", &S, extra);
    cong.set_report(false);

    auto start = std::chrono::high_resolution_clock::now();
    cong.nr_classes();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
  }
}

BENCHMARK(BM_Congruence_full_PBR_monoid)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(2)
    ->UseManualTime();

static void BM_Congruence_full_PBR_monoid_max_2(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<Element*> gens = {
        new PBR(new std::vector<std::vector<u_int32_t>>({{2}, {3}, {0}, {1}})),
        new PBR(
            new std::vector<std::vector<u_int32_t>>({{}, {2}, {1}, {0, 3}})),
        new PBR(
            new std::vector<std::vector<u_int32_t>>({{0, 3}, {2}, {1}, {}})),
        new PBR(
            new std::vector<std::vector<u_int32_t>>({{1, 2}, {3}, {0}, {1}})),
        new PBR(
            new std::vector<std::vector<u_int32_t>>({{2}, {3}, {0}, {1, 3}})),
        new PBR(new std::vector<std::vector<u_int32_t>>({{3}, {1}, {0}, {1}})),
        new PBR(
            new std::vector<std::vector<u_int32_t>>({{3}, {2}, {0}, {0, 1}})),
        new PBR(new std::vector<std::vector<u_int32_t>>({{3}, {2}, {0}, {1}})),
        new PBR(new std::vector<std::vector<u_int32_t>>({{3}, {2}, {0}, {3}})),
        new PBR(new std::vector<std::vector<u_int32_t>>({{3}, {2}, {1}, {0}})),
        new PBR(
            new std::vector<std::vector<u_int32_t>>({{3}, {2, 3}, {0}, {1}}))};

    Semigroup S = Semigroup(gens);
    S.set_report(false);
    really_delete_cont(gens);

    std::vector<relation_t> extra(
        {relation_t({7, 10, 9, 3, 6, 9, 4, 7, 9, 10},
                    {9, 3, 6, 6, 10, 9, 4, 7}),
         relation_t({8, 7, 5, 8, 9, 8}, {6, 3, 8, 6, 1, 2, 4})});
    Congruence cong("twosided", &S, extra);
    cong.set_report(false);
    cong.set_max_threads(2);

    auto start = std::chrono::high_resolution_clock::now();
    cong.nr_classes();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
  }
}

BENCHMARK(BM_Congruence_full_PBR_monoid_max_2)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(2)
    ->UseManualTime();

BENCHMARK_MAIN();
