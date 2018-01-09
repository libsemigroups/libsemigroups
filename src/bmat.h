//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2017 Finn Smith
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

// This file contains a declaration of fast boolean matrices up to dimension 8
// x 8.

#ifndef LIBSEMIGROUPS_SRC_BMAT_H_
#define LIBSEMIGROUPS_SRC_BMAT_H_

#include <array>
#include <climits>
#include <functional>
#include <iostream>
#include <random>

#define intersect(A, B) ~(~A & B) & B

namespace libsemigroups {
  class BMat8 {
   public:
    BMat8() = default;
    explicit BMat8(uint64_t mat) : _data(mat) {}
    explicit BMat8(std::vector<std::vector<size_t>> const& mat);

    friend std::ostream& operator<<(std::ostream& os, BMat8 const& bm) {
      os << bm.to_string();
      return os;
    }

    BMat8(BMat8 const&) = default;             // Copy constructor
    BMat8(BMat8&&)      = default;             // Move constructor
    BMat8& operator=(BMat8 const&) = default;  // Copy assignment operator
    BMat8& operator=(BMat8&&) = default;       // Move assignment operator
    ~BMat8()                  = default;       // Destructor

    BMat8 operator*(BMat8 const& that) const;

    bool operator==(BMat8 const& that) const {
      return _data == that._data;
    }

    bool operator!=(BMat8 const& that) const {
      return _data != that._data;
    }

    bool operator<(BMat8 const& that) const {
      return _data < that._data;
    }

    bool operator>(BMat8 const& that) const {
      return _data > that._data;
    }

    bool operator()(size_t i, size_t j) const;

    // Bit hacks
    static std::vector<uint64_t> const ROW_MASK;
    static std::vector<uint64_t> const COL_MASK;

    static std::array<uint64_t, 8> for_sorting;

    static inline void swap_for_sorting(size_t a, size_t b) {
      if (for_sorting[b] < for_sorting[a]) {
        std::swap(for_sorting[a], for_sorting[b]);
      }
    }

    // Cyclically shifts bits to left by 8m
    // https://stackoverflow.com/a/776523
    static inline uint64_t cyclic_shift(uint64_t n, uint64_t m = 1) {
      const unsigned int mask
          = (CHAR_BIT * sizeof(n) - 1);  // assumes width is a power of 2.

      // assert ( (c<=mask) &&"rotate by type width or more");
      unsigned int c = 8 * m;
      c &= mask;
      return (n << c) | (n >> ((-c) & mask));
    }

    // Given 8 x 8 matrices A and B, this function returns a matrix where each
    // row is the corresponding row of B if that row of B is a subset of the
    // corresponding row of A, and 0 otherwise.
    static inline uint64_t zero_if_row_not_contained(uint64_t A, uint64_t B) {
      uint64_t w = intersect(A, B);
      for (size_t i = 0; i < 8; ++i) {
        if ((w & ROW_MASK[i]) == (B & ROW_MASK[i])) {
          w |= (B & ROW_MASK[i]);
        } else {
          w &= ~(B & ROW_MASK[i]);
        }
      }
      return w;
    }
    std::string     to_string() const;
    inline uint64_t to_int() const {
      return _data;
    }
    BMat8 row_space_basis() const;
    BMat8 col_space_basis() const;

    inline BMat8 transpose() const {
      uint64_t x = _data;
      uint64_t y = (x ^ (x >> 7)) & 0xAA00AA00AA00AA;
      x          = x ^ y ^ (y << 7);
      y          = (x ^ (x >> 14)) & 0xCCCC0000CCCC;
      x          = x ^ y ^ (y << 14);
      y          = (x ^ (x >> 28)) & 0xF0F0F0F0;
      x          = x ^ y ^ (y << 28);
      return BMat8(x);
    }

    inline bool is_invertible() const {
      return *this * transpose() == one();
    }

    // returns the inverse of this matrix, if it is invertible
    // TODO: what if not?
    inline BMat8 inverse() const {
      // LIBSEMIGROUPS_ASSERT(is_invertible());
      // a boolean matrix has an inverse iff it is orthogonal
      return transpose();
    }

    // https://stackoverflow.com/a/18448513

    void redefine(BMat8 const& A, BMat8 const& B) {
      uint64_t y    = B.transpose()._data;
      _data         = 0;
      uint64_t tmp  = 0;
      uint64_t diag = 0x8040201008040201;
      for (int i = 0; i < 8; ++i) {
        tmp = A._data & y;
        tmp |= tmp >> 1;
        tmp |= tmp >> 2;
        tmp |= tmp >> 4;
        tmp &= 0x0101010101010101;
        tmp *= 255;
        tmp &= diag;
        _data |= tmp;

        y    = cyclic_shift(y);
        tmp  = 0;
        diag = cyclic_shift(diag);
      }
    }

    inline void sort_rows() {
      for (size_t i = 0; i < 8; ++i) {
        for_sorting[i] = (_data << 8 * i) & ROW_MASK[0];
      }
      swap_for_sorting(0, 1);
      swap_for_sorting(2, 3);
      swap_for_sorting(0, 2);
      swap_for_sorting(1, 3);
      swap_for_sorting(1, 2);
      swap_for_sorting(4, 5);
      swap_for_sorting(6, 7);
      swap_for_sorting(4, 6);
      swap_for_sorting(5, 7);
      swap_for_sorting(5, 6);
      swap_for_sorting(0, 4);
      swap_for_sorting(1, 5);
      swap_for_sorting(1, 4);
      swap_for_sorting(2, 6);
      swap_for_sorting(3, 7);
      swap_for_sorting(3, 6);
      swap_for_sorting(2, 4);
      swap_for_sorting(3, 5);
      swap_for_sorting(3, 4);
      _data = 0;
      for (size_t i = 0; i < 7; ++i) {
        _data |= for_sorting[i];
        _data = (_data >> 8);
      }
      _data |= for_sorting[7];
    }

    inline BMat8 one() const {
      return BMat8(0x8040201008040201);
    }

    void swap_rows(size_t i, size_t j);

    // FIXME remove this
    size_t complexity() const {
      return 0;
    }
    // FIXME remove this
    size_t degree() const {
      return 8;
    }

    static BMat8 random();
    static BMat8 random(size_t dim);

    // BMat8 lvalue(BMat8 cols, BMat8 *tmp);

   private:
    void assign(uint64_t data);

    uint64_t _data;

    static std::mt19937                          _gen;
    static std::uniform_int_distribution<size_t> _dist;
  };

}  // namespace libsemigroups

namespace std {
  template <> struct hash<libsemigroups::BMat8> {
    size_t operator()(libsemigroups::BMat8 const& bm) const {
      return hash<uint64_t>()(bm.to_int());
    }
  };
}  // namespace std
#endif  // LIBSEMIGROUPS_SRC_BMAT_H_
