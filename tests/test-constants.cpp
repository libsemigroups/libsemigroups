//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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

#include "catch.hpp"                    // for REQUIRE, REQUIRE_NOTHROW
#include "libsemigroups/constants.hpp"  // for UNDEFINED, POSITIVE_INFINITY, ...
#include "test-main.hpp"                // for LIBSEMIGROUPS_TEST_CASE

namespace libsemigroups {
  // struct ForTesting {};

  LIBSEMIGROUPS_TEST_CASE("Constants", "001", "Undefined", "[quick]") {
    // operator==
    REQUIRE(UNDEFINED == UNDEFINED);
    REQUIRE(!(UNDEFINED == POSITIVE_INFINITY));
    REQUIRE(!(UNDEFINED == NEGATIVE_INFINITY));
    REQUIRE(!(UNDEFINED == LIMIT_MAX));

    REQUIRE(!(UNDEFINED == 0));
    REQUIRE(!(UNDEFINED == size_t(0)));
    REQUIRE(!(UNDEFINED == int64_t(0)));
    REQUIRE(!(0 == UNDEFINED));
    REQUIRE(!(size_t(0) == UNDEFINED));
    REQUIRE(!(int64_t(0) == UNDEFINED));

    // operator!=
    REQUIRE(!(UNDEFINED != UNDEFINED));
    REQUIRE(UNDEFINED != POSITIVE_INFINITY);
    REQUIRE(UNDEFINED != NEGATIVE_INFINITY);
    REQUIRE(UNDEFINED != LIMIT_MAX);

    REQUIRE(UNDEFINED != 0);
    REQUIRE(UNDEFINED != size_t(0));
    REQUIRE(UNDEFINED != int64_t(0));
    REQUIRE(0 != UNDEFINED);
    REQUIRE(size_t(0) != UNDEFINED);
    REQUIRE(int64_t(0) != UNDEFINED);

    // operator>
    REQUIRE(!(UNDEFINED > UNDEFINED));

    // operator<
    REQUIRE(!(UNDEFINED < UNDEFINED));

    // Doesn't compile (intentionally!)
    // REQUIRE(UNDEFINED < POSITIVE_INFINITY);
    // REQUIRE(POSITIVE_INFINITY < UNDEFINED);
    // REQUIRE(UNDEFINED < NEGATIVE_INFINITY);
    // REQUIRE(NEGATIVE_INFINITY < UNDEFINED);
    // REQUIRE(UNDEFINED < LIMIT_MAX);
    // REQUIRE(LIMIT_MAX < UNDEFINED);

    // Doesn't compile (intentionally!)
    // REQUIRE(ForTesting() == UNDEFINED);
    // REQUIRE(ForTesting() != UNDEFINED);
    // REQUIRE(ForTesting() > UNDEFINED);
    // REQUIRE(ForTesting() < UNDEFINED);
    // REQUIRE(UNDEFINED == ForTesting());
    // REQUIRE(UNDEFINED != ForTesting());
    // REQUIRE(UNDEFINED > ForTesting());
    // REQUIRE(UNDEFINED < ForTesting());
  }

