//****************************************************************************//
//     Copyright (C) 2023-2024 James D. Mitchell <jdm3@st-andrews.ac.uk>      //
//     Copyright (C) 2023-2024 Florent Hivert <Florent.Hivert@lisn.fr>,       //
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

#ifndef HPCOMBI_BENCHMARK_BENCH_MAIN_HPP_
#define HPCOMBI_BENCHMARK_BENCH_MAIN_HPP_

#define BENCHMARK_MEM_FN(mem_fn, sample)                                       \
    BENCHMARK(#mem_fn) {                                                       \
        for (auto &elem : sample) {                                            \
            volatile auto dummy = elem.mem_fn();                               \
        }                                                                      \
        return true;                                                           \
    };

#define BENCHMARK_FREE_FN(msg, free_fn, sample)                                \
    BENCHMARK(#free_fn " " msg) {                                              \
        for (auto elem : sample) {                                             \
            volatile auto dummy = free_fn(elem);                               \
        }                                                                      \
        return true;                                                           \
    };

#define BENCHMARK_LAMBDA(msg, free_fn, sample)                                 \
    BENCHMARK(#free_fn " " msg) {                                              \
        auto lambda__xxx = [](auto val) { return free_fn(val); };              \
        for (auto elem : sample) {                                             \
            volatile auto dummy = lambda__xxx(elem);                           \
        }                                                                      \
        return true;                                                           \
    };

#define BENCHMARK_LAMBDA2(msg, free_fn, sample)                                \
    BENCHMARK(#free_fn " " msg) {                                              \
        auto lambda__xxx = [](auto const &x, auto const &y) {                  \
            return free_fn(x, y);                                              \
        };                                                                     \
        for (auto [x, y] : sample) {                                           \
            volatile auto dummy = lambda__xxx(x, y);                           \
        }                                                                      \
        return true;                                                           \
    };

#define BENCHMARK_MEM_FN_PAIR(mem_fn, sample)                                  \
    BENCHMARK(#mem_fn) {                                                       \
        for (auto &pair : sample) {                                            \
            volatile auto val = pair.first.mem_fn(pair.second);                \
        }                                                                      \
        return true;                                                           \
    };

#define BENCHMARK_MEM_FN_PAIR_EQ(mem_fn, sample)                               \
    BENCHMARK(#mem_fn) {                                                       \
        for (auto &pair : sample) {                                            \
            auto val =                                                         \
                std::make_pair(pair.first.mem_fn(), pair.second.mem_fn());     \
            REQUIRE(val.first == val.second);                                  \
        }                                                                      \
        return true;                                                           \
    };

#define BENCHMARK_FREE_FN_PAIR(free_fn, sample)                                \
    BENCHMARK(#free_fn) {                                                      \
        for (auto &pair : sample) {                                            \
            volatile auto val = free_fn(pair.first, pair.second);              \
        }                                                                      \
        return true;                                                           \
    };
#endif  // HPCOMBI_BENCHMARK_BENCH_MAIN_HPP_
