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
@brief declaration of
\ref HPCombi::PTransf16 "PTransf16",
\ref HPCombi::Transf16  "Transf16",
\ref HPCombi::PPerm16   "PPerm16" and
\ref HPCombi::Perm16    "Perm16"
*/

#ifndef HPCOMBI_PERM16_HPP_
#define HPCOMBI_PERM16_HPP_

#include <cstddef>           // for size_t
#include <cstdint>           // for uint8_t, uint64_t, uint32_t
#include <initializer_list>  // for initializer_list
#include <memory>            // for hash
#include <type_traits>       // for is_trivial
#include <vector>            // for vector

#include "epu8.hpp"    // for epu8, permuted, etc
#include "power.hpp"   // for pow
#include "vect16.hpp"  // for hash, is_partial_permutation

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wpacked"
#endif
#include "simde/x86/sse4.1.h"
#include "simde/x86/sse4.2.h"
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

namespace HPCombi {

// Forward declaration
struct Perm16;
struct PTransf16;
struct Transf16;

/** Partial transformation of @f$\{0\dots 15\}@f$; see HPCombi::Transf16;
partial means it might not be defined everywhere.
Undefined images are encoded as 0xFF. */
struct alignas(16) PTransf16 : public Vect16 {
    static constexpr size_t size() { return 16; }

    using vect = HPCombi::Vect16;
    using array = typename decltype(Epu8)::array;

    PTransf16() = default;

    constexpr PTransf16(const vect vv) : Vect16(vv) {}
    constexpr PTransf16(const epu8 x) : Vect16(x) {}
    PTransf16(std::vector<uint8_t> dom, std::vector<uint8_t> rng,
              size_t = 0 /* unused */);
    PTransf16(std::initializer_list<uint8_t> il);

    //! Return whether \c *this is a well constructed object
    bool validate(size_t k = 16) const {
        return HPCombi::is_partial_transformation(v, k);
    }

    //! The identity partial transformation.
    static constexpr PTransf16 one() { return Epu8.id(); }
    //! The product of two partial transformations.
    PTransf16 operator*(const PTransf16 &p) const {
        return HPCombi::permuted(v, p.v) | (p.v == Epu8(0xFF));
    }

    /** Returns a mask for the image of \c *this */
    epu8 image_mask_cmpestrm(bool complement = false) const;
    /** Returns a mask for the image of \c *this */
    epu8 image_mask_ref(bool complement = false) const;
    epu8 image_mask(bool complement = false) const {
#ifdef SIMDE_X86_SSE4_2_NATIVE
        return image_mask_cmpestrm(complement);
#else
        return image_mask_ref(complement);
#endif
    }
    /** Returns a bit mask for the image of \c *this */
    uint32_t image_bitset(bool complement = false) const;
    /** Returns a mask for the domain of \c *this */
    epu8 domain_mask(bool complement = false) const;
    /** Returns a bit mask for the domain of \c *this */
    uint32_t domain_bitset(bool complement = false) const;

    /** Returns the partial right identity for \c *this */
    PTransf16 right_one() const;
    /** Returns the partial left identity for \c *this */
    PTransf16 left_one() const;

    /** Returns the size of the image of \c *this */
    uint32_t rank_ref() const;
    /** Returns the size of the image of \c *this */
    uint32_t rank() const;
    //! Returns the size of the image of \c *this
    uint32_t rank_cmpestrm() const;