  LIBSEMIGROUPS_TEST_CASE("Constants", "002", "PositiveInfinity", "[quick]") {
    // operator==
    REQUIRE(POSITIVE_INFINITY == POSITIVE_INFINITY);
    REQUIRE(!(POSITIVE_INFINITY == UNDEFINED));
    REQUIRE(!(POSITIVE_INFINITY == NEGATIVE_INFINITY));
    REQUIRE(!(POSITIVE_INFINITY == LIMIT_MAX));

    REQUIRE(!(POSITIVE_INFINITY == 0));
    REQUIRE(!(POSITIVE_INFINITY == size_t(0)));
    REQUIRE(!(POSITIVE_INFINITY == int64_t(0)));
    REQUIRE(!(0 == POSITIVE_INFINITY));
    REQUIRE(!(size_t(0) == POSITIVE_INFINITY));
    REQUIRE(!(int64_t(0) == POSITIVE_INFINITY));

    // operator!=
    REQUIRE(!(POSITIVE_INFINITY != POSITIVE_INFINITY));
    REQUIRE(POSITIVE_INFINITY != UNDEFINED);
    REQUIRE(POSITIVE_INFINITY != NEGATIVE_INFINITY);
    REQUIRE(POSITIVE_INFINITY != LIMIT_MAX);

    REQUIRE(!(POSITIVE_INFINITY == 0));
    REQUIRE(POSITIVE_INFINITY != size_t(0));
    REQUIRE(POSITIVE_INFINITY != int64_t(0));
    REQUIRE(0 != POSITIVE_INFINITY);
    REQUIRE(size_t(0) != POSITIVE_INFINITY);
    REQUIRE(int64_t(0) != POSITIVE_INFINITY);

    // operator<
    REQUIRE(!(POSITIVE_INFINITY < 0));
    REQUIRE(!(POSITIVE_INFINITY < POSITIVE_INFINITY));
    REQUIRE(0 < POSITIVE_INFINITY);
    REQUIRE(1000 < POSITIVE_INFINITY);
    REQUIRE(size_t(0) < POSITIVE_INFINITY);
    REQUIRE(size_t(1000) < POSITIVE_INFINITY);
    REQUIRE(int64_t(0) < POSITIVE_INFINITY);
    REQUIRE(int64_t(1000) < POSITIVE_INFINITY);
    REQUIRE(NEGATIVE_INFINITY < POSITIVE_INFINITY);

    // operator>
    REQUIRE(!(POSITIVE_INFINITY > POSITIVE_INFINITY));
    REQUIRE(POSITIVE_INFINITY > 0);
    REQUIRE(!(0 > POSITIVE_INFINITY));
    REQUIRE(POSITIVE_INFINITY > 100);

    // Doesn't compile (intentionally!)
    // REQUIRE(ForTesting() < POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEST_CASE("Constants", "003", "NegativeInfinity", "[quick]") {
    // operator==
    REQUIRE(NEGATIVE_INFINITY == NEGATIVE_INFINITY);
    REQUIRE(!(NEGATIVE_INFINITY == UNDEFINED));
    REQUIRE(!(NEGATIVE_INFINITY == POSITIVE_INFINITY));
    REQUIRE(!(NEGATIVE_INFINITY == LIMIT_MAX));

    REQUIRE(!(NEGATIVE_INFINITY == 0));
    REQUIRE(!(NEGATIVE_INFINITY == int64_t(0)));
    REQUIRE(!(0 == NEGATIVE_INFINITY));
    REQUIRE(!(int64_t(0) == NEGATIVE_INFINITY));

    // operator!=
    REQUIRE(!(NEGATIVE_INFINITY != NEGATIVE_INFINITY));
    REQUIRE(NEGATIVE_INFINITY != UNDEFINED);
    REQUIRE(NEGATIVE_INFINITY != POSITIVE_INFINITY);
    REQUIRE(NEGATIVE_INFINITY != LIMIT_MAX);

    REQUIRE(NEGATIVE_INFINITY != 0);
    REQUIRE(NEGATIVE_INFINITY != int64_t(0));
    REQUIRE(0 != NEGATIVE_INFINITY);
    REQUIRE(int64_t(0) != NEGATIVE_INFINITY);

    // operator<
    REQUIRE(NEGATIVE_INFINITY < 0);
    REQUIRE(!(NEGATIVE_INFINITY < NEGATIVE_INFINITY));
    REQUIRE(!(0 < NEGATIVE_INFINITY));
    REQUIRE(!(1000 < NEGATIVE_INFINITY));
    REQUIRE(!(int64_t(0) < NEGATIVE_INFINITY));
    REQUIRE(!(int64_t(1000) < NEGATIVE_INFINITY));
    REQUIRE(!(POSITIVE_INFINITY < NEGATIVE_INFINITY));

    // operator>
    REQUIRE(!(NEGATIVE_INFINITY > NEGATIVE_INFINITY));
    REQUIRE(!(NEGATIVE_INFINITY > 0));
    REQUIRE(0 > NEGATIVE_INFINITY);
    REQUIRE(!(NEGATIVE_INFINITY > 100));

    // Don't compile on purpose!
    // REQUIRE(ForTesting() < NEGATIVE_INFINITY);
    // REQUIRE(!(size_t(0) < NEGATIVE_INFINITY));
    // REQUIRE(!(size_t(1000) < NEGATIVE_INFINITY));
    // REQUIRE(NEGATIVE_INFINITY != size_t(0));
    // REQUIRE(size_t(0) != NEGATIVE_INFINITY);
    // REQUIRE(!(size_t(0) == NEGATIVE_INFINITY));
    // REQUIRE(!(NEGATIVE_INFINITY == size_t(0)));
  }

  LIBSEMIGROUPS_TEST_CASE("Constants", "004", "LimitMax", "[quick]") {
    // operator==
    REQUIRE(LIMIT_MAX == LIMIT_MAX);
    REQUIRE(!(LIMIT_MAX == UNDEFINED));
    REQUIRE(!(LIMIT_MAX == POSITIVE_INFINITY));
    REQUIRE(!(LIMIT_MAX == NEGATIVE_INFINITY));

    REQUIRE(!(LIMIT_MAX == -1));
    REQUIRE(!(LIMIT_MAX == int64_t(0)));
    REQUIRE(!(-1 == LIMIT_MAX));
    REQUIRE(!(int64_t(0) == LIMIT_MAX));

    // operator!=
    REQUIRE(!(LIMIT_MAX != LIMIT_MAX));
    REQUIRE(LIMIT_MAX != UNDEFINED);
    REQUIRE(LIMIT_MAX != POSITIVE_INFINITY);
    REQUIRE(LIMIT_MAX != NEGATIVE_INFINITY);

    REQUIRE(LIMIT_MAX != -1);
    REQUIRE(LIMIT_MAX != int64_t(0));
    REQUIRE(-1 != LIMIT_MAX);
    REQUIRE(int64_t(0) != LIMIT_MAX);

    // operator>
    REQUIRE(!(LIMIT_MAX > LIMIT_MAX));
    REQUIRE(!(LIMIT_MAX < LIMIT_MAX));
    REQUIRE_NOTHROW(LIMIT_MAX - 1);
    REQUIRE_NOTHROW(1 - LIMIT_MAX);
    REQUIRE_NOTHROW(LIMIT_MAX > 0);
    REQUIRE_NOTHROW(!(0 > LIMIT_MAX));
    REQUIRE_NOTHROW(0 < LIMIT_MAX);
    REQUIRE_NOTHROW(100 < LIMIT_MAX);
    REQUIRE_NOTHROW(!(0 > LIMIT_MAX));
    REQUIRE_NOTHROW(!(100 > LIMIT_MAX));
  }
}  // namespace libsemigroups
