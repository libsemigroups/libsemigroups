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

using HPCombi::Transf16;

// Full transformation semigroup on 7 points
const Transf16 s{1, 0, 2, 3, 4, 5, 6};
const Transf16 cy{1, 2, 3, 4, 5, 6, 0};
const Transf16 pi{0, 0, 2, 3, 4, 5, 6};
const std::vector<Transf16> gens{s, cy, pi};

/* Full transformation semigroup on 9 points
const Transf16 s  {1, 0, 2, 3, 4, 5, 6, 7, 8};
const Transf16 cy {1, 2, 3, 4, 5, 6, 7, 8, 0};
const Transf16 pi {0, 0, 2, 3, 4, 5, 6, 7, 8};
const std::vector<Transf16> gens{s, cy, pi};
*/

/* James favourite
const Transf16 a1 {1, 7, 2, 6, 0, 4, 1, 5};
const Transf16 a2 {2, 4, 6, 1, 4, 5, 2, 7};
const Transf16 a3 {3, 0, 7, 2, 4, 6, 2, 4};
const Transf16 a4 {3, 2, 3, 4, 5, 3, 0, 1};
const Transf16 a5 {4, 3, 7, 7, 4, 5, 0, 4};
const Transf16 a6 {5, 6, 3, 0, 3, 0, 5, 1};
const Transf16 a7 {6, 0, 1, 1, 1, 6, 3, 4};
const Transf16 a8 {7, 7, 4, 0, 6, 4, 1, 7};
const vector<Transf16> gens{a1,a2,a3,a4,a5,a6,a7,a8};
*/

const uint8_t FE = 0xfe;

int main() {
    int lg = 0;

#ifdef HPCOMBI_HAVE_DENSEHASHSET
    using google::dense_hash_set;
    dense_hash_set<Transf16, std::hash<Transf16>, std::equal_to<Transf16>> res;
    res.set_empty_key(
        {FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE});
    // res.resize(500000000);
#else
    using std::unordered_set;
    unordered_set<Transf16> res;
    // res.reserve(500000000);
#endif

    res.insert(Transf16::one());

    std::vector<Transf16> todo, newtodo;
    todo.push_back(Transf16::one());
    while (todo.size()) {
        newtodo.clear();
        lg++;
        for (auto v : todo) {
            for (auto g : gens) {
                auto el = v * g;
                if (res.insert(el).second)
                    newtodo.push_back(el);
            }
        }
        std::swap(todo, newtodo);
        std::cout << lg << ", todo = " << todo.size()
                  << ", res = " << res.size()
                  << ", #Bucks = " << res.bucket_count() << std::endl;
    }
    std::cout << "res =  " << res.size() << std::endl;
    exit(0);
}