    /** Returns a mask for the fix point of \c *this */
    epu8 fix_points_mask(bool complement = false) const;
    /** Returns a bit mask for the fix point of \c *this */
    uint32_t fix_points_bitset(bool complement = false) const;
    /** Returns the smallest fix point of \c *this */
    uint8_t smallest_fix_point() const;
    /** Returns the smallest non fix point of \c *this */
    uint8_t smallest_moved_point() const;
    /** Returns the largest fix point of \c *this */
    uint8_t largest_fix_point() const;
    /** Returns the largest non fix point of \c *this */
    uint8_t largest_moved_point() const;
    /** Returns the number of fix points of \c *this */
    uint8_t nb_fix_points() const;
};

/** Full transformation of @f$\{0\dots 15\}@f$:
a transformation is a mapping of a set of n elements *into* itself;
ie as opposed to a permutation, it is not necessarily injective.
Here n is hard-coded to 16. */
struct Transf16 : public PTransf16 {
    Transf16() = default;
    constexpr Transf16(const Transf16 &vv) = default;
    /* implicit */ constexpr Transf16(const vect vv)  // NOLINT
        : PTransf16(vv) {}
    /* implicit */ constexpr Transf16(const epu8 x) : PTransf16(x) {}  // NOLINT
    Transf16(std::initializer_list<uint8_t> il) : PTransf16(il) {}
    Transf16 &operator=(const Transf16 &) = default;

    //! Return whether \c *this is a well constructed object
    bool validate(size_t k = 16) const {
        return HPCombi::is_transformation(v, k);
    }

    //! The identity transformation.
    static constexpr Transf16 one() { return Epu8.id(); }
    //! The product of two transformations.
    Transf16 operator*(const Transf16 &p) const {
        return HPCombi::permuted(v, p.v);
    }

    //! Construct a transformation from its 64 bits compressed.
    explicit Transf16(uint64_t compressed);
    //! The 64 bit compressed form of a transformation.
    explicit operator uint64_t() const;
};

/** Partial permutation of @f$\{0\dots 15\}@f$; see also HPCombi::Perm16;
partial means it might not be defined everywhere (but where it's defined, it's
injective). Undefined images are encoded as 0xFF. */
struct PPerm16 : public PTransf16 {
    PPerm16() = default;
    constexpr PPerm16(const PPerm16 &vv) = default;
    /* implicit */ constexpr PPerm16(const vect vv)  // NOLINT
        : PTransf16(vv) {}
    /* implicit */ constexpr PPerm16(const epu8 x) : PTransf16(x) {}  // NOLINT
    PPerm16(std::vector<uint8_t> dom, std::vector<uint8_t> rng,
            size_t = 0 /* unused */)
        : PTransf16(dom, rng) {}
    PPerm16(std::initializer_list<uint8_t> il) : PTransf16(il) {}
    PPerm16 &operator=(const PPerm16 &) = default;

    //! Return whether \c *this is a well constructed object
    bool validate(size_t k = 16) const {
        return HPCombi::is_partial_permutation(v, k);
    }

    //! The identity partial permutations.
    static constexpr PPerm16 one() { return Epu8.id(); }
    //! The product of two partial perrmutations.
    PPerm16 operator*(const PPerm16 &p) const {
        return this->PTransf16::operator*(p);
    }

    /**
     * @brief The inverse of a partial permutation
     * @details
     * @returns the inverse of \c *this. The inverse of @f$p@f$ is the unique
     * partial permutation @f$i@f$ such that @f$ p * i * p = p@f$ and
     * @f$ i * p * i = i@f$
     * @par Example:
     * @code
     * Perm16 x = {0,3,2,4,0xFF,5,6,0xFF,8,9,11,0xFF,12,0xFF,0xFF,0xFF};
     * x.inverse()
     * @endcode
     * Returns
     * @verbatim {0,0xFF,2,1,3,5,6,0xFF,8,9,0xFF,10,12,0xFF,0xFF,0xFF}
     * @endverbatim
     * @par Algorithm:
     * @f$O(n)@f$ algorithm using reference cast to arrays
     */
    PPerm16 inverse_ref() const;

#ifdef SIMDE_X86_SSE4_2_NATIVE
    /** Same as \ref HPCombi::PPerm16::inverse_ref "inverse_ref" but with a
     * different algorithm.
     *  @par Algorithm:
     *  @f$O(\log n)@f$ algorithm using some kind of vectorized dichotomic
     * search.
     */
    PPerm16 inverse_find() const;
#endif

