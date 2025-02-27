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

// NOLINT(build/header_guard)

// This is the implementation part of epu8.hpp this should be seen as
// implementation details and should not be included directly.

#include <initializer_list>
#include <iostream>
#include <random>
#include <sstream>

#include "vect_generic.hpp"

#ifdef SIMDE_X86_SSE4_2_NATIVE
// Comparison mode for _mm_cmpestri
#define FIRST_DIFF                                                             \
    (SIMDE_SIDD_UBYTE_OPS | SIMDE_SIDD_CMP_EQUAL_EACH |                        \
     SIMDE_SIDD_NEGATIVE_POLARITY)
#define LAST_DIFF                                                              \
    (SIMDE_SIDD_UBYTE_OPS | SIMDE_SIDD_CMP_EQUAL_EACH |                        \
     SIMDE_SIDD_NEGATIVE_POLARITY | SIMDE_SIDD_MOST_SIGNIFICANT)
#define FIRST_ZERO (SIMDE_SIDD_UBYTE_OPS | SIMDE_SIDD_CMP_EQUAL_ANY)
#define LAST_ZERO                                                              \
    (SIMDE_SIDD_UBYTE_OPS | SIMDE_SIDD_CMP_EQUAL_ANY |                         \
     SIMDE_SIDD_MOST_SIGNIFICANT)
#define FIRST_NON_ZERO                                                         \
    (SIMDE_SIDD_UBYTE_OPS | SIMDE_SIDD_CMP_EQUAL_ANY |                         \
     SIMDE_SIDD_MASKED_NEGATIVE_POLARITY)
#define LAST_NON_ZERO                                                          \
    (SIMDE_SIDD_UBYTE_OPS | SIMDE_SIDD_CMP_EQUAL_ANY |                         \
     SIMDE_SIDD_MASKED_NEGATIVE_POLARITY | SIMDE_SIDD_MOST_SIGNIFICANT)
#endif

