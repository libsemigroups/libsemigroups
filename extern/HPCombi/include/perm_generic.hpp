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

#ifndef HPCOMBI_PERM_GENERIC_HPP
#define HPCOMBI_PERM_GENERIC_HPP

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <ostream>

namespace HPCombi {

template <size_t _Size, typename Expo = uint8_t>
struct PermGeneric : public VectGeneric<_Size, Expo> {

    using vect = VectGeneric<_Size, Expo>;

    static constexpr size_t size() { return _Size; }

    PermGeneric() = default;
    PermGeneric(const vect v) : vect(v) {};
    PermGeneric(std::initializer_list<Expo> il);

    PermGeneric operator*(const PermGeneric &p) const {
        return this->permuted(p);
    }
    static PermGeneric one() { return PermGeneric({}); }
    static PermGeneric elementary_transposition(uint64_t i);

    PermGeneric inverse() const ;
    static PermGeneric random();

    vect lehmer() const;
    uint64_t length() const;
    uint64_t nb_descents() const;
    uint64_t nb_cycles() const;

    bool left_weak_leq(PermGeneric other) const;

};

/*****************************************************************************/
/** Memory layout concepts check  ********************************************/
/*****************************************************************************/

static_assert(sizeof(VectGeneric<12>) == sizeof(PermGeneric<12>),
              "VectGeneric and PermGeneric have a different memory layout !");
static_assert(std::is_trivial<PermGeneric<12>>(),
              "PermGeneric is not trivial !");

}  //  namespace HPCombi

#include "perm_generic_impl.hpp"

#endif  // HPCOMBI_PERM_GENERIC_HPP
