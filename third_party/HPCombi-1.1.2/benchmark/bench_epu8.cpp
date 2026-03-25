//****************************************************************************//
//       Copyright (C) 2018-2024 Florent Hivert <Florent.Hivert@lisn.fr>,     //
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
#include <iostream>
#include <string>

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include "bench_fixture.hpp"
#include "bench_main.hpp"

#include "hpcombi/epu8.hpp"

namespace HPCombi {

namespace {

struct RoundsMask {
    constexpr RoundsMask() : arr() {
        for (unsigned i = 0; i < sorting_rounds.size(); ++i)
            arr[i] = sorting_rounds[i] < Epu8.id();
    }
    epu8 arr[sorting_rounds.size()];  // NOLINT(runtime/arrays]
};

const auto rounds_mask = RoundsMask();

inline epu8 sort_pair(epu8 a) {
    for (unsigned i = 0; i < sorting_rounds.size(); ++i) {
        epu8 minab, maxab, b = permuted(a, sorting_rounds[i]);
        minab = simde_mm_min_epi8(a, b);
        maxab = simde_mm_max_epi8(a, b);
        a = simde_mm_blendv_epi8(minab, maxab, rounds_mask.arr[i]);
    }
    return a;
}

inline epu8 sort_odd_even(epu8 a) {
    const uint8_t FF = 0xff;
    static constexpr const epu8 even = {1, 0, 3,  2,  5,  4,  7,  6,
                                        9, 8, 11, 10, 13, 12, 15, 14};
    static constexpr const epu8 odd = {0, 2,  1, 4,  3,  6,  5,  8,
                                       7, 10, 9, 12, 11, 14, 13, 15};
    static constexpr const epu8 mask = {0, FF, 0, FF, 0, FF, 0, FF,
                                        0, FF, 0, FF, 0, FF, 0, FF};
    epu8 b, minab, maxab;
    for (unsigned i = 0; i < 8; ++i) {
        b = permuted(a, even);
        minab = simde_mm_min_epi8(a, b);
        maxab = simde_mm_max_epi8(a, b);
        a = simde_mm_blendv_epi8(minab, maxab, mask);
        b = permuted(a, odd);
        minab = simde_mm_min_epi8(a, b);
        maxab = simde_mm_max_epi8(a, b);
        a = simde_mm_blendv_epi8(maxab, minab, mask);
    }
    return a;
}

inline epu8 insertion_sort(epu8 p) {
    auto &a = as_array(p);
    for (int i = 0; i < 16; i++)
        for (int j = i; j > 0 && a[j] < a[j - 1]; j--)
            std::swap(a[j], a[j - 1]);
    return p;
}

__attribute__((always_inline)) inline epu8 radix_sort(epu8 p) {
    auto &a = as_array(p);
    std::array<uint8_t, 16> stat{};
    for (int i = 0; i < 16; i++)
        stat[a[i]]++;
    int c = 0;
    for (int i = 0; i < 16; i++)
        for (int j = 0; j < stat[i]; j++)
            a[c++] = i;
    return p;
}

inline epu8 std_sort(epu8 &p) {
    auto &ar = as_array(p);
    std::sort(ar.begin(), ar.end());
    return p;
}

inline epu8 arr_sort(epu8 &p) {
    auto &ar = as_array(p);
    return Epu8(sorted_vect(ar));
}

inline epu8 gen_sort(epu8 p) {
    as_VectGeneric(p).sort();
    return p;
}

static const epu8 bla = {0, 2, 1, 4, 3, 6, 5, 8, 7, 10, 9, 12, 11, 14, 13, 15};

}  // namespace

TEST_CASE_METHOD(Fix_epu8, "Sorting", "[Epu8][000]") {
    BENCHMARK_FREE_FN("| no lambda | perms", std_sort, Fix_epu8::perms);
    BENCHMARK_LAMBDA("| lambda | perms", std_sort, Fix_epu8::perms);

    BENCHMARK_FREE_FN("| no lambda | perms", arr_sort, Fix_epu8::perms);
    BENCHMARK_LAMBDA("| lambda | perms", arr_sort, Fix_epu8::perms);

    BENCHMARK_FREE_FN("| no lambda | perms", gen_sort, Fix_epu8::perms);
    BENCHMARK_LAMBDA("| lambda | perms", gen_sort, Fix_epu8::perms);

    BENCHMARK_FREE_FN("| no lambda | perms", insertion_sort, Fix_epu8::perms);
    BENCHMARK_LAMBDA("| lambda | perms", insertion_sort, Fix_epu8::perms);

    BENCHMARK_FREE_FN("| no lambda | perms", sort_odd_even, Fix_epu8::perms);
    BENCHMARK_LAMBDA("| lambda | perms", sort_odd_even, Fix_epu8::perms);

    BENCHMARK_LAMBDA("| lambda | perms", radix_sort, Fix_epu8::perms);
    BENCHMARK_FREE_FN("| no lambda | perms", radix_sort, Fix_epu8::perms);

    BENCHMARK_FREE_FN("| no lambda | perms", sort_pair, Fix_epu8::perms);
    BENCHMARK_LAMBDA("| lambda | perms", sort_pair, Fix_epu8::perms);

    BENCHMARK_FREE_FN("| no lambda | perms", sorted, Fix_epu8::perms);
    BENCHMARK_LAMBDA("| lambda | perms", sorted, Fix_epu8::perms);

    // lambda function is needed for inlining

    BENCHMARK_LAMBDA("| lambda | vects", std_sort, Fix_epu8::vects);
    BENCHMARK_LAMBDA("| lambda | vects", arr_sort, Fix_epu8::vects);
    BENCHMARK_LAMBDA("| lambda | vects", gen_sort, Fix_epu8::vects);
    BENCHMARK_LAMBDA("| lambda | vects", insertion_sort, Fix_epu8::vects);
    BENCHMARK_LAMBDA("| lambda | vects", sort_odd_even, Fix_epu8::vects);
    BENCHMARK_LAMBDA("| lambda | vects", sort_pair, Fix_epu8::vects);
    BENCHMARK_LAMBDA("| lambda | vects", sorted, Fix_epu8::vects);
}

TEST_CASE_METHOD(Fix_epu8, "Permuting", "[Epu8][001]") {
    BENCHMARK_FREE_FN_PAIR(HPCombi::permuted_ref, pairs);
    BENCHMARK_FREE_FN_PAIR(HPCombi::permuted, pairs);
}

TEST_CASE_METHOD(Fix_epu8, "hsum", "[Epu8][002]") {
    BENCHMARK_FREE_FN("| no lambda", horiz_sum_ref, Fix_epu8::perms);
    BENCHMARK_FREE_FN("| no lambda", horiz_sum_gen, Fix_epu8::perms);
    BENCHMARK_FREE_FN("| no lambda", horiz_sum4, Fix_epu8::perms);
    BENCHMARK_FREE_FN("| no lambda", horiz_sum3, Fix_epu8::perms);

    BENCHMARK_LAMBDA("| lambda ", horiz_sum_ref, Fix_epu8::perms);
    BENCHMARK_LAMBDA("| lambda ", horiz_sum_gen, Fix_epu8::perms);
    BENCHMARK_LAMBDA("| lambda ", horiz_sum4, Fix_epu8::perms);
    BENCHMARK_LAMBDA("| lambda ", horiz_sum3, Fix_epu8::perms);
}

TEST_CASE_METHOD(Fix_epu8, "partial sums", "[Epu8][003]") {
    BENCHMARK_FREE_FN("| no lambda", partial_sums_ref, Fix_epu8::perms);
    BENCHMARK_FREE_FN("| no lambda", partial_sums_gen, Fix_epu8::perms);
    BENCHMARK_FREE_FN("| no lambda", partial_sums_round, Fix_epu8::perms);

    BENCHMARK_LAMBDA("| lambda", partial_sums_ref, Fix_epu8::perms);
    BENCHMARK_LAMBDA("| lambda", partial_sums_gen, Fix_epu8::perms);
    BENCHMARK_LAMBDA("| lambda", partial_sums_round, Fix_epu8::perms);
}

TEST_CASE_METHOD(Fix_epu8, "horiz max", "[Epu8][004]") {
    BENCHMARK_FREE_FN("| no lambda", horiz_max_ref, Fix_epu8::perms);
    BENCHMARK_FREE_FN("| no lambda", horiz_max_ref, Fix_epu8::perms);
    BENCHMARK_FREE_FN("| no lambda", horiz_max_ref, Fix_epu8::perms);

    BENCHMARK_FREE_FN("| no lambda", horiz_max_ref, Fix_epu8::perms);
    //    BENCHMARK_FREE_FN("| no lambda", horiz_max_gen, Fix_epu8::perms);
    BENCHMARK_FREE_FN("| no lambda", horiz_max4, Fix_epu8::perms);
    BENCHMARK_FREE_FN("| no lambda", horiz_max3, Fix_epu8::perms);

    BENCHMARK_LAMBDA("| lambda", horiz_max_ref, Fix_epu8::perms);
    //    BENCHMARK_LAMBDA("| lambda", horiz_max_gen, Fix_epu8::perms);
    BENCHMARK_LAMBDA("| lambda", horiz_max4, Fix_epu8::perms);
    BENCHMARK_LAMBDA("| lambda", horiz_max3, Fix_epu8::perms);
}

TEST_CASE_METHOD(Fix_epu8, "partial max", "[Epu8][005]") {
    BENCHMARK_FREE_FN("| no lambda", partial_max_ref, Fix_epu8::perms);
    BENCHMARK_FREE_FN("| no lambda", partial_max_ref, Fix_epu8::perms);
    BENCHMARK_FREE_FN("| no lambda", partial_max_ref, Fix_epu8::perms);

    BENCHMARK_FREE_FN("| no lambda", partial_max_ref, Fix_epu8::perms);
    //    BENCHMARK_FREE_FN("| no lambda", partial_max_gen, Fix_epu8::perms);
    BENCHMARK_FREE_FN("| no lambda", partial_max_round, Fix_epu8::perms);

    BENCHMARK_LAMBDA("| lambda", partial_max_ref, Fix_epu8::perms);
    //    BENCHMARK_LAMBDA("| lambda", partial_max_gen, Fix_epu8::perms);
    BENCHMARK_LAMBDA("| lambda", partial_max_round, Fix_epu8::perms);
}

TEST_CASE_METHOD(Fix_epu8, "horiz min", "[Epu8][006]") {
    BENCHMARK_FREE_FN("| no lambda", horiz_min_ref, Fix_epu8::perms);
    //    BENCHMARK_FREE_FN("| no lambda", horiz_min_gen, Fix_epu8::perms);
    BENCHMARK_FREE_FN("| no lambda", horiz_min4, Fix_epu8::perms);
    BENCHMARK_FREE_FN("| no lambda", horiz_min3, Fix_epu8::perms);

    BENCHMARK_LAMBDA("| lambda", horiz_min_ref, Fix_epu8::perms);
    //    BENCHMARK_LAMBDA("| lambda", horiz_min_gen, Fix_epu8::perms);
    BENCHMARK_LAMBDA("| lambda", horiz_min4, Fix_epu8::perms);
    BENCHMARK_LAMBDA("| lambda", horiz_min3, Fix_epu8::perms);
}

TEST_CASE_METHOD(Fix_epu8, "partial min", "[Epu8][007]") {
    BENCHMARK_FREE_FN("| no lambda", partial_min_ref, Fix_epu8::perms);
    //    BENCHMARK_FREE_FN("| no lambda", partial_min_gen, Fix_epu8::perms);
    BENCHMARK_FREE_FN("| no lambda", partial_min_round, Fix_epu8::perms);
    //    BENCHMARK_LAMBDA("| lambda", partial_min_gen, Fix_epu8::perms);
    BENCHMARK_LAMBDA("| lambda", partial_min_ref, Fix_epu8::perms);
    BENCHMARK_LAMBDA("| lambda", partial_min_round, Fix_epu8::perms);
}

TEST_CASE_METHOD(Fix_epu8, "eval16", "[Epu8][008]") {
    BENCHMARK_FREE_FN("| no lambda", eval16_ref, Fix_epu8::perms);
    BENCHMARK_FREE_FN("| no lambda", eval16_gen, Fix_epu8::perms);
    BENCHMARK_FREE_FN("| no lambda", eval16_popcount, Fix_epu8::perms);
    BENCHMARK_FREE_FN("| no lambda", eval16_arr, Fix_epu8::perms);
    BENCHMARK_FREE_FN("| no lambda", eval16_cycle, Fix_epu8::perms);

    BENCHMARK_LAMBDA("| lambda", eval16_ref, Fix_epu8::perms);
    BENCHMARK_LAMBDA("| lambda", eval16_gen, Fix_epu8::perms);
    BENCHMARK_LAMBDA("| lambda", eval16_popcount, Fix_epu8::perms);
    BENCHMARK_LAMBDA("| lambda", eval16_arr, Fix_epu8::perms);
    BENCHMARK_LAMBDA("| lambda", eval16_cycle, Fix_epu8::perms);
}

TEST_CASE_METHOD(Fix_epu8, "first diff", "[Epu8][009]") {
    BENCHMARK_LAMBDA2("| lambda", first_diff_ref, Fix_epu8::pairs);
#ifdef SIMDE_X86_SSE4_2_NATIVE
    BENCHMARK_LAMBDA2("| lambda", first_diff_cmpstr, Fix_epu8::pairs);
#endif
    BENCHMARK_LAMBDA2("| lambda", first_diff_mask, Fix_epu8::pairs);
}

TEST_CASE_METHOD(Fix_epu8, "last diff", "[Epu8][010]") {
    BENCHMARK_LAMBDA2("| lambda", last_diff_ref, Fix_epu8::pairs);
#ifdef SIMDE_X86_SSE4_2_NATIVE
    BENCHMARK_LAMBDA2("| lambda", last_diff_cmpstr, Fix_epu8::pairs);
#endif
    BENCHMARK_LAMBDA2("| lambda", last_diff_mask, Fix_epu8::pairs);
}

TEST_CASE_METHOD(Fix_epu8, "is_permutation", "[Epu8][011]") {
    BENCHMARK_FREE_FN("| no lambda", is_permutation_sort, Fix_epu8::perms);
    BENCHMARK_FREE_FN("| no lambda", is_permutation_eval, Fix_epu8::perms);
    BENCHMARK_FREE_FN("| no lambda", is_permutation, Fix_epu8::perms);

    BENCHMARK_LAMBDA("| lambda", is_permutation_sort, Fix_epu8::perms);
    BENCHMARK_LAMBDA("| lambda", is_permutation_eval, Fix_epu8::perms);
    BENCHMARK_LAMBDA("| lambda", is_permutation, Fix_epu8::perms);
}

}  // namespace HPCombi
