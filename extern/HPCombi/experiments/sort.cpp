//****************************************************************************//
//       Copyright (C) 2016 Florent Hivert <Florent.Hivert@lri.fr>,           //
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

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <vector>
#include <x86intrin.h>

#include "hpcombi/epu8.hpp"

using namespace std;
using namespace std::chrono;
using namespace HPCombi;

#define ASSERT(test)                                                           \
    if (!(test))                                                               \
    cout << "Test failed in file " << __FILE__ << " line " << __LINE__         \
         << ": " #test << endl

std::vector<epu8> rand_sample(size_t sz) {
    std::vector<epu8> res;
    for (size_t i = 0; i < sz; i++)
        res.push_back(random_epu8(256));
    return res;
}

inline epu8 rand_perm() {
    epu8 res = epu8id;
    auto &ar = as_array(res);
    std::random_shuffle(ar.begin(), ar.end());
    return res;
}

std::vector<epu8> rand_perms(int sz) {
    std::vector<epu8> res(sz);
    std::srand(std::time(0));
    for (int i = 0; i < sz; i++)
        res[i] = rand_perm();
    return res;
}

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

struct RoundsMask {
    // commented out due to a bug in gcc
    /* constexpr */ RoundsMask() : arr() {
        for (unsigned i = 0; i < HPCombi::sorting_rounds.size(); ++i)
            arr[i] = HPCombi::sorting_rounds[i] < epu8id;
    }
    epu8 arr[HPCombi::sorting_rounds.size()];
};

const auto rounds_mask = RoundsMask();

inline epu8 sort_pair(epu8 a) {
    for (unsigned i = 0; i < HPCombi::sorting_rounds.size(); ++i) {
        epu8 minab, maxab, b = permuted(a, HPCombi::sorting_rounds[i]);
        minab = _mm_min_epi8(a, b);
        maxab = _mm_max_epi8(a, b);
        a = _mm_blendv_epi8(minab, maxab, rounds_mask.arr[i]);
    }
    return a;
}

inline epu8 sort_odd_even(epu8 a) {
    const uint8_t FF = 0xff;
    static const epu8 even = {1, 0, 3,  2,  5,  4,  7,  6,
                              9, 8, 11, 10, 13, 12, 15, 14};
    static const epu8 odd = {0, 2,  1, 4,  3,  6,  5,  8,
                             7, 10, 9, 12, 11, 14, 13, 15};
    static const epu8 mask = {0, FF, 0, FF, 0, FF, 0, FF,
                              0, FF, 0, FF, 0, FF, 0, FF};
    epu8 b, minab, maxab;
    for (unsigned i = 0; i < 8; ++i) {
        b = permuted(a, even);
        minab = _mm_min_epi8(a, b);
        maxab = _mm_max_epi8(a, b);
        a = _mm_blendv_epi8(minab, maxab, mask);
        b = permuted(a, odd);
        minab = _mm_min_epi8(a, b);
        maxab = _mm_max_epi8(a, b);
        a = _mm_blendv_epi8(maxab, minab, mask);
    }
    return a;
}

inline epu8 insertion_sort(epu8 p) {
    auto &a = HPCombi::as_array(p);
    for (int i = 0; i < 16; i++)
        for (int j = i; j > 0 && a[j] < a[j - 1]; j--)
            std::swap(a[j], a[j - 1]);
    return p;
}

inline epu8 radix_sort(epu8 p) {
    auto &a = HPCombi::as_array(p);
    std::array<uint8_t, 16> stat{};
    for (int i = 0; i < 16; i++)
        stat[a[i]]++;
    int c = 0;
    for (int i = 0; i < 16; i++)
        for (int j = 0; j < stat[i]; j++)
            a[c++] = i;
    return p;
}

int main() {
    // epu8 a = { 5, 4,12,15,10, 8, 9, 2, 3,13,14, 0, 1, 7,11, 6};

    auto vrand = rand_perms(1000);
    int rep = 10000;
    cout << "Std lib: ";
    double reftime = timethat(
        [vrand]() {
            for (epu8 v : vrand) {
                auto &ar = as_array(v);
                std::sort(ar.begin(), ar.end());
                ASSERT(equal(v, epu8id));  // avoid optimization
            }
        },
        rep);
    cout << "OddEv : ";
    timethat(
        [vrand]() {
            for (epu8 v : vrand)
                ASSERT(equal(sort_odd_even(v), epu8id));
        },
        rep, reftime);
    cout << "Insert : ";
    timethat(
        [vrand]() {
            for (epu8 v : vrand)
                ASSERT(equal(insertion_sort(v), epu8id));
        },
        rep, reftime);
    cout << "Radix16: ";
    timethat(
        [vrand]() {
            for (epu8 v : vrand)
                ASSERT(equal(radix_sort(v), epu8id));
        },
        rep, reftime);
    cout << "Pair  : ";
    timethat(
        [vrand]() {
            for (epu8 v : vrand)
                ASSERT(equal(sort_pair(v), epu8id));
        },
        rep, reftime);
    cout << "Funct  : ";
    timethat(
        [vrand]() {
            for (epu8 v : vrand)
                ASSERT(equal(sorted(v), epu8id));
        },
        rep, reftime);

    return EXIT_SUCCESS;
}
