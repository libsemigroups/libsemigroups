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

#include <array>
#include <cassert>
#include <cstdint>
#include <functional>  // less<>
#include <iostream>
#include <vector>
#ifdef HPCOMBI_HAVE_DENSEHASHSET
#include <sparsehash/dense_hash_set>
#else
#include <unordered_set>
#endif

#include "hpcombi/perm16.hpp"

using HPCombi::epu8;
using HPCombi::PTransf16;

const PTransf16 id{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

const PTransf16 s0{0, 1, 2, 3, 4, 5, 6, 8, 7, 9, 10, 11, 12, 13, 14, 15};

const PTransf16 s1e{0, 1, 2, 3, 4, 5, 7, 6, 9, 8, 10, 11, 12, 13, 14, 15};
const PTransf16 s1f{0, 1, 2, 3, 4, 5, 8, 9, 6, 7, 10, 11, 12, 13, 14, 15};

const PTransf16 s2{0, 1, 2, 3, 4, 6, 5, 7, 8, 10, 9, 11, 12, 13, 14, 15};
const PTransf16 s3{0, 1, 2, 3, 5, 4, 6, 7, 8, 9, 11, 10, 12, 13, 14, 15};
const PTransf16 s4{0, 1, 2, 4, 3, 5, 6, 7, 8, 9, 10, 12, 11, 13, 14, 15};
const PTransf16 s5{0, 1, 3, 2, 4, 5, 6, 7, 8, 9, 10, 11, 13, 12, 14, 15};
const PTransf16 s6{0, 2, 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 13, 15};
const PTransf16 s7{1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 14};

const uint8_t FF = 0xff;
const uint8_t FE = 0xfe;

const PTransf16 gene{FF, FF, FF, FF, FF, FF, FF, FF,
                     8,  9,  10, 11, 12, 13, 14, 15};
const PTransf16 genf{FF, FF, FF, FF, FF, FF, FF, 7,
                     FF, 9,  10, 11, 12, 13, 14, 15};

inline PTransf16 act1(PTransf16 x, PTransf16 y) { return x * y; }

inline PTransf16 act0(PTransf16 x, PTransf16 y) {
    PTransf16 minab, maxab, mask, b = x * y;
    mask = simde_mm_cmplt_epi8(y, PTransf16::one());
    minab = simde_mm_min_epi8(x, b);
    maxab = simde_mm_max_epi8(x, b);
    return static_cast<epu8>(simde_mm_blendv_epi8(maxab, minab, mask)) |
           (y.v == HPCombi::Epu8(0xFF));
}

int main() {
    using namespace std;
    // vector<PTransf16> gens {gene, genf, s1e, s1f, s2, s3, s4, s5};
    // vector<PTransf16> gens {gene, genf, s1e, s1f};
    vector<PTransf16> gens{gene, genf, s1e, s1f, s2, s3, s4, s5, s6};
    // vector<PTransf16> gens {gene, s1e, s2, s3, s4, s5, s6};
    // const PTransf16 toFind =
    //  {FF,FF,FF,FF,FF,FF,FF,FF,  FF, FF, FF, FF, FF, 13, 14, 15};
    // cout << act0(s2,genf) << endl;
    int lg = 0;

#ifdef HPCOMBI_HAVE_DENSEHASHSET
    // using google::sparse_hash_set;
    // sparse_hash_set<PTransf16, hash<PTransf16>, equal_to<PTransf16>> res;

    using google::dense_hash_set;
    dense_hash_set<PTransf16, hash<PTransf16>, equal_to<PTransf16>> res;
    res.set_empty_key(
        {FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE});
    res.resize(250000000);
#else
    unordered_set<PTransf16> res;
    res.reserve(250000000);
#endif

    res.insert(id);

    vector<PTransf16> todo, newtodo;
    todo.push_back(id);
    while (todo.size()) {
        newtodo.clear();
        lg++;
        for (auto v : todo) {
            for (auto g : gens) {
                auto el = act0(v, g);
                if (res.insert(el).second)
                    newtodo.push_back(el);
                //        if (el == toFind) cout << v << endl;
            }
        }
        std::swap(todo, newtodo);
        cout << lg << ", todo = " << todo.size() << ", res = " << res.size()
             << ", #Bucks = " << res.bucket_count() << endl;
        // cout << "Trouve " << (res.find(toFind) != res.end()) << endl;
        // if (res.find(toFind) != res.end()) break;
    }
    cout << "res =  " << res.size() << endl;
    assert(res.size() == 248318309);
    // 1  2   3     4      5       6         7          8
    // 4 37 541 10625 258661 7464625 248318309 9339986689

    exit(0);
}
