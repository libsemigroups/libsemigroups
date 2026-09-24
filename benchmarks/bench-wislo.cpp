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
#include "libsemigroups/order.hpp"       // for lenlex_cmp
#include "libsemigroups/word-range.hpp"  // for number_of_words

#include "libsemigroups/detail/word-iterators.hpp"

#include LIBSEMIGROUPS_CATCH_ALL_HEADER  // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS

namespace libsemigroups {
  namespace {
    // This is the essentially what const_wislo_iterator does in its operator++.
    std::vector<word_type> words_in_shortlex_order(size_t const     n,
                                                   word_type const& first,
                                                   word_type const& last) {
      std::vector<word_type> out;
      out.reserve(number_of_words(n, first.size(), last.size()));
      word_type current = first;

      while (lenlex_cmp(current, last)) {
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
      REQUIRE(std::is_sorted(w.cbegin(), w.cend(), LenLexCmp()));
    };

    BENCHMARK("vector of all words length 0 to 13 using iterators") {
      word_type first = {};
      word_type last(13, 0);

      std::vector<word_type> w;
      w.reserve(number_of_words(3, 0, 13));
      w.assign(cbegin_wislo(3, first, last), cend_wislo(3, first, last));
      REQUIRE(w.size() == number_of_words(3, 0, 13));
      REQUIRE(w.size() == 797161);
      REQUIRE(std::is_sorted(w.cbegin(), w.cend(), LenLexCmp()));
    };
  }

  TEST_CASE("wislo vs wio", "[quick]") {
    using literals:: operator""_w;
    size_t           upper_bound   = 7;
    size_t           alphabet_size = 5;
    word_type const& last          = words::pow("0"_w, upper_bound + 1);

    BENCHMARK("wio + LenLexCmp for length 0 to 7") {
      size_t              count = 0;
      Alphabet<word_type> alphabet(alphabet_size);
      LenLexCmp           cmp(alphabet);
      auto const          wio_end = detail::const_wio_iterator(
          upper_bound, word_type(last), word_type(last), cmp);

      for (auto it = detail::const_wio_iterator(
               upper_bound, ""_w, word_type(last), cmp);
           it != wio_end;
           ++it) {
        count++;
      }
      REQUIRE(count == 97656);
    };

    BENCHMARK("wislo for length 0 to 7") {
      size_t     count     = 0;
      auto const wislo_end = cend_wislo(alphabet_size, ""_w, last);
      for (auto it = cbegin_wislo(alphabet_size, ""_w, last); it != wislo_end;
           ++it) {
        count++;
      }
      REQUIRE(count == 97656);
    };
  }

  TEST_CASE("new WordRange vs old WordRange", "[quick]") {
    using literals::operator""_w;
    size_t          alphabet_size = 5;
    size_t          max_length    = 10;

    BENCHMARK("new WordRange LenLexCmp for length 0 to 10") {
      size_t        count = 0;
      v4::WordRange new_wr;
      new_wr.order(LenLexCmp(Alphabet<word_type>(alphabet_size)))
          .last(word_type(max_length, 0));

      for (auto const& word : new_wr) {
        count++;
      }
      REQUIRE(count == 2441406);
    };
    BENCHMARK("old WordRange LenLexCmp for length 0 to 10") {
      size_t    count = 0;
      WordRange old_wr;
      old_wr.order(Order::lenlex).alphabet_size(alphabet_size).max(max_length);

      for (auto const& word : old_wr) {
        count++;
      }
      REQUIRE(count == 2441406);
    };
  }
}  // namespace libsemigroups
