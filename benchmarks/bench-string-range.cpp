//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-2026 James D. Mitchell
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

#include "libsemigroups/config.hpp"      // for LIBSEMIGROUPS_CATCH_ALL_HEADER
#include "libsemigroups/order.hpp"       // for LexCmp
#include "libsemigroups/ranges.hpp"      // for LexCmp
#include "libsemigroups/word-range.hpp"  // for number_of_words

#include LIBSEMIGROUPS_CATCH_ALL_HEADER  // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS

namespace libsemigroups {

  TEST_CASE("StringRange", "[quick]") {
    BENCHMARK("3-letter alphabet + length 0 to 13 + lex") {
      StringRange strings;
      std::string last(13, 'c');
      strings.alphabet("abc")
          .first("")
          .last(last)
          .order(Order::lex)
          .upper_bound(13);

      REQUIRE(strings.count() == number_of_words(3, 0, 13));
      REQUIRE(strings.count() == 797'161);
      REQUIRE(is_sorted(strings, LexCmp()));
    };

    BENCHMARK("3-letter alphabet + length 0 to 13 + shortlex") {
      StringRange strings;
      std::string last(13, 'c');
      strings.alphabet("abc").first("").last(last).order(Order::lenlex);
      REQUIRE(strings.count() == number_of_words(3, 0, 13));
      REQUIRE(strings.count() == 797'161);
      REQUIRE(is_sorted(strings, LenLexCmp()));
    };
  }
}  // namespace libsemigroups