namespace HPCombi {

///////////////////////////////////////////////////////////////////////////////
// Implementation part for inline functions
///////////////////////////////////////////////////////////////////////////////

/** Permuting a #HPCombi::epu8 */
inline epu8 permuted_ref(epu8 a, epu8 b) noexcept {
    epu8 res;
    for (uint64_t i = 0; i < 16; i++)
        res[i] = a[b[i] & 0xF];
    return res;
}

// Msk is supposed to be a boolean mask (i.e. each entry is either 0 or
// 255)
inline uint64_t first_mask(epu8 msk, size_t bound) {
    uint64_t res = simde_mm_movemask_epi8(msk & (Epu8.id() < Epu8(bound)));
    return res == 0 ? 16 : (__builtin_ffsll(res) - 1);
}
inline uint64_t last_mask(epu8 msk, size_t bound) {
    auto res = simde_mm_movemask_epi8(msk & (Epu8.id() < Epu8(bound)));
    return res == 0 ? 16 : (63 - __builtin_clzll(res));
}

inline uint64_t first_diff_ref(epu8 a, epu8 b, size_t bound) noexcept {
    for (size_t i = 0; i < bound; i++)
        if (a[i] != b[i])
            return i;
    return 16;
}
#ifdef SIMDE_X86_SSE4_2_NATIVE
inline uint64_t first_diff_cmpstr(epu8 a, epu8 b, size_t bound) noexcept {
    return unsigned(_mm_cmpestri(a, bound, b, bound, FIRST_DIFF));
}
#endif
inline uint64_t first_diff_mask(epu8 a, epu8 b, size_t bound) noexcept {
    return first_mask(a != b, bound);
}

inline uint64_t last_diff_ref(epu8 a, epu8 b, size_t bound) noexcept {
    while (bound != 0) {
        --bound;
        if (a[bound] != b[bound])
            return bound;
    }
    return 16;
}
#ifdef SIMDE_X86_SSE4_2_NATIVE
inline uint64_t last_diff_cmpstr(epu8 a, epu8 b, size_t bound) noexcept {
    return unsigned(_mm_cmpestri(a, bound, b, bound, LAST_DIFF));
}
#endif
inline uint64_t last_diff_mask(epu8 a, epu8 b, size_t bound) noexcept {
    return last_mask(a != b, bound);
}

inline bool less(epu8 a, epu8 b) noexcept {
    uint64_t diff = first_diff(a, b);
    return (diff < 16) && (a[diff] < b[diff]);
}
inline int8_t less_partial(epu8 a, epu8 b, int k) noexcept {
    uint64_t diff = first_diff(a, b, k);
    return (diff == 16)
               ? 0
               : static_cast<int8_t>(a[diff]) - static_cast<int8_t>(b[diff]);
}

inline uint64_t first_zero(epu8 v, int bnd) noexcept {
    return first_mask(v == epu8{}, bnd);
}
inline uint64_t last_zero(epu8 v, int bnd) noexcept {
    return last_mask(v == epu8{}, bnd);
}
inline uint64_t first_non_zero(epu8 v, int bnd) noexcept {
    return first_mask(v != epu8{}, bnd);
}
inline uint64_t last_non_zero(epu8 v, int bnd) noexcept {
    return last_mask(v != epu8{}, bnd);
}

/// Apply a sorting network
template <bool Increasing = true, size_t sz>
inline epu8 network_sort(epu8 res, std::array<epu8, sz> rounds) {
    for (auto round : rounds) {
        // This conditional should be optimized out by the compiler
        epu8 mask = Increasing ? round < Epu8.id() : Epu8.id() < round;
        epu8 b = permuted(res, round);
        // res = mask ? min(res,b) : max(res,b); is not accepted by clang
        res = simde_mm_blendv_epi8(min(res, b), max(res, b), mask);
    }
    return res;
}

/// Apply a sorting network in place and return the permutation
template <bool Increasing = true, size_t sz>
inline epu8 network_sort_perm(epu8 &v, std::array<epu8, sz> rounds) {
    epu8 res = Epu8.id();
    for (auto round : rounds) {
        // This conditional should be optimized out by the compiler
        epu8 mask = Increasing ? round < Epu8.id() : Epu8.id() < round;
        epu8 b = permuted(v, round);
        epu8 cmp = simde_mm_blendv_epi8(b < v, v < b, mask);
        v = simde_mm_blendv_epi8(v, b, cmp);
        res = simde_mm_blendv_epi8(res, permuted(res, round), cmp);
    }
    return res;
}

/** A 16-way sorting network
 * @details Sorting network from Knuth [AoCP3] Fig. 51 p 229.
 * used by the #sorted function
 *
 * [AoCP3]: "D. Knuth, The art of computer programming vol. 3"
 */
constexpr std::array<epu8, 9> sorting_rounds
    //     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15
    {{epu8{1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14},
      epu8{2, 3, 0, 1, 6, 7, 4, 5, 10, 11, 8, 9, 14, 15, 12, 13},
      epu8{4, 5, 6, 7, 0, 1, 2, 3, 12, 13, 14, 15, 8, 9, 10, 11},
      epu8{8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7},
      epu8{0, 2, 1, 12, 8, 10, 9, 11, 4, 6, 5, 7, 3, 14, 13, 15},
      epu8{0, 4, 8, 10, 1, 9, 12, 13, 2, 5, 3, 14, 6, 7, 11, 15},
      epu8{0, 1, 4, 5, 2, 3, 8, 9, 6, 7, 12, 13, 10, 11, 14, 15},
      epu8{0, 1, 2, 6, 4, 8, 3, 10, 5, 12, 7, 11, 9, 13, 14, 15},
      epu8{0, 1, 2, 4, 3, 6, 5, 8, 7, 10, 9, 12, 11, 13, 14, 15}}};

/** A duplicated 8-way sorting network
 * @details [Batcher odd-Even mergesort] sorting network
 * used by the #sorted function
 *
 * [Batcher odd-Even mergesort]:
 * https://en.wikipedia.org/wiki/Batcher_odd%E2%80%93even_mergesort
 * "Batcher odd–even mergesort"
 */
constexpr std::array<epu8, 6> sorting_rounds8
    // clang-format off
    //     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15
{{
    epu8 { 1,  0,  3,  2,  5,  4,  7,  6,  9,  8, 11, 10, 13, 12, 15, 14},
    epu8 { 2,  3,  0,  1,  6,  7,  4,  5, 10, 11,  8,  9, 14, 15, 12, 13},
    epu8 { 0,  2,  1,  3,  4,  6,  5,  7,  8, 10,  9, 11, 12, 14, 13, 15},
    epu8 { 4,  5,  6,  7,  0,  1,  2,  3, 12, 13, 14, 15,  8,  9, 10, 11},
    epu8 { 0,  1,  4,  5,  2,  3,  6,  7,  8,  9, 12, 13, 10, 11, 14, 15},
    epu8 { 0,  2,  1,  4,  3,  6,  5,  7,  8, 10,  9, 12, 11, 14, 13, 15}
}};
// clang-format on

inline bool is_sorted(epu8 a) noexcept {
    return simde_mm_movemask_epi8(shifted_right(a) > a) == 0;
}
inline epu8 sorted(epu8 a) noexcept {
    return network_sort<true>(a, sorting_rounds);
}
inline epu8 sorted8(epu8 a) noexcept {
    return network_sort<true>(a, sorting_rounds8);
}
inline epu8 revsorted(epu8 a) noexcept {
    return network_sort<false>(a, sorting_rounds);
}
inline epu8 revsorted8(epu8 a) noexcept {
    return network_sort<false>(a, sorting_rounds8);
}

inline epu8 sort_perm(epu8 &a) noexcept {
    return network_sort_perm<true>(a, sorting_rounds);
}
inline epu8 sort8_perm(epu8 &a) noexcept {
    return network_sort_perm<true>(a, sorting_rounds8);
}

constexpr std::array<epu8, 6> merge_rounds
    // clang-format off
    //     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15
{{
    epu8 { 8,  9, 10, 11, 12, 13, 14, 15,  0,  1,  2,  3,  4,  5,  6,  7},
    epu8 { 4,  5,  6,  7,  0,  1,  2,  3, 12, 13, 14, 15,  8,  9, 10, 11},
    epu8 { 2,  3,  0,  1,  6,  7,  4,  5, 10, 11,  8,  9, 14, 15, 12, 13},
    epu8 { 1,  0,  3,  2,  5,  4,  7,  6,  9,  8, 11, 10, 13, 12, 15, 14},
}};
// clang-format on
inline void merge_rev(epu8 &a, epu8 &b) noexcept {
    epu8 mn = min(a, b);
    b = max(a, b);
    a = mn;
    a = network_sort<true>(a, merge_rounds);
    b = network_sort<true>(b, merge_rounds);
}
inline void merge(epu8 &a, epu8 &b) noexcept {
    a = permuted(a, Epu8.rev());
    merge_rev(a, b);
}
// TODO : AVX2 version.
// TODO : compute merge_rounds on the fly instead of loading those from
// memory

inline epu8 random_epu8(uint16_t bnd) {
    epu8 res;

    static std::random_device rd;
    static std::default_random_engine e1(rd());
    std::uniform_int_distribution<int> uniform_dist(0, bnd - 1);

    for (size_t i = 0; i < 16; i++)
        res[i] = uniform_dist(e1);
    return res;
}

inline epu8 remove_dups(epu8 v, uint8_t repl) noexcept {
    // Vector ternary operator is not supported by clang.
    // return (v != shifted_right(v) ? v : Epu8(repl);
    return simde_mm_blendv_epi8(Epu8(repl), v, v != shifted_right(v));
}

// Gather at the front numbers with (3-i)-th bit not set.
constexpr std::array<epu8, 3> inverting_rounds{{
    // clang-format off
    //     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15
    epu8 { 0,  1,  2,  3,  8,  9, 10, 11,  4,  5,  6,  7, 12, 13, 14, 15},
    epu8 { 0,  1,  4,  5,  8,  9, 12, 13,  2,  3,  6,  7, 10, 11, 14, 15},
    epu8 { 0,  2,  4,  6,  8, 10, 12, 14,  1,  3,  5,  7,  9, 11, 13, 15}
    // clang-format on
}};

#ifdef SIMDE_X86_SSE4_2_NATIVE
#define FIND_IN_VECT                                                           \
    (SIMDE_SIDD_UBYTE_OPS | SIMDE_SIDD_CMP_EQUAL_ANY | SIMDE_SIDD_UNIT_MASK |  \
     SIMDE_SIDD_NEGATIVE_POLARITY)
#define FIND_IN_VECT_COMPL                                                     \
    (SIMDE_SIDD_UBYTE_OPS | SIMDE_SIDD_CMP_EQUAL_ANY | SIMDE_SIDD_UNIT_MASK)

inline epu8 permutation_of_cmpestrm(epu8 a, epu8 b) noexcept {
    epu8 res = -static_cast<epu8>(_mm_cmpestrm(a, 8, b, 16, FIND_IN_VECT));
    for (epu8 round : inverting_rounds) {
        a = permuted(a, round);
        res <<= 1;
        res -= static_cast<epu8>(_mm_cmpestrm(a, 8, b, 16, FIND_IN_VECT));
    }
    return res;
}
#endif

inline epu8 permutation_of_ref(epu8 a, epu8 b) noexcept {
    auto ar = as_array(a);
    epu8 res{};
    for (size_t i = 0; i < 16; i++) {
        res[i] =
            std::distance(ar.begin(), std::find(ar.begin(), ar.end(), b[i]));
    }
    return res;
}
inline epu8 permutation_of(epu8 a, epu8 b) noexcept {
#ifdef SIMDE_X86_SSE4_2_NATIVE
    return permutation_of_cmpestrm(a, b);
#else
    return permutation_of_ref(a, b);
#endif
}

#if defined(FF)
#error FF is defined !
#endif /* FF */
#define FF 0xff

/// Permutation Round for partial and horizontal sums
constexpr std::array<epu8, 4> summing_rounds{{
    // clang-format off
    //      0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15
    epu8 { FF,  0, FF,  2, FF,  4, FF,  6, FF,  8, FF, 10, FF, 12, FF, 14},
    epu8 { FF, FF,  1,  1, FF, FF,  5,  5, FF, FF,  9,  9, FF, FF, 13, 13},
    epu8 { FF, FF, FF, FF,  3,  3,  3,  3, FF, FF, FF, FF, 11, 11, 11, 11},
    epu8 { FF, FF, FF, FF, FF, FF, FF, FF,  7,  7,  7,  7,  7,  7,  7,  7}
    // clang-format on
}};

constexpr std::array<epu8, 4> mining_rounds{{
    // clang-format off
    //      0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15
    epu8 {  0,  0,  2,  2,  4,  4,  6,  6,  8,  8, 10, 10, 12, 12, 14, 14},
    epu8 {  0,  1,  1,  1,  4,  5,  5,  5,  8,  9,  9,  9, 12, 13, 13, 13},
    epu8 {  0,  1,  2,  3,  3,  3,  3,  3,  8,  9, 10, 11, 11, 11, 11, 11},
    epu8 {  0,  1,  2,  3,  4,  5,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7}
    // clang-format on
}};

#undef FF

inline uint8_t horiz_sum_ref(epu8 v) noexcept {
    uint8_t res = 0;
    for (size_t i = 0; i < 16; i++)
        res += v[i];
    return res;
}
inline uint8_t horiz_sum_gen(epu8 v) noexcept {
    return as_VectGeneric(v).horiz_sum();
}
inline uint8_t horiz_sum4(epu8 v) noexcept { return partial_sums_round(v)[15]; }
inline uint8_t horiz_sum3(epu8 v) noexcept {
    auto sr = summing_rounds;
    v += permuted(v, sr[0]);
    v += permuted(v, sr[1]);
    v += permuted(v, sr[2]);
    return v[7] + v[15];
}

inline epu8 partial_sums_ref(epu8 v) noexcept {
    epu8 res{};
    res[0] = v[0];
    for (size_t i = 1; i < 16; i++)
        res[i] = res[i - 1] + v[i];
    return res;
}
inline epu8 partial_sums_gen(epu8 v) noexcept {
    as_VectGeneric(v).partial_sums_inplace();
    return v;
}
inline epu8 partial_sums_round(epu8 v) noexcept {
    for (epu8 round : summing_rounds)
        v += permuted(v, round);
    return v;
}

inline uint8_t horiz_max_ref(epu8 v) noexcept {
    uint8_t res = 0;
    for (size_t i = 0; i < 16; i++)
        res = std::max(res, v[i]);
    return res;
}
inline uint8_t horiz_max_gen(epu8 v) noexcept {
    return as_VectGeneric(v).horiz_max();
}
inline uint8_t horiz_max4(epu8 v) noexcept { return partial_max_round(v)[15]; }
inline uint8_t horiz_max3(epu8 v) noexcept {
    auto sr = summing_rounds;
    v = max(v, permuted(v, sr[0]));
    v = max(v, permuted(v, sr[1]));
    v = max(v, permuted(v, sr[2]));
    return std::max(v[7], v[15]);
}

inline epu8 partial_max_ref(epu8 v) noexcept {
    epu8 res;
    res[0] = v[0];
    for (size_t i = 1; i < 16; i++)
        res[i] = std::max(res[i - 1], v[i]);
    return res;
}
inline epu8 partial_max_gen(epu8 v) noexcept {
    as_VectGeneric(v).partial_max_inplace();
    return v;
}
inline epu8 partial_max_round(epu8 v) noexcept {
    for (epu8 round : summing_rounds)
        v = max(v, permuted(v, round));
    return v;
}

inline uint8_t horiz_min_ref(epu8 v) noexcept {
    uint8_t res = 255;
    for (size_t i = 0; i < 16; i++)
        res = std::min(res, v[i]);
    return res;
}
inline uint8_t horiz_min_gen(epu8 v) noexcept {
    return as_VectGeneric(v).horiz_min();
}
inline uint8_t horiz_min4(epu8 v) noexcept { return partial_min_round(v)[15]; }
inline uint8_t horiz_min3(epu8 v) noexcept {
    auto sr = mining_rounds;
    v = min(v, permuted(v, sr[0]));
    v = min(v, permuted(v, sr[1]));
    v = min(v, permuted(v, sr[2]));
    return std::min(v[7], v[15]);
}

inline epu8 partial_min_ref(epu8 v) noexcept {
    epu8 res;
    res[0] = v[0];
    for (size_t i = 1; i < 16; i++)
        res[i] = std::min(res[i - 1], v[i]);
    return res;
}
inline epu8 partial_min_gen(epu8 v) noexcept {
    as_VectGeneric(v).partial_min_inplace();
    return v;
}
inline epu8 partial_min_round(epu8 v) noexcept {
    for (epu8 round : mining_rounds)
        v = min(v, permuted(v, round));
    return v;
}

inline epu8 eval16_ref(epu8 v) noexcept {
    epu8 res{};
    for (size_t i = 0; i < 16; i++)
        if (v[i] < 16)
            res[v[i]]++;
    return res;
}

inline epu8 eval16_arr(epu8 v8) noexcept {
    decltype(Epu8)::array res{};
    auto v = as_array(v8);
    for (size_t i = 0; i < 16; i++)
        if (v[i] < 16)
            res[v[i]]++;
    return Epu8(res);
}
inline epu8 eval16_gen(epu8 v) noexcept {
    return Epu8(as_VectGeneric(v).eval().v);
}
inline epu8 eval16_cycle(epu8 v) noexcept {
    epu8 res = -(Epu8.id() == v);
    for (int i = 1; i < 16; i++) {
        v = permuted(v, Epu8.left_cycle());
        res -= (Epu8.id() == v);
    }
    return res;
}
inline epu8 eval16_popcount(epu8 v) noexcept {
    epu8 res{};
    for (size_t i = 0; i < 16; i++) {
        res[i] =
            __builtin_popcountl(simde_mm_movemask_epi8(v == Epu8(uint8_t(i))));
    }
    return res;
}

inline epu8 popcount16(epu8 v) noexcept {
    return (permuted(Epu8.popcount(), v & Epu8(0x0f)) +
            permuted(Epu8.popcount(), v >> 4));
}

inline bool is_partial_transformation(epu8 v, const size_t k) noexcept {
    uint64_t diff = last_diff(v, Epu8.id(), 16);
    // (forall x in v, x + 1 <= 16)  and
    // (v = Perm16::one()   or  last diff index < 16)
    return (simde_mm_movemask_epi8(v + Epu8(1) <= Epu8(0x10)) == 0xffff) &&
           (diff == 16 || diff < k);
}

inline bool is_transformation(epu8 v, const size_t k) noexcept {
    uint64_t diff = last_diff(v, Epu8.id(), 16);
    return (simde_mm_movemask_epi8(v < Epu8(0x10)) == 0xffff) &&
           (diff == 16 || diff < k);
}

inline bool is_partial_permutation(epu8 v, const size_t k) noexcept {
    uint64_t diff = last_diff(v, Epu8.id(), 16);
    // (forall x in v, x <= 15)  and
    // (forall x < 15, multiplicity x v <= 1
    // (v = Perm16::one()   or  last diff index < 16)
    return (simde_mm_movemask_epi8(v + Epu8(1) <= Epu8(0x10)) == 0xffff) &&
           (simde_mm_movemask_epi8(eval16(v) <= Epu8(1)) == 0xffff) &&
           (diff == 16 || diff < k);
}

#ifdef SIMDE_X86_SSE4_2_NATIVE
inline bool is_permutation_cmpestri(epu8 v, const size_t k) noexcept {
    uint64_t diff = last_diff(v, Epu8.id(), 16);
    // (forall x in v, x in Perm16::one())  and
    // (forall x in Perm16::one(), x in v)  and
    // (v = Perm16::one()   or  last diff index < 16)
    return _mm_cmpestri(Epu8.id(), 16, v, 16, FIRST_NON_ZERO) == 16 &&
           _mm_cmpestri(v, 16, Epu8.id(), 16, FIRST_NON_ZERO) == 16 &&
           (diff == 16 || diff < k);
}
#endif

inline bool is_permutation_sort(epu8 v, const size_t k) noexcept {
    uint64_t diff = last_diff(v, Epu8.id(), 16);
    return equal(sorted(v), Epu8.id()) && (diff == 16 || diff < k);
}
inline bool is_permutation_eval(epu8 v, const size_t k) noexcept {
    uint64_t diff = last_diff(v, Epu8.id(), 16);
    return equal(eval16(v), Epu8({}, 1)) && (diff == 16 || diff < k);
}

inline bool is_permutation(epu8 v, const size_t k) noexcept {
#ifdef SIMDE_X86_SSE4_2_NATIVE
    return is_permutation_cmpestri(v, k);
#else
    return is_permutation_sort(v, k);
#endif
}

}  // namespace HPCombi

