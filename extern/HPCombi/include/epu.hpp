//****************************************************************************//
//     Copyright (C) 2016-2018 Florent Hivert <Florent.Hivert@lri.fr>,        //
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

#ifndef HPCOMBI_EPU_HPP_INCLUDED
#define HPCOMBI_EPU_HPP_INCLUDED

#include <array>
#include <cassert>
#include <cstdint>
#include <functional>  // less<>, equal_to<>
#include <iomanip>
#include <ostream>
#include <x86intrin.h>

#ifdef HPCOMBI_HAVE_CONFIG
#include "HPCombi-config.h"
#endif

#if __cplusplus <= 201103L
#include "fallback/seq.hpp"
#endif

#include "vect_generic.hpp"

#ifdef HPCOMBI_CONSTEXPR_FUN_ARGS
#define HPCOMBI_CONSTEXPR constexpr
#define HPCOMBI_CONSTEXPR_CONSTRUCTOR constexpr
#else
#pragma message "Using a constexpr broken compiler ! "                         \
                "Performance may not be optimal"
#define HPCOMBI_CONSTEXPR const
#define HPCOMBI_CONSTEXPR_CONSTRUCTOR
#endif


namespace HPCombi {

/// Unsigned 8 bits int constant.
inline constexpr uint8_t operator"" _u8(unsigned long long arg) noexcept {
    return static_cast<uint8_t>(arg);
}

/// SIMD vector of 16 unsigned bytes
using epu8 = uint8_t __attribute__((vector_size(16)));

static_assert(alignof(epu8) == 16,
              "epu8 type is not properly aligned by the compiler !");

/// SIMD vector of 32 unsigned bytes
using xpu8 = uint8_t __attribute__((vector_size(32)));

static_assert(alignof(xpu8) == 32,
              "xpu8 type is not properly aligned by the compiler !");


namespace {  // Implementation detail code

/// A handmade C++11 constexpr lambda
template <typename T> struct ConstFun {
    HPCOMBI_CONSTEXPR_CONSTRUCTOR ConstFun(T cc) : cst(cc) {}
    HPCOMBI_CONSTEXPR T operator()(T) const { return cst; }
    /// constant value for constexpr lambda
    T cst;
};

/// Factory object for various SIMD constants in particular constexpr
template <class TPU> struct TPUBuild {

    using type_elem =
        typename std::remove_reference<decltype((TPU{})[0])>::type;
    static constexpr size_t size_elem = sizeof(type_elem);
    static constexpr size_t size = sizeof(TPU) / size_elem;
    using array = std::array<type_elem, size>;

    template <class Fun, std::size_t... Is>
    static HPCOMBI_CONSTEXPR TPU make_helper(Fun f,
                                             std::index_sequence<Is...>) {
        return TPU{f(Is)...};
    }

    inline TPU operator()(const std::initializer_list<type_elem> il,
                          type_elem def) const {
        assert(il.size() <= size);
        array res;
        std::copy(il.begin(), il.end(), res.begin());
        std::fill(res.begin() + il.size(), res.end(), def);
        return reinterpret_cast<const TPU &>(res);
    }

    template <class Fun> inline HPCOMBI_CONSTEXPR TPU operator()(Fun f) const {
        return make_helper(f, std::make_index_sequence<size>{});
    }

    inline HPCOMBI_CONSTEXPR TPU operator()(type_elem c) const {
        return make_helper(ConstFun<type_elem>(c),
                           std::make_index_sequence<size>{});
    }
    // explicit overloading for int constants
    inline HPCOMBI_CONSTEXPR TPU operator()(int c) const {
        return operator()(uint8_t(c));
    }
    inline HPCOMBI_CONSTEXPR TPU operator()(size_t c) const {
        return operator()(uint8_t(c));
    }
};

// The following functions should be constexpr lambdas writen directly in
// their corresponding methods. However until C++17, constexpr lambda are
// forbidden. So we put them here.
/// The image of i by the identity function
HPCOMBI_CONSTEXPR uint8_t id_fun(uint8_t i) { return i; }
/// The image of i by the left cycle function
HPCOMBI_CONSTEXPR uint8_t left_cycle_fun(uint8_t i) { return (i + 15) % 16; }
/// The image of i by the right cycle function
HPCOMBI_CONSTEXPR
uint8_t right_cycle_fun(uint8_t i) { return (i + 1) % 16; }
/// The image of i by a left shift duplicating the hole
HPCOMBI_CONSTEXPR
uint8_t left_dup_fun(uint8_t i) { return i == 15 ? 15 : i + 1; }
/// The image of i by a right shift duplicating the hole
HPCOMBI_CONSTEXPR
uint8_t right_dup_fun(uint8_t i) { return i == 0 ? 0 : i - 1; }
/// The complement of i to 15
HPCOMBI_CONSTEXPR
uint8_t complement_fun(uint8_t i) { return 15 - i; }
HPCOMBI_CONSTEXPR uint8_t popcount4_fun(uint8_t i) {
    return ((i & 1) != 0 ? 1 : 0)
        +  ((i & 2) != 0 ? 1 : 0)
        +  ((i & 4) != 0 ? 1 : 0)
        +  ((i & 8) != 0 ? 1 : 0);
}

}  // Anonymous namespace


/// Factory object for various SIMD constants in particular constexpr
TPUBuild<epu8> Epu8;

/// The indentity #HPCombi::epu8
HPCOMBI_CONSTEXPR epu8 epu8id = Epu8(id_fun);
/// The reverted identity #HPCombi::epu8
HPCOMBI_CONSTEXPR epu8 epu8rev = Epu8(complement_fun);
/// Left cycle #HPCombi::epu8 permutation
HPCOMBI_CONSTEXPR epu8 left_cycle = Epu8(left_cycle_fun);
/// Right cycle #HPCombi::epu8 permutation
HPCOMBI_CONSTEXPR epu8 right_cycle = Epu8(right_cycle_fun);
/// Left shift #HPCombi::epu8, duplicating the rightmost entry
HPCOMBI_CONSTEXPR epu8 left_dup = Epu8(left_dup_fun);
/// Right shift #HPCombi::epu8, duplicating the leftmost entry
HPCOMBI_CONSTEXPR epu8 right_dup = Epu8(right_dup_fun);
/// Popcount #HPCombi::epu8: the ith entry contains the number of bits set in i
HPCOMBI_CONSTEXPR epu8 popcount4 = Epu8(popcount4_fun);

/** Cast a #HPCombi::epu8 to a c++ \c std::array
 *
 *  This is usually faster for algorithm using a lot of indexed acces.
 */
inline TPUBuild<epu8>::array &as_array(epu8 &v) {
    return reinterpret_cast<typename TPUBuild<epu8>::array &>(v);
}
/** Cast a constant #HPCombi::epu8 to a C++ \c std::array
 *
 *  This is usually faster for algorithm using a lot of indexed acces.
 */
inline const TPUBuild<epu8>::array &as_array(const epu8 &v) {
    return reinterpret_cast<const typename TPUBuild<epu8>::array &>(v);
}
/** Cast a C++ \c std::array to a #HPCombi::epu8 */
// Passing the argument by reference triggers a segfault in gcc
// Since vector types doesn't belongs to the standard, I didn't manage
// to know if I'm using undefined behavior here.
inline epu8 from_array(TPUBuild<epu8>::array a) {
    return reinterpret_cast<const epu8 &>(a);
}

/** Cast a #HPCombi::epu8 to a c++ #HPCombi::VectGeneric
 *
 *  This is usually faster for algorithm using a lot of indexed acces.
 */
inline VectGeneric<16> &as_VectGeneric(epu8 &v) {
    return reinterpret_cast<VectGeneric<16> &>(as_array(v));
}

/** Cast a #HPCombi::epu8 to a c++ #HPCombi::VectGeneric
 *
 *  This is usually faster for algorithm using a lot of indexed acces.
 */
inline const VectGeneric<16> &as_VectGeneric(const epu8 &v) {
    return reinterpret_cast<const VectGeneric<16> &>(as_array(v));
}

/** Test whether all the entries of a #HPCombi::epu8 are zero */
inline bool is_all_zero(epu8 a) { return _mm_testz_si128(a, a); }
/** Test whether all the entries of a #HPCombi::epu8 are one */
inline bool is_all_one(epu8 a) { return _mm_testc_si128(a, Epu8(0xFF)); }

/** Equality of #HPCombi::epu8 */
inline bool equal(epu8 a, epu8 b) { return is_all_zero(_mm_xor_si128(a, b)); }
/** Non equality of #HPCombi::epu8 */
inline bool not_equal(epu8 a, epu8 b) { return not equal(a, b); }

/** Permuting a #HPCombi::epu8 */
inline epu8 permuted(epu8 a, epu8 b) { return _mm_shuffle_epi8(a, b); }
/** Left shifted of a #HPCombi::epu8 inserting a 0
 * @warning we use the convention that the 0 entry is on the left !
 */
inline epu8 shifted_right(epu8 a) { return _mm_bslli_si128(a, 1); }
/** Right shifted of a #HPCombi::epu8 inserting a 0
 * @warning we use the convention that the 0 entry is on the left !
 */
inline epu8 shifted_left(epu8 a) { return _mm_bsrli_si128(a, 1); }
/** Reverting a #HPCombi::epu8 */
inline epu8 reverted(epu8 a) { return permuted(a, epu8rev); }

/** Vector min between two #HPCombi::epu8 0 */
inline epu8 min(epu8 a, epu8 b) { return _mm_min_epu8(a, b); }
/** Vector max between two #HPCombi::epu8 0 */
inline epu8 max(epu8 a, epu8 b) { return _mm_max_epu8(a, b); }

/** Testing if a #HPCombi::epu8 is sorted */
inline bool is_sorted(epu8 a);
/** Return a sorted #HPCombi::epu8
 * @details
 * @par Algorithm:
 * Uses the 9 stages sorting network #sorting_rounds
 */
inline epu8 sorted(epu8 a);
/** Return a #HPCombi::epu8 with the two half sorted
 * @details
 * @par Algorithm: Uses a 6 stages sorting network #sorting_rounds8
 */
inline epu8 sorted8(epu8 a);
/** Return a reverse sorted #HPCombi::epu8
 * @details
 * @par Algorithm:
 * Uses the 9 stages sorting network #sorting_rounds
 */
inline epu8 revsorted(epu8 a);
/** Return a #HPCombi::epu8 with the two half reverse sorted
 * @details
 * @par Algorithm: Uses a 6 stages sorting network #sorting_rounds8
 */
inline epu8 revsorted8(epu8 a);

/** Sort \c this and return the sorting permutation
 * @details
 * @par Algorithm: Uses a 9 stages sorting network #sorting_rounds8
 */
inline epu8 sort_perm(epu8 & a);
/** Sort \c this and return the sorting permutation
 * @details
 * @par Algorithm: Uses a 9 stages sorting network #sorting_rounds8
 */
inline epu8 sort8_perm(epu8 & a);


/** Find if a vector is a permutation of one other
 * @details
 * @param a, b: two #HPCombi::epu8
 * @returns a #HPCombi::epu8
 * For each @f$0 \leq i < 16@f$, \c res[i] is the position in \c a of \c b[i]
     if \c b[i] appears exactly once in \c a, or undefined if not.
 */
inline epu8 permutation_of(epu8 a, epu8 b);

/** A prime number good for hashing */
const uint64_t prime = 0x9e3779b97f4a7bb9;

/** A random #HPCombi::epu8
 * @details
 * @param bnd : the upper bound for the value of the entries
 * @returns a random #HPCombi::epu8 with value in the interval
 *    @f$[0, 1, 2, ..., bnd-1]@f$.
 */
inline epu8 random_epu8(uint16_t bnd);

/** Remove duplicates in a sorted #HPCombi::epu8
 * @details
 * @param a: supposed to be sorted
 * @param repl: the value replacing the duplicate entries (default to 0)
 * @return a where repeated occurences of entries are replaced by \c repl
 */
inline epu8 remove_dups(epu8 a, uint8_t repl = 0);

/** @class common_horiz_sum
 * @brief Horizontal sum of a  #HPCombi::epu8
 * @details
 * @returns the horizontal sum of the input
 * @par Example:
 * @code
 * horiz_sum(epu8 { 5, 5, 2, 5, 1, 6,12, 4, 0, 3, 2,11,12,13,14,15});
 * @endcode
 * Returns `110`
 * @warning The result is supposed to fit in a \c uint8_t
 */
/** @copydoc common_horiz_sum
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and indexed access
 */
inline uint8_t horiz_sum_ref(epu8);
/** @copydoc common_horiz_sum
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and indexed access
 *  through #HPCombi::VectGeneric
 */
inline uint8_t horiz_sum_gen(epu8);
/** @copydoc common_horiz_sum
 *  @par Algorithm:
 *  4-stages paralell algorithm
 */
inline uint8_t horiz_sum4(epu8);
/** @copydoc common_horiz_sum
 *  @par Algorithm:
 *  3-stages paralell algorithm + indexed access
 */
inline uint8_t horiz_sum3(epu8);
/** @copydoc common_horiz_sum */
inline uint8_t horiz_sum(epu8 v) { return horiz_sum3(v); }

/** @class common_partial_sums
 * @brief Horizontal partial sum of a #HPCombi::epu8
 * @details
 * @returns the partials sums of the input
 * @par Example:
 * @code
 * partial_sums(epu8 { 5, 5, 2, 5, 1, 6,12, 4, 0, 3, 2,11,12,13,14,15});
 * @endcode
 * Returns `{ 5,10,12,17,18,24,36,40,40,43,45,56,68,81,95,110}`
 */
/** @copydoc common_partial_sums
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and indexed access
 */
inline epu8 partial_sums_ref(epu8);
/** @copydoc common_partial_sums
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and indexed access
 *  through #HPCombi::VectGeneric
 */
inline epu8 partial_sums_gen(epu8);
/** @copydoc common_partial_sums
 *  @par Algorithm:
 *  4-stages paralell algorithm
 */
inline epu8 partial_sums_round(epu8);
/** @copydoc common_partial_sums */
inline epu8 partial_sums(epu8 v) { return partial_sums_round(v); }


/** @class common_horiz_max
 * @brief Horizontal sum of a  #HPCombi::epu8
 * @details
 * @returns the horizontal sum of the input
 * @par Example:
 * @code
 * horiz_max(epu8 { 5, 5, 2, 5, 1, 6,12, 4, 0, 3, 2, 0,12, 0, 0, 0});
 * @endcode
 * Returns `12`
 */
/** @copydoc common_horiz_max
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and indexed access
 */
inline uint8_t horiz_max_ref(epu8);
/** @copydoc common_horiz_max
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and indexed access
 *  through #HPCombi::VectGeneric
 */
inline uint8_t horiz_max_gen(epu8);
/** @copydoc common_horiz_max
 *  @par Algorithm:
 *  4-stages paralell algorithm
 */
inline uint8_t horiz_max4(epu8);
/** @copydoc common_horiz_max
 *  @par Algorithm:
 *  3-stages paralell algorithm + indexed access
 */
inline uint8_t horiz_max3(epu8);
/** @copydoc common_horiz_max */
inline uint8_t horiz_max(epu8 v) { return horiz_max4(v); }

/** @class common_partial_max
 * @brief Horizontal partial sum of a #HPCombi::epu8
 * @details
 * @returns the partials max of the input
 * @par Example:
 * @code
 * partial_max(epu8 { 5, 5, 2, 5, 1, 6,12, 4, 0, 3, 2,11,12,13,14,15});
 * @endcode
 * Returns `{ 5, 5, 5, 5, 5, 6,12,12,12,12,12,12,12,13,14,15}`
 */
/** @copydoc common_partial_max
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and indexed access
 */
inline epu8 partial_max_ref(epu8);
/** @copydoc common_partial_max
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and indexed access
 *  through #HPCombi::VectGeneric
 */
inline epu8 partial_max_gen(epu8);
/** @copydoc common_partial_max
 *  @par Algorithm:
 *  4-stages paralell algorithm
 */
inline epu8 partial_max_round(epu8);
/** @copydoc common_partial_max */
inline epu8 partial_max(epu8 v) { return partial_max_round(v); }


/** @class common_horiz_min
 * @brief Horizontal sum of a  #HPCombi::epu8
 * @details
 * @returns the horizontal sum of the input
 * @par Example:
 * @code
 * horiz_min(epu8 { 5, 5, 2, 5, 1, 6,12, 4, 1, 3, 2, 2,12, 3, 4, 4});
 * @endcode
 * Returns `1`
 */
/** @copydoc common_horiz_min
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and indexed access
 */
inline uint8_t horiz_min_ref(epu8);
/** @copydoc common_horiz_min
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and indexed access
 *  through #HPCombi::VectGeneric
 */
inline uint8_t horiz_min_gen(epu8);
/** @copydoc common_horiz_min
 *  @par Algorithm:
 *  4-stages paralell algorithm
 */
inline uint8_t horiz_min4(epu8);
/** @copydoc common_horiz_min
 *  @par Algorithm:
 *  3-stages paralell algorithm + indexed access
 */
inline uint8_t horiz_min3(epu8);
/** @copydoc common_horiz_min */
inline uint8_t horiz_min(epu8 v) { return horiz_min4(v); }

/** @class common_partial_min
 * @brief Horizontal partial sum of a #HPCombi::epu8
 * @details
 * @returns the partials min of the input
 * @par Example:
 * @code
 * partial_min(epu8 { 5, 5, 2, 5, 1, 6,12, 4, 0, 3, 2,11,12,13,14,15});
 * @endcode
 * Returns `{ 5, 5, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0}`
 */
/** @copydoc common_partial_min
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and indexed access
 */
inline epu8 partial_min_ref(epu8);
/** @copydoc common_partial_min
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and indexed access
 *  through #HPCombi::VectGeneric
 */
inline epu8 partial_min_gen(epu8);
/** @copydoc common_partial_min
 *  @par Algorithm:
 *  4-stages paralell algorithm
 */
inline epu8 partial_min_round(epu8);
/** @copydoc common_partial_min */
inline epu8 partial_min(epu8 v) { return partial_min_round(v); }


/** @class common_eval16
 * @brief Evaluation of a #HPCombi::epu8
 * @details
 * @param v : a #HPCombi::epu8
 * @returns the evaluation, that is the #HPCombi::epu8 \c r such that
 *     \c r[i] is the number of occurrence of \c i in the input \c v
 * @par Example:
 * @code
 * eval16(epu8 { 5, 5, 2, 5, 1, 6,12, 4, 0, 3, 2,11,12,13,14,15});
 * @endcode
 * Returns `{ 1, 1, 2, 1, 1, 3, 1, 0, 0, 0, 0, 1, 2, 1, 1, 1}`
 * @warning The entries larger than 15 are ignored
 */
/** @copydoc common_eval16
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and indexed access
 */
inline epu8 eval16_ref(epu8 v);
/** @copydoc common_eval16
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and cast to array
 */
inline epu8 eval16_arr(epu8 v);
/** @copydoc common_eval16
 *  @par Algorithm:
 *  Vector @f$O(n)@f$ using cyclic shifting
 */
inline epu8 eval16_cycle(epu8 v);
/** @copydoc common_eval16
 *  @par Algorithm:
 *  Vector @f$O(n)@f$ using popcount
 */
inline epu8 eval16_popcount(epu8 v);
/** @copydoc common_eval16 */
inline epu8 eval16(epu8 v) { return eval16_cycle(v); };

/** @class common_first_diff
 * @brief The first difference between two #HPCombi::epu8
 * @details
 * @param a, b : two #HPCombi::epu8
 * @param bound : a \c size_t
 * @returns the smallest index @f$i<bound@f$ such that \c a[i] and \c b[i]
 * differ, 16 if there is no differences before bound.
 * @par Example:
 * @code
 * epu8 a { 5, 5, 2, 5, 1, 6,12, 4, 0, 3, 2,11,12,13,14,15};
 * epu8 b { 5, 5, 2, 9, 1, 6,12, 4, 0, 4, 4, 4,12,13,14,15};
 * @endcode
 * then `first_diff(a, b)` returns `3`,
 * `first_diff(a, b, 3)` returns `16`,
 * `first_diff(a, b, 4)` returns `3`,
 * `first_diff(a, b, 7)` returns `3`.
 * @warning `bound` is assumed to be smaller or equal than 16
 */
/** @copydoc common_first_diff
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and indexed access
 */
inline uint64_t first_diff_ref(epu8 a, epu8 b, size_t bound = 16);
/** @copydoc common_first_diff
 *  @par Algorithm:
 *  Using \c cmpestri instruction
 */
inline uint64_t first_diff_cmpstr(epu8 a, epu8 b, size_t bound = 16);
/** @copydoc common_first_diff
 *  @par Algorithm:
 *  Using vector comparison and mask
 */
inline uint64_t first_diff_mask(epu8 a, epu8 b, size_t bound = 16);
/** @copydoc common_first_diff */
inline uint64_t first_diff(epu8 a, epu8 b, size_t bound = 16) {
    return first_diff_mask(a, b, bound);
}

/** @class common_last_diff
 * @brief The last difference between two #HPCombi::epu8
 * @details
 * @param a, b : two #HPCombi::epu8
 * @param bound : a \c size_t
 * @returns the largest index @f$i<bound@f$ such that \c a[i] and \c b[i]
 * differ, 16 if there is no differences before bound.
 * @par Example:
 * @code
 * epu8 a { 5, 5, 2, 5, 1, 6,12, 4, 0, 3, 2,11,12,13,14,15};
 * epu8 b { 5, 5, 2, 9, 1, 6,12, 4, 0, 4, 4, 4,12,13,14,15};
 * @endcode
 * then `last_diff(a, b)` returns `11`,
 * `last_diff(a, b, 3)` returns `16`,
 * `last_diff(a, b, 4)` returns `3`,
 * `last_diff(a, b, 7)` returns `3`.
 * @warning `bound` is assumed to be smaller or equal than 16
 */
/** @copydoc common_last_diff
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and indexed access
 */
inline uint64_t last_diff_ref(epu8 a, epu8 b, size_t bound = 16);
/** @copydoc common_last_diff
 *  @par Algorithm:
 *  Using \c cmpestri instruction
 */
inline uint64_t last_diff_cmpstr(epu8 a, epu8 b, size_t bound = 16);
/** @copydoc common_last_diff
 *  @par Algorithm:
 *  Using vector comparison and mask
 */
inline uint64_t last_diff_mask(epu8 a, epu8 b, size_t bound = 16);
/** @copydoc common_last_diff */
inline uint64_t last_diff(epu8 a, epu8 b, size_t bound = 16) {
    return last_diff_mask(a, b, bound);
}

/** Lexicographic comparison between two #HPCombi::epu8 */
inline bool less(epu8 a, epu8 b);
/** Partial lexicographic comparison between two #HPCombi::epu8
 * @param a, b : the vectors to compare
 * @param k : the bound for the lexicographic comparison
 * @return a positive, negative or zero char depending on the result
 */
inline char less_partial(epu8 a, epu8 b, int k);

/** return the index of the first zero entry or 16 if there are none
 *  Only index smaller than bound are taken into account.
 */
inline uint64_t first_zero(epu8 v, int bnd);
/** return the index of the last zero entry or 16 if there are none
 *  Only index smaller than bound are taken into account.
 */
inline uint64_t last_zero(epu8 v, int bnd);
/** return the index of the first non zero entry or 16 if there are none
 *  Only index smaller than bound are taken into account.
 */
inline uint64_t first_non_zero(epu8 v, int bnd);
/** return the index of the last non zero entry or 16 if there are none
 *  Only index smaller than bound are taken into account.
 */
inline uint64_t last_non_zero(epu8 v, int bnd);

/** a vector popcount function
 */
inline epu8 popcount16(epu8 v);

/** Test for partial transformation
 * @details
 * @returns whether \c v is a partial transformation.
 * @param v the vector to test
 * @param k the size of \c *this (default 16)
 *
 * Points where the function is undefined are mapped to \c 0xff. If \c *this
 * is a tranformation of @f$0\dots n-1@f$ for @f$n<16@f$, it should be completed
 * to a transformation of @f$0\dots 15@f$ by adding fixed points. That is the
 * values @f$i\geq n@f$ should be mapped to themself.
 * @par Example:
 * The partial tranformation
 * @f$\begin{matrix}0 1 2 3 4 5\\ 2 0 5 . . 4 \end{matrix}@f$
 * is encoded by the array {2,0,5,0xff,0xff,4,6,7,8,9,10,11,12,13,14,15}
 */
inline bool is_partial_transformation(epu8 v, const size_t k = 16);

/** Test for transformation
 * @details
 * @returns whether \c *this is a transformation.
 * @param v the vector to test
 * @param k the size of \c *this (default 16)
 *
 * If \c *this is a tranformation of @f$0\dots n-1@f$ for @f$n<16@f$,
 * it should be completed to a transformation of @f$0\dots 15@f$
 * by adding fixed points. That is the values @f$i\geq n@f$ should be
 * mapped to themself.
 * @par Example:
 * The tranformation
 * @f$\begin{matrix}0 1 2 3 4 5\\ 2 0 5 2 1 4 \end{matrix}@f$
 * is encoded by the array {2,0,5,2,1,4,6,7,8,9,10,11,12,13,14,15}
 */
inline bool is_transformation(epu8 v, const size_t k = 16);

/** Test for partial permutations
 * @details
 * @returns whether \c *this is a partial permutation.
 * @param v the vector to test
 * @param k the size of \c *this (default 16)
 *
 * Points where the function is undefined are mapped to \c 0xff.
 * If \c *this is a partial permutation of @f$0\dots n-1@f$ for @f$n<16@f$,
 * it should be completed to a partial permutation of @f$0\dots 15@f$
 * by adding fixed points. That is the values @f$i\geq n@f$ should be
 * mapped to themself.
 * @par Example:
 * The permutation
 * @f$\begin{matrix}0 1 2 3 4 5\\ 2 0 5 . . 4 \end{matrix}@f$
 * is encoded by the array {2,0,5,0xFF,0xFF,4,6,7,8,9,10,11,12,13,14,15}
 */
inline bool is_partial_permutation(epu8 v, const size_t k = 16);

/** Test for permutations
 * @details
 * @returns whether \c *this is a permutation.
 * @param v the vector to test
 * @param k the size of \c *this (default 16)
 *
 * If \c *this is a permutation of @f$0\dots n-1@f$ for @f$n<16@f$,
 * it should be completed to a permutation of @f$0\dots 15@f$
 * by adding fixed points. That is the values @f$i\geq n@f$ should be
 * mapped to themself.
 * @par Example:
 * The permutation
 * @f$\begin{matrix}0 1 2 3 4 5\\ 2 0 5 3 1 4 \end{matrix}@f$
 * is encoded by the array {2,0,5,3,1,4,6,7,8,9,10,11,12,13,14,15}
 */
inline bool is_permutation(epu8 v, const size_t k = 16);

}  // namespace HPCombi

namespace std {

inline std::ostream &operator<<(std::ostream &stream, HPCombi::epu8 const &a);

/** We also specialize the struct
 *  - std::equal_to<epu8>
 *  - std::not_equal_to<epu8>
 *  - std::hash<epu8>
 *  - std::less<epu8>
 */
}

#include "epu_impl.hpp"

#endif  // HPCOMBI_EPU_HPP_INCLUDED
