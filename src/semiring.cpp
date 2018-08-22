//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 James D. Mitchell
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

#include "semiring.hpp"

#include <algorithm>  // for min, max

#include "internal/libsemigroups-debug.hpp"  // for LIBSEMIGROUPS_ASSERT

#include "constants.hpp"  // for POSITIVE_INFINITY

namespace libsemigroups {

  BooleanSemiring::BooleanSemiring() : Semiring() {}

  bool BooleanSemiring::one() const {
    return true;
  }

  bool BooleanSemiring::zero() const {
    return false;
  }

  bool BooleanSemiring::prod(bool x, bool y) const {
    return x && y;
  }

  bool BooleanSemiring::plus(bool x, bool y) const {
    return x || y;
  }

  Integers::Integers() : Semiring<int64_t>() {}

  int64_t Integers::one() const {
    return 1;
  }

  int64_t Integers::zero() const {
    return 0;
  }

  int64_t Integers::prod(int64_t x, int64_t y) const {
    return x * y;
  }

  int64_t Integers::plus(int64_t x, int64_t y) const {
    return x + y;
  }

  MaxPlusSemiring::MaxPlusSemiring() : Semiring<int64_t>() {}

  int64_t MaxPlusSemiring::one() const {
    return 0;
  }

  int64_t MaxPlusSemiring::zero() const {
    return NEGATIVE_INFINITY;
  }

  int64_t MaxPlusSemiring::prod(int64_t x, int64_t y) const {
    if (x == NEGATIVE_INFINITY || y == NEGATIVE_INFINITY) {
      return NEGATIVE_INFINITY;
    }
    return x + y;  // TODO(later) could define addition of NEGATIVE_INFINITY
  }

  int64_t MaxPlusSemiring::plus(int64_t x, int64_t y) const {
    if (x == NEGATIVE_INFINITY) {
      return y;
    } else if (y == NEGATIVE_INFINITY) {
      return x;
    }
    return std::max(x, y);
  }

  MinPlusSemiring::MinPlusSemiring() : Semiring<int64_t>() {}

  int64_t MinPlusSemiring::one() const {
    return 0;
  }

  int64_t MinPlusSemiring::zero() const {
    return POSITIVE_INFINITY;
  }

  int64_t MinPlusSemiring::prod(int64_t x, int64_t y) const {
    if (x == POSITIVE_INFINITY || y == POSITIVE_INFINITY) {
      return POSITIVE_INFINITY;
    }
    return x + y;
  }

  int64_t MinPlusSemiring::plus(int64_t x, int64_t y) const {
    if (x == POSITIVE_INFINITY) {
      return y;
    } else if (y == POSITIVE_INFINITY) {
      return x;
    }
    return std::min(x, y);
  }

  SemiringWithThreshold::SemiringWithThreshold(int64_t threshold)
      : Semiring<int64_t>(), _threshold(threshold) {}

  int64_t SemiringWithThreshold::threshold() const {
    return _threshold;
  }

  TropicalMaxPlusSemiring::TropicalMaxPlusSemiring(int64_t threshold)
      : SemiringWithThreshold(threshold) {}

  int64_t TropicalMaxPlusSemiring::one() const {
    return 0;
  }

  int64_t TropicalMaxPlusSemiring::zero() const {
    return NEGATIVE_INFINITY;
  }

  int64_t TropicalMaxPlusSemiring::prod(int64_t x, int64_t y) const {
    LIBSEMIGROUPS_ASSERT((x >= 0 && x <= this->threshold())
                         || x == NEGATIVE_INFINITY);
    LIBSEMIGROUPS_ASSERT((y >= 0 && y <= this->threshold())
                         || y == NEGATIVE_INFINITY);
    if (x == NEGATIVE_INFINITY || y == NEGATIVE_INFINITY) {
      return NEGATIVE_INFINITY;
    }
    return std::min(x + y, threshold());
  }

