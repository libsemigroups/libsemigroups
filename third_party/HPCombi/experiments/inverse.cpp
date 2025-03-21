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

#include "hpcombi/perm16.hpp"
#include "testtools.hpp"

using namespace std;
using namespace std::chrono;
using namespace HPCombi;

template <typename Func, typename Sample, typename SampleRef>
double timecheck(Func fun, Sample &sample, SampleRef &ref, double reftime = 0) {
    std::vector<Perm16> cur(sample.size());
    double time = timethat(
        [&sample, &cur, fun]() {
            std::transform(sample.begin(), sample.end(), cur.begin(),
                           [fun](Perm16 p) -> Perm16 {
                               for (int i = 0; i < 100; i++)
                                   p = fun(p);
                               return p;
                           });
        },
        100, reftime);
    if (ref.size() == 0)
        ref = std::move(cur);
    else
        assert(cur == ref);
    return time;
}

int main() {
    std::srand(std::time(0));

    // Perm16 p = {5, 4, 12, 15, 10, 8, 9, 2, 3, 13, 14, 0, 1, 7, 11, 6};

    Perm16 p = Perm16::random();

    cout << p << endl << p.inverse_ref() << endl;
    cout << p.inverse_pow() << endl;

    assert(p.inverse_ref() == p.inverse_sort());
    assert(p.inverse_ref() == p.inverse_find());
    assert(p.inverse_ref() == p.inverse_cycl());
    assert(p.inverse_ref() == p.inverse_pow());
    assert(p * p.inverse_find() == Perm16::one());
    assert(p.inverse_find() * p == Perm16::one());
    assert(p * p.inverse_cycl() == Perm16::one());
    assert(p.inverse_cycl() * p == Perm16::one());

    uint_fast64_t sz = 10000;
    auto sample = rand_perms(sz);
    std::vector<Perm16> refres;
    cout << "Ref  : ";
    double tmref =
        timecheck([](Perm16 p) { return p.inverse_ref(); }, sample, refres);
    cout << "Arr  : ";
    timecheck([](Perm16 p) { return p.inverse_arr(); }, sample, refres, tmref);
    cout << "Sort : ";
    timecheck([](Perm16 p) { return p.inverse_sort(); }, sample, refres, tmref);
    cout << "Find : ";
    timecheck([](Perm16 p) { return p.inverse_find(); }, sample, refres, tmref);
    cout << "Pow  : ";
    timecheck([](Perm16 p) { return p.inverse_pow(); }, sample, refres, tmref);
    cout << "Cycl : ";
    timecheck([](Perm16 p) { return p.inverse_cycl(); }, sample, refres, tmref);

    return EXIT_SUCCESS;
}
