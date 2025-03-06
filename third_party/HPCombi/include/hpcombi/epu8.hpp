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
@brief declaration of HPCombi::epu8.

Contains renaming of some low level functions,
eg simde_mm_testz_si128(a,a) → is_all_zero(a) */

#ifndef HPCOMBI_EPU8_HPP_
#define HPCOMBI_EPU8_HPP_

#include <array>    // for array
#include <cstddef>  // for size_t
#include <cstdint>  // for uint8_t, uint64_t, int8_t
#include <ostream>  // for ostream
#include <string>   // for string

#include "builder.hpp"       // for TPUBuild
#include "debug.hpp"         // for HPCOMBI_ASSERT
#include "vect_generic.hpp"  // for VectGeneric

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wpacked"
#endif
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wbitwise-instead-of-logical"
#endif
#include "simde/x86/sse4.1.h"  // for simde_mm_max_epu8, simde...
#include "simde/x86/sse4.2.h"  // for ???
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

namespace HPCombi {

/// Unsigned 8 bits int constant.
inline constexpr uint8_t
operator"" _u8(unsigned long long arg) noexcept {  // NOLINT
    return static_cast<uint8_t>(arg);
}

/**
epu8 stands for *Extended Packed Unsigned, grouped by 8 bits*;
this is the low level type chosen by Intel for their API to intrinsics,
ie a SIMD vector of 16 unsigned bytes (16×8 = 128bits).
Functions using this type use semantically equivalent types,
eg a _m128 which is a vector containing 2 signed 64 bits integers.
A flag tells the compiler to silently consider those types equivalent.
*/
using epu8 = uint8_t __attribute__((vector_size(16)));

static_assert(alignof(epu8) == 16,
              "epu8 type is not properly aligned by the compiler !");

/** Factory object acting as a class constructor for type #HPCombi::epu8.
 * see #HPCombi::TPUBuild for usage and capability
 */
constexpr TPUBuild<epu8> Epu8{};

/** Test whether all the entries of a #HPCombi::epu8 are zero */
inline bool is_all_zero(epu8 a) noexcept { return simde_mm_testz_si128(a, a); }
/** Test whether all the entries of a #HPCombi::epu8 are one */
inline bool is_all_one(epu8 a) noexcept {
    return simde_mm_testc_si128(a, Epu8(0xFF));
}

/** Equality of #HPCombi::epu8 */
inline bool equal(epu8 a, epu8 b) noexcept {
    return is_all_zero(simde_mm_xor_si128(a, b));
}
/** Non equality of #HPCombi::epu8 */
inline bool not_equal(epu8 a, epu8 b) noexcept { return !equal(a, b); }

/** Apply a permutation \c b on the vector \c a: for i=0..16 {result[i] =
 * a[b[i]} */
inline epu8 permuted_ref(epu8 a, epu8 b) noexcept;

/** Same as \ref HPCombi::permuted_ref "permuted_ref"
but with an optimized implementation using intrinsics. */
inline epu8 permuted(epu8 a, epu8 b) noexcept {
    return simde_mm_shuffle_epi8(a, b);
}

/** Left shifted of a #HPCombi::epu8 inserting a 0
 * @warning we use the convention that the 0 entry is on the left !
 */
inline epu8 shifted_right(epu8 a) noexcept {
    return simde_mm_bslli_si128(a, 1);
}

/** Right shifted of a #HPCombi::epu8 inserting a 0
 * @warning we use the convention that the 0 entry is on the left !
 */
inline epu8 shifted_left(epu8 a) noexcept { return simde_mm_bsrli_si128(a, 1); }

/** Reverting a #HPCombi::epu8 */
inline epu8 reverted(epu8 a) noexcept { return permuted(a, Epu8.rev()); }

/** Vector min between two #HPCombi::epu8 0 */
inline epu8 min(epu8 a, epu8 b) noexcept { return simde_mm_min_epu8(a, b); }
/** Vector max between two #HPCombi::epu8 0 */
inline epu8 max(epu8 a, epu8 b) noexcept { return simde_mm_max_epu8(a, b); }

/** Testing if a #HPCombi::epu8 is sorted */
inline bool is_sorted(epu8 a) noexcept;
/** Return a sorted #HPCombi::epu8
 * @details
 * @par Algorithm:
 * Uses the 9 stages sorting network #sorting_rounds
 */
inline epu8 sorted(epu8 a) noexcept;
/** Return a #HPCombi::epu8 with both halves sorted
 * @details
 * @par Algorithm: Uses a 6 stages sorting network #sorting_rounds8
 */
inline epu8 sorted8(epu8 a) noexcept;
/** Return a reverse sorted #HPCombi::epu8
 * @details
 * @par Algorithm:
 * Uses the 9 stages sorting network #sorting_rounds
 */
inline epu8 revsorted(epu8 a) noexcept;
/** Return a #HPCombi::epu8 with both halves reverse sorted
 * @details
 * @par Algorithm: Uses a 6 stages sorting network #sorting_rounds8
 */
inline epu8 revsorted8(epu8 a) noexcept;

/** Sort \c this and return the sorting permutation
 * @details
 * @par Algorithm: Uses a 9 stages sorting network #sorting_rounds8
 */
inline epu8 sort_perm(epu8 &a) noexcept;
/** Sort \c this and return the sorting permutation
 * @details
 * @par Algorithm: Uses a 6 stages sorting network #sorting_rounds8
 */
inline epu8 sort8_perm(epu8 &a) noexcept;

/**
 * @brief Merge two sorted epu8
 * @details
 * @param a, b: two #HPCombi::epu8
 * after executing merge, \c a and \c b are sorted and \c a[15] <= \c b[0]
 * @par Algorithm: bitonic merge sorting network
 */
inline void merge(epu8 &a, epu8 &b) noexcept;

#ifdef SIMDE_X86_SSE4_2_NATIVE
/** Same interface as \ref HPCombi::permutation_of "permutation_of" but with a
   different implementation.
    @par Algorithm: uses string matching cpmestrm intrinsics
 */
inline epu8 permutation_of_cmpestrm(epu8 a, epu8 b) noexcept;
#endif

/** Same interface as \ref HPCombi::permutation_of "permutation_of" but with a
   different implementation.
    @par Algorithm: reference implementation
 */
inline epu8 permutation_of_ref(epu8 a, epu8 b) noexcept;

/**
 * @brief Find if a vector is a permutation of another one
 * @details
 * @param a, b: two #HPCombi::epu8
 * @returns a #HPCombi::epu8
 * For each @f$0 \leq i < 16@f$, \c res[i] is the position in \c a of \c b[i]
     if \c b[i] appears exactly once in \c a, or undefined if not.
 * @par Algorithm: architecture dependent
 */
inline epu8 permutation_of(epu8 a, epu8 b) noexcept;

/** A prime number good for hashing */
constexpr uint64_t prime = 0x9e3779b97f4a7bb9;

/** A random #HPCombi::epu8
 * @details
 * @param bnd : the upper bound for the value of the entries.
 *    \c bnd must verify @f$ 0 < bnd \leq 256 @f$. This is not checked.
 * @returns a random #HPCombi::epu8 with value in the interval
 *    @f$[0, 1, 2, ..., bnd-1]@f$.
 */
inline epu8 random_epu8(uint16_t bnd);

/** Remove duplicates in a sorted #HPCombi::epu8
 * @details
 * @param a: supposed to be sorted
 * @param repl: the value replacing the duplicate entries (default to 0)
 * @return the vector \c a where repeated occurrences of entries are replaced
 *    by \c repl
 */
inline epu8 remove_dups(epu8 a, uint8_t repl = 0) noexcept;

/** Same interface as \ref HPCombi::horiz_sum "horiz_sum" but with a different
 * implementation.
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and indexed access
 */

inline uint8_t horiz_sum_ref(epu8) noexcept;

/** Same interface as \ref HPCombi::horiz_sum "horiz_sum" but with a different
 * implementation.
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and indexed access
 *  through #HPCombi::VectGeneric
 */

inline uint8_t horiz_sum_gen(epu8) noexcept;

/** Same interface as \ref HPCombi::horiz_sum "horiz_sum" but with a different
 * implementation.
 *  @par Algorithm:
 *  4-stages parallel algorithm
 */
inline uint8_t horiz_sum4(epu8) noexcept;

/** Same interface as \ref HPCombi::horiz_sum "horiz_sum" but with a different
 * implementation.
 *  @par Algorithm:
 *  3-stages parallel algorithm + indexed access
 */
inline uint8_t horiz_sum3(epu8) noexcept;

/**
 * @brief Horizontal sum of a #HPCombi::epu8
 * @details
 * @returns the horizontal sum of the input
 * @par Example:
 * @code
 * horiz_sum(epu8 { 5, 5, 2, 5, 1, 6,12, 4, 0, 3, 2,11,12,13,14,15});
 * @endcode
 * Returns `110`
 * @warning The result is supposed to fit in a \c uint8_t
 */
inline uint8_t horiz_sum(epu8 v) noexcept { return horiz_sum3(v); }

/** Same interface as \ref HPCombi::partial_sums "partial_sums" but with a
 * different implementation.
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and indexed access
 */
inline epu8 partial_sums_ref(epu8) noexcept;

/** Same interface as \ref HPCombi::partial_sums "partial_sums" but with a
 * different implementation.
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and indexed access
 *  through #HPCombi::VectGeneric
 */
inline epu8 partial_sums_gen(epu8) noexcept;

/** Same interface as \ref HPCombi::partial_sums "partial_sums" but with a
 * different implementation.
 *  @par Algorithm:
 *  4-stages parallel algorithm
 */
inline epu8 partial_sums_round(epu8) noexcept;

/**
 * @brief Horizontal partial sum of a #HPCombi::epu8
 * @details
 * @returns the partials sums of the input
 * @par Example:
 * @code
 * partial_sums(epu8 { 5, 5, 2, 5, 1, 6,12, 4, 0, 3, 2,11,12,13,14,15});
 * @endcode
 * Returns `{ 5,10,12,17,18,24,36,40,40,43,45,56,68,81,95,110}`
 */
inline epu8 partial_sums(epu8 v) noexcept { return partial_sums_round(v); }

/** Same interface as \ref HPCombi::horiz_max "horiz_max" but with a different
 * implementation.
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and indexed access
 */
inline uint8_t horiz_max_ref(epu8) noexcept;

/** Same interface as \ref HPCombi::horiz_max "horiz_max" but with a different
 * implementation.
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and indexed access
 *  through #HPCombi::VectGeneric
 */
inline uint8_t horiz_max_gen(epu8) noexcept;

/** Same interface as \ref HPCombi::horiz_max "horiz_max" but with a different
 * implementation.
 *  @par Algorithm:
 *  4-stages parallel algorithm
 */
inline uint8_t horiz_max4(epu8) noexcept;

/** Same interface as \ref HPCombi::horiz_max "horiz_max" but with a different
 * implementation.
 *  @par Algorithm:
 *  3-stages parallel algorithm + indexed access
 */
inline uint8_t horiz_max3(epu8) noexcept;

/**
 * @brief Horizontal sum of a  #HPCombi::epu8
 * @details
 * @returns the horizontal sum of the input
 * @par Example:
 * @code
 * horiz_max(epu8 { 5, 5, 2, 5, 1, 6,12, 4, 0, 3, 2, 0,12, 0, 0, 0});
 * @endcode
 * Returns `12`
 */
inline uint8_t horiz_max(epu8 v) noexcept { return horiz_max4(v); }

/** Same interface as \ref HPCombi::partial_max "partial_max" but with a
 * different implementation.
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and indexed access
 */
inline epu8 partial_max_ref(epu8) noexcept;

/** Same interface as \ref HPCombi::partial_max "partial_max" but with a
 * different implementation.
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and indexed access
 *  through #HPCombi::VectGeneric
 */
inline epu8 partial_max_gen(epu8) noexcept;

/** Same interface as \ref HPCombi::partial_max "partial_max" but with a
 * different implementation.
 *  @par Algorithm:
 *  4-stages parallel algorithm
 */
inline epu8 partial_max_round(epu8) noexcept;

/**
 * @brief Horizontal partial sum of a #HPCombi::epu8
 * @details
 * @returns the partials max of the input
 * @par Example:
 * @code
 * partial_max(epu8 { 5, 5, 2, 5, 1, 6,12, 4, 0, 3, 2,11,12,13,14,15});
 * @endcode
 * Returns `{ 5, 5, 5, 5, 5, 6,12,12,12,12,12,12,12,13,14,15}`
 */
inline epu8 partial_max(epu8 v) noexcept { return partial_max_round(v); }

/** Same interface as \ref HPCombi::horiz_min "horiz_min" but with a different
 * implementation.
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and indexed access
 */
inline uint8_t horiz_min_ref(epu8) noexcept;

/** Same interface as \ref HPCombi::horiz_min "horiz_min" but with a different
 * implementation.
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and indexed access
 *  through #HPCombi::VectGeneric
 */
inline uint8_t horiz_min_gen(epu8) noexcept;

/** Same interface as \ref HPCombi::horiz_min "horiz_min" but with a different
 * implementation.
 *  @par Algorithm:
 *  4-stages parallel algorithm
 */
inline uint8_t horiz_min4(epu8) noexcept;

/** Same interface as \ref HPCombi::horiz_min "horiz_min" but with a different
 * implementation.
 *  @par Algorithm:
 *  3-stages parallel algorithm + indexed access
 */
inline uint8_t horiz_min3(epu8) noexcept;

/**
 * @brief Horizontal sum of a  #HPCombi::epu8
 * @details
 * @returns the horizontal sum of the input
 * @par Example:
 * @code
 * horiz_min(epu8 { 5, 5, 2, 5, 1, 6,12, 4, 1, 3, 2, 2,12, 3, 4, 4});
 * @endcode
 * Returns `1`
 */
inline uint8_t horiz_min(epu8 v) noexcept { return horiz_min4(v); }

/** Same interface as \ref HPCombi::partial_min "partial_min" but with a
 * different implementation.
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and indexed access
 */
inline epu8 partial_min_ref(epu8) noexcept;

/** Same interface as \ref HPCombi::partial_min "partial_min" but with a
 * different implementation.
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and indexed access
 *  through #HPCombi::VectGeneric
 */
inline epu8 partial_min_gen(epu8) noexcept;

/** Same interface as \ref HPCombi::partial_min "partial_min" but with a
 * different implementation.
 *  @par Algorithm:
 *  4-stages parallel algorithm
 */
inline epu8 partial_min_round(epu8) noexcept;

/**
 * @brief Horizontal partial sum of a #HPCombi::epu8
 * @details
 * @returns the partials min of the input
 * @par Example:
 * @code
 * partial_min(epu8 { 5, 5, 2, 5, 1, 6,12, 4, 0, 3, 2,11,12,13,14,15});
 * @endcode
 * Returns `{ 5, 5, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0}`
 */
inline epu8 partial_min(epu8 v) noexcept { return partial_min_round(v); }

/** Same interface as \ref HPCombi::eval16 "eval16" but with a different
 * implementation.
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and indexed access
 */
inline epu8 eval16_ref(epu8 v) noexcept;

/** Same interface as \ref HPCombi::eval16 "eval16" but with a different
 * implementation.
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and cast to array
 */
inline epu8 eval16_arr(epu8 v) noexcept;

/** Same interface as \ref HPCombi::eval16 "eval16" but with a different
 * implementation.
 *  @par Algorithm:
 *  Vector @f$O(n)@f$ using cyclic shifting
 */
inline epu8 eval16_cycle(epu8 v) noexcept;

/** Same interface as \ref HPCombi::eval16 "eval16" but with a different
 * implementation.
 *  @par Algorithm:
 *  Vector @f$O(n)@f$ using popcount
 */
inline epu8 eval16_popcount(epu8 v) noexcept;

/**
 * @brief Evaluation of a #HPCombi::epu8: count how many times each int of 0..15
 * appears in the input.
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
inline epu8 eval16(epu8 v) noexcept { return eval16_cycle(v); }

/** Same interface as \ref HPCombi::first_diff "first_diff" but with a different
 * implementation.
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and indexed access
 */
inline uint64_t first_diff_ref(epu8 a, epu8 b, size_t bound = 16) noexcept;

#ifdef SIMDE_X86_SSE4_2_NATIVE
/** Same interface as \ref HPCombi::first_diff "first_diff" but with a different
 * implementation.
 *  @par Algorithm:
 *  Using \c cmpestri instruction
 */
inline uint64_t first_diff_cmpstr(epu8 a, epu8 b, size_t bound = 16) noexcept;
#endif

/** Same interface as \ref HPCombi::first_diff "first_diff" but with a different
 * implementation.
 *  @par Algorithm:
 *  Using vector comparison and mask
 */
inline uint64_t first_diff_mask(epu8 a, epu8 b, size_t bound = 16) noexcept;

/**
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
inline uint64_t first_diff(epu8 a, epu8 b, size_t bound = 16) noexcept {
    return first_diff_mask(a, b, bound);
}

/** Same interface as \ref HPCombi::last_diff "last_diff" but with a different
 * implementation.
 *  @par Algorithm:
 *  Reference @f$O(n)@f$ algorithm using loop and indexed access
 */
inline uint64_t last_diff_ref(epu8 a, epu8 b, size_t bound = 16) noexcept;

#ifdef SIMDE_X86_SSE4_2_NATIVE
/** Same interface as \ref HPCombi::last_diff "last_diff" but with a different
 * implementation.
 *  @par Algorithm:
 *  Using \c cmpestri instruction
 */
inline uint64_t last_diff_cmpstr(epu8 a, epu8 b, size_t bound = 16) noexcept;
#endif

/** Same interface as \ref HPCombi::last_diff "last_diff" but with a different
 * implementation.
 *  @par Algorithm:
 *  Using vector comparison and mask
 */
inline uint64_t last_diff_mask(epu8 a, epu8 b, size_t bound = 16) noexcept;

/**
 * @brief The last difference between two #HPCombi::epu8
 * @details
 * @param a, b : #HPCombi::epu8
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
inline uint64_t last_diff(epu8 a, epu8 b, size_t bound = 16) noexcept {
    return last_diff_mask(a, b, bound);
}

/** Lexicographic comparison between two #HPCombi::epu8 */
inline bool less(epu8 a, epu8 b) noexcept;
/** Partial lexicographic comparison between two #HPCombi::epu8
 * @param a, b : the vectors to compare
 * @param k : the bound for the lexicographic comparison
 * @return a positive, negative or zero int8_t depending on the result
 */
inline int8_t less_partial(epu8 a, epu8 b, int k) noexcept;

/** return the index of the first zero entry or 16 if there are none
 *  Only index smaller than bound are taken into account.
 */
inline uint64_t first_zero(epu8 v, int bnd) noexcept;
/** return the index of the last zero entry or 16 if there are none
 *  Only index smaller than bound are taken into account.
 */
inline uint64_t last_zero(epu8 v, int bnd) noexcept;
/** return the index of the first non zero entry or 16 if there are none
 *  Only index smaller than bound are taken into account.
 */
inline uint64_t first_non_zero(epu8 v, int bnd) noexcept;
/** return the index of the last non zero entry or 16 if there are none
 *  Only index smaller than bound are taken into account.
 */
inline uint64_t last_non_zero(epu8 v, int bnd) noexcept;

/** a vector popcount function
 */
inline epu8 popcount16(epu8 v) noexcept;

/** Test for partial transformation
 * @details
 * @returns whether \c v is a partial transformation.
 * @param v the vector to test
 * @param k the size of \c *this (default 16)
 *
 * Points where the function is undefined are mapped to \c 0xff. If \c *this
 * is a transformation of @f$0\dots n-1@f$ for @f$n<16@f$, it should be
 * completed to a transformation of @f$0\dots 15@f$ by adding fixed points. That
 * is the values @f$i\geq n@f$ should be mapped to themself.
 * @par Example:
 * The partial transformation
 * @f$\begin{matrix}0 1 2 3 4 5\\ 2 0 5 . . 4 \end{matrix}@f$
 * is encoded by the array {2,0,5,0xff,0xff,4,6,7,8,9,10,11,12,13,14,15}
 */
inline bool is_partial_transformation(epu8 v, const size_t k = 16) noexcept;

/** Test for transformation
 * @details
 * @returns whether \c *this is a transformation.
 * @param v the vector to test
 * @param k the size of \c *this (default 16)
 *
 * If \c *this is a transformation of @f$0\dots n-1@f$ for @f$n<16@f$,
 * it should be completed to a transformation of @f$0\dots 15@f$
 * by adding fixed points. That is the values @f$i\geq n@f$ should be
 * mapped to themself.
 * @par Example:
 * The transformation
 * @f$\begin{matrix}0 1 2 3 4 5\\ 2 0 5 2 1 4 \end{matrix}@f$
 * is encoded by the array {2,0,5,2,1,4,6,7,8,9,10,11,12,13,14,15}
 */
inline bool is_transformation(epu8 v, const size_t k = 16) noexcept;

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
inline bool is_partial_permutation(epu8 v, const size_t k = 16) noexcept;

#ifdef SIMDE_X86_SSE4_2_NATIVE
/** Same interface as \ref HPCombi::is_permutation "is_permutation" but with a
   different implementation.
    @par Algorithm: uses string matching cpmestri intrinsics
 */
inline bool is_permutation_cpmestri(epu8 v, const size_t k = 16) noexcept;
#endif
/** Same interface as \ref HPCombi::is_permutation "is_permutation" but with a
   different implementation.
    @par Algorithm: sort the vector and compare to identity
 */
inline bool is_permutation_sort(epu8 v, const size_t k = 16) noexcept;

/** Same interface as \ref HPCombi::is_permutation "is_permutation" but with a
   different implementation.
    @par Algorithm: uses evaluation
 */
inline bool is_permutation_eval(epu8 v, const size_t k = 16) noexcept;

/**
 * @details
 * @returns whether \c *this is a permutation.
 * @param v the vector to test
 * @param k the size of \c *this (default 16)
 *
 * If \c *this is a permutation of @f$0\dots n-1@f$ for @f$n<16@f$,
 * it should be completed to a permutation of @f$0\dots 15@f$
 * by adding fixed points. That is the values @f$i\geq n@f$ should be
 * mapped to themselves.
 * @par Example:
 * The permutation
 * @f$\begin{matrix}0 1 2 3 4 5\\ 2 0 5 3 1 4 \end{matrix}@f$
 * is encoded by the array {2,0,5,3,1,4,6,7,8,9,10,11,12,13,14,15}
 * @par Algorithm: architecture dependent
 */
inline bool is_permutation(epu8 v, const size_t k = 16) noexcept;

}  // namespace HPCombi

namespace std {

inline std::ostream &operator<<(std::ostream &stream, HPCombi::epu8 const &a);

inline std::string to_string(HPCombi::epu8 const &a);

/** We also specialize the struct
 *  - std::equal_to<epu8>
 *  - std::not_equal_to<epu8>
 *  - std::hash<epu8>
 *  - std::less<epu8>
 */
}  // namespace std

#include "epu8_impl.hpp"

#endif  // HPCOMBI_EPU8_HPP_