  int64_t TropicalMaxPlusSemiring::plus(int64_t x, int64_t y) const {
    LIBSEMIGROUPS_ASSERT((x >= 0 && x <= this->threshold())
                         || x == NEGATIVE_INFINITY);
    LIBSEMIGROUPS_ASSERT((y >= 0 && y <= this->threshold())
                         || y == NEGATIVE_INFINITY);
    if (x == NEGATIVE_INFINITY) {
      return y;
    } else if (y == NEGATIVE_INFINITY) {
      return x;
    }
    return std::max(x, y);
  }

  bool TropicalMaxPlusSemiring::contains(int64_t x) const {
    return ((x >= 0 && x <= this->threshold()) || x == NEGATIVE_INFINITY);
  }

  TropicalMinPlusSemiring::TropicalMinPlusSemiring(int64_t threshold)
      : SemiringWithThreshold(threshold) {}

  int64_t TropicalMinPlusSemiring::one() const {
    return 0;
  }

  int64_t TropicalMinPlusSemiring::zero() const {
    return POSITIVE_INFINITY;
  }

  int64_t TropicalMinPlusSemiring::prod(int64_t x, int64_t y) const {
    LIBSEMIGROUPS_ASSERT((x >= 0 && x <= this->threshold())
                         || x == POSITIVE_INFINITY);
    LIBSEMIGROUPS_ASSERT((y >= 0 && y <= this->threshold())
                         || y == POSITIVE_INFINITY);
    if (x == POSITIVE_INFINITY || y == POSITIVE_INFINITY) {
      return POSITIVE_INFINITY;
    }
    return std::min(x + y, threshold());
  }

  int64_t TropicalMinPlusSemiring::plus(int64_t x, int64_t y) const {
    LIBSEMIGROUPS_ASSERT((x >= 0 && x <= this->threshold())
                         || x == POSITIVE_INFINITY);
    LIBSEMIGROUPS_ASSERT((y >= 0 && y <= this->threshold())
                         || y == POSITIVE_INFINITY);
    if (x == POSITIVE_INFINITY) {
      return y;
    } else if (y == POSITIVE_INFINITY) {
      return x;
    }
    return std::min(x, y);
  }

  bool TropicalMinPlusSemiring::contains(int64_t x) const {
    return ((x >= 0 && x <= this->threshold()) || x == POSITIVE_INFINITY);
  }

  NaturalSemiring::NaturalSemiring(int64_t t, int64_t p)
      : SemiringWithThreshold(t), _period(p) {
    LIBSEMIGROUPS_ASSERT(_period > 0);
    LIBSEMIGROUPS_ASSERT(this->threshold() >= 0);
  }

  int64_t NaturalSemiring::one() const {
    return 1;
  }

  int64_t NaturalSemiring::zero() const {
    return 0;
  }

  int64_t NaturalSemiring::prod(int64_t x, int64_t y) const {
    LIBSEMIGROUPS_ASSERT(x >= 0 && x <= _period + this->threshold() - 1);
    LIBSEMIGROUPS_ASSERT(y >= 0 && y <= _period + this->threshold() - 1);
    return thresholdperiod(x * y);
  }

  int64_t NaturalSemiring::plus(int64_t x, int64_t y) const {
    LIBSEMIGROUPS_ASSERT(x >= 0 && x <= _period + this->threshold() - 1);
    LIBSEMIGROUPS_ASSERT(y >= 0 && y <= _period + this->threshold() - 1);
    return thresholdperiod(x + y);
  }

  int64_t NaturalSemiring::period() const {
    return _period;
  }

  bool NaturalSemiring::contains(int64_t x) const {
    return x >= 0 && x < _period + this->threshold();
  }

  int64_t NaturalSemiring::thresholdperiod(int64_t x) const {
    int64_t threshold = this->threshold();
    if (x > threshold) {
      return threshold + (x - threshold) % _period;
    }
    return x;
  }

}  // namespace libsemigroups
