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

#ifndef HPCOMBI_PERM16_HPP_INCLUDED
#define HPCOMBI_PERM16_HPP_INCLUDED

#include <array>
#include <cassert>
#include <cstdint>
#include <functional>  // less<>
#include <ostream>
#include <vector>
#include <x86intrin.h>

#include "epu.hpp"
#include "vect16.hpp"

namespace HPCombi {

// Forward declaration
struct Perm16;
struct PTransf16;
struct Transf16;

/** Partial transformation of @f$\{0\dots 15\}@f$
 *
 */
struct alignas(16) PTransf16 : public Vect16 {

    static constexpr size_t size() { return 16; };

    using vect = HPCombi::Vect16;
    using array = TPUBuild<epu8>::array;

    PTransf16() = default;
    HPCOMBI_CONSTEXPR_CONSTRUCTOR PTransf16(const PTransf16 &v) = default;
    HPCOMBI_CONSTEXPR_CONSTRUCTOR PTransf16(const vect v) : Vect16(v) {}
    HPCOMBI_CONSTEXPR_CONSTRUCTOR PTransf16(const epu8 x) : Vect16(x) {}
    PTransf16(std::vector<uint8_t> dom, std::vector<uint8_t> rng,
              size_t = 0 /* unused */);
    PTransf16(std::initializer_list<uint8_t> il);

    PTransf16 &operator=(const PTransf16 &) = default;
    PTransf16 &operator=(const epu8 &vv) { v = vv; return *this; }

    //! Return whether \c *this is a well constructed object
    bool validate(size_t k = 16) const {
        return HPCombi::is_partial_transformation(v, k);
    }

    //! The identity partial transformation.
    static HPCOMBI_CONSTEXPR PTransf16 one() { return epu8id; }
    //! The product of two partial transformations.
    PTransf16 operator*(const PTransf16 &p) const {
        return HPCombi::permuted(v, p.v) | (p.v == Epu8(0xFF));
    }

    /** Returns a mask for the image of \c *this */
    epu8 image_mask(bool complement=false) const;
    /** Returns a bit mask for the image of \c *this */
    uint32_t image_bitset(bool complement=false) const;
    /** Returns a mask for the domain of \c *this */
    epu8 domain_mask(bool complement=false) const;
    /** Returns a bit mask for the domain of \c *this */
    uint32_t domain_bitset(bool complement=false) const;

    /** Returns the partial right identity for \c *this */
    PTransf16 right_one() const;
    /** Returns the partial left identity for \c *this */
    PTransf16 left_one() const;

    /** Returns the size of the image of \c *this */
    uint32_t rank_ref() const;
    /** Returns the size of the image of \c *this */
    uint32_t rank() const;

    /** Returns a mask for the fix point of \c *this */
    epu8 fix_points_mask(bool complement=false) const;
    /** Returns a bit mask for the fix point of \c *this */
    uint32_t fix_points_bitset(bool complement=false) const;
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

/** Full transformation of @f$\{0\dots 15\}@f$
 *
 */
struct Transf16 : public PTransf16 {

    Transf16() = default;
    HPCOMBI_CONSTEXPR_CONSTRUCTOR Transf16(const Transf16 &v) = default;
    HPCOMBI_CONSTEXPR_CONSTRUCTOR Transf16(const vect v) : PTransf16(v) {}
    HPCOMBI_CONSTEXPR_CONSTRUCTOR Transf16(const epu8 x) : PTransf16(x) {}
    Transf16(std::initializer_list<uint8_t> il) : PTransf16(il) {}
    Transf16 &operator=(const Transf16 &) = default;

    //! Return whether \c *this is a well constructed object
    bool validate(size_t k = 16) const {
        return HPCombi::is_transformation(v, k);
    }

    //! The identity transformation.
    static HPCOMBI_CONSTEXPR Transf16 one() { return epu8id; }
    //! The product of two transformations.
    Transf16 operator*(const Transf16 &p) const {
        return HPCombi::permuted(v, p.v);
    }

