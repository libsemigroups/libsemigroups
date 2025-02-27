//****************************************************************************//
//       Copyright (C) 2014 Florent Hivert <Florent.Hivert@lri.fr>,           //
//                                                                            //
//  Distributed under the terms of the GNU General Public License (GPL)       //
//                                                                            //
//    This code is distributed in the hope that it will be useful,            //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of          //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       //
//   General Public License for more details.                                 //
//                                                                            //
//  The full text of the GPL is available at:                                 //
//                                                                            //
//                  http://www.gnu.org/licenses/                              //
//****************************************************************************//

#ifndef HPCOMBI_TESTTOOLS_HPP_INCLUDED
#define HPCOMBI_TESTTOOLS_HPP_INCLUDED

#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>

#include "perm16.hpp"
namespace HPCombi {

constexpr unsigned int factorial(unsigned int n) {
    return n > 1 ? n * factorial(n - 1) : 1;
}

inline epu8 rand_perm() {
    epu8 res = HPCombi::epu8id;
    auto &ar = HPCombi::as_array(res);
    std::random_shuffle(ar.begin(), ar.end());
    return res;
}

inline std::vector<epu8> rand_perms(int sz) {
    std::vector<epu8> res(sz);
    std::srand(std::time(0));
    for (int i = 0; i < sz; i++)
        res[i] = rand_perm();
    return res;
}

// std::vector<Perm16> all_perms(int n);

// using a template allows us to ignore the differences between functors,
// function pointers and lambda
template <typename Func>
double timethat(Func fun, int rep = 1, double reftime = 0) {
    using std::chrono::duration;
    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    auto tstart = high_resolution_clock::now();
    for (int i = 0; i < rep; i++)
        fun();
    auto tfin = high_resolution_clock::now();

    auto tm = duration_cast<duration<double>>(tfin - tstart);
    std::cout << "time = " << std::fixed << std::setprecision(6) << tm.count()
              << "s";
    if (reftime != 0)
        std::cout << ", speedup = " << std::setprecision(3)
                  << reftime / tm.count();
    std::cout << std::endl;
    return tm.count();
}

}  // namespace HPCombi
#endif  // HPCOMBI_TESTTOOLS_HPP_INCLUDED
