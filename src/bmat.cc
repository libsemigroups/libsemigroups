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

// This file contains an implementation of fast boolean matrices up to
// dimension 8 x 8.

#include "bmat.h"

#include <random>
#include <string>
#include <vector>

namespace libsemigroups {
  static_assert(std::is_trivial<BMat8>(), "BMat8 is not a trivial class!");
  std::vector<uint64_t> const BMat8::ROW_MASK = {0xff00000000000000,
                                                 0xff000000000000,
                                                 0xff0000000000,
                                                 0xff00000000,
                                                 0xff000000,
                                                 0xff0000,
                                                 0xff00,
                                                 0xff};

  std::vector<uint64_t> const BMat8::COL_MASK = {0x8080808080808080,
                                                 0x4040404040404040,
                                                 0x2020202020202020,
                                                 0x1010101010101010,
                                                 0x808080808080808,
                                                 0x404040404040404,
                                                 0x202020202020202,
                                                 0x101010101010101};

  std::vector<uint64_t> const BMat8::BIT_MASK = {
      0x8000000000000000,
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

  std::random_device BMat8::_rd;
  std::mt19937                          BMat8::_gen(_rd());
  std::uniform_int_distribution<size_t> BMat8::_dist(0, 0xffffffffffffffff);

  BMat8::BMat8(std::vector<std::vector<size_t>> const& mat) {
    LIBSEMIGROUPS_ASSERT(mat.size() <= 8);
    LIBSEMIGROUPS_ASSERT(0 < mat.size());
    _data        = 0;
    uint64_t pow = 1;
    pow          = pow << 63;
    for (auto row : mat) {
      LIBSEMIGROUPS_ASSERT(row.size() == mat.size());
      for (auto entry : row) {
        if (entry) {
          _data ^= pow;
        }
        pow = pow >> 1;
      }
      pow = pow >> (8 - mat.size());
    }
  }

  BMat8 BMat8::random() {
    return BMat8(_dist(_gen));
  }

  BMat8 BMat8::random(size_t const dim) {
    LIBSEMIGROUPS_ASSERT(0 < dim && dim <= 8);
    BMat8 bm = BMat8::random();
    for (size_t i = dim + 1; i < 8; ++i) {
      bm._data &= ~ROW_MASK[i];
      bm._data &= ~COL_MASK[i];
    }
    return bm;
  }
}  // namespace libsemigroups