    //! Construct a transformation from its 64 bits compressed.
    explicit Transf16(uint64_t compressed);
    //! The 64 bit compressed form of a transformation.
    explicit operator uint64_t() const;
};

/** Partial permutationof @f$\{0\dots 15\}@f$
 *
 */
struct PPerm16 : public PTransf16 {

    PPerm16() = default;
    HPCOMBI_CONSTEXPR_CONSTRUCTOR PPerm16(const PPerm16 &v) = default;
    HPCOMBI_CONSTEXPR_CONSTRUCTOR PPerm16(const vect v) : PTransf16(v) {}
    HPCOMBI_CONSTEXPR_CONSTRUCTOR PPerm16(const epu8 x) : PTransf16(x) {}
    PPerm16(std::vector<uint8_t> dom, std::vector<uint8_t> rng,
            size_t = 0 /* unused */) : PTransf16(dom, rng) {}
    PPerm16(std::initializer_list<uint8_t> il) : PTransf16(il) {}
    PPerm16 &operator=(const PPerm16 &) = default;

    //! Return whether \c *this is a well constructed object
    bool validate(size_t k = 16) const {
        return HPCombi::is_partial_permutation(v, k);
    }

    //! The identity partial permutations.
    static HPCOMBI_CONSTEXPR PPerm16 one() { return epu8id; }
    //! The product of two partial perrmutations.
    PPerm16 operator*(const PPerm16 &p) const {
        return this->PTransf16::operator*(p);
    }

    /** @class common_inverse_pperm
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
     * @verbatim {0,0xFF,2,1,3,5,6,0xFF,8,9,0xFF,10,12,0xFF,0xFF,0xFF} @endverbatim
     */
    /** @copydoc common_inverse_pperm
     *  @par Algorithm:
     *  @f$O(n)@f$ algorithm using reference cast to arrays
     */
    PPerm16 inverse_ref() const;
    /** @copydoc common_inverse_pperm
     *  @par Algorithm:
     *  @f$O(\log n)@f$ algorithm using some kind of vectorized dichotomic
     * search.
     */
    PPerm16 inverse_find() const;

    PPerm16 right_one() const { return PTransf16::right_one(); }
    PPerm16 left_one() const { return PTransf16::left_one(); }
};

/** Permutations of @f$\{0\dots 15\}@f$
 *
 */
struct Perm16 : public Transf16 /* public PPerm : diamond problem */ {

    Perm16() = default;
    HPCOMBI_CONSTEXPR_CONSTRUCTOR Perm16(const Perm16 &) = default;
    HPCOMBI_CONSTEXPR_CONSTRUCTOR Perm16(const vect v) : Transf16(v) {}
    HPCOMBI_CONSTEXPR_CONSTRUCTOR Perm16(const epu8 x) : Transf16(x) {}
    Perm16 &operator=(const Perm16 &) = default;
    Perm16(std::initializer_list<uint8_t> il) : Transf16(il) {}

    //! Return whether \c *this is a well constructed object
    bool validate(size_t k = 16) const {
        return HPCombi::is_permutation(v, k);
    }

    // It's not possible to have a static constexpr member of same type as class
    // being defined (see https://stackoverflow.com/questions/11928089/)
    // therefore we chose to have functions.
    //! The identity partial permutation.
    static HPCOMBI_CONSTEXPR Perm16 one() { return epu8id; }
    //! The product of two permutations
    Perm16 operator*(const Perm16 &p) const {
        return HPCombi::permuted(v, p.v);
    }

    //! Construct a permutations from its 64 bits compressed.
    explicit Perm16(uint64_t compressed) : Transf16(compressed) {}

