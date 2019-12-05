//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 Finn Smith
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

// This file contains an implemesntation of fast boolean matrices up to
// dimension 8 x 8.

#include "bmat8.hpp"

#include <algorithm>  // for uniform_int_distribution
#include <array>      // for array
#include <climits>    // for CHAR_BIT
#include <iostream>   // for operator<<, ostringstream
#include <random>     // for mt19937, random_device
#include <vector>     // for vector

#include "libsemigroups-exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION

namespace libsemigroups {

  ////////////////////////////////////////////////////////////////////////
  // Static helper data and functions
  ////////////////////////////////////////////////////////////////////////

  static std::array<uint64_t, 8> const ROW_MASK = {0xff00000000000000,
                                                   0xff000000000000,
                                                   0xff0000000000,
                                                   0xff00000000,
                                                   0xff000000,
                                                   0xff0000,
                                                   0xff00,
                                                   0xff};

  static std::array<uint64_t, 8> const COL_MASK = {0x8080808080808080,
                                                   0x4040404040404040,
                                                   0x2020202020202020,
                                                   0x1010101010101010,
                                                   0x808080808080808,
                                                   0x404040404040404,
                                                   0x202020202020202,
                                                   0x101010101010101};

  static std::array<uint64_t, 64> const BIT_MASK = {0x8000000000000000,
                                                    0x4000000000000000,
                                                    0x2000000000000000,
                                                    0x1000000000000000,
                                                    0x800000000000000,
                                                    0x400000000000000,
                                                    0x200000000000000,
                                                    0x100000000000000,
                                                    0x80000000000000,
                                                    0x40000000000000,
                                                    0x20000000000000,
                                                    0x10000000000000,
                                                    0x8000000000000,
                                                    0x4000000000000,
                                                    0x2000000000000,
                                                    0x1000000000000,
                                                    0x800000000000,
                                                    0x400000000000,
                                                    0x200000000000,
                                                    0x100000000000,
                                                    0x80000000000,
                                                    0x40000000000,
                                                    0x20000000000,
                                                    0x10000000000,
                                                    0x8000000000,
                                                    0x4000000000,
                                                    0x2000000000,
                                                    0x1000000000,
                                                    0x800000000,
                                                    0x400000000,
                                                    0x200000000,
                                                    0x100000000,
                                                    0x80000000,
                                                    0x40000000,
                                                    0x20000000,
                                                    0x10000000,
                                                    0x8000000,
                                                    0x4000000,
                                                    0x2000000,
                                                    0x1000000,
                                                    0x800000,
                                                    0x400000,
                                                    0x200000,
                                                    0x100000,
                                                    0x80000,
                                                    0x40000,
                                                    0x20000,
                                                    0x10000,
                                                    0x8000,
                                                    0x4000,
                                                    0x2000,
                                                    0x1000,
                                                    0x800,
                                                    0x400,
                                                    0x200,
                                                    0x100,
                                                    0x80,
                                                    0x40,
                                                    0x20,
                                                    0x10,
                                                    0x8,
                                                    0x4,
                                                    0x2,
                                                    0x1};

  // Cyclically shifts bits to left by 8m
  // https://stackoverflow.com/a/776523
  static uint64_t cyclic_shift(uint64_t n) noexcept {
    const unsigned int mask
        = (CHAR_BIT * sizeof(n) - 1);  // assumes width is a power of 2.
    // assert ( (c<=mask) &&"rotate by type width or more");
    unsigned int c = 8;
    c &= mask;
    return (n << c) | (n >> ((-c) & mask));
  }

  // Given 8 x 8 matrices A and B, this function returns a matrix where each
  // row is the corresponding row of B if that row of B is a subset of the
  // corresponding row of A, and 0 otherwise.
  static uint64_t zero_if_row_not_contained(uint64_t A, uint64_t B) noexcept {
    uint64_t w = A & B;
    for (size_t i = 0; i < 8; ++i) {
      if ((w & ROW_MASK[i]) == (B & ROW_MASK[i])) {
        w |= (B & ROW_MASK[i]);
      } else {
        w &= ~(ROW_MASK[i]);
      }
    }
    return w;
  }

  static std::array<uint64_t, 8> for_sorting = {0, 0, 0, 0, 0, 0, 0, 0};

