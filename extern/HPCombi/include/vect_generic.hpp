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

#ifndef HPCOMBI_VECT_GENERIC_HPP
#define HPCOMBI_VECT_GENERIC_HPP

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <ostream>
#include <type_traits>

namespace HPCombi {

template <size_t Size, typename Expo = uint8_t>
std::array<Expo, Size> sorted_vect(std::array<Expo, Size> v) {
    std::sort(v.begin(), v.end());
    return v;
}

/** A generic class for combinatorial integer vectors.
 */
template <size_t _Size, typename Expo = uint8_t> struct VectGeneric {

    static constexpr size_t Size() { return _Size; };
    using array = std::array<Expo, _Size>;
    array v;

    VectGeneric() = default;
    VectGeneric(std::array<Expo, _Size> _v) : v(_v){};
    VectGeneric(std::initializer_list<Expo> il, Expo def = 0) {
        assert(il.size() <= _Size);
        std::copy(il.begin(), il.end(), v.begin());
        std::fill(v.begin() + il.size(), v.end(), def);
    }

    VectGeneric &operator=(const VectGeneric &) = default;
    VectGeneric &operator=(const array &vv) {
        v = vv;
        return *this;
    }

    Expo operator[](uint64_t i) const { return v[i]; }
    Expo &operator[](uint64_t i) { return v[i]; }

    size_t first_diff(const VectGeneric &u, size_t bound = _Size) const {
        for (size_t i = 0; i < bound; i++)
            if (v[i] != u[i])
                return i;
        return _Size;
    }

    size_t last_diff(const VectGeneric &u, size_t bound = _Size) const {
        while (bound != 0) {
            --bound;
            if (u[bound] != v[bound])
                return bound;
        }
        return _Size;
    }

    using value_type = Expo;
    using iterator = typename array::iterator;
    using const_iterator = typename array::const_iterator;
    iterator begin() { return v.begin(); }
    iterator end() { return v.end(); }
    const_iterator begin() const { return v.begin(); }
    const_iterator end() const { return v.end(); }

    bool operator==(const VectGeneric &u) const {
        return first_diff(u) == _Size;
    }
    bool operator!=(const VectGeneric &u) const {
        return first_diff(u) != _Size;
    }

    bool operator<(const VectGeneric &u) const {
        uint64_t diff = first_diff(u);
        return (diff != _Size) and v[diff] < u[diff];
    }

    char less_partial(const VectGeneric &u, int k) const {
        uint64_t diff = first_diff(u, k);
        return (diff == _Size) ? 0 : char(v[diff]) - char(u[diff]);
    }

    VectGeneric permuted(const VectGeneric &u) const {
        VectGeneric res;
        for (uint64_t i = 0; i < _Size; i++)
            res[i] = v[u[i]];
        return res;
    };

    void sort() { std::sort(v.begin(), v.end()); }

    bool is_sorted() const {
        for (uint64_t i = 1; i < _Size; i++)
            if (v[i - 1] < v[i])
                return false;
        return true;
    }

    static VectGeneric random() {
        VectGeneric<_Size, Expo> res = VectGeneric<_Size, Expo>(0, 0);
        std::random_shuffle(res.begin(), res.end());
        return res;
    }

    uint64_t first_non_zero(size_t bound = _Size) const {
        for (uint64_t i = 0; i < bound; i++)
            if (v[i] != 0)
                return i;
        return _Size;
    }
    uint64_t first_zero(size_t bound = _Size) const {
        for (uint64_t i = 0; i < bound; i++)
            if (v[i] == 0)
                return i;
        return _Size;
    }
    uint64_t last_non_zero(size_t bound = _Size) const {
        for (int64_t i = bound - 1; i >= 0; i--)
            if (v[i] != 0)
                return i;
        return _Size;
    }
    uint64_t last_zero(size_t bound = _Size) const {
        for (int64_t i = bound - 1; i >= 0; i--)
            if (v[i] == 0)
                return i;
        return _Size;
    }

    bool is_permutation(const size_t k = _Size) const {
        auto temp = v;
        std::sort(temp.begin(), temp.end());
        for (uint64_t i = 0; i < _Size; i++)
            if (temp[i] != i)
                return false;
        for (uint64_t i = k; i < _Size; i++)
            if (v[i] != i)
                return false;
        return true;
    }

    uint64_t horiz_sum() const {
        Expo res = 0;
        for (uint64_t i = 0; i < _Size; i++)
            res += v[i];
        return res;
    }

    VectGeneric partial_sums() const {
        auto res = *this;
        for (uint64_t i = 1; i < _Size; i++)
            res[i] += res[i - 1];
        return res;
    }

    void partial_sums_inplace() {
        for (uint64_t i = 1; i < _Size; i++)
            v[i] += v[i - 1];
    }

    Expo horiz_max() const {
        Expo res = v[0];
        for (uint64_t i = 1; i < _Size; i++)
            res = std::max(res, v[i]);
        return res;
    }

    void partial_max_inplace() {
        for (uint64_t i = 1; i < _Size; i++)
            v[i] = std::max(v[i], v[i - 1]);
    }

    Expo horiz_min() const {
        Expo res = v[0];
        for (uint64_t i = 1; i < _Size; i++)
            res = std::min(res, v[i]);
        return res;
    }

    void partial_min_inplace() {
        for (uint64_t i = 1; i < _Size; i++)
            v[i] = std::min(v[i], v[i - 1]);
    }

    VectGeneric eval() const {
        VectGeneric res{};
        for (size_t i = 0; i < _Size; i++)
            if (v[i] < _Size)
                res[v[i]]++;
        return res;
    }
};

static_assert(std::is_trivial<VectGeneric<12>>(),
              "VectGeneric is not a trivial class !");

}  // namespace HPCombi

namespace std {

template <size_t _Size, typename Expo>
std::ostream &operator<<(std::ostream &stream,
                         const HPCombi::VectGeneric<_Size, Expo> &v) {
    stream << "[" << std::setw(2) << unsigned(v[0]);
    for (unsigned i = 1; i < _Size; ++i)
        stream << "," << std::setw(2) << unsigned(v[i]);
    stream << "]";
    return stream;
}

template <size_t _Size, typename Expo>
struct hash<HPCombi::VectGeneric<_Size, Expo>> {
    size_t operator()(const HPCombi::VectGeneric<_Size, Expo> &ar) const {
        size_t h = 0;
        for (size_t i = 0; i < _Size; i++)
            h = hash<Expo>()(ar[i]) + (h << 6) + (h << 16) - h;
        return h;
    }
};

}  // namespace std

#endif  // HPCOMBI_VECT_GENERIC_HPP
