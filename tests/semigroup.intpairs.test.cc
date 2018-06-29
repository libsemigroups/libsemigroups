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

#include "catch.hpp"
#include "src/semigroup.h"

#define SEMIGROUPS_REPORT false

using namespace libsemigroups;

class IntPair {
 public:
  IntPair() : _x(0), _y(0) {}
  IntPair(int re, int im) : _x(re), _y(im) {}

  IntPair operator*(IntPair const& that) const {
    return IntPair(_x * that._x, _y * that._y);
  }

  bool operator==(IntPair const& that) const {
    return _x == that._x && _y == that._y;
  }

  bool operator<(IntPair const& that) const {
    return _x < that._x || (_x == that._x && _y < that._y);
  }

  IntPair one() const {
    return IntPair(1, 1);
  }

  size_t hash() const {
    return _x * 17 + _y;
  }

 private:
  int _x;
  int _y;
};

namespace libsemigroups {
#ifdef LIBSEMIGROUPS_DENSEHASHMAP
  template <> IntPair empty_key(IntPair) {
    return IntPair();
  }
#endif
}  // namespace libsemigroups

namespace std {
  template <> struct hash<IntPair> {
    size_t operator()(IntPair const& x) const {
      return x.hash();
    }
  };
}  // namespace std

static_assert(!std::is_trivial<IntPair>::value, "IntPair is not non-trivial");

TEST_CASE("Semigroup of IntPairs (non-trivial user type)",
          "[quick][semigroup][nontrivial][finite][097]") {
  Semigroup<IntPair> S({IntPair(1, 1)});
  REQUIRE(S.size() == 1);
  REQUIRE(S.nridempotents() == 1);
}
