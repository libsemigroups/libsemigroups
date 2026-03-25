//****************************************************************************//
//       Copyright (C) 2016-2024 Florent Hivert <Florent.Hivert@lisn.fr>,     //
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
#ifndef HPCOMBI_BENCHMARK_BENCH_FIXTURE_HPP_
#define HPCOMBI_BENCHMARK_BENCH_FIXTURE_HPP_

#include <random>
#include <vector>

#include "hpcombi/epu8.hpp"

using HPCombi::epu8;

constexpr uint_fast64_t size = 10;
// constexpr uint_fast64_t repeat = 100;

std::vector<epu8> rand_epu8(size_t sz) {
    std::vector<epu8> res;
    for (size_t i = 0; i < sz; i++)
        res.push_back(HPCombi::random_epu8(256));
    return res;
}

inline epu8 rand_perm() {
    static std::random_device rd;
    static std::mt19937 g(rd());
    epu8 res = HPCombi::Epu8.id();
    auto &ar = HPCombi::as_array(res);
    std::shuffle(ar.begin(), ar.end(), g);
    return res;
}

std::vector<epu8> rand_perms(int sz) {
    std::vector<epu8> res(sz);
    std::srand(std::time(0));
    for (int i = 0; i < sz; i++)
        res[i] = rand_perm();
    return res;
}

std::vector<epu8> rand_transf(int sz) {
    std::vector<epu8> res(sz);
    std::srand(std::time(0));
    for (int i = 0; i < sz; i++)
        res[i] = HPCombi::random_epu8(15);
    return res;
}

std::vector<std::pair<epu8, epu8>> make_pair_sample(size_t sz) {
    std::vector<std::pair<epu8, epu8>> res{};
    for (size_t i = 0; i < sz; i++) {
        res.emplace_back(HPCombi::random_epu8(15), HPCombi::random_epu8(15));
    }
    return res;
}

class Fix_epu8 {
 public:
    Fix_epu8()
        : vects(rand_epu8(size)), transf(rand_transf(size)),
          perms(rand_perms(size)), pairs(make_pair_sample(size)) {}
    ~Fix_epu8() {}
    const std::vector<epu8> vects;
    const std::vector<epu8> transf;
    const std::vector<epu8> perms;
    const std::vector<std::pair<epu8, epu8>> pairs;
};

#endif  // HPCOMBI_BENCHMARK_BENCH_FIXTURE_HPP_
