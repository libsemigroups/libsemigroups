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

namespace libsemigroups {
  template <typename TIntegralType>
  struct complexity<
      TIntegralType,
      typename std::enable_if<std::is_integral<TIntegralType>::value>::type> {
    size_t operator()(TIntegralType) {
      return 0;
    }
  };

  template <typename TIntegralType>
  struct degree<
      TIntegralType,
      typename std::enable_if<std::is_integral<TIntegralType>::value>::type> {
    size_t operator()(TIntegralType) {
      return 0;
    }
  };

  template <typename TIntegralType>
  struct increase_degree_by<
      TIntegralType,
      typename std::enable_if<std::is_integral<TIntegralType>::value>::type> {
    TIntegralType operator()(TIntegralType x) {
      LIBSEMIGROUPS_ASSERT(false);
      return x;
    }
  };

  template <typename TIntegralType>
  struct less<
      TIntegralType,
      typename std::enable_if<std::is_integral<TIntegralType>::value>::type> {
    bool operator()(TIntegralType x, TIntegralType y) {
      return x < y;
    }
  };

  template <typename TIntegralType>
  struct one<
      TIntegralType,
      typename std::enable_if<std::is_integral<TIntegralType>::value>::type> {
    TIntegralType operator()(TIntegralType) {
      return 1;
    }
  };

  template <typename TIntegralType>
  struct product<
      TIntegralType,
      typename std::enable_if<std::is_integral<TIntegralType>::value>::type> {
    void operator()(TIntegralType& xy,
                    TIntegralType  x,
                    TIntegralType  y,
                    size_t = 0) {
      xy = x * y;
    }
  };

#ifdef LIBSEMIGROUPS_DENSEHASHMAP
  template <> int empty_key(int) {
    return -1;
  }
  template <> uint8_t empty_key(uint8_t) {
    return -1;
  }
#endif
}  // namespace libsemigroups

TEST_CASE("Semigroup of Integers", "[quick][semigroup][integers][finite][01]") {
  Semigroup<int> S({2});
  REQUIRE(S.size() == 32);
  REQUIRE(S.nridempotents() == 1);
  Semigroup<int>::const_iterator it = S.cbegin();
  REQUIRE(*it == 2);

  Semigroup<uint8_t> T({2, 3});
  REQUIRE(T.size() == 130);
  REQUIRE(T.nridempotents() == 2);
  REQUIRE(*T.cbegin_idempotents() == 0);
  REQUIRE(*T.cbegin_idempotents() + 1 == 1);
}
