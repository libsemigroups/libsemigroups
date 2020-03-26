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

#include "bench-main.hpp"  // for LIBSEMIGROUPS_BENCHMARK
#include "catch.hpp"       // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS

#include "libsemigroups/order.hpp"  // for LexicographicalCompare
#include "libsemigroups/siso.hpp"   // for cbegin_siso
#include "libsemigroups/word.hpp"   // for number_of_words

namespace libsemigroups {

  TEST_CASE("siso", "[quick]") {
    BENCHMARK("silo: words length 0 to 13 using iterators") {
      std::string first = "";
      std::string last(13, 'c');

      std::vector<std::string> w;
      w.reserve(number_of_words(3, 0, 13));
      w.assign(cbegin_silo("abc", 13, first, last),
               cend_silo("abc", 13, first, last));
      REQUIRE(w.size() == number_of_words(3, 0, 13));
      REQUIRE(w.size() == 797161);
      REQUIRE(std::is_sorted(
          w.cbegin(), w.cend(), LexicographicalCompare<std::string>()));
    };

    BENCHMARK("sislo: words length 0 to 13 using iterators") {
      std::string first = "";
      std::string last(13, 'a');

      std::vector<std::string> w;
      w.reserve(number_of_words(3, 0, 13));
      w.assign(cbegin_sislo("abc", first, last),
               cend_sislo("abc", first, last));
      REQUIRE(w.size() == number_of_words(3, 0, 13));
      REQUIRE(w.size() == 797161);
      REQUIRE(
          std::is_sorted(w.cbegin(), w.cend(), ShortLexCompare<std::string>()));
    };
  }
}  // namespace libsemigroups
