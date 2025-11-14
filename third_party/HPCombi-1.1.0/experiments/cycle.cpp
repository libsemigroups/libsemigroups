//****************************************************************************//
//     Copyright (C) 2016-2024 Florent Hivert <Florent.Hivert@lisn.fr>,       //
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

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <vector>

#include "hpcombi/perm16.hpp"
#include "testtools.hpp"

using namespace std;
using namespace std::chrono;
using namespace HPCombi;

uint8_t nb_cycles_ref(Perm16 p) {
    Vect16 v{};
    int i, j, c = 0;
    for (i = 0; i < 16; i++) {
        if (v[i] == 0) {
            for (j = i; v[j] == 0; j = p[j])
                v[j] = 1;
            c++;
        }
    }
    return c;
}

uint8_t nb_cycles(Perm16 p) {
    Vect16 x0, x1 = Perm16::one();
    Perm16 pp = p;
    do {
        x0 = x1;
        x1 = _mm_min_epi8(x0, x0.permuted(pp));
        pp = pp * pp;
    } while (x0 != x1);
    x0.v = (Perm16::one().v == x1.v);
    return _mm_popcnt_u32(_mm_movemask_epi8(x0));
}

uint8_t nb_cycles2(Perm16 p) {
    Vect16 x0, x1 = Perm16::one();
    Perm16 pp = p;
    do {
        x0 = _mm_min_epi8(x1, x1.permuted(pp));
        pp = pp * pp;
        x1 = _mm_min_epi8(x0, x0.permuted(pp));
        pp = pp * pp;
    } while (x0 != x1);
    x0.v = (Perm16::one().v == x1.v);
    return _mm_popcnt_u32(_mm_movemask_epi8(x0));
}

/** This is by far the fastest implementation !
 *  42 the default implem up there **/
inline Vect16 cycles_mask_unroll(Perm16 p) {
    Vect16 x0, x1 = Perm16::one();
    x0 = _mm_min_epi8(x1, x1.permuted(p));
    p = p * p;
    x1 = _mm_min_epi8(x0, x0.permuted(p));
    p = p * p;
    x0 = _mm_min_epi8(x1, x1.permuted(p));
    p = p * p;
    x1 = _mm_min_epi8(x0, x0.permuted(p));
    return x1;
}

inline uint8_t nb_cycles_unroll(Perm16 p) {
    Perm16 res;
    res.v = (Perm16::one().v == cycles_mask_unroll(p).v);
    return _mm_popcnt_u32(_mm_movemask_epi8(res));
}

Vect16 cycle_type_ref(Perm16 p) {
    Vect16 v{}, res{};
    int i, j, c = 0;
    for (i = 0; i < 16; i++) {
        if (v[i] == 0) {
            uint8_t lc = 0;
            for (j = i; v[j] == 0; j = p[j]) {
                v[j] = 1;
                lc++;
            }
            res[c] = lc;
            c++;
        }
    }
    sort(p.begin(), p.begin() + c);
    return res;
}

Vect16 evaluation(Vect16 v) {
    Vect16 res;
    res.v = -(Perm16::one().v == v.v);
    for (int i = 0; i < 15; i++) {
        v = v.permuted(Perm16::left_cycle());
        res.v -= (Perm16::one().v == v.v);
    }
    return res;
}

Vect16 cycle_type(Perm16 p) {
    return (evaluation(cycles_mask_unroll(p))).revsorted();
}

inline Vect16 cycle_type_unroll(Perm16 p) {
    Perm16 pp = p;
    Vect16 one16 = Perm16::one().v * cst_epu8_0x0F;
    Vect16 res = one16;

    res = _mm_min_epu8(res, Vect16(res.v + cst_epu8_0x01).permuted(pp));
    pp = pp * pp;
    res = _mm_min_epu8(res, Vect16(res.v + cst_epu8_0x02).permuted(pp));
    pp = pp * pp;
    res = _mm_min_epu8(res, Vect16(res.v + cst_epu8_0x04).permuted(pp));
    pp = pp * pp;
    res = _mm_min_epu8(res, Vect16(res.v + cst_epu8_0x08).permuted(pp));
    res = res.permuted(p);

    res = (res.v - one16.v + cst_epu8_0x01) &
          ((res.v & cst_epu8_0xF0) == one16.v);
    return res.revsorted();
}

