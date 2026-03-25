//****************************************************************************//
//     Copyright (C) 2023-2024 Florent Hivert <Florent.Hivert@lisn.fr>,       //
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
@brief HPCombi::TPUBuild and casts from HPCombi::TPU */

#ifndef HPCOMBI_BUILDER_HPP_
#define HPCOMBI_BUILDER_HPP_

#include <array>             // for array
#include <cstddef>           // for size_t
#include <initializer_list>  // for initializer_list
#include <type_traits>       // for remove_reference_t
#include <utility>           // for make_index_sequence, ind...

#include "vect_generic.hpp"  // for VectGeneric

namespace HPCombi {

/** Given a transformation from 0..15 → 0..15,
 * build at compile-time the array representing the transformation.
 *
 * @details
 * Class for factory object associated to a SIMD packed unsigned integers.
 * The main purpose of this class is to be able to construct in a \c constexpr
 * way various instances of the \c TPU SIMD vector type. The behavior of
 * an instance of \c TPUBuild<TPU> is designed to mimic the behavior of \c TPU
 * if it where a class:
 * - calling \c operator() on an instance which acts similarly to a
 *   class constructor,
 * - calling a member function such as #id acts as a static member function.
 */
template <class TPU> struct TPUBuild {
    /// Type of the elements
    using type_elem = typename std::remove_reference_t<decltype((TPU{})[0])>;

    /// Size of the elements
    static constexpr size_t size_elem = sizeof(type_elem);

    /// Number of elements
    static constexpr size_t size = sizeof(TPU) / size_elem;

    /// Array equivalent type
    using array = std::array<type_elem, size>;

    template <class Fun, decltype(size)... Is>
    static constexpr TPU make_helper(Fun f, std::index_sequence<Is...>) {
        static_assert(std::is_invocable_v<Fun, type_elem>);
        return TPU{f(Is)...};
    }

    /// Construct a TPU from an \c std::initializer_list and a default value
    inline constexpr TPU operator()(std::initializer_list<type_elem> il,
                                    type_elem def) const {
        HPCOMBI_ASSERT(il.size() <= size);
        array res;
        std::copy(il.begin(), il.end(), res.begin());
        std::fill(res.begin() + il.size(), res.end(), def);
        return reinterpret_cast<const TPU &>(res);
    }

    /// Construct a TPU from a function giving the values at \f$1,2,\dots\f$
    template <class Fun> inline constexpr TPU operator()(Fun f) const {
        static_assert(std::is_invocable_v<Fun, type_elem>);
        return make_helper(f, std::make_index_sequence<size>{});
    }

    /// Construct a constant TPU
    inline constexpr TPU operator()(type_elem c) const {
        return operator()([c](auto) { return c; });
    }
    /// explicit overloading for int constants
    inline constexpr TPU operator()(int c) const {
        return operator()(type_elem(c));
    }
    /// explicit overloading for size_t constants
    inline constexpr TPU operator()(size_t c) const {
        return operator()(type_elem(c));
    }

    /// explicit overloading for #array
    // Passing the argument by reference used to trigger a segfault in gcc
    // Since vector types doesn't belongs to the standard, I didn't manage
    // to know if I'm using undefined behavior here.
    inline constexpr TPU operator()(array a) const {
        return reinterpret_cast<const TPU &>(a);
    }

    /// Return the identity element of type \c TPU
    constexpr TPU id() const {
        return operator()([](type_elem i) { return i; });
    }
    /// Return the reversed element of type \c TPU
    constexpr TPU rev() const {
        return (*this)([](type_elem i) { return size - 1 - i; });
    }
    /// Left cycle \c TPU permutation
    constexpr TPU left_cycle() const {
        return (*this)([](type_elem i) { return (i + size - 1) % size; });
    }
    /// Right cycle \c TPU permutation
    constexpr TPU right_cycle() const {
        return (*this)([](type_elem i) { return (i + 1) % size; });
    }
    /// Left shift \c TPU, duplicating the rightmost entry
    constexpr TPU left_dup() const {
        return (*this)([](type_elem i) { return i == 15 ? 15 : i + 1; });
    }
    /// Right shift \c TPU, duplicating the leftmost entry
    constexpr TPU right_dup() const {
        return (*this)([](type_elem i) { return i == 0 ? 0 : i - 1; });
    }
    /// Popcount \c TPU: the ith entry contains the number of bits set in i
    constexpr TPU popcount() const {
        return (*this)([](type_elem i) {
            return (((i & 0x01) != 0 ? 1 : 0) + ((i & 0x02) != 0 ? 1 : 0) +
                    ((i & 0x04) != 0 ? 1 : 0) + ((i & 0x08) != 0 ? 1 : 0) +
                    ((i & 0x10) != 0 ? 1 : 0) + ((i & 0x20) != 0 ? 1 : 0) +
                    ((i & 0x40) != 0 ? 1 : 0) + ((i & 0x80) != 0 ? 1 : 0));
        });
    }
};

/** Cast a TPU to a c++ \c std::array
 *
 *  This is usually faster for algorithm using a lot of indexed access.
 */
template <class TPU>
inline typename TPUBuild<TPU>::array &as_array(TPU &v) noexcept {
    return reinterpret_cast<typename TPUBuild<TPU>::array &>(v);
}
/** Cast a constant TPU to a constant c++ \c std::array
 *
 *  This is usually faster for algorithm using a lot of indexed access.
 */
template <class TPU>
inline const typename TPUBuild<TPU>::array &as_array(const TPU &v) noexcept {
    return reinterpret_cast<const typename TPUBuild<TPU>::array &>(v);
}

/** Cast a #HPCombi::epu8 to a c++ #HPCombi::VectGeneric
 *
 *  This is usually faster for algorithm using a lot of indexed access.
 */
template <class TPU>
inline VectGeneric<TPUBuild<TPU>::size> &as_VectGeneric(TPU &v) {
    return reinterpret_cast<VectGeneric<TPUBuild<TPU>::size> &>(as_array(v));
}

/** Cast a #HPCombi::epu8 to a c++ #HPCombi::VectGeneric
 *
 *  This is usually faster for algorithm using a lot of indexed access.
 */
template <class TPU>
inline const VectGeneric<TPUBuild<TPU>::size> &as_VectGeneric(const TPU &v) {
    return reinterpret_cast<const VectGeneric<TPUBuild<TPU>::size> &>(
        as_array(v));
}

}  // namespace HPCombi

#endif  // HPCOMBI_BUILDER_HPP_
