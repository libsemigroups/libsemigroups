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

#include <cstddef>      // for size_t
#include <type_traits>  // for integral_constant<>::value

#include "adapters.hpp"             // for complexity etc
#include "froidure-pin.hpp"         // for FroidurePin<>::element_i...
#include "libsemigroups-debug.hpp"  // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups.tests.hpp"  // for LIBSEMIGROUPS_TEST_CASE

namespace libsemigroups {
  constexpr bool REPORT = false;

  class IntPair {
   public:
    constexpr IntPair() noexcept : _x(1), _y(1) {}
    IntPair(int x, int y) : _x(x), _y(y) {}

    IntPair operator*(IntPair const& that) const {
      return IntPair(_x * that._x, _y * that._y);
    }

    bool operator==(IntPair const& that) const {
      return _x == that._x && _y == that._y;
    }

    bool operator<(IntPair const& that) const {
      return _x < that._x || (_x == that._x && _y < that._y);
    }

    size_t hash() const {
      return _x * 17 + _y;
    }

   private:
    int _x;
    int _y;
  };

  static_assert(!std::is_trivial<IntPair>::value,
                "IntPair should be non-trivial");

  template <>
  struct complexity<IntPair> {
    constexpr size_t operator()(IntPair) const noexcept {
      return 0;
    }
  };

  template <>
  struct degree<IntPair> {
    constexpr size_t operator()(IntPair) const noexcept {
      return 0;
    }
  };

  template <>
  struct increase_degree_by<IntPair> {
    IntPair operator()(IntPair const& x) const noexcept {
      LIBSEMIGROUPS_ASSERT(false);
      return x;
    }
  };

  template <>
  struct less<IntPair> {
    bool operator()(IntPair x, IntPair y) const noexcept {
      return x < y;
    }
  };

  template <>
  struct one<IntPair> {
    constexpr IntPair operator()(IntPair) const noexcept {
      return IntPair();
    }
  };

  template <>
  struct product<IntPair> {
    void operator()(IntPair& xy, IntPair x, IntPair y, size_t = 0) const
        noexcept {
      xy = x * y;
    }
  };

#ifdef LIBSEMIGROUPS_DENSEHASHMAP
  template <>
  IntPair empty_key(IntPair) {
    return IntPair();
  }
#endif
}  // namespace libsemigroups

namespace std {
  template <>
  struct hash<libsemigroups::IntPair> {
    size_t operator()(libsemigroups::IntPair const& x) const {
      return x.hash();
    }
  };
}  // namespace std

namespace libsemigroups {
  static_assert(!std::is_trivial<IntPair>::value, "IntPair is not non-trivial");

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "108",
                          "(pairs of integers) non-trivial user type",
                          "[quick][froidure-pin][intpairs][108]") {
    FroidurePin<IntPair> S({IntPair(1, 1)});
    REQUIRE(S.size() == 1);
    REQUIRE(S.nr_idempotents() == 1);
  }
}  // namespace libsemigroups
