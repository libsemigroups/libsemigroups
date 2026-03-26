//****************************************************************************//
//     Copyright (C) 2018-2024 Florent Hivert <Florent.Hivert@lisn.fr>,       //
//                                                                            //
//  This file is part of HP-Combi <https://github.com/libsemigroups/HPCombi>  //
//                                                                            //
//  HP-Combi is free software: you can redistribute it and/or modify it       //
//  under the terms of the GNU General Public License as published by the     //
//  Free Software Foundation, either version 3 of the License, or             //
//  (at your option) any later version.                                       //
//                                                                            //
//  HP-Combi is distributed in the hope that it will be useful, but WITHOUT   //
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     //
//  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License      //
//  for  more details.                                                        //
//                                                                            //
//  You should have received a copy of the GNU General Public License along   //
//  with HP-Combi. If not, see <https://www.gnu.org/licenses/>.               //
//****************************************************************************//

#include <cstdlib>

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include "bench_fixture.hpp"
#include "bench_main.hpp"

#include "hpcombi/perm16.hpp"
#include "hpcombi/perm_generic.hpp"

using HPCombi::epu8;
using HPCombi::Perm16;
using HPCombi::PTransf16;
using HPCombi::Transf16;
using HPCombi::Vect16;

// using namespace FeatureDetector;
// const std::string SIMDSET = cpu_x86::get_highest_SIMD();

std::vector<Perm16> make_Perm16(size_t n) {
    std::vector<epu8> gens = rand_perms(n);
    std::vector<Perm16> res{};
    std::transform(gens.cbegin(), gens.cend(), std::back_inserter(res),
                   [](epu8 x) -> Perm16 { return x; });
    return res;
}

std::vector<std::pair<Perm16, Perm16>> make_Pair_Perm16(size_t n) {
    std::vector<epu8> gens = rand_perms(n);
    std::vector<std::pair<Perm16, Perm16>> res{};
    for (auto g1 : gens)
        for (auto g2 : gens) {
            res.push_back({g1, g2});
        }
    return res;
}

std::vector<Transf16> make_Transf16(size_t n) {
    std::vector<epu8> gens = rand_transf(n);
    std::vector<Transf16> res{};
    std::transform(gens.cbegin(), gens.cend(), std::back_inserter(res),
                   [](epu8 x) -> Transf16 { return x; });
    return res;
}

class Fix_Perm16 {
 public:
    Fix_Perm16()
        : sample_Perm16(make_Perm16(1000)),
          sample_Transf16(make_Transf16(1000)),
          sample_pair_Perm16(make_Pair_Perm16(40)) {}
    ~Fix_Perm16() {}
    const std::vector<Perm16> sample_Perm16;
    const std::vector<Transf16> sample_Transf16;
    const std::vector<std::pair<Perm16, Perm16>> sample_pair_Perm16;
};

TEST_CASE_METHOD(Fix_Perm16, "Inverse of 1000 Perm16", "[Perm16][000]") {
    BENCHMARK_MEM_FN(inverse_ref, sample_Perm16);
    BENCHMARK_MEM_FN(inverse_arr, sample_Perm16);
    BENCHMARK_MEM_FN(inverse_sort, sample_Perm16);
    BENCHMARK_MEM_FN(inverse_find, sample_Perm16);
    BENCHMARK_MEM_FN(inverse_pow, sample_Perm16);
    BENCHMARK_MEM_FN(inverse_cycl, sample_Perm16);
    BENCHMARK_MEM_FN(inverse, sample_Perm16);
}

TEST_CASE_METHOD(Fix_Perm16, "Lehmer code of 1000 Perm16", "[Perm16][001]") {
    BENCHMARK_MEM_FN(lehmer_ref, sample_Perm16);
    BENCHMARK_MEM_FN(lehmer_arr, sample_Perm16);
    BENCHMARK_MEM_FN(lehmer, sample_Perm16);
}

TEST_CASE_METHOD(Fix_Perm16, "Coxeter Length of 1000 Perm16", "[Perm16][002]") {
    BENCHMARK_MEM_FN(length_ref, sample_Perm16);
    BENCHMARK_MEM_FN(length_arr, sample_Perm16);
    BENCHMARK_MEM_FN(length, sample_Perm16);
}

TEST_CASE_METHOD(Fix_Perm16, "Number of descents of 1000 Perm16",
                 "[Perm16][003]") {
    BENCHMARK_MEM_FN(nb_descents_ref, sample_Perm16);
    BENCHMARK_MEM_FN(nb_descents, sample_Perm16);
}

TEST_CASE_METHOD(Fix_Perm16, "Number of cycles of 1000 Perm16",
                 "[Perm16][004]") {
    BENCHMARK_MEM_FN(nb_cycles_ref, sample_Perm16);
    BENCHMARK_MEM_FN(nb_cycles, sample_Perm16);
}

TEST_CASE_METHOD(Fix_Perm16, "Weak order comparison of 1600 pairs of Perm16",
                 "[Perm16][005]") {
    BENCHMARK_MEM_FN_PAIR(left_weak_leq_ref, sample_pair_Perm16);
    BENCHMARK_MEM_FN_PAIR(left_weak_leq_length, sample_pair_Perm16);
    BENCHMARK_MEM_FN_PAIR(left_weak_leq, sample_pair_Perm16);
}

TEST_CASE_METHOD(Fix_Perm16, "Rank of 1000 PTransf16", "[PTransf16][006]") {
    BENCHMARK_MEM_FN(rank_ref, sample_Transf16);
    BENCHMARK_MEM_FN(rank_cmpestrm, sample_Transf16);
    BENCHMARK_MEM_FN(rank, sample_Transf16);
}
