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

#include "libsemigroups/order.hpp"  // for lexicographical_compare
#include "libsemigroups/wilo.hpp"   // for cbegin_wilo
#include "libsemigroups/word.hpp"   // for number_of_words

namespace libsemigroups {
  namespace {
    // This is the initial version of the code
    std::vector<word_type> words_in_lex_order(size_t const n,
                                              size_t const min,
                                              size_t const max) {
      std::vector<word_type> out;
      out.reserve(number_of_words(n, min, max));
      if (max <= min) {
        return out;
      } else if (min == 0) {
        out.push_back({});
      }

      word_type   next;
      letter_type b = 0;
    begin:
      if (next.size() < max - 1 && b != n) {
        next.push_back(b);
        b = 0;
        if (next.size() >= min) {
          out.push_back(next);
        }
        goto begin;
      } else if (!next.empty()) {
        b = next.back() + 1;
        next.pop_back();
        goto begin;
      }
      return out;
    }

    // This is the essentially what const_wilo_iterator does in its operator++.
    std::vector<word_type> words_in_lex_order2(size_t const     n,
                                               size_t const     upper_bound,
                                               word_type const& first,
                                               word_type const& last) {
      std::vector<word_type> out;
      out.reserve(number_of_words(n, first.size(), last.size()));
      if (!lexicographical_compare(first, last)) {
        return out;
      } else if (first.empty()) {
        out.push_back({});
      }

      word_type   current;
      letter_type letter = 0;
    begin:
      if (current.size() < upper_bound - 1 && letter != n) {
        current.push_back(letter);
        letter = 0;
        if (lexicographical_compare(current, last)) {
          out.push_back(current);
        }
        goto begin;
      } else if (!current.empty()) {
        letter = ++current.back();
        current.pop_back();
        goto begin;
      }
      return out;
    }
  }  // namespace

  TEST_CASE("wilo", "[quick]") {
    BENCHMARK("vector of all words length 0 to 13 using for loop 1") {
      auto w = words_in_lex_order(3, 0, 13);
      REQUIRE(w.size() == number_of_words(3, 0, 13));
      REQUIRE(w.size() == 797161);
      REQUIRE(std::is_sorted(
          w.cbegin(), w.cend(), LexicographicalCompare<word_type>()));
    };

    BENCHMARK("vector of all words length 0 to 13 using for loop 2") {
      word_type first = {};
      word_type last(13, 2);
      auto      w = words_in_lex_order2(3, 13, first, last);
      REQUIRE(w.size() == number_of_words(3, 0, 13));
      REQUIRE(w.size() == 797161);
      REQUIRE(std::is_sorted(
          w.cbegin(), w.cend(), LexicographicalCompare<word_type>()));
    };

    BENCHMARK("vector of all words length 0 to 13 using iterators") {
      word_type first = {};
      word_type last(13, 2);

      std::vector<word_type> w;
      w.reserve(number_of_words(3, 0, 13));
      w.assign(cbegin_wilo(3, 13, first, last), cend_wilo(3, 13, first, last));
      REQUIRE(w.size() == number_of_words(3, 0, 13));
      REQUIRE(w.size() == 797161);
      REQUIRE(std::is_sorted(
          w.cbegin(), w.cend(), LexicographicalCompare<word_type>()));
    };
  }
}  // namespace libsemigroups
