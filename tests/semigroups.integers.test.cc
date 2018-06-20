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

#include "../src/bmat8.h"
#include "../src/semigroups.h"
#include "catch.hpp"
#include <iostream>

#define SEMIGROUPS_REPORT false

using namespace libsemigroups;

namespace libsemigroups {
  template <> int one(int) {
    return 1;
  }
  template <> uint8_t one(uint8_t) {
    return 1;
  }
#ifdef LIBSEMIGROUPS_DENSEHASHMAP
  template <> int empty_key(int) {
    return -1;
  }
  template <> uint8_t empty_key(uint8_t) {
    return -1;
  }
#endif
}  // namespace libsemigroups

TEST_CASE("Semigroup of Integers",
          "[quick][semigroup][integers][finite][01]") {
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
