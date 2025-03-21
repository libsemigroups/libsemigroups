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
#include <x86intrin.h>

#include "perm16.hpp"
#include "testtools.hpp"

using namespace std;
using namespace std::chrono;
using namespace HPCombi;

using Statistic = array<uint64_t, 256>;

template <uint8_t (Perm16::*fun)() const>
double timef(const vector<Perm16> &v, double reftime, int nloop = 1) {
    high_resolution_clock::time_point tstart, tfin;
    Statistic stat = {};
    uint_fast64_t sz = v.size();
    tstart = high_resolution_clock::now();
    for (int loop = 0; loop < nloop; loop++)
        for (uint_fast64_t i = 0; i < sz; i++)
            stat[(v[i].*fun)()]++;
    tfin = high_resolution_clock::now();

    for (int i = 0; i <= 120; i++)
        cout << stat[i] / nloop << " ";
    cout << endl;
    auto tm = duration_cast<duration<double>>(tfin - tstart);
    cout << "time = " << tm.count() << "s";
    if (reftime != 0)
        cout << ", speedup = " << reftime / tm.count();
    cout << endl;
    return tm.count();
}

void timeit(const vector<Perm16> &v, int nloop = 1) {
    double ref;

    cout << "Reference: ";
    ref = timef<&Perm16::length_ref>(v, 0., nloop);
    cout << "Fast     : ";
    ref = timef<&Perm16::length>(v, ref, nloop);
}

int main() {
    std::srand(std::time(0));

    Perm16 p = {5, 4, 12, 15, 10, 8, 9, 2, 3, 13, 14, 0, 1, 7, 11, 6};

    assert(&p[0] == &(p.as_array()[0]));

    cout << Perm16::one() << endl;
    cout << p << endl << endl;
    cout << int(p.length()) << endl;
    cout << int(p.length_ref()) << endl;
    auto vv = rand_perms(10000);
    timeit(vv, 100);
    cout << endl;
    return EXIT_SUCCESS;
}