    PPerm16 right_one() const { return PTransf16::right_one(); }
    PPerm16 left_one() const { return PTransf16::left_one(); }
};

/** Permutations of @f$\{0\dots 15\}@f$:
 * A permutation is a bijective mapping of a set of n elements onto itself.
 * Here n is hard-coded to 16.
 */
struct Perm16 : public Transf16 /* public PPerm : diamond problem */ {
    Perm16() = default;
    constexpr Perm16(const Perm16 &) = default;
    /* implicit */ constexpr Perm16(const vect vv) : Transf16(vv) {}  // NOLINT
    /* implicit */ constexpr Perm16(const epu8 x) : Transf16(x) {}    // NOLINT
    Perm16 &operator=(const Perm16 &) = default;
    Perm16(std::initializer_list<uint8_t> il) : Transf16(il) {}

    //! Return whether \c *this is a well constructed object
    bool validate(size_t k = 16) const { return HPCombi::is_permutation(v, k); }

    // It's not possible to have a static constexpr member of same type as class
    // being defined (see https://stackoverflow.com/questions/11928089/)
    // therefore we chose to have functions.
    //! The identity partial permutation.
    static constexpr Perm16 one() { return Epu8.id(); }
    //! The product of two permutations
    Perm16 operator*(const Perm16 &p) const {
        return HPCombi::permuted(v, p.v);
    }

    //! Construct a permutations from its 64 bits compressed.
    explicit Perm16(uint64_t compressed) : Transf16(compressed) {}

    /** @brief The inverse permutation
     *
     * @details
     * @returns the inverse of \c *this
     * @par Example:
     * @code
     * Perm16 x = {0,3,2,4,1,5,6,7,8,9,10,11,12,13,14,15};
     * x.inverse()
     * @endcode
     * Returns
     * @verbatim {0,4,2,1,3,5,6,7,8,9,10,11,12,13,14,15} @endverbatim

     * Frontend method: currently aliased to #inverse_cycl */
    Perm16 inverse() const { return inverse_cycl(); }

    /** Same as \ref HPCombi::Perm16::inverse "inverse" but with a different
     * algorithm.
     *  @par Algorithm:
     *  Reference @f$O(n)@f$ algorithm using loop and indexed access
     */
    Perm16 inverse_ref() const;

    /** Same as \ref HPCombi::Perm16::inverse "inverse" but with a different
     * algorithm.
     *  @par Algorithm:
     *  @f$O(n)@f$ algorithm using reference cast to arrays
     */
    Perm16 inverse_arr() const;

    /** Same as \ref HPCombi::Perm16::inverse "inverse" but with a different
     * algorithm.
     *  @par Algorithm:
     *  Insert the identity in the least significant bits and sort using a
     *  sorting network. The number of rounds of the optimal sorting network is
     *  open as far as I know, therefore the complexity is unknown.
     */
    Perm16 inverse_sort() const;

    /** Same as \ref HPCombi::Perm16::inverse "inverse" but with a different
     * algorithm.
     *  @par Algorithm:
     *  @f$O(\log n)@f$ algorithm using some kind of vectorized dichotomic
     * search.
     */
    Perm16 inverse_find() const { return permutation_of(v, one()); }

    /** Same as \ref HPCombi::Perm16::inverse "inverse" but with a different
     * algorithm.
     *  @par Algorithm:
     *
     * Use HPCombi::pow to
     * raise \e *this to power @f$\text{LCM}(1, 2, ..., n) - 1@f$ so complexity
     * is @f$O(log (\text{LCM}(1, 2, ..., n) - 1)) = O(n)@f$
     */
    Perm16 inverse_pow() const;

    /** Same as \ref HPCombi::Perm16::inverse "inverse" but with a different
     * algorithm.
     *  @par Algorithm:
     *  Compute power from @f$n/2@f$ to @f$n@f$, when @f$\sigma^k(i)=i@f$ then
     *  @f$\sigma^{-1}(i)=\sigma^{k-1}(i)@f$. Complexity @f$O(n)@f$
     */
    Perm16 inverse_cycl() const;

