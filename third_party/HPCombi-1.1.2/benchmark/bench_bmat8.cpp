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
#include <iostream>
#include <string>

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include "bench_fixture.hpp"
#include "bench_main.hpp"

#include "hpcombi/bmat8.hpp"

namespace HPCombi {

std::vector<BMat8> make_sample(size_t n) {
    std::vector<BMat8> res{};
    for (size_t i = 0; i < n; i++) {
        res.push_back(BMat8::random());
    }
    return res;
}

std::vector<std::pair<BMat8, BMat8>> make_pair_sample(size_t n) {
    std::vector<std::pair<BMat8, BMat8>> res{};
    for (size_t i = 0; i < n; i++) {
        auto x = BMat8::random();
        res.push_back(std::make_pair(x, x));
    }
    return res;
}

class Fix_BMat8 {
 public:
    Fix_BMat8()
        : sample(make_sample(1000)), pair_sample(make_pair_sample(1000)) {}
    ~Fix_BMat8() {}
    const std::vector<BMat8> sample;
    std::vector<std::pair<BMat8, BMat8>>
        pair_sample;  // not const, transpose2 is in place
};

TEST_CASE_METHOD(Fix_BMat8, "Row space size", "[BMat8][000]") {
    BENCHMARK_MEM_FN(row_space_size_ref, sample);
    BENCHMARK_MEM_FN(row_space_size_bitset, sample);
    BENCHMARK_MEM_FN(row_space_size_incl1, sample);
    BENCHMARK_MEM_FN(row_space_size_incl, sample);
    BENCHMARK_MEM_FN(row_space_size, sample);
}

TEST_CASE_METHOD(Fix_BMat8, "Transpose", "[BMat8][001]") {
    BENCHMARK_MEM_FN(transpose, sample);
    BENCHMARK_MEM_FN(transpose_mask, sample);
    BENCHMARK_MEM_FN(transpose_maskd, sample);
    BENCHMARK_MEM_FN(transpose_naive, sample);
}

TEST_CASE_METHOD(Fix_BMat8, "Transpose pairs", "[BMat8][002]") {
    BENCHMARK_MEM_FN_PAIR_EQ(transpose, pair_sample);
    BENCHMARK_MEM_FN_PAIR_EQ(transpose_mask, pair_sample);
    BENCHMARK_MEM_FN_PAIR_EQ(transpose_maskd, pair_sample);
    BENCHMARK_MEM_FN_PAIR_EQ(transpose_naive, pair_sample);
    BENCHMARK("transpose2") {
        for (auto &pair : pair_sample) {
            BMat8::transpose2(pair.first, pair.second);
            REQUIRE(pair.first == pair.second);
        }
        return true;
    };
}

TEST_CASE_METHOD(Fix_BMat8, "Row spaces inclusion", "[BMat8][003]") {
    BENCHMARK_MEM_FN_PAIR(row_space_included_ref, pair_sample);
    BENCHMARK_MEM_FN_PAIR(row_space_included_bitset, pair_sample);
    BENCHMARK_MEM_FN_PAIR(row_space_included, pair_sample);
}

TEST_CASE_METHOD(Fix_BMat8, "Pair row space inclusion", "[BMat8][004]") {
    BENCHMARK("rotating pairs implementation") {
        for (auto &pair : pair_sample) {
            auto res = BMat8::row_space_included2(pair.first, pair.second,
                                                  pair.second, pair.first);
            volatile auto val = (res.first == res.second);
        }
        return true;
    };
    BENCHMARK("Calling twice implementation") {
        for (auto &pair : pair_sample) {
            volatile auto val = (pair.first.row_space_included(pair.second) ==
                                 pair.second.row_space_included(pair.first));
        }
        return true;
    };
}

TEST_CASE_METHOD(Fix_BMat8, "Multiplication", "[BMat8][005]") {
    BENCHMARK_MEM_FN_PAIR(BMat8::operator*, pair_sample);
    BENCHMARK_MEM_FN_PAIR(mult_naive, pair_sample);
    BENCHMARK_MEM_FN_PAIR(mult_naive_array, pair_sample);
}

}  // namespace HPCombi
