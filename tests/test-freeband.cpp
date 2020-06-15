//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020 James D. Mitchell
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

#include "catch.hpp"      // for REQUIRE etc
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/freeband.hpp"
#include "libsemigroups/constants.hpp"

namespace libsemigroups {

  LIBSEMIGROUPS_TEST_CASE("freeband_equal_to", "001", "", "[freeband][quick]") {
    REQUIRE(right({0, 0, 0, 0, 1, 1, 0, 0, 2}, 2)
            == word_type({7, 7, 7, 7, 7, 7, 8, 8, UNDEFINED}));

    REQUIRE(right({0, 0, 0, 0, 1, 1, 0, 0, 2}, 3)
            == word_type({8, 8, 8, 8, 8, 8, UNDEFINED, UNDEFINED, UNDEFINED}));

    REQUIRE(right({0, 0, 0, 1, 2, 1, 2, 1, 3, 1, 2, 1, 0, 1, 2, 3}, 2) ==
            word_type({3, 3, 3, 7, 7, 7, 7, 9, 9, 11, 11, 13, 13, 14, 15, UNDEFINED}));
  }
}  // namespace libsemigroups
