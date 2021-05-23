//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-21 James D. Mitchell
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

#include "catch.hpp"      // for TEST_CASE
#include "test-main.hpp"  // LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/config.hpp"  // for LIBSEMIGROUPS_SIZEOF_VO...
#include "libsemigroups/types.hpp"   // for SmallestInteger, Smalle...

namespace libsemigroups {
  LIBSEMIGROUPS_TEST_CASE("SmallestInteger", "001", "", "[quick]") {
    REQUIRE(sizeof(SmallestInteger<0>::type) == 1);
    REQUIRE(sizeof(SmallestInteger<255>::type) == 1);
    REQUIRE(sizeof(SmallestInteger<256>::type) == 2);
    REQUIRE(sizeof(SmallestInteger<65535>::type) == 2);
    REQUIRE(sizeof(SmallestInteger<65536>::type) == 4);
#if LIBSEMIGROUPS_SIZEOF_VOID_P == 8
    REQUIRE(sizeof(SmallestInteger<4294967295>::type) == 4);
    REQUIRE(sizeof(SmallestInteger<4294967296>::type) == 8);
#endif
  }

}  // namespace libsemigroups
