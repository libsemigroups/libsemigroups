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

// This file contains some benchmarks for libsemigroups/src/elements.h

#include <chrono>

#include <benchmark/benchmark.h>
#include <libsemigroups/libsemigroups.h>

using namespace libsemigroups;

static void BM_Transf_No_Except_No_Move(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<u_int32_t> vec(65536, 17);
    try {
      auto x = Transformation<u_int32_t>(vec);
    } catch (...) {
    }
  }
}

BENCHMARK(BM_Transf_No_Except_No_Move)->MinTime(1);

static void BM_Transf_No_Except_Move(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<u_int32_t> vec(65536, 17);
    try {
      auto x = Transformation<u_int32_t>(std::move(vec));
    } catch (...) {
    }
  }
}

BENCHMARK(BM_Transf_No_Except_Move)->MinTime(1);

static void BM_Transf_Except_No_Move(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<u_int32_t> vec(65535, 17);
    vec.push_back(65537);
    try {
      auto x = Transformation<u_int32_t>(vec);
    } catch (...) {
    }
  }
}

BENCHMARK(BM_Transf_Except_No_Move)->MinTime(1);

static void BM_Transf_Except_Move(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<u_int32_t> vec(65535, 17);
    vec.push_back(65537);
    try {
      auto x = Transformation<u_int32_t>(std::move(vec));
    } catch (...) {
    }
  }
}

BENCHMARK(BM_Transf_Except_Move)->MinTime(1);

static void BM_Transf_Identity(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<u_int32_t> vec(65536, 17);
    auto                   x     = Transformation<u_int32_t>(std::move(vec));
    auto                   start = std::chrono::high_resolution_clock::now();
    auto                   y     = x.identity();
    auto                   end   = std::chrono::high_resolution_clock::now();
    auto                   elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
  }
}

BENCHMARK(BM_Transf_Identity)->UseManualTime()->MinTime(1);

static void BM_PPerm_No_Except_No_Move(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<u_int32_t> vec(65536);
    std::iota(vec.begin(), vec.end(), 0);
    try {
      auto x = PartialPerm<u_int32_t>(vec);
    } catch (...) {
    }
  }
}

BENCHMARK(BM_PPerm_No_Except_No_Move)->MinTime(1);

static void BM_PPerm_No_Except_Move(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<u_int32_t> vec(65536);
    std::iota(vec.begin(), vec.end(), 0);
    try {
      auto x = PartialPerm<u_int32_t>(std::move(vec));
    } catch (...) {
    }
  }
}

BENCHMARK(BM_PPerm_No_Except_Move)->MinTime(1);

static void BM_PPerm_Except_No_Move(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<u_int32_t> vec(65536);
    std::iota(vec.begin(), vec.end(), 0);
    vec.push_back(0);
    try {
      auto x = PartialPerm<u_int32_t>(vec);
    } catch (...) {
    }
  }
}

BENCHMARK(BM_PPerm_Except_No_Move)->MinTime(1);

static void BM_PPerm_Except_Move(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<u_int32_t> vec(65536);
    std::iota(vec.begin(), vec.end(), 0);
    vec.push_back(0);
    try {
      auto x = PartialPerm<u_int32_t>(std::move(vec));
    } catch (...) {
    }
  }
}

BENCHMARK(BM_PPerm_Except_Move)->MinTime(1);

static void BM_PPerm_Identity(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<u_int32_t> vec(65536);
    std::iota(vec.begin(), vec.end(), 0);
    auto x     = Transformation<u_int32_t>(std::move(vec));
    auto start = std::chrono::high_resolution_clock::now();
    auto y     = x.identity();
    auto end   = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
  }
}

BENCHMARK(BM_PPerm_Identity)->UseManualTime()->MinTime(1);

static void BM_Bipart_No_Except_No_Move(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<u_int32_t> vec(65536);
    std::iota(vec.begin(), vec.end(), 0);
    try {
      auto x = Bipartition(vec);
    } catch (...) {
    }
  }
}

BENCHMARK(BM_Bipart_No_Except_No_Move)->MinTime(1);

static void BM_Bipart_No_Except_Move(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<u_int32_t> vec(65536);
    std::iota(vec.begin(), vec.end(), 0);
    try {
      auto x = Bipartition(std::move(vec));
    } catch (...) {
    }
  }
}

BENCHMARK(BM_Bipart_No_Except_Move)->MinTime(1);

static void BM_Bipart_Except_No_Move(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<u_int32_t> vec(65536);
    std::iota(vec.begin(), vec.end(), 0);
    vec.push_back(65537);
    try {
      auto x = Bipartition(vec);
    } catch (...) {
    }
  }
}

BENCHMARK(BM_Bipart_Except_No_Move)->MinTime(1);

static void BM_Bipart_Except_Move(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<u_int32_t> vec(65536);
    std::iota(vec.begin(), vec.end(), 0);
    vec.push_back(65537);
    try {
      auto x = Bipartition(std::move(vec));
    } catch (...) {
    }
  }
}

BENCHMARK(BM_Bipart_Except_Move)->MinTime(1);

static void BM_Bipart_Identity(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<u_int32_t> vec(65536);
    std::iota(vec.begin(), vec.end(), 0);
    auto x     = Bipartition(std::move(vec));
    auto start = std::chrono::high_resolution_clock::now();
    auto y     = x.identity();
    auto end   = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());
  }
}

BENCHMARK(BM_Bipart_Identity)->UseManualTime()->MinTime(1);

BENCHMARK_MAIN();
