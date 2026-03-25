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

/** @file
@brief declaration of HPCombi::PermGeneric */

#ifndef HPCOMBI_PERM_GENERIC_HPP_
#define HPCOMBI_PERM_GENERIC_HPP_

#include <algorithm>         // for shuffle
#include <array>             // for array
#include <cstddef>           // for size_t
#include <cstdint>           // for uint64_t, uint8_t
#include <functional>        // for hash
#include <initializer_list>  // for initializer_list
#include <memory>            // for hash
#include <random>            // for mt19937
#include <type_traits>       // for is_trivial

#include "debug.hpp"         // for HPCOMBI_ASSERT
#include "vect_generic.hpp"  // for VectGeneric

namespace HPCombi {

/** Vanilla (ie NOT optimized) implementation of a permutation, used to check
for test correctness and as baseline to measure speedup. Implemented as an std
array, so the permutation is not necessarily of size n=16. PermGeneric<16>
should implement as much as possibles of Perm16 (currently not everything due to
lack of time/need). No optimisation, so prefer to use Perm16.

About Expo, see comment on HPCombi::VectGeneric.
*/
template <size_t Size, typename Expo = uint8_t>
struct PermGeneric : public VectGeneric<Size, Expo> {
    using vect = VectGeneric<Size, Expo>;

    static constexpr size_t size() { return Size; }

    PermGeneric() = default;
    PermGeneric(const vect v) : vect(v) {}  // NOLINT
    // Not marked explicit because we want to be able to pass non-initializer
    // lists here
    PermGeneric(std::initializer_list<Expo> il);  // NOLINT

    PermGeneric operator*(const PermGeneric &p) const {
        return this->permuted(p);
    }
    static PermGeneric one() { return PermGeneric({}); }
    static PermGeneric elementary_transposition(uint64_t i);

    PermGeneric inverse() const;
    static PermGeneric random();

    vect lehmer() const;
    uint64_t length() const;
    uint64_t nb_descents() const;
    uint64_t nb_cycles() const;

    bool left_weak_leq(PermGeneric other) const;
};

///////////////////////////////////////////////////////////////////////////////
// Memory layout concepts check  //////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static_assert(sizeof(VectGeneric<12>) == sizeof(PermGeneric<12>),
              "VectGeneric and PermGeneric have a different memory layout !");
static_assert(std::is_trivial<PermGeneric<12>>(),
              "PermGeneric is not trivial !");

}  //  namespace HPCombi

#include "perm_generic_impl.hpp"

#endif  // HPCOMBI_PERM_GENERIC_HPP_
