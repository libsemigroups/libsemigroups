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

#include <vector>  // for vector

#include "catch.hpp"      // for REQUIRE etc
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/order.hpp"  // for LexicographicalCompare
#include "libsemigroups/types.hpp"  // for word_type
#include "libsemigroups/wilo.hpp"   // for cbegin_wilo
#include "libsemigroups/word.hpp"   // for number_of_words

namespace libsemigroups {

  LIBSEMIGROUPS_TEST_CASE("wilo", "001", "corner cases", "[wilo][quick]") {
    word_type const u  = {0, 0, 0, 0};
    word_type const v  = {1, 1, 1, 1};
    auto            w1 = std::vector<word_type>(cbegin_wilo(2, 1, v, u),
                                     cend_wilo(2, 1, v, u));
    REQUIRE(w1.empty());
    auto w2 = std::vector<word_type>(cbegin_wilo(2, 1, u, u),
                                     cend_wilo(2, 1, u, u));
    REQUIRE(w2.empty());
    auto w3 = std::vector<word_type>(cbegin_wilo(2, 2, {}, {1, 1}),
                                     cend_wilo(2, 2, {}, {1, 1}));
    REQUIRE(w3.size() == 3);
    REQUIRE(w3 == std::vector<word_type>({{}, {0}, {1}}));
    REQUIRE(word_type(0, 0) == word_type());
    auto w4 = std::vector<word_type>(cbegin_wilo(2, 1, {}, {1, 1}),
                                     cend_wilo(2, 1, {}, {1, 1}));
    REQUIRE(w4 == std::vector<word_type>({{}}));
    w4 = std::vector<word_type>(cbegin_wilo(2, 1, {}, {0}),
                                cend_wilo(2, 1, {}, {0}));
    REQUIRE(w4 == std::vector<word_type>({{}}));
  }

  LIBSEMIGROUPS_TEST_CASE("wilo",
                          "002",
                          "letters: 2, min: 1, max: 4",
                          "[wilo][quick]") {
    auto w = std::vector<word_type>(cbegin_wilo(2, 4, {0}, {1, 1, 1, 1}),
                                    cend_wilo(2, 4, {0}, {1, 1, 1, 1}));
    REQUIRE(w
            == std::vector<word_type>({{0},
                                       {0, 0},
                                       {0, 0, 0},
                                       {0, 0, 1},
                                       {0, 1},
                                       {0, 1, 0},
                                       {0, 1, 1},
                                       {1},
                                       {1, 0},
                                       {1, 0, 0},
                                       {1, 0, 1},
                                       {1, 1},
                                       {1, 1, 0},
                                       {1, 1, 1}}));
    REQUIRE(w.size() == 14);
    REQUIRE(std::is_sorted(
        w.cbegin(), w.cend(), LexicographicalCompare<word_type>()));
    REQUIRE(std::is_sorted(cbegin_wilo(2, 4, {0}, {1, 1, 1, 1}),
                           cend_wilo(2, 4, {0}, {1, 1, 1, 1}),
                           LexicographicalCompare<word_type>()));
  }

