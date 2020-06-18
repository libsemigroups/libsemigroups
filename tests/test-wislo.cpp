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

#include "libsemigroups/order.hpp"  // for ShortLexCompare
#include "libsemigroups/types.hpp"  // for word_type
#include "libsemigroups/wilo.hpp"   // for cbegin_wilo
#include "libsemigroups/wislo.hpp"  // for cbegin_wislo
#include "libsemigroups/word.hpp"   // for number_of_words

namespace libsemigroups {

  LIBSEMIGROUPS_TEST_CASE("wislo",
                          "000",
                          "n: 2, min; 1, max: 4",
                          "[wislo][quick]") {
    word_type first = {0};
    word_type last  = {0, 0, 0, 0};
    auto      w     = std::vector<word_type>(cbegin_wislo(2, first, last),
                                    cend_wislo(2, first, last));
    REQUIRE(w
            == std::vector<word_type>({{0},
                                       {1},
                                       {0, 0},
                                       {0, 1},
                                       {1, 0},
                                       {1, 1},
                                       {0, 0, 0},
                                       {0, 0, 1},
                                       {0, 1, 0},
                                       {0, 1, 1},
                                       {1, 0, 0},
                                       {1, 0, 1},
                                       {1, 1, 0},
                                       {1, 1, 1}}));
    REQUIRE(w.size() == 14);
    REQUIRE(std::is_sorted(w.cbegin(), w.cend(), ShortLexCompare<word_type>()));
  }

  LIBSEMIGROUPS_TEST_CASE("wislo", "001", "corner cases", "[wislo][quick]") {
    word_type first = {0, 0, 0, 0};
    word_type last  = {0, 0, 0, 0, 0};
    auto      w1    = std::vector<word_type>(cbegin_wislo(2, last, first),
                                     cend_wislo(2, last, first));
    REQUIRE(w1.empty());
    auto w2 = std::vector<word_type>(cbegin_wislo(2, last, last),
                                     cend_wislo(2, last, last));
    REQUIRE(w2.empty());

    auto w3 = std::vector<word_type>(cbegin_wislo(2, last, word_type(6, 0)),
                                     cend_wislo(2, last, word_type(6, 0)));
    REQUIRE(w3.size() == number_of_words(2, 5, 6));
    REQUIRE(w3.size() == 32);
    REQUIRE(w3
            == std::vector<word_type>(
                {{0, 0, 0, 0, 0}, {0, 0, 0, 0, 1}, {0, 0, 0, 1, 0},
                 {0, 0, 0, 1, 1}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 1},
                 {0, 0, 1, 1, 0}, {0, 0, 1, 1, 1}, {0, 1, 0, 0, 0},
                 {0, 1, 0, 0, 1}, {0, 1, 0, 1, 0}, {0, 1, 0, 1, 1},
                 {0, 1, 1, 0, 0}, {0, 1, 1, 0, 1}, {0, 1, 1, 1, 0},
                 {0, 1, 1, 1, 1}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 1},
                 {1, 0, 0, 1, 0}, {1, 0, 0, 1, 1}, {1, 0, 1, 0, 0},
                 {1, 0, 1, 0, 1}, {1, 0, 1, 1, 0}, {1, 0, 1, 1, 1},
                 {1, 1, 0, 0, 0}, {1, 1, 0, 0, 1}, {1, 1, 0, 1, 0},
                 {1, 1, 0, 1, 1}, {1, 1, 1, 0, 0}, {1, 1, 1, 0, 1},
                 {1, 1, 1, 1, 0}, {1, 1, 1, 1, 1}}));
    REQUIRE(
        std::is_sorted(w3.cbegin(), w3.cend(), ShortLexCompare<word_type>()));
    REQUIRE(std::is_sorted(
        w3.cbegin(), w3.cend(), LexicographicalCompare<word_type>()));
  }

  LIBSEMIGROUPS_TEST_CASE("wislo",
                          "002",
                          "n: 3, min; 0, max: 10",
                          "[wislo][quick]") {
    word_type first = {};
    word_type last(10, 0);
    auto      w = std::vector<word_type>(cbegin_wislo(3, first, last),
                                    cend_wislo(3, first, last));
    REQUIRE(w.size() == number_of_words(3, 0, 10));
    REQUIRE(w.size() == 29524);
    REQUIRE(std::is_sorted(w.cbegin(), w.cend(), ShortLexCompare<word_type>()));
  }

  LIBSEMIGROUPS_TEST_CASE("wislo",
                          "003",
                          "n: 3, min; 0, max: 13",
                          "[wislo][quick][no-valgrind]") {
    word_type              first = {};
    word_type              last(13, 0);
    std::vector<word_type> w;
    w.reserve(number_of_words(3, 0, 13));
    w.insert(w.end(), cbegin_wislo(3, first, last), cend_wislo(3, first, last));
    REQUIRE(w.size() == number_of_words(3, 0, 13));
    REQUIRE(w.size() == 797161);
    REQUIRE(std::is_sorted(w.cbegin(), w.cend(), ShortLexCompare<word_type>()));
  }

  LIBSEMIGROUPS_TEST_CASE("wislo",
                          "004",
                          "lex + sort",
                          "[wislo][quick][no-valgrind]") {
    word_type first = {};
    word_type last(13, 2);
    auto      w = std::vector<word_type>(cbegin_wilo(3, 13, first, last),
                                    cend_wilo(3, 13, first, last));
    std::sort(w.begin(), w.end(), ShortLexCompare<word_type>());
    REQUIRE(w.size() == number_of_words(3, 0, 13));
    REQUIRE(w.size() == 797161);
    REQUIRE(std::is_sorted(w.cbegin(), w.cend(), ShortLexCompare<word_type>()));
  }

  LIBSEMIGROUPS_TEST_CASE("wislo", "005", "code coverage", "[wislo][quick]") {
    word_type first = {0, 0, 0};
    word_type last  = {0, 0, 0, 0};

    const_wislo_iterator it;
    it = cbegin_wislo(2, first, last);
    REQUIRE(*it == word_type({0, 0, 0}));
    REQUIRE(it->size() == 3);
    REQUIRE(*it++ == word_type({0, 0, 0}));
    REQUIRE(*it == word_type({0, 0, 1}));

    auto it2 = it;
    REQUIRE(it == it2);
    it++;
    REQUIRE(it2 != it);
    REQUIRE(*it == word_type({0, 1, 0}));
    REQUIRE(*it2 == word_type({0, 0, 1}));

    swap(it, it2);
    REQUIRE(it2 != it);
    REQUIRE(*it2 == word_type({0, 1, 0}));
    REQUIRE(*it == word_type({0, 0, 1}));

    std::swap(it, it2);
    REQUIRE(it2 != it);
    REQUIRE(*it == word_type({0, 1, 0}));
    REQUIRE(*it2 == word_type({0, 0, 1}));
    it2++;
    REQUIRE(it == it2);
    REQUIRE(it++ == it2++);
    REQUIRE(it == it2);
    REQUIRE(++it == ++it2);

    const_wislo_iterator it3(cbegin_wislo(2, first, last));
    it3 = cbegin_wislo(2, first, last);
    REQUIRE(*it3 == word_type({0, 0, 0}));
    REQUIRE(it->size() == 3);
    REQUIRE(*it3++ == word_type({0, 0, 0}));
    REQUIRE(*it3 == word_type({0, 0, 1}));
  }

}  // namespace libsemigroups
