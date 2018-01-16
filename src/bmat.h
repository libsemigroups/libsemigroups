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

// This file contains a declaration of fast boolean matrices up to dimension 8.

#ifndef LIBSEMIGROUPS_SRC_BMAT_H_
#define LIBSEMIGROUPS_SRC_BMAT_H_

#include <climits>
#include <functional>
#include <iostream>
#include <random>

#include "libsemigroups-debug.h"
#include "timer.h"

namespace libsemigroups {
  class BMat8 {
   public:
    BMat8() = default;
    explicit BMat8(uint64_t mat) : _data(mat) {}
    explicit BMat8(std::vector<std::vector<size_t>> const& mat);

    BMat8(BMat8 const&) = default;             // Copy constructor
    BMat8(BMat8&&)      = default;             // Move constructor
    BMat8& operator=(BMat8 const&) = default;  // Copy assignment operator
    BMat8& operator=(BMat8&&) = default;       // Move assignment operator
    ~BMat8()                  = default;       // Destructor

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

    bool operator()(size_t i, size_t j) const {
      LIBSEMIGROUPS_ASSERT(0 <= i && i < 8);
      LIBSEMIGROUPS_ASSERT(0 <= j && j < 8);
      return (_data << (8 * i + j)) >> 63;
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

    inline uint64_t to_int() const {
      return _data;
    }

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

    // https://stackoverflow.com/a/18448513
    inline BMat8 operator*(BMat8 const& that) const {
      uint64_t y    = that.transpose()._data;
      uint64_t data = 0;
      uint64_t tmp  = 0;
      uint64_t diag = 0x8040201008040201;
      for (int i = 0; i < 8; ++i) {
        tmp = _data & y;
        tmp |= tmp >> 1;
        tmp |= tmp >> 2;
        tmp |= tmp >> 4;
        tmp &= 0x0101010101010101;
        tmp *= 255;
        tmp &= diag;
        data |= tmp;
        y    = cyclic_shift(y);
        tmp  = 0;
        diag = cyclic_shift(diag);
      }
      return BMat8(data);
    }

    inline BMat8 one() const {
      return BMat8(0x8040201008040201);
    }

    friend std::ostream& operator<<(std::ostream& os, BMat8 const& bm) {
      os << bm.to_string();
      return os;
    }

    std::string  to_string() const;
    static BMat8 random();
    static BMat8 random(size_t dim);

   private:
    uint64_t                                     _data;
    static std::mt19937                          _gen;
    static std::uniform_int_distribution<size_t> _dist;
    static std::vector<uint64_t> const           ROW_MASK;
    static std::vector<uint64_t> const           COL_MASK;
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
