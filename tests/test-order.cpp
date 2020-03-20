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

#include "libsemigroups/order.hpp"  // for words
#include "libsemigroups/types.hpp"  // for word_type

namespace libsemigroups {
  LIBSEMIGROUPS_TEST_CASE("number_of_words", "001", "corner cases", "[quick]") {
    REQUIRE(1 - std::pow(3, 4) / (1 - 4) == 0);
    REQUIRE(39 == number_of_words(3, 1, 4));
    REQUIRE(29524 == number_of_words(3, 0, 10));
    REQUIRE(32 == number_of_words(2, 5, 6));
    REQUIRE(797161 == number_of_words(3, 0, 13));
    REQUIRE(number_of_words(2, 4, 1) == 0);
    REQUIRE(number_of_words(2, 4, 4) == 0);
    REQUIRE(number_of_words(2, 4, 2) == 0);
  }

}  // namespace libsemigroups
