// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 Finn Smith
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

#include <cstddef>  // for size_t

#include "catch.hpp"  // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS
#include "libsemigroups/forest.hpp"  // for Forest
#include "test-main.hpp"             // for LIBSEMIGROUPS_TEST_CASE

namespace libsemigroups {
  struct LibsemigroupsException;

  LIBSEMIGROUPS_TEST_CASE("Forest", "001", "test forest", "[quick]") {
    Forest forest(100);
    REQUIRE(forest.number_of_nodes() == 100);
    for (size_t i = 1; i < 100; ++i) {
      forest.set(i, i - 1, i * i % 7);
    }
    REQUIRE_THROWS_AS(forest.set(0, -1, 0), LibsemigroupsException);

    for (size_t i = 1; i < 100; ++i) {
      REQUIRE(forest.label(i) == i * i % 7);
      REQUIRE(forest.parent(i) == i - 1);
    }
    REQUIRE(
        std::vector<size_t>(forest.cbegin_parent(), forest.cend_parent())
        == std::vector<size_t>(
            {UNDEFINED, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13,
             14,        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28,
             29,        30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43,
             44,        45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58,
             59,        60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73,
             74,        75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88,
             89,        90, 91, 92, 93, 94, 95, 96, 97, 98}));
    REQUIRE_NOTHROW(forest.clear());
    REQUIRE(forest.number_of_nodes() == 0);
    REQUIRE_NOTHROW(forest.add_nodes(10));
    REQUIRE(forest.number_of_nodes() == 10);
  }
}  // namespace libsemigroups
