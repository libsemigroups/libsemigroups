////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////

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
