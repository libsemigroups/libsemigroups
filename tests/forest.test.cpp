// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 Finn Smith
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

#include "forest.hpp"
#include "libsemigroups.tests.hpp"  // for LIBSEMIGROUPS_TEST_CASE
#include <stddef.h>                 // for size_t

namespace libsemigroups {
  LIBSEMIGROUPS_TEST_CASE("Forest", "001", "test forest", "[quick]") {
    Forest forest(100);
    REQUIRE(forest.size() == 100);
    for (size_t i = 0; i < 100; ++i) {
      forest.set(i, i - 1, i * i % 7);
    }
    for (size_t i = 0; i < 100; ++i) {
      REQUIRE(forest.label(i) == i * i % 7);
      REQUIRE(forest.parent(i) == i - 1);
    }
  }
}  // namespace libsemigroups