  static inline void swap_for_sorting(size_t a, size_t b) noexcept {
    if (for_sorting[b] < for_sorting[a]) {
      std::swap(for_sorting[a], for_sorting[b]);
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // BMat8 - static data members - private
  ////////////////////////////////////////////////////////////////////////

  std::random_device                      BMat8::_rd;
  std::mt19937                            BMat8::_gen(_rd());
  std::uniform_int_distribution<uint64_t> BMat8::_dist(0, 0xffffffffffffffff);

  ////////////////////////////////////////////////////////////////////////
  // BMat8 - constructors - public
  ////////////////////////////////////////////////////////////////////////

  BMat8::BMat8(std::vector<std::vector<bool>> const& mat) {
    if (0 == mat.size() || mat.size() > 8) {
      LIBSEMIGROUPS_EXCEPTION(
          "expected a vector with size in [1, 8], got a vector of size %d",
          mat.size());
    }
    _data        = 0;
    uint64_t pow = 1;
    pow          = pow << 63;
    for (auto row : mat) {
      if (row.size() != mat.size()) {
        LIBSEMIGROUPS_EXCEPTION("the vectors must all have the same length!");
      }
      for (auto entry : row) {
        if (entry) {
          _data ^= pow;
        }
        pow = pow >> 1;
      }
      pow = pow >> (8 - mat.size());
    }
  }

  // Not noexcept because it can throw.
  BMat8 BMat8::random(size_t const dim) {
    if (0 == dim || dim > 8) {
      LIBSEMIGROUPS_EXCEPTION("the argument should be in [1, 8], got %d", dim);
    }
    BMat8 bm = BMat8::random();
    for (size_t i = dim; i < 8; ++i) {
      bm._data &= ~(ROW_MASK[i]);
      bm._data &= ~(COL_MASK[i]);
    }
    return bm;
  }

  ////////////////////////////////////////////////////////////////////////
  // BMat8 - member functions - public
  ////////////////////////////////////////////////////////////////////////

  BMat8 BMat8::row_space_basis() const noexcept {
    uint64_t out            = 0;
    uint64_t combined_masks = 0;

    BMat8 bm(_data);
    bm.sort_rows();
    uint64_t no_dups = bm._data;
    for (size_t i = 0; i < 7; ++i) {
      combined_masks |= ROW_MASK[i];
      while ((no_dups & ROW_MASK[i + 1]) << 8 == (no_dups & ROW_MASK[i])
             && (no_dups & ROW_MASK[i]) != 0) {
        no_dups = ((no_dups & combined_masks)
                   | (no_dups & ~combined_masks & ~ROW_MASK[i + 1]) << 8);
      }
    }
    uint64_t cm = no_dups;
    for (size_t i = 0; i < 7; ++i) {
      cm = cyclic_shift(cm);
      out |= zero_if_row_not_contained(no_dups, cm);
    }
    for (size_t i = 0; i < 8; ++i) {
      if ((out & ROW_MASK[i]) == (no_dups & ROW_MASK[i])) {
        out &= ~ROW_MASK[i];
      } else {
        out |= (no_dups & ROW_MASK[i]);
      }
    }
    combined_masks = 0;
    for (size_t i = 0; i < 8; ++i) {
      combined_masks |= ROW_MASK[i];
      while ((out & ROW_MASK[i]) == 0 && ((out & ~combined_masks) != 0)) {
        out = (out & combined_masks) | ((out & ~combined_masks) << 8);
      }
    }
    return BMat8(out);
  }

  void BMat8::set(size_t i, size_t j, bool val) {
    if (7 < i || 7 < j) {
      LIBSEMIGROUPS_EXCEPTION(
          "the arguments should each be at most 7, got %d as the %s argument",
          (7 < i) ? i : j,
          (7 < i) ? std::string("first") : std::string("second"));
    }
    _data ^= (-val ^ _data) & BIT_MASK[8 * i + j];
  }

  BMat8 BMat8::operator*(BMat8 const& that) const noexcept {
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
      diag = cyclic_shift(diag);
    }
    return BMat8(data);
  }

  size_t BMat8::row_space_size() const {
    std::array<char, 256> lookup;
    lookup.fill(0);
    std::vector<uint8_t> row_vec = row_space_basis().rows();
    auto                 last = std::remove(row_vec.begin(), row_vec.end(), 0);
    row_vec.erase(last, row_vec.end());
    for (uint8_t x : row_vec) {
      lookup[x] = true;
    }
    std::vector<uint8_t> row_space(row_vec.begin(), row_vec.end());
    for (size_t i = 0; i < row_space.size(); ++i) {
      for (uint8_t row : row_vec) {
        uint8_t x = row_space[i] | row;
        if (!lookup[x]) {
          row_space.push_back(x);
          lookup[x] = true;
        }
      }
    }
    return row_space.size() + 1;
  }

  std::vector<uint8_t> BMat8::rows() const {
    std::vector<uint8_t> rows;
    for (size_t i = 0; i < 8; ++i) {
      uint8_t row = static_cast<uint8_t>(_data << (8 * i) >> 56);
      rows.push_back(row);
    }
    return rows;
  }

  ////////////////////////////////////////////////////////////////////////
  // BMat8 - member functions - private
  ////////////////////////////////////////////////////////////////////////

  void BMat8::sort_rows() noexcept {
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

  ////////////////////////////////////////////////////////////////////////
  // BMat8 - friend functions
  ////////////////////////////////////////////////////////////////////////

  std::ostringstream& operator<<(std::ostringstream& os, BMat8 const& bm) {
    uint64_t x   = bm.to_int();
    uint64_t pow = 1;
    pow          = pow << 63;
    for (size_t i = 0; i < 8; ++i) {
      for (size_t j = 0; j < 8; ++j) {
        if (pow & x) {
          os << "1";
        } else {
          os << "0";
        }
        x = x << 1;
      }
      os << "\n";
    }
    return os;
  }

  ////////////////////////////////////////////////////////////////////////
  // BMat8 helpers
  ////////////////////////////////////////////////////////////////////////

  namespace bmat8_helpers {
    size_t minimum_dim(BMat8 const& x) noexcept {
      size_t i = 0;
      uint64_t c = x.to_int();
      uint64_t d = x.to_int();
      uint64_t y = x.transpose().to_int();
      uint64_t z = x.transpose().to_int();

      do {
        d = d >> 8;
        y = y >> 8;
        ++i;
      } while ((d << (8 * i)) == c && (y << (8 * i)) == z && i < 8);

      return 9 - i;
    }
  }  // namespace bmat8_helpers
}  // namespace libsemigroups
