#include "../../src/semigroups.h"
#include "benchmark/benchmark.h"

using namespace libsemigroups;

template <typename T> static inline void really_delete_cont(T cont) {
  for (Element* x : cont) {
    x->really_delete();
    delete x;
  }
}

static void BM_Semigroup_size_no_reserve_01(benchmark::State& state) {
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

BENCHMARK(BM_Semigroup_size_no_reserve_01)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(10)
    ->UseManualTime();

static void BM_Semigroup_size_reserve_01(benchmark::State& state) {
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
    S.reserve(597369);
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

BENCHMARK(BM_Semigroup_size_reserve_01)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(10)
    ->UseManualTime();

BENCHMARK_MAIN();
