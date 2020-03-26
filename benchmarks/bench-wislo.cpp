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

#include "libsemigroups/order.hpp"  // for shortlex_compare
#include "libsemigroups/wislo.hpp"  // for cbegin_wislo
#include "libsemigroups/word.hpp"   // for number_of_words

namespace libsemigroups {
  namespace {
    // This is the essentially what const_wislo_iterator does in its operator++.
    std::vector<word_type> words_in_shortlex_order(size_t const     n,
                                                   word_type const& first,
                                                   word_type const& last) {
      std::vector<word_type> out;
      out.reserve(number_of_words(n, first.size(), last.size()));
      word_type current = first;

      while (shortlex_compare(current, last)) {
        out.push_back(current);
        size_t m = current.size();
        while (!current.empty() && ++current.back() == n) {
          current.pop_back();
        }
        current.resize((current.empty() ? m + 1 : m), 0);
      }
      return out;
    }
  }  // namespace

  TEST_CASE("wislo", "[quick]") {
    BENCHMARK("vector of all words length 0 to 13 using for loop 1") {
      word_type first = {};
      word_type last(13, 0);
      auto      w = words_in_shortlex_order(3, first, last);
      REQUIRE(w.size() == number_of_words(3, 0, 13));
      REQUIRE(w.size() == 797161);
      REQUIRE(
          std::is_sorted(w.cbegin(), w.cend(), ShortLexCompare<word_type>()));
    };

    BENCHMARK("vector of all words length 0 to 13 using iterators") {
      word_type first = {};
      word_type last(13, 0);

      std::vector<word_type> w;
      w.reserve(number_of_words(3, 0, 13));
      w.assign(cbegin_wislo(3, first, last), cend_wislo(3, first, last));
      REQUIRE(w.size() == number_of_words(3, 0, 13));
      REQUIRE(w.size() == 797161);
      REQUIRE(
          std::is_sorted(w.cbegin(), w.cend(), ShortLexCompare<word_type>()));
    };
  }
}  // namespace libsemigroups