    /** The elementary transposition exchanging @f$i@f$ and @f$i+1@f$ */
    static Perm16 elementary_transposition(uint64_t i);
    /** A random permutation of size @f$n@f$*/
    static Perm16 random(uint64_t n = 16);
    /** The \c r -th permutation of size \c n for the
     *  Steinhaus–Johnson–Trotter order.
     */
    static Perm16 unrankSJT(int n, int r);

    /**
     * @brief The Lehmer code of a permutation
     * @details
     * @returns the Lehmer code of \c *this
     * @par Example:
     * @code
     * Perm16 x = {0,3,2,4,1,5,6,7,8,9,10,11,12,13,14,15};
     * x.lehmer()
     * @endcode
     * Returns
     * @verbatim {0,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0} @endverbatim
     * @par Algorithm:
     * Fast @f$O(n)@f$ algorithm using vector comparison
     */
    epu8 lehmer() const;

    /** Same interface as \ref HPCombi::Perm16::lehmer "lehmer" but with a
     * different implementation.
     * @par Algorithm:
     * Reference @f$O(n^2)@f$ algorithm using loop and indexed access
     */
    epu8 lehmer_ref() const;

    /** Same interface as \ref HPCombi::Perm16::lehmer "lehmer" but with a
     * different implementation.
     * @par Algorithm:
     * Reference @f$O(n^2)@f$ algorithm using array, loop and indexed access
     */
    epu8 lehmer_arr() const;

    /**
     * @brief The Coxeter length (ie: number of inversion) of a permutation
     * @details
     * @returns the number of inversions of \c *this
     * @par Example:
     * @code
     * Perm16 x = {0,3,2,4,1,5,6,7,8,9,10,11,12,13,14,15};
     * x.length()
     * @endcode
     * Returns @verbatim 4 @endverbatim
     *  @par Algorithm:
     *  @f$O(n)@f$ using vector lehmer and fast horizontal sum
     */
    uint8_t length() const;

    /** Same interface as \ref HPCombi::Perm16::length "length", with a
     * different implementation.
     *  @par Algorithm:
     *  Reference @f$O(n^2)@f$ algorithm using loop and indexed access
     */
    uint8_t length_ref() const;

    /** Same interface as \ref HPCombi::Perm16::length "length", with a
     * different implementation.
     *  @par Algorithm:
     *  Reference @f$O(n^2)@f$ algorithm using loop and indexed access after
     *     a cast to \c std::array
     */
    uint8_t length_arr() const;

    /**
     * @brief The number of descent of a permutation
     * @details
     * @returns the number of inversions of \c *this
     * @par Example:
     * @code
     * Perm16 x {0,3,2,4,1,5,6,7,8,9,10,11,12,13,14,15};
     * x.length()
     * @endcode
     * Returns @verbatim 2 @endverbatim
     *  @par Algorithm:
     *  Reference @f$O(1)@f$ using vector shift and comparison
     */
    uint8_t nb_descents() const;

    /** Same interface as \ref HPCombi::Perm16::nb_descents "nb_descents", with
     * a different implementation.
     *  @par Algorithm:
     *  Reference @f$O(n)@f$ using a loop
     */
    uint8_t nb_descents_ref() const;

    /** The set partition of the cycles of a permutation
     * @details
     * @returns the a vector @f$v@f$ where @$fv[i]@$f contains the smallest
     *     element in the cycle of $i$ in \c *this
     * @par Example:
     * @code
     * Perm16 x {1,2,3,6,0,5,4,7,8,9,10,11,12,15,14,13}
     * x.cycles_partition()
     * @endcode
     * Returns
     @verbatim
     [ 0, 0, 0, 0, 0, 5, 0, 7, 8, 9,10,11,12,13,14,13]
     @endverbatim
     */
    epu8 cycles_partition() const;