    /** @class common_inverse
     * @brief The inverse permutation
     * @details
     * @returns the inverse of \c *this
     * @par Example:
     * @code
     * Perm16 x = {0,3,2,4,1,5,6,7,8,9,10,11,12,13,14,15};
     * x.inverse()
     * @endcode
     * Returns
     * @verbatim {0,4,2,1,3,5,6,7,8,9,10,11,12,13,14,15} @endverbatim
     */
    /** @copydoc common_inverse
     *  @par Algorithm:
     *  Reference @f$O(n)@f$ algorithm using loop and indexed access
     */
    Perm16 inverse_ref() const;
    /** @copydoc common_inverse
     *  @par Algorithm:
     *  @f$O(n)@f$ algorithm using reference cast to arrays
     */
    Perm16 inverse_arr() const;
    /** @copydoc common_inverse
     *  @par Algorithm:
     *  Insert the identity in the least significant bits and sort using a
     *  sorting network. The number of round of the optimal sorting network is
     *  open as far as I know, therefore the complexity is unknown.
     */
    Perm16 inverse_sort() const;
    /** @copydoc common_inverse
     *  @par Algorithm:
     *  @f$O(\log n)@f$ algorithm using some kind of vectorized dichotomic
     * search.
     */
    Perm16 inverse_find() const { return permutation_of(v, one()); }
    /** @copydoc common_inverse
     *  @par Algorithm:
     *
     * Raise \e *this to power @f$\text{LCM}(1, 2, ..., n) - 1@f$ so complexity
     * is in @f$O(log (\text{LCM}(1, 2, ..., n) - 1)) = O(n)@f$
     */
    Perm16 inverse_pow() const;
    /** @copydoc common_inverse
     *  @par Algorithm:
     *  Compute power from @f$n/2@f$ to @f$n@f$, when @f$\sigma^k(i)=i@f$ then
     *  @f$\sigma^{-1}(i)=\sigma^{k-1}(i)@f$. Complexity @f$O(n)@f$
     */
    Perm16 inverse_cycl() const;
    /** @copydoc common_inverse
     *
     *  Frontend method: currently aliased to #inverse_cycl */
    Perm16 inverse() const { return inverse_cycl(); }

    /** The elementary transposition exchanging @f$i@f$ and @f$i+1@f$ */
    static Perm16 elementary_transposition(uint64_t i);
    /** A random permutation of size @f$n@f$*/
    static Perm16 random(uint64_t n = 16);
    /** The \c r -th permutation of size \c n for the
     *  Steinhaus–Johnson–Trotter order.
     */
    static Perm16 unrankSJT(int n, int r);

    /** @class common_lehmer
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
     */
    /** @copydoc common_lehmer
     *  @par Algorithm:
     *  Reference @f$O(n^2)@f$ algorithm using loop and indexed access
     */
    epu8 lehmer_ref() const;
    /** @copydoc common_lehmer
     *  @par Algorithm:
     *  Reference @f$O(n^2)@f$ algorithm using array, loop and indexed access
     */
    epu8 lehmer_arr() const;
    /** @copydoc common_lehmer
     *  @par Algorithm:
     *  Fast @f$O(n)@f$ algorithm using vector comparison
     */
    epu8 lehmer() const;

    /** @class common_length
     * @brief The Coxeter length (ie: number of inversion) of a permutation
     * @details
     * @returns the number of inversions of \c *this
     * @par Example:
     * @code
     * Perm16 x = {0,3,2,4,1,5,6,7,8,9,10,11,12,13,14,15};
     * x.length()
     * @endcode
     * Returns @verbatim 4 @endverbatim
     */
    /** @copydoc common_length
     *  @par Algorithm:
     *  Reference @f$O(n^2)@f$ algorithm using loop and indexed access
     */
    uint8_t length_ref() const;
    /** @copydoc common_length
     *  @par Algorithm:
     *  Reference @f$O(n^2)@f$ algorithm using loop and indexed access after
     *     a cast to \c std::array
     */
    uint8_t length_arr() const;
    /** @copydoc common_length
     *  @par Algorithm:
     *  @f$O(n)@f$ using vector lehmer and fast horizontal sum
     */
    uint8_t length() const;