inline uint8_t nb_cycles_type_ref(Perm16 p) {
    return _mm_popcnt_u32(
        _mm_movemask_epi8(cycle_type_ref(p).v != cst_epu8_0x00));
}
inline uint8_t nb_cycles_type_mask(Perm16 p) {
    return _mm_popcnt_u32(_mm_movemask_epi8(cycle_type(p).v != cst_epu8_0x00));
}
inline uint8_t nb_cycles_type_unroll(Perm16 p) {
    return _mm_popcnt_u32(
        _mm_movemask_epi8(cycle_type_unroll(p).v != cst_epu8_0x00));
}

auto func = {nb_cycles_ref, nb_cycles, nb_cycles2, nb_cycles_unroll,
             nb_cycles_type_unroll};

using Statistic = array<uint64_t, 17>;

std::ostream &operator<<(std::ostream &stream, Statistic const &term) {
    stream << "[" << unsigned(term[0]);
    for (unsigned i = 1; i < 17; i++)
        stream << "," << unsigned(term[i]);
    stream << "]";
    return stream;
}

template <uint8_t ncycles(Perm16 p)>
double timef(const vector<Perm16> &v, double reftime) {
    high_resolution_clock::time_point tstart, tfin;
    Statistic stat = {};
    uint_fast64_t sz = v.size();

    tstart = high_resolution_clock::now();
    for (uint_fast64_t i = 0; i < sz; i++)
        stat[ncycles(v[i])]++;
    tfin = high_resolution_clock::now();

    auto tm = duration_cast<duration<double>>(tfin - tstart);
    cout << stat << endl;
    cout << "time = " << tm.count() << "s";
    if (reftime != 0)
        cout << ", speedup = " << reftime / tm.count();
    cout << endl;
    return tm.count();
}

void timeit(vector<Perm16> v) {
    double sp_ref;

    cout << "Reference: " << endl;
    sp_ref = timef<nb_cycles_ref>(v, 0.);
    cout << "Loop 1   : " << endl;
    timef<nb_cycles>(v, sp_ref);
    cout << "Loop 2   : " << endl;
    timef<nb_cycles2>(v, sp_ref);
    cout << "Unroll   : " << endl;
    timef<nb_cycles_unroll>(v, sp_ref);
    cout << endl;
    cout << "RefType  : " << endl;
    sp_ref = timef<nb_cycles_type_ref>(v, 0.);
    cout << "MaskType : " << endl;
    timef<nb_cycles_type_mask>(v, sp_ref);
    cout << "UnrollTyp: " << endl;
    timef<nb_cycles_type_unroll>(v, sp_ref);
}

void democycle(Perm16 p) {
    Vect16 x0, x1 = Perm16::one();
    cout << "one " << x1 << endl;
    cout << "sig " << p << endl;
    cout << "perm" << x1.permuted(p) << endl;
    x0 = _mm_min_epi8(x1, x1.permuted(p));
    cout << "min " << x0 << endl;
    p = p * p;
    cout << "p^2 " << p << endl;
    cout << "pe^2" << x0.permuted(p) << endl;
    x1 = _mm_min_epi8(x0, x0.permuted(p));
    cout << "min " << x1 << endl;
    p = p * p;
    cout << "p^4 " << p << endl;
    cout << "pe^4" << x1.permuted(p) << endl;
    x0 = _mm_min_epi8(x1, x1.permuted(p));
    cout << "min " << x0 << endl;
    p = p * p;
    cout << "p^8 " << p << endl;
    cout << "pe^8" << x0.permuted(p) << endl;
    x1 = _mm_min_epi8(x0, x0.permuted(p));
    cout << "min " << x1 << endl;
}

// (-1)**(len(self)-len(self.to_cycles()))
inline uint8_t sign_nb_cycles_unroll(Perm16 p, uint8_t n = 16) {
    return (n - nb_cycles_unroll(p)) & 1;
}

int main() {
    std::srand(std::time(0));

    Perm16 p = {5, 4, 12, 15, 10, 8, 9, 2, 3, 13, 14, 0, 1, 7, 11, 6};
    // p = { 13, 6, 11, 14, 5, 2, 12, 4, 9, 1, 7, 0, 8, 10, 3, 15};

    p = Perm16::random();
    cout << Perm16::one() << endl
         << p << endl
         << cycles_mask_unroll(p) << endl
         << evaluation(cycles_mask_unroll(p))
         << " #= " << unsigned(nb_cycles_unroll(p)) << endl
         << cycle_type(p) << endl
         << cycle_type_unroll(p) << endl;

    cout << "Sign = " << int(sign_nb_cycles_unroll(p)) << endl;

    for (auto f : func)
        cout << f(p) << " ";
    cout << endl;

    timeit(rand_perms(10000000));
    cout << endl;

    timeit(all_perms(11));

    return EXIT_SUCCESS;
}
