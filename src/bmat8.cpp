//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-24 Finn Smith
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

#include "libsemigroups/bmat8.hpp"

#include <algorithm>  // for uniform_int_distribution
#include <array>      // for array
#include <climits>    // for CHAR_BIT
#include <random>     // for mt19937, random_device
#include <sstream>    // for operator<<, ostringstream
#include <vector>     // for vector

#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION

#include "libsemigroups/detail/string.hpp"  // for to_string

namespace libsemigroups {

  namespace {
    void throw_if_indices_out_of_bound(size_t r, size_t c) {
      if (r >= 8) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 1st argument (row index) must be < 8, found {}", r);
      }
      if (c >= 8) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 2nd argument (column index) must be < 8, found {}", c);
      }
    }

    void throw_if_dim_out_of_bound(size_t dim) {
      if (0 == dim || dim > 8) {
        LIBSEMIGROUPS_EXCEPTION(
            "the argument (dimension) should be in [1, 8], got {}", dim);
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // Helper data and functions
    ////////////////////////////////////////////////////////////////////////

    constexpr std::array<uint64_t, 8> const ROW_MASK = {0xff00000000000000,
                                                        0xff000000000000,
                                                        0xff0000000000,
                                                        0xff00000000,
                                                        0xff000000,
                                                        0xff0000,
                                                        0xff00,
                                                        0xff};

    constexpr std::array<uint64_t, 8> const COL_MASK = {0x8080808080808080,
                                                        0x4040404040404040,
                                                        0x2020202020202020,
                                                        0x1010101010101010,
                                                        0x808080808080808,
                                                        0x404040404040404,
                                                        0x202020202020202,
                                                        0x101010101010101};

    // Cyclically shifts bits to left by 8m
    // https://stackoverflow.com/a/776523
    [[nodiscard]] constexpr uint64_t cyclic_shift(uint64_t n) noexcept {
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
    [[nodiscard]] constexpr uint64_t
    zero_if_row_not_contained(uint64_t A, uint64_t B) noexcept {
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

    std::array<uint64_t, 8> for_sorting = {0, 0, 0, 0, 0, 0, 0, 0};

    inline void swap_for_sorting(size_t a, size_t b) noexcept {
      if (for_sorting[b] < for_sorting[a]) {
        std::swap(for_sorting[a], for_sorting[b]);
      }
    }

    void sort_rows(BMat8& x) noexcept {
      auto data = x.to_int();
      for (size_t i = 0; i < 8; ++i) {
        for_sorting[i] = (data << 8 * i) & ROW_MASK[0];
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
      data = 0;
      for (size_t i = 0; i < 7; ++i) {
        data |= for_sorting[i];
        data = (data >> 8);
      }
      data |= for_sorting[7];
      x = BMat8(data);
    }
  }  // namespace

  ////////////////////////////////////////////////////////////////////////
  // BMat8 - constructors - public
  ////////////////////////////////////////////////////////////////////////

  BMat8::BMat8(std::vector<std::vector<bool>> const& mat) {
    throw_if_dim_out_of_bound(mat.size());
    _data        = 0;
    uint64_t pow = 1;
    pow          = pow << 63;
    for (size_t i = 0; i < mat.size(); ++i) {
      if (mat[i].size() != mat.size()) {
        LIBSEMIGROUPS_EXCEPTION(
            "the entries of the argument (rows) must all be the same length "
            "{}, found length {} for row with index {}",
            mat.size(),
            mat[i].size(),
            i);
      }
      for (auto entry : mat[i]) {
        if (entry) {
          _data ^= pow;
        }
        pow = pow >> 1;
      }
      pow = pow >> (8 - mat.size());
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // BMat8 - member functions - public
  ////////////////////////////////////////////////////////////////////////

  bool BMat8::at(size_t r, size_t c) const {
    throw_if_indices_out_of_bound(r, c);
    return this->operator()(r, c);
  }

  BMat8::BitRef BMat8::at(size_t r, size_t c) {
    throw_if_indices_out_of_bound(r, c);
    return this->operator()(r, c);
  }

  uint8_t BMat8::at(size_t r) const {
    throw_if_indices_out_of_bound(r, 0);
    return this->operator()(r);
  }

  BMat8 BMat8::operator*(BMat8 const& that) const noexcept {
    uint64_t y    = bmat8::transpose(that).to_int();
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

  BMat8& BMat8::operator*=(BMat8 const& that) noexcept {
    BMat8 val(*this * that);
    this->swap(val);
    return *this;
  }

  ////////////////////////////////////////////////////////////////////////
  // Free functions
  ////////////////////////////////////////////////////////////////////////

  std::ostream& operator<<(std::ostream& os, BMat8 const& bm) {
    os << detail::to_string(bm);
    return os;
  }

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

  std::string to_string(BMat8 const& x, std::string const& braces) {
    if (braces.size() != 2) {
      LIBSEMIGROUPS_EXCEPTION(
          "the 2nd argument (braces) must have size 2, found {}",
          braces.size());
    }
    auto        lbrace = braces[0], rbrace = braces[1];
    std::string result = fmt::format("BMat8({0}", lbrace);

    auto n = bmat8::minimum_dim(x);
    if (n == 0) {
      return "BMat8(0)";
    }
    std::string rindent;
    for (size_t r = 0; r < n; ++r) {
      result += fmt::format("{}{}", rindent, lbrace);
      rindent          = "       ";
      std::string csep = "";
      for (size_t c = 0; c < n; ++c) {
        result += fmt::format("{}{:d}", csep, x(r, c));
        csep = ", ";
      }
      result += fmt::format("{}", rbrace);
      if (r != n - 1) {
        result += ",\n";
      }
    }
    result += fmt::format("{})", rbrace);
    return result;
  }

  ////////////////////////////////////////////////////////////////////////
  // BMat8 helpers
  ////////////////////////////////////////////////////////////////////////

  namespace bmat8 {
    BMat8 row_space_basis(BMat8 const& x) noexcept {
      uint64_t out            = 0;
      uint64_t combined_masks = 0;

      BMat8 bm(x);
      sort_rows(bm);
      uint64_t no_dups = bm.to_int();
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

    size_t row_space_size(BMat8 const& x) {
      std::array<char, 256> lookup;
      lookup.fill(0);
      auto row_vec = bmat8::rows(bmat8::row_space_basis(x));
      auto last    = std::remove(row_vec.begin(), row_vec.end(), 0);
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

    BMat8 random() {
      static std::mt19937 gen(
          std::random_device{}());  // NOLINT(whitespace/braces)
      static std::uniform_int_distribution<uint64_t> dist(0,
                                                          0xffffffffffffffff);
      return BMat8(dist(gen));
    }

    // Not noexcept because it can throw.
    BMat8 random(size_t dim) {
      throw_if_dim_out_of_bound(dim);
      uint64_t val = random().to_int();
      for (size_t i = dim; i < 8; ++i) {
        val &= ~(ROW_MASK[i]);
        val &= ~(COL_MASK[i]);
      }
      return BMat8(val);
    }

    size_t minimum_dim(BMat8 const& x) noexcept {
      size_t   i = 0;
      uint64_t c = x.to_int();
      if (!c) {
        return 0;
      }
      uint64_t d = x.to_int();
      uint64_t y = transpose(x).to_int();
      uint64_t z = transpose(x).to_int();

      do {
        d = d >> 8;
        y = y >> 8;
        ++i;
      } while (i < 8 && (d << (8 * i)) == c && (y << (8 * i)) == z);

      return 9 - i;
    }

    std::vector<uint8_t> rows(BMat8 const& x) {
      std::vector<uint8_t> rows;
      push_back_rows(rows, x);
      return rows;
    }

    bool is_regular_element(BMat8 const& x) noexcept {
      return x
                 * bmat8::transpose(BMat8(
                     ~(x * bmat8::transpose(BMat8(~x.to_int())) * x).to_int()))
                 * x
             == x;
    }

    std::vector<bool> to_vector(uint8_t row) {
      std::vector<bool> result;
      uint8_t           mask = 128;
      for (size_t i = 0; i < 8; ++i) {
        result.push_back(row & mask);
        mask >>= 1;
      }
      return result;
    }
  }  // namespace bmat8
}  // namespace libsemigroups
