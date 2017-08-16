#include "../../src/cong.h"
#include "benchmark/benchmark.h"

using namespace libsemigroups;

template <typename T> static inline void really_delete_cont(T cont) {
  for (Element* x : cont) {
    x->really_delete();
    delete x;
  }
}

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
    Partition<word_t>* ntc;

    auto start = std::chrono::high_resolution_clock::now();
    cong.nr_classes();
    ntc      = cong.nontrivial_classes();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());

    delete ntc;
  }
}

BENCHMARK(BM_Congruence_full_PBR_monoid)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(10)
    ->UseManualTime();

static void BM_Congruence_full_PBR_monoid_reserve(benchmark::State& state) {
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
    S.reserve(65536);
    S.set_report(false);
    really_delete_cont(gens);

    std::vector<relation_t> extra(
        {relation_t({7, 10, 9, 3, 6, 9, 4, 7, 9, 10},
                    {9, 3, 6, 6, 10, 9, 4, 7}),
         relation_t({8, 7, 5, 8, 9, 8}, {6, 3, 8, 6, 1, 2, 4})});
    Congruence cong("twosided", &S, extra);
    cong.set_report(false);
    Partition<word_t>* ntc;

    auto start = std::chrono::high_resolution_clock::now();
    cong.nr_classes();
    ntc      = cong.nontrivial_classes();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());

    delete ntc;
  }
}

BENCHMARK(BM_Congruence_full_PBR_monoid_reserve)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(10)
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

    Partition<word_t>* ntc;

    auto start = std::chrono::high_resolution_clock::now();
    cong.nr_classes();
    ntc      = cong.nontrivial_classes();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds
        = std::chrono::duration_cast<std::chrono::duration<double>>(end
                                                                    - start);
    state.SetIterationTime(elapsed_seconds.count());

    delete ntc;
  }
}

BENCHMARK(BM_Congruence_full_PBR_monoid_max_2)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(10)
    ->UseManualTime();

BENCHMARK_MAIN();