    /**
     * @brief The number of cycles of a permutation
     * @details
     * @returns the number of cycles of \c *this
     * @par Example:
     * @code
     * Perm16 x {1,2,3,6,0,5,4,7,8,9,10,11,12,15,14,13}
     * x.nb_cycles()
     * @endcode
     * Returns @verbatim 10 @endverbatim
     *  @par Algorithm: aliased to #nb_cycles_unroll
     */
    uint8_t nb_cycles() const { return nb_cycles_unroll(); }

    /** Same interface as \ref HPCombi::Perm16::nb_cycles "nb_cycles" but with a
     * different implementation.
     *  @par Algorithm:
     *  Reference @f$O(n)@f$ using a boolean vector
     */
    uint8_t nb_cycles_ref() const;

    /** Same interface as \ref HPCombi::Perm16::nb_cycles "nb_cycles" but with a
     * different implementation.
     *  @par Algorithm:
     *  Reference @f$O(\log(n))@f$ using #cycles_partition
     */
    uint8_t nb_cycles_unroll() const;

    /**
     * @brief Compare two permutations for the left weak order
     * @par Example:
     * @code
     * Perm16 x{2,0,3,1}, y{3,0,2,1};
     * x.left_weak_leq(y)
     * @endcode
     * Returns @verbatim true @endverbatim
     *  @par Algorithm:
     *  @f$O(n)@f$ algorithm using length
     */
    bool left_weak_leq(Perm16 other) const;

    /** Same interface as \ref HPCombi::Perm16::left_weak_leq "left_weak_leq"
     * but with a different implementation.
     *  @par Algorithm:
     *  Reference @f$O(n^2)@f$ testing inclusion of inversions one by one
     */
    bool left_weak_leq_ref(Perm16 other) const;

    /** Same interface as \ref HPCombi::Perm16::left_weak_leq "left_weak_leq"
     * but with a different implementation.
     *  @par Algorithm:
     *  Reference @f$O(n)@f$ with vectorized test of inclusion
     */
    bool left_weak_leq_length(Perm16 other) const;
};

///////////////////////////////////////////////////////////////////////////////
/// Memory layout concepts check  /////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static_assert(sizeof(epu8) == sizeof(Perm16),
              "epu8 and Perm16 have a different memory layout !");
static_assert(std::is_trivial<epu8>(), "epu8 is not a trivial class !");
static_assert(std::is_trivial<Perm16>(), "Perm16 is not a trivial class !");

}  // namespace HPCombi

#include "perm16_impl.hpp"

namespace std {
// Hash operators for Transf and Perm:

//! This type appears in the doc because we provide a hash function for
//! HPCombi::PTransf16.
template <> struct hash<HPCombi::PTransf16> {
    //! A hash operator for #HPCombi::PTransf16
    size_t operator()(const HPCombi::PTransf16 &ar) const {
        return std::hash<HPCombi::epu8>{}(ar.v);
    }
};

//! This type appears in the doc because we provide a hash function for
//! HPCombi::Transf16.
template <> struct hash<HPCombi::Transf16> {
    //! A hash operator for #HPCombi::Transf16
    size_t operator()(const HPCombi::Transf16 &ar) const {
        return static_cast<uint64_t>(ar);
    }
};

//! This type appears in the doc because we provide a hash function for
//! HPCombi::PPerm16.
template <> struct hash<HPCombi::PPerm16> {
    //! A hash operator for #HPCombi::PPerm16
    size_t operator()(const HPCombi::PPerm16 &ar) const {
        return std::hash<HPCombi::epu8>{}(ar.v);
    }
};

//! This type appears in the doc because we provide a hash function for
//! HPCombi::Perm16.
template <> struct hash<HPCombi::Perm16> {
    //! A hash operator for #HPCombi::Perm16
    size_t operator()(const HPCombi::Perm16 &ar) const {
        return static_cast<uint64_t>(ar);
    }
};

}  // namespace std

#endif  // HPCOMBI_PERM16_HPP_
