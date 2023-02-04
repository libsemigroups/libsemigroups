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

#include "libsemigroups/types.hpp"  // for word_type
#include "libsemigroups/word.hpp"   // for number_of_words

namespace libsemigroups {
  LIBSEMIGROUPS_TEST_CASE("number_of_words", "001", "", "[quick]") {
    REQUIRE(39 == number_of_words(3, 1, 4));
    REQUIRE(29524 == number_of_words(3, 0, 10));
    REQUIRE(32 == number_of_words(2, 5, 6));
    REQUIRE(797161 == number_of_words(3, 0, 13));
    REQUIRE(number_of_words(2, 4, 1) == 0);
    REQUIRE(number_of_words(2, 4, 4) == 0);
    REQUIRE(number_of_words(2, 4, 2) == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("operator\"\" _w", "003", "literal", "[quick]") {
    using namespace literals;
    REQUIRE(0120210_w == word_type({0, 1, 2, 0, 2, 1, 0}));
    REQUIRE(0_w == word_type({0}));
    REQUIRE(1_w == word_type({1}));
    REQUIRE(2_w == word_type({2}));
    // Require ""s to avoid interpretation as bad octal.
    REQUIRE("08"_w == word_type({0, 8}));
  }

}  // namespace libsemigroups