    /** @class common_nb_descent
     * @brief The number of descent of a permutation
     * @details
     * @returns the number of inversions of \c *this
     * @par Example:
     * @code
     * Perm16 x {0,3,2,4,1,5,6,7,8,9,10,11,12,13,14,15};
     * x.length()
     * @endcode
     * Returns @verbatim 2 @endverbatim
     */
    /** @copydoc common_nb_descent
     *  @par Algorithm:
     *  Reference @f$O(n)@f$ using a loop
     */
    uint8_t nb_descents_ref() const;
    /** @copydoc common_nb_descent
     *  @par Algorithm:
     *  Reference @f$O(1)@f$ using vector shift and comparison
     */
    uint8_t nb_descents() const;

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

    /** @class common_nb_cycles
     * @brief The number of cycles of a permutation
     * @details
     * @returns the number of cycles of \c *this
     * @par Example:
     * @code
     * Perm16 x {1,2,3,6,0,5,4,7,8,9,10,11,12,15,14,13}
     * x.nb_cycles()
     * @endcode
     * Returns @verbatim 10 @endverbatim
     */
    /** @copydoc common_nb_cycles
     *  @par Algorithm:
     *  Reference @f$O(n)@f$ using a boolean vector
     */
    uint8_t nb_cycles_ref() const;
    /** @copydoc common_nb_cycles
     *  @par Algorithm:
     *  Reference @f$O(\log(n))@f$ using #cycles_partition
     */
    uint8_t nb_cycles_unroll() const;
    /** @copydoc common_nb_cycles
     *  @par Algorithm: aliased to #nb_cycles_unroll
     */
    uint8_t nb_cycles() const { return nb_cycles_unroll(); }

    /** @class common_left_weak_leq
     * @brief Compare two permutations for the left weak order
     * @par Example:
     * @code
     * Perm16 x{2,0,3,1}, y{3,0,2,1};
     * x.left_weak_leq(y)
     * @endcode
     * Returns @verbatim true @endverbatim
     */
    /** @copydoc common_left_weak_leq
     *  @par Algorithm:
     *  Reference @f$O(n^2)@f$ testing inclusion of inversions one by one
     */
    bool left_weak_leq_ref(Perm16 other) const;
    /** @copydoc common_left_weak_leq
     *  @par Algorithm:
     *  Reference @f$O(n)@f$ with vectorized test of inclusion
     */
    bool left_weak_leq_length(Perm16 other) const;
    /** @copydoc common_left_weak_leq
     *  @par Algorithm:
     *  @f$O(n)@f$ algorithm using length
     */
    bool left_weak_leq(Perm16 other) const;

};

/*****************************************************************************/
/** Memory layout concepts check  ********************************************/
/*****************************************************************************/

static_assert(sizeof(epu8) == sizeof(Perm16),
              "epu8 and Perm16 have a different memory layout !");
static_assert(std::is_trivial<epu8>(), "epu8 is not a trivial class !");
static_assert(std::is_trivial<Perm16>(), "Perm16 is not a trivial class !");

}  // namespace HPCombi

#include "perm16_impl.hpp"

namespace std {

template <> struct hash<HPCombi::PTransf16> {
    //! A hash operator for #HPCombi::PTransf16
    size_t operator()(const HPCombi::PTransf16 &ar) const {
        return std::hash<HPCombi::epu8>{}(ar.v);
    }
};

template <> struct hash<HPCombi::Transf16> {
    //! A hash operator for #HPCombi::Transf16
    size_t operator()(const HPCombi::Transf16 &ar) const {
        return uint64_t(ar);
    }
};

template <> struct hash<HPCombi::PPerm16> {
    //! A hash operator for #HPCombi::PPerm16
    size_t operator()(const HPCombi::PPerm16 &ar) const {
        return std::hash<HPCombi::epu8>{}(ar.v);
    }
};

template <> struct hash<HPCombi::Perm16> {
    //! A hash operator for #HPCombi::Perm16
    size_t operator()(const HPCombi::Perm16 &ar) const {
        return uint64_t(ar);
    }
};

}  // namespace std

#endif  // HPCOMBI_PERM16_HPP_INCLUDED