  LIBSEMIGROUPS_TEST_CASE("wilo",
                          "003",
                          "letters: 2, min: 1, max: 4",
                          "[wilo][quick]") {
    word_type first = {0};
    word_type last  = {2, 2, 2, 2};
    auto      w     = std::vector<word_type>(cbegin_wilo(3, 4, first, last),
                                    cend_wilo(3, 4, first, last));
    REQUIRE(w.size() == 39);
    REQUIRE(w.size() == number_of_words(3, 1, 4));
    REQUIRE(
        w
        == std::vector<word_type>(
            {{0},       {0, 0},    {0, 0, 0}, {0, 0, 1}, {0, 0, 2}, {0, 1},
             {0, 1, 0}, {0, 1, 1}, {0, 1, 2}, {0, 2},    {0, 2, 0}, {0, 2, 1},
             {0, 2, 2}, {1},       {1, 0},    {1, 0, 0}, {1, 0, 1}, {1, 0, 2},
             {1, 1},    {1, 1, 0}, {1, 1, 1}, {1, 1, 2}, {1, 2},    {1, 2, 0},
             {1, 2, 1}, {1, 2, 2}, {2},       {2, 0},    {2, 0, 0}, {2, 0, 1},
             {2, 0, 2}, {2, 1},    {2, 1, 0}, {2, 1, 1}, {2, 1, 2}, {2, 2},
             {2, 2, 0}, {2, 2, 1}, {2, 2, 2}}));
    REQUIRE(std::is_sorted(
        w.cbegin(), w.cend(), LexicographicalCompare<word_type>()));
    REQUIRE(std::is_sorted(cbegin_wilo(3, 4, first, last),
                           cend_wilo(3, 4, first, last),
                           LexicographicalCompare<word_type>()));
  }

  LIBSEMIGROUPS_TEST_CASE("wilo",
                          "004",
                          "letters: 3, min: 0, max: 10",
                          "[wilo][quick]") {
    word_type first = {};
    word_type last(10, 2);
    auto      w = std::vector<word_type>(cbegin_wilo(3, 10, first, last),
                                    cend_wilo(3, 10, first, last));
    REQUIRE(w.size() == number_of_words(3, 0, 10));
    REQUIRE(w.size() == 29524);
    REQUIRE(std::is_sorted(
        w.cbegin(), w.cend(), LexicographicalCompare<word_type>()));
    REQUIRE(std::is_sorted(cbegin_wilo(3, 10, first, last),
                           cend_wilo(3, 10, first, last),
                           LexicographicalCompare<word_type>()));
  }

  LIBSEMIGROUPS_TEST_CASE("wilo",
                          "005",
                          "letters: 3, min: 0, max: 13",
                          "[wilo][quick][no-valgrind]") {
    word_type first = {};
    word_type last(13, 2);

    std::vector<word_type> w(cbegin_wilo(3, 13, first, last),
                             cend_wilo(3, 13, first, last));
    REQUIRE(w.size() == 797161);
    REQUIRE(w.size() == number_of_words(3, 0, 13));
    REQUIRE(std::is_sorted(
        w.cbegin(), w.cend(), LexicographicalCompare<word_type>()));
  }

  LIBSEMIGROUPS_TEST_CASE("wilo",
                          "006",
                          "forward iterator requirements",
                          "[wilo][quick][no-valgrind]") {
    word_type first = {};
    word_type last(4, 1);
    auto      it = cbegin_wilo(2, 4, first, last);
    REQUIRE(*it == word_type());
    ++it;
    REQUIRE(*it == word_type({0}));

    first = {0};
    last  = word_type(13, 2);
    std::vector<word_type> w;
    w.reserve(number_of_words(3, 1, 13));
    w.insert(w.end(),
             cbegin_wilo(3, 13, first, last),
             cend_wilo(3, 13, first, last));
    REQUIRE(w.size() == number_of_words(3, 1, 13));
    REQUIRE(std::is_sorted(
        w.cbegin(), w.cend(), LexicographicalCompare<word_type>()));
  }