namespace std {

inline std::ostream &operator<<(std::ostream &stream, HPCombi::epu8 const &a) {
    stream << "{" << std::setw(2) << unsigned(a[0]);
    for (unsigned i = 1; i < 16; ++i)
        stream << "," << std::setw(2) << unsigned(a[i]);
    stream << "}";
    return stream;
}

inline std::string to_string(HPCombi::epu8 const &a) {
    std::ostringstream ss;
    ss << a;
    return ss.str();
}

template <> struct equal_to<HPCombi::epu8> {
    bool operator()(const HPCombi::epu8 &lhs,
                    const HPCombi::epu8 &rhs) const noexcept {
        return HPCombi::equal(lhs, rhs);
    }
};

template <> struct not_equal_to<HPCombi::epu8> {
    bool operator()(const HPCombi::epu8 &lhs,
                    const HPCombi::epu8 &rhs) const noexcept {
        return HPCombi::not_equal(lhs, rhs);
    }
};

template <> struct hash<HPCombi::epu8> {
    inline size_t operator()(HPCombi::epu8 a) const noexcept {
        unsigned __int128 v0 = simde_mm_extract_epi64(a, 0);
        unsigned __int128 v1 = simde_mm_extract_epi64(a, 1);
        return ((v1 * HPCombi::prime + v0) * HPCombi::prime) >> 64;

        /* The following is extremely slow on Renner benchmark
           uint64_t v0 = simde_mm_extract_epi64(ar.v, 0);
           uint64_t v1 = simde_mm_extract_epi64(ar.v, 1);
           size_t seed = v0 + 0x9e3779b9;
           seed ^= v1 + 0x9e3779b9 + (seed<<6) + (seed>>2);
           return seed;
        */
    }
};

template <> struct less<HPCombi::epu8> {
    // WARNING: due to endianness this is not lexicographic comparison,
    //          but we don't care when using in std::set.
    // 10% faster than calling the lexicographic comparison operator !
    inline size_t operator()(const HPCombi::epu8 &v1,
                             const HPCombi::epu8 &v2) const noexcept {
        simde__m128 v1v = simde__m128(v1), v2v = simde__m128(v2);
        return v1v[0] == v2v[0] ? v1v[1] < v2v[1] : v1v[0] < v2v[0];
    }
};

}  // namespace std
