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

#include "power.hpp"
#include <algorithm>
#include <iomanip>
#include <random>

#ifdef HAVE_EXPERIMENTAL_NUMERIC_LCM
#include <experimental/numeric>  // lcm until c++17
#else
#include "fallback/gcdlcm.hpp"  // lcm until c++17
#endif                 // HAVE_EXPERIMENTAL_NUMERIC_LCM

namespace HPCombi {

/*****************************************************************************/
/** Implementation part for inline functions *********************************/
/*****************************************************************************/

inline PTransf16::PTransf16(std::initializer_list<uint8_t> il) : Vect16(epu8id) {
    assert(il.size() <= 16);
    std::copy(il.begin(), il.end(), HPCombi::as_array(v).begin());
}

inline PTransf16::PTransf16(std::vector<uint8_t> dom,
                            std::vector<uint8_t> rng, size_t /*unused */) :
    Vect16(Epu8(0xFF)) {
    assert(dom.size() == rng.size());
    assert(dom.size() <= 16);
    for (size_t i = 0; i < dom.size(); ++i) {
        assert(dom[i] < 16);
        v[dom[i]] = rng[i];
    }
}

inline epu8 PTransf16::domain_mask(bool complement) const {
    return complement ? v == Epu8(0xFF) : v != Epu8(0xFF);
}
inline uint32_t PTransf16::domain_bitset(bool complement) const {
    return _mm_movemask_epi8(domain_mask(complement));
}
inline PTransf16 PTransf16::right_one() const {
    return domain_mask(true) | epu8id;
}

inline epu8 PTransf16::image_mask(bool complement) const {
    return complement ? _mm_cmpestrm(v, 16, one().v, 16, FIND_IN_VECT)
                      : _mm_cmpestrm(v, 16, one().v, 16, FIND_IN_VECT_COMPL);
}
inline uint32_t PTransf16::image_bitset(bool complement) const {
    return _mm_movemask_epi8(image_mask(complement));
}
inline PTransf16 PTransf16::left_one() const {
    return image_mask(true) | epu8id;
}
inline uint32_t PTransf16::rank_ref() const {
    TPUBuild<epu8>::array tmp{};
    for (size_t i = 0; i < 16; i++)
        tmp[v[i]] = 1;
    uint32_t res = 0;
    for (size_t i = 0; i < 16; i++)
        res += tmp[i];
    return res;
}
inline uint32_t PTransf16::rank() const {
    return _mm_popcnt_u32(image_bitset());
}

inline epu8 PTransf16::fix_points_mask(bool complement) const {
    return complement ? v != one().v : v == one().v;
}
inline uint32_t PTransf16::fix_points_bitset(bool complement) const {
    return _mm_movemask_epi8(fix_points_mask(complement));
}

inline uint8_t PTransf16::smallest_fix_point() const {
    uint32_t res = fix_points_bitset(false);
    return res == 0 ? 0xFF : _bit_scan_forward(res);
}
/** Returns the smallest non fix point of \c *this */
inline uint8_t PTransf16::smallest_moved_point() const {
    uint32_t res = fix_points_bitset(true);
    return res == 0 ? 0xFF : _bit_scan_forward(res);
}
/** Returns the largest fix point of \c *this */
inline uint8_t PTransf16::largest_fix_point() const {
    uint32_t res = fix_points_bitset(false);;
    return res == 0 ? 0xFF : _bit_scan_reverse(res);
}
/** Returns the largest non fix point of \c *this */
inline uint8_t PTransf16::largest_moved_point() const {
    uint32_t res = fix_points_bitset(true);;
    return res == 0 ? 0xFF : _bit_scan_reverse(res);
}
/** Returns the number of fix points of \c *this */
inline uint8_t PTransf16::nb_fix_points() const {
    return _mm_popcnt_u32(fix_points_bitset());
}

inline static HPCOMBI_CONSTEXPR uint8_t hilo_exchng_fun(uint8_t i) {
    return i < 8 ? i + 8 : i - 8;
}
static HPCOMBI_CONSTEXPR epu8 hilo_exchng = Epu8(hilo_exchng_fun);
inline static HPCOMBI_CONSTEXPR uint8_t hilo_mask_fun(uint8_t i) {
    return i < 8 ? 0x0 : 0xFF;
}
static HPCOMBI_CONSTEXPR epu8 hilo_mask = Epu8(hilo_mask_fun);

inline Transf16::Transf16(uint64_t compressed) {
    epu8 res = _mm_set_epi64x(compressed, compressed);
    v = _mm_blendv_epi8(res & Epu8(0x0F), res >> 4, hilo_mask);
}

inline Transf16::operator uint64_t() const {
    epu8 res = static_cast<epu8>(_mm_slli_epi32(v, 4));
    res = HPCombi::permuted(res, hilo_exchng) + v;
    return _mm_extract_epi64(res, 0);
}

inline PPerm16 PPerm16::inverse_ref() const {
    epu8 res = Epu8(0xFF);
    for (size_t i = 0; i < 16; ++i)
        if (v[i] < 16)
            res[v[i]] = i;
    return res;
}

inline PPerm16 PPerm16::inverse_find() const {
    epu8 mask = _mm_cmpestrm(v, 16, one(), 16, FIND_IN_VECT);
    return permutation_of(v, one()) | mask;
}

inline Perm16 Perm16::random(uint64_t n) {
    Perm16 res = one();
    auto ar = res.as_array();
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(ar.begin(), ar.begin() + n, g);
    return res;
}

// From Ruskey : Combinatorial Generation page 138
inline Perm16 Perm16::unrankSJT(int n, int r) {
    int j;
    std::array<int, 16> dir;
    epu8 res{};
    for (j = 0; j < n; ++j)
        res[j] = 0xFF;
    for (j = n - 1; j >= 0; --j) {
        int k, rem, c;
        rem = r % (j + 1);
        r = r / (j + 1);
        if ((r & 1) != 0) {
            k = -1;
            dir[j] = +1;
        } else {
            k = n;
            dir[j] = -1;
        }
        c = -1;
        do {
            k = k + dir[j];
            if (res[k] == 0xFF)
                ++c;
        } while (c < rem);
        res[k] = j;
    }
    return res;
}

inline Perm16 Perm16::elementary_transposition(uint64_t i) {
    assert(i < 16);
    epu8 res = one();
    res[i] = i + 1;
    res[i + 1] = i;
    return res;
}

inline Perm16 Perm16::inverse_ref() const {
    epu8 res;
    for (size_t i = 0; i < 16; ++i)
        res[v[i]] = i;
    return res;
}

inline Perm16 Perm16::inverse_arr() const {
    epu8 res;
    auto &arres = HPCombi::as_array(res);
    auto self = as_array();
    for (size_t i = 0; i < 16; ++i)
        arres[self[i]] = i;
    return res;
}

inline Perm16 Perm16::inverse_sort() const {
    // G++-7 compile this shift by 3 additions.
    // epu8 res = (v << 4) + one().v;
    // I call directly the shift intrinsic
    epu8 res = static_cast<epu8>(_mm_slli_epi32(v, 4)) + one().v;
    res = sorted(res) & Epu8(0x0F);
    return res;
}

// We declare PERM16 as a correct Monoid
namespace power_helper {

using Perm16 = Perm16;

template <> struct Monoid<Perm16> {
    static const Perm16 one() { return Perm16::one(); }
    static Perm16 prod(Perm16 a, Perm16 b) { return a * b; }
};

}  // namespace power_helper

inline Perm16 Perm16::inverse_cycl() const {
    Perm16 res = one();
    Perm16 newpow = pow<8>(*this);
    for (int i = 9; i <= 16; i++) {
        Perm16 oldpow = newpow;
        newpow = oldpow * *this;
        res.v = _mm_blendv_epi8(res, oldpow, newpow.v == one().v);
    }
    return res;
}

static constexpr unsigned lcm_range(unsigned n) {
#if __cplusplus <= 201103L
    return n == 1 ? 1 : std::experimental::lcm(lcm_range(n - 1), n);
#else
    unsigned res = 1;
    for (unsigned i = 1; i <= n; ++i)
        res = std::experimental::lcm(res, i);
    return res;
#endif
}

inline Perm16 Perm16::inverse_pow() const {
    return pow<lcm_range(16) - 1>(*this);
}

inline epu8 Perm16::lehmer_ref() const {
    epu8 res{};
    for (size_t i = 0; i < 16; i++)
        for (size_t j = i + 1; j < 16; j++)
            if (v[i] > v[j])
                res[i]++;
    return res;
}

inline epu8 Perm16::lehmer_arr() const {
    TPUBuild<epu8>::array res{};
    TPUBuild<epu8>::array ar = as_array();
    for (size_t i = 0; i < 16; i++)
        for (size_t j = i + 1; j < 16; j++)
            if (ar[i] > ar[j])
                res[i]++;
    return from_array(res);
}

inline epu8 Perm16::lehmer() const {
    epu8 vsh = v, res = -one().v;
    for (int i = 1; i < 16; i++) {
        vsh = shifted_left(vsh);
        res -= (v >= vsh);
    }
    return res;
}

inline uint8_t Perm16::length_ref() const {
    uint8_t res = 0;
    for (size_t i = 0; i < 16; i++)
        for (size_t j = i + 1; j < 16; j++)
            if (v[i] > v[j])
                res++;
    return res;
}

inline uint8_t Perm16::length_arr() const {
    uint8_t res = 0;
    TPUBuild<epu8>::array ar = as_array();
    for (size_t i = 0; i < 16; i++)
        for (size_t j = i + 1; j < 16; j++)
            if (ar[i] > ar[j])
                res++;
    return res;
}

inline uint8_t Perm16::length() const { return horiz_sum(lehmer()); }

inline uint8_t Perm16::nb_descents_ref() const {
    uint8_t res = 0;
    for (size_t i = 0; i < 16 - 1; i++)
        if (v[i] > v[i + 1])
            res++;
    return res;
}
inline uint8_t Perm16::nb_descents() const {
    return _mm_popcnt_u32(_mm_movemask_epi8(v < shifted_right(v)));
}

inline uint8_t Perm16::nb_cycles_ref() const {
    std::array<bool, 16> b{};
    uint8_t c = 0;
    for (size_t i = 0; i < 16; i++) {
        if (not b[i]) {
            for (size_t j = i; not b[j]; j = v[j])
                b[j] = true;
            c++;
        }
    }
    return c;
}

inline epu8 Perm16::cycles_partition() const {
    epu8 x0, x1 = one();
    Perm16 p = *this;
    x0 = _mm_min_epi8(x1, HPCombi::permuted(x1, p));
    p = p * p;
    x1 = _mm_min_epi8(x0, HPCombi::permuted(x0, p));
    p = p * p;
    x0 = _mm_min_epi8(x1, HPCombi::permuted(x1, p));
    p = p * p;
    x1 = _mm_min_epi8(x0, HPCombi::permuted(x0, p));
    return x1;
}

inline uint8_t Perm16::nb_cycles_unroll() const {
    epu8 res = (epu8id == cycles_partition());
    return _mm_popcnt_u32(_mm_movemask_epi8(res));
}

inline bool Perm16::left_weak_leq_ref(Perm16 other) const {
    for (size_t i = 0; i < 16; i++) {
        for (size_t j = i + 1; j < 16; j++) {
            if ((v[i] > v[j]) && (other[i] < other[j]))
                return false;
        }
    }
    return true;
}

inline bool Perm16::left_weak_leq(Perm16 other) const {
    epu8 srot = v, orot = other;
    for (size_t i = 0; i < 15; i++) {
        srot = shifted_right(srot);
        orot = shifted_right(orot);
        uint64_t sinv = _mm_movemask_epi8(v < srot);
        uint64_t oinv = _mm_movemask_epi8(other.v < orot);
        if ((sinv & oinv) != sinv)
            return false;
    }
    return true;
}

inline bool Perm16::left_weak_leq_length(Perm16 other) const {
    Perm16 prod = *this * other.inverse();
    return other.length() == length() + prod.length();
}

}  // namespace HPCombi