  LIBSEMIGROUPS_TEST_CASE("wilo", "007", "more corner cases", "[wilo][quick]") {
    word_type first = {};
    word_type last(10, 0);
    auto      w = std::vector<word_type>(cbegin_wilo(1, 10, first, last),
                                    cend_wilo(1, 10, first, last));
    REQUIRE(w.size() == 10);
    REQUIRE(w
            == std::vector<word_type>({{},
                                       {0},
                                       {0, 0},
                                       {0, 0, 0},
                                       {0, 0, 0, 0},
                                       {0, 0, 0, 0, 0},
                                       {0, 0, 0, 0, 0, 0},
                                       {0, 0, 0, 0, 0, 0, 0},
                                       {0, 0, 0, 0, 0, 0, 0, 0},
                                       {0, 0, 0, 0, 0, 0, 0, 0, 0}}));
    w = std::vector<word_type>(cbegin_wilo(0, 0, first, last),
                               cend_wilo(0, 0, first, last));
    REQUIRE(w.size() == 0);
    REQUIRE(w.empty());
    first = {0, 0};
    last  = {0, 0, 0, 0};
    w     = std::vector<word_type>(cbegin_wilo(1, 4, first, last),
                               cend_wilo(1, 4, first, last));
    REQUIRE(w == std::vector<word_type>({{0, 0}, {0, 0, 0}}));
    w = std::vector<word_type>(cbegin_wilo(1, 1, first, last),
                               cend_wilo(1, 1, first, last));
    REQUIRE(w == std::vector<word_type>());
    w = std::vector<word_type>(cbegin_wilo(1, 5, {0}, {1}),
                               cend_wilo(1, 5, {0}, {1}));
    REQUIRE(w
            == std::vector<word_type>({{0}, {0, 0}, {0, 0, 0}, {0, 0, 0, 0}}));
  }

  LIBSEMIGROUPS_TEST_CASE("wilo",
                          "008",
                          "starting at a given word 1",
                          "[wilo][quick]") {
    word_type first = {0, 1, 2, 3};
    word_type last(5, 4);
    auto      result   = std::vector<word_type>(cbegin_wilo(4, 5, first, last),
                                         cend_wilo(4, 5, first, last));
    auto      expected = std::vector<word_type>(
        std::find(
            cbegin_wilo(4, 5, {}, last), cend_wilo(4, 5, {}, last), first),
        cend_wilo(4, 5, {}, last));

    REQUIRE(result.size() == expected.size());
    REQUIRE(result.size() == 303);
    REQUIRE(result == expected);
  }

  LIBSEMIGROUPS_TEST_CASE("wilo",
                          "009",
                          "starting at a given word 2",
                          "[wilo][quick]") {
    word_type first  = {0, 1};
    word_type last   = {1, 1, 1};
    auto      result = std::vector<word_type>(cbegin_wilo(2, 3, first, last),
                                         cend_wilo(2, 3, first, last));
    REQUIRE(result == std::vector<word_type>({{0, 1}, {1}, {1, 0}, {1, 1}}));
    REQUIRE(result.size() == 4);
    result = std::vector<word_type>(cbegin_wilo(2, 1, first, last),
                                    cend_wilo(2, 1, first, last));
    REQUIRE(result.empty());
  }

  LIBSEMIGROUPS_TEST_CASE("wilo", "010", "code coverage", "[wilo][quick]") {
    word_type first = {0, 0, 0};
    word_type last  = {1, 1, 1, 1};

    const_wilo_iterator it;
    it = cbegin_wilo(2, 5, first, last);
    REQUIRE(*it == word_type({0, 0, 0}));
    REQUIRE(it->size() == 3);
    REQUIRE(*it++ == word_type({0, 0, 0}));
    REQUIRE(*it == word_type({0, 0, 0, 0}));

    auto it2 = it;
    REQUIRE(it == it2);
    it++;
    REQUIRE(it2 != it);
    REQUIRE(*it == word_type({0, 0, 0, 1}));
    REQUIRE(*it2 == word_type({0, 0, 0, 0}));

    swap(it, it2);
    REQUIRE(it2 != it);
    REQUIRE(*it2 == word_type({0, 0, 0, 1}));
    REQUIRE(*it == word_type({0, 0, 0, 0}));

    std::swap(it, it2);
    REQUIRE(it2 != it);
    REQUIRE(*it == word_type({0, 0, 0, 1}));
    REQUIRE(*it2 == word_type({0, 0, 0, 0}));
    it2++;
    REQUIRE(it == it2);
    REQUIRE(it++ == it2++);
    REQUIRE(it == it2);
    REQUIRE(++it == ++it2);
  }
}  // namespace libsemigroups
