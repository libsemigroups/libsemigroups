////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////

#ifndef HPCOMBI_VECT16_HPP_
#define HPCOMBI_VECT16_HPP_

#include <cstddef>           // for size_t
#include <cstdint>           // for uint8_t, uint64_t, int8_t
#include <initializer_list>  // for initializer_list
#include <iosfwd>            // for ostream
#include <memory>            // for hash
#include <type_traits>       // for is_trivial

#include "epu8.hpp"

namespace HPCombi {

struct alignas(16) Vect16 {
    static constexpr size_t size() { return 16; }
    using array = typename decltype(Epu8)::array;
    epu8 v;

    Vect16() = default;
    constexpr Vect16(epu8 x) : v(x) {}
    Vect16(std::initializer_list<uint8_t> il, uint8_t def = 0)
        : v(Epu8(il, def)) {}
    constexpr operator epu8() const { return v; }

    array &as_array() { return HPCombi::as_array(v); }
    const array &as_array() const { return HPCombi::as_array(v); }

    const uint8_t &operator[](uint64_t i) const { return as_array()[i]; }
    uint8_t &operator[](uint64_t i) { return as_array()[i]; }

    size_t first_diff(const Vect16 &u, size_t bound = size()) const {
        return HPCombi::first_diff(v, u.v, bound);
    }
    size_t last_diff(const Vect16 &u, size_t bound = size()) const {
        return HPCombi::last_diff(v, u.v, bound);
    }

    size_t first_zero(size_t bound = size()) const {
        return HPCombi::first_zero(v, bound);
    }
    size_t last_zero(size_t bound = size()) const {
        return HPCombi::last_zero(v, bound);
    }
    size_t first_non_zero(size_t bound = size()) const {
        return HPCombi::first_non_zero(v, bound);
    }
    size_t last_non_zero(size_t bound = size()) const {
        return HPCombi::last_non_zero(v, bound);
    }

    using value_type = uint8_t;
    using iterator = typename array::iterator;
    using const_iterator = typename array::const_iterator;

    const_iterator cbegin() const { return as_array().begin(); }
    const_iterator cend() const { return as_array().end(); }

    iterator begin() { return as_array().begin(); }
    iterator end() { return as_array().end(); }

    const_iterator begin() const { return as_array().begin(); }
    const_iterator end() const { return as_array().end(); }

    bool operator==(const Vect16 &b) const { return HPCombi::equal(v, b.v); }

    bool operator!=(const Vect16 &b) const {
        return HPCombi::not_equal(v, b.v);
    }

    bool operator<(const Vect16 &b) const { return less(v, b.v); }
    int8_t less_partial(const Vect16 &b, int k) const {
        return HPCombi::less_partial(v, b.v, k);
    }
    Vect16 permuted(const Vect16 &b) const { return HPCombi::permuted(v, b.v); }
    uint8_t sum() const { return HPCombi::horiz_sum(v); }
    Vect16 partial_sums() const { return HPCombi::partial_sums(v); }
    Vect16 eval16() const { return HPCombi::eval16(v); }

    bool is_permutation() const { return HPCombi::is_permutation(v); }
    bool is_permutation(size_t k) const {
        return HPCombi::is_permutation(v, k);
    }
};

static_assert(std::is_trivial<Vect16>(), "Vect16 is not a trivial class !");

}  // namespace HPCombi

namespace std {

inline std::ostream &operator<<(std::ostream &stream,
                                const HPCombi::Vect16 &ar) {
    return operator<<(stream, ar.v);
}

template <> struct hash<HPCombi::Vect16> {
    size_t operator()(const HPCombi::Vect16 &ar) const {
        return std::hash<HPCombi::epu8>{}(ar.v);
    }
};

}  // namespace std

#endif  // HPCOMBI_VECT16_HPP_
