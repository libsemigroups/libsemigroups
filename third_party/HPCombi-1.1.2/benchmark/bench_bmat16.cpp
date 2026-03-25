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

#include "hpcombi/bmat16.hpp"

namespace HPCombi {

std::vector<BMat16> make_sample(size_t n) {
    std::vector<BMat16> res{};
    for (size_t i = 0; i < n; i++) {
        res.push_back(BMat16::random());
    }
    return res;
}

std::vector<std::pair<BMat16, BMat16>> make_pair_sample(size_t n) {
    std::vector<std::pair<BMat16, BMat16>> res{};
    for (size_t i = 0; i < n; i++) {
        auto x = BMat16::random();
        res.push_back(std::make_pair(x, x));
    }
    return res;
}

class Fix_BMat16 {
 public:
    Fix_BMat16()
        : sample(make_sample(1000)), pair_sample(make_pair_sample(1000)) {}
    ~Fix_BMat16() {}
    const std::vector<BMat16> sample;
    std::vector<std::pair<BMat16, BMat16>>
        pair_sample;  // not const, transpose2 is in place
};

TEST_CASE_METHOD(Fix_BMat16, "Transpose", "[BMat16][000]") {
    BENCHMARK_MEM_FN(transpose, sample);
    BENCHMARK_MEM_FN(transpose_naive, sample);
}

TEST_CASE_METHOD(Fix_BMat16, "Multiplication", "[BMat16][001]") {
    BENCHMARK_MEM_FN_PAIR(BMat16::operator*, pair_sample);
    BENCHMARK_MEM_FN_PAIR(mult_4bmat8, pair_sample);
    BENCHMARK_MEM_FN_PAIR(mult_naive, pair_sample);
    BENCHMARK_MEM_FN_PAIR(mult_naive_array, pair_sample);
}

}  // namespace HPCombi
