//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-2023 James D. Mitchell
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

#include <algorithm>      // for find, is_sorted, sort
#include <cstddef>        // for size_t
#include <iterator>       // for distance
#include <utility>        // for swap
#include <vector>         // for vector
                          //
#include "catch.hpp"      // for REQUIRE etc
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/order.hpp"  // for number_of_words
#include "libsemigroups/types.hpp"  // for word_type
#include "libsemigroups/words.hpp"  // for number_of_words

namespace libsemigroups {
  using namespace literals;
  using namespace rx;

  LIBSEMIGROUPS_TEST_CASE("number_of_words", "000", "", "[quick]") {
    REQUIRE(39 == number_of_words(3, 1, 4));
    REQUIRE(29'524 == number_of_words(3, 0, 10));
    REQUIRE(32 == number_of_words(2, 5, 6));
    REQUIRE(797'161 == number_of_words(3, 0, 13));
    REQUIRE(number_of_words(2, 4, 1) == 0);
    REQUIRE(number_of_words(2, 4, 4) == 0);
    REQUIRE(number_of_words(2, 4, 2) == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("string_to_word", "001", "", "[quick]") {
    detail::StringToWord string_to_word("BCA");
    REQUIRE(string_to_word("BCABACB") == word_type({0, 1, 2, 0, 2, 1, 0}));
    REQUIRE(string_to_word("B") == word_type({0}));
    REQUIRE(string_to_word("C") == word_type({1}));
    REQUIRE(string_to_word("A") == word_type({2}));
  }

  LIBSEMIGROUPS_TEST_CASE("operator\"\" _w", "002", "literal", "[quick]") {
    REQUIRE(0120210_w == word_type({0, 1, 2, 0, 2, 1, 0}));
    REQUIRE(0_w == word_type({0}));
    REQUIRE(1_w == word_type({1}));
    REQUIRE(2_w == word_type({2}));
    // Require ""s to avoid interpretation as bad octal.
    REQUIRE("08"_w == word_type({0, 8}));
  }

  LIBSEMIGROUPS_TEST_CASE("Words",
                          "003",
                          "n: 2, min; 1, max: 4",
                          "[wislo][quick]") {
    word_type first = 0_w;
    word_type last  = 0000_w;
    auto      w     = std::vector<word_type>(cbegin_wislo(2, first, last),
                                    cend_wislo(2, first, last));
    REQUIRE(w
            == std::vector<word_type>({0_w,
                                       1_w,
                                       00_w,
                                       01_w,
                                       10_w,
                                       11_w,
                                       000_w,
                                       001_w,
                                       010_w,
                                       011_w,
                                       100_w,
                                       101_w,
                                       110_w,
                                       111_w}));
    REQUIRE(w.size() == 14);
    REQUIRE(std::is_sorted(w.cbegin(), w.cend(), ShortLexCompare()));
  }

  LIBSEMIGROUPS_TEST_CASE("Words", "004", "corner cases", "[wislo][quick]") {
    word_type first = 0000_w;
    word_type last  = 00000_w;
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
                {00000_w, 00001_w, 00010_w, 00011_w, 00100_w, 00101_w, 00110_w,
                 00111_w, 01000_w, 01001_w, 01010_w, 01011_w, 01100_w, 01101_w,
                 01110_w, 01111_w, 10000_w, 10001_w, 10010_w, 10011_w, 10100_w,
                 10101_w, 10110_w, 10111_w, 11000_w, 11001_w, 11010_w, 11011_w,
                 11100_w, 11101_w, 11110_w, 11111_w}));
    REQUIRE(std::is_sorted(w3.cbegin(), w3.cend(), ShortLexCompare()));
    REQUIRE(std::is_sorted(w3.cbegin(), w3.cend(), LexicographicalCompare()));
  }

  LIBSEMIGROUPS_TEST_CASE("Words",
                          "005",
                          "n: 3, min; 0, max: 10",
                          "[wislo][quick]") {
    word_type first = {};
    word_type last(10, 0);
    auto      w = std::vector<word_type>(cbegin_wislo(3, first, last),
                                    cend_wislo(3, first, last));
    REQUIRE(w.size() == number_of_words(3, 0, 10));
    REQUIRE(w.size() == 29524);
    REQUIRE(std::is_sorted(w.cbegin(), w.cend(), ShortLexCompare()));
  }

  LIBSEMIGROUPS_TEST_CASE("Words",
                          "006",
                          "n: 3, min; 0, max: 13",
                          "[wislo][quick][no-valgrind]") {
    word_type              first = {};
    word_type              last(13, 0);
    std::vector<word_type> w;
    w.reserve(number_of_words(3, 0, 13));
    w.insert(w.end(), cbegin_wislo(3, first, last), cend_wislo(3, first, last));
    REQUIRE(w.size() == number_of_words(3, 0, 13));
    REQUIRE(w.size() == 797161);
    REQUIRE(std::is_sorted(w.cbegin(), w.cend(), ShortLexCompare()));
  }

  LIBSEMIGROUPS_TEST_CASE("Words",
                          "007",
                          "lex + sort",
                          "[wislo][quick][no-valgrind]") {
    word_type first = {};
    word_type last(13, 2);
    auto      w = std::vector<word_type>(cbegin_wilo(3, 13, first, last),
                                    cend_wilo(3, 13, first, last));
    std::sort(w.begin(), w.end(), ShortLexCompare());
    REQUIRE(w.size() == number_of_words(3, 0, 13));
    REQUIRE(w.size() == 797161);
    REQUIRE(std::is_sorted(w.cbegin(), w.cend(), ShortLexCompare()));
  }

  LIBSEMIGROUPS_TEST_CASE("Words", "008", "code coverage", "[wislo][quick]") {
    word_type first = 000_w;
    word_type last  = 0000_w;

    const_wislo_iterator it;
    it = cbegin_wislo(2, first, last);
    REQUIRE(*it == 000_w);
    REQUIRE(it->size() == 3);
    REQUIRE(*it++ == 000_w);
    REQUIRE(*it == 001_w);

    auto it2 = it;
    REQUIRE(it == it2);
    it++;
    REQUIRE(it2 != it);
    REQUIRE(*it == 010_w);
    REQUIRE(*it2 == 001_w);

    swap(it, it2);
    REQUIRE(it2 != it);
    REQUIRE(*it2 == word_type(010_w));
    REQUIRE(*it == word_type(001_w));

    std::swap(it, it2);
    REQUIRE(it2 != it);
    REQUIRE(*it == word_type(010_w));
    REQUIRE(*it2 == word_type(001_w));
    it2++;
    REQUIRE(it == it2);
    REQUIRE(it++ == it2++);
    REQUIRE(it == it2);
    REQUIRE(++it == ++it2);

    const_wislo_iterator it3(cbegin_wislo(2, first, last));
    it3 = cbegin_wislo(2, first, last);
    REQUIRE(*it3 == word_type(000_w));
    REQUIRE(it->size() == 3);
    REQUIRE(*it3++ == word_type(000_w));
    REQUIRE(*it3 == word_type(001_w));
  }
  LIBSEMIGROUPS_TEST_CASE("Words", "009", "corner cases", "[wilo][quick]") {
    word_type const u  = 0000_w;
    word_type const v  = 1111_w;
    auto            w1 = std::vector<word_type>(cbegin_wilo(2, 1, v, u),
                                     cend_wilo(2, 1, v, u));
    REQUIRE(w1.empty());
    auto w2 = std::vector<word_type>(cbegin_wilo(2, 1, u, u),
                                     cend_wilo(2, 1, u, u));
    REQUIRE(w2.empty());
    auto w3 = std::vector<word_type>(cbegin_wilo(2, 2, {}, 11_w),
                                     cend_wilo(2, 2, {}, 11_w));
    REQUIRE(w3.size() == 3);
    REQUIRE(w3 == std::vector<word_type>({{}, 0_w, 1_w}));
    REQUIRE(word_type(0, 0) == word_type());
    auto w4 = std::vector<word_type>(cbegin_wilo(2, 1, {}, 11_w),
                                     cend_wilo(2, 1, {}, 11_w));
    REQUIRE(w4 == std::vector<word_type>({{}}));
    w4 = std::vector<word_type>(cbegin_wilo(2, 1, {}, 0_w),
                                cend_wilo(2, 1, {}, 0_w));
    REQUIRE(w4 == std::vector<word_type>({{}}));
  }

  LIBSEMIGROUPS_TEST_CASE("Words",
                          "010",
                          "letters: 2, min: 1, max: 4",
                          "[wilo][quick]") {
    auto w = std::vector<word_type>(cbegin_wilo(2, 4, 0_w, 1111_w),
                                    cend_wilo(2, 4, 0_w, 1111_w));
    REQUIRE(w
            == std::vector<word_type>({0_w,
                                       00_w,
                                       000_w,
                                       001_w,
                                       01_w,
                                       010_w,
                                       011_w,
                                       1_w,
                                       10_w,
                                       100_w,
                                       101_w,
                                       11_w,
                                       110_w,
                                       111_w}));
    REQUIRE(w.size() == 14);
    REQUIRE(std::is_sorted(w.cbegin(), w.cend(), LexicographicalCompare()));
    REQUIRE(std::is_sorted(cbegin_wilo(2, 4, 0_w, 1111_w),
                           cend_wilo(2, 4, 0_w, 1111_w),
                           LexicographicalCompare()));
  }

  LIBSEMIGROUPS_TEST_CASE("Words",
                          "011",
                          "letters: 2, min: 1, max: 4",
                          "[wilo][quick]") {
    word_type first = 0_w;
    word_type last  = 2222_w;
    auto      w     = std::vector<word_type>(cbegin_wilo(3, 4, first, last),
                                    cend_wilo(3, 4, first, last));
    REQUIRE(w.size() == 39);
    REQUIRE(w.size() == number_of_words(3, 1, 4));
    REQUIRE(w
            == std::vector<word_type>(
                {0_w,   00_w,  000_w, 001_w, 002_w, 01_w,  010_w, 011_w,
                 012_w, 02_w,  020_w, 021_w, 022_w, 1_w,   10_w,  100_w,
                 101_w, 102_w, 11_w,  110_w, 111_w, 112_w, 12_w,  120_w,
                 121_w, 122_w, 2_w,   20_w,  200_w, 201_w, 202_w, 21_w,
                 210_w, 211_w, 212_w, 22_w,  220_w, 221_w, 222_w}));
    REQUIRE(std::is_sorted(w.cbegin(), w.cend(), LexicographicalCompare()));
    REQUIRE(std::is_sorted(cbegin_wilo(3, 4, first, last),
                           cend_wilo(3, 4, first, last),
                           LexicographicalCompare()));
  }

  LIBSEMIGROUPS_TEST_CASE("Words",
                          "012",
                          "letters: 3, min: 0, max: 10",
                          "[wilo][quick]") {
    word_type first = {};
    word_type last(10, 2);
    auto      w = std::vector<word_type>(cbegin_wilo(3, 10, first, last),
                                    cend_wilo(3, 10, first, last));
    REQUIRE(w.size() == number_of_words(3, 0, 10));
    REQUIRE(w.size() == 29524);
    REQUIRE(std::is_sorted(w.cbegin(), w.cend(), LexicographicalCompare()));
    REQUIRE(std::is_sorted(cbegin_wilo(3, 10, first, last),
                           cend_wilo(3, 10, first, last),
                           LexicographicalCompare()));
  }

  LIBSEMIGROUPS_TEST_CASE("Words",
                          "013",
                          "letters: 3, min: 0, max: 13",
                          "[wilo][quick][no-valgrind]") {
    word_type first = {};
    word_type last(13, 2);

    std::vector<word_type> w(cbegin_wilo(3, 13, first, last),
                             cend_wilo(3, 13, first, last));
    REQUIRE(w.size() == 797161);
    REQUIRE(w.size() == number_of_words(3, 0, 13));
    REQUIRE(std::is_sorted(w.cbegin(), w.cend(), LexicographicalCompare()));
  }

  LIBSEMIGROUPS_TEST_CASE("Words",
                          "014",
                          "forward iterator requirements",
                          "[wilo][quick][no-valgrind]") {
    word_type first = {};
    word_type last(4, 1);
    auto      it = cbegin_wilo(2, 4, first, last);
    REQUIRE(*it == word_type());
    ++it;
    REQUIRE(*it == 0_w);

    first = {0};
    last  = word_type(13, 2);
    std::vector<word_type> w;
    w.reserve(number_of_words(3, 1, 13));
    w.insert(w.end(),
             cbegin_wilo(3, 13, first, last),
             cend_wilo(3, 13, first, last));
    REQUIRE(w.size() == number_of_words(3, 1, 13));
    REQUIRE(std::is_sorted(w.cbegin(), w.cend(), LexicographicalCompare()));
  }

  LIBSEMIGROUPS_TEST_CASE("Words",
                          "015",
                          "more corner cases",
                          "[wilo][quick]") {
    word_type first = {};
    word_type last(10, 0);
    auto      w = std::vector<word_type>(cbegin_wilo(1, 10, first, last),
                                    cend_wilo(1, 10, first, last));
    REQUIRE(w.size() == 10);
    REQUIRE(w
            == std::vector<word_type>({{},
                                       0_w,
                                       00_w,
                                       000_w,
                                       0000_w,
                                       00000_w,
                                       000000_w,
                                       0000000_w,
                                       00000000_w,
                                       000000000_w}));
    w = std::vector<word_type>(cbegin_wilo(0, 0, first, last),
                               cend_wilo(0, 0, first, last));
    REQUIRE(w.size() == 0);
    REQUIRE(w.empty());
    first = 00_w;
    last  = 0000_w;
    w     = std::vector<word_type>(cbegin_wilo(1, 4, first, last),
                               cend_wilo(1, 4, first, last));
    REQUIRE(w == std::vector<word_type>({00_w, 000_w}));
    w = std::vector<word_type>(cbegin_wilo(1, 1, first, last),
                               cend_wilo(1, 1, first, last));
    REQUIRE(w == std::vector<word_type>());
    w = std::vector<word_type>(cbegin_wilo(1, 5, {0}, {1}),
                               cend_wilo(1, 5, {0}, {1}));
    REQUIRE(w == std::vector<word_type>({0_w, 00_w, 000_w, 0000_w}));
  }

  LIBSEMIGROUPS_TEST_CASE("Words",
                          "016",
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

  LIBSEMIGROUPS_TEST_CASE("Words",
                          "017",
                          "starting at a given word 2",
                          "[wilo][quick]") {
    word_type first  = {0, 1};
    word_type last   = {1, 1, 1};
    auto      result = std::vector<word_type>(cbegin_wilo(2, 3, first, last),
                                         cend_wilo(2, 3, first, last));
    REQUIRE(result == std::vector<word_type>({01_w, 1_w, 10_w, 11_w}));
    REQUIRE(result.size() == 4);
    result = std::vector<word_type>(cbegin_wilo(2, 1, first, last),
                                    cend_wilo(2, 1, first, last));
    REQUIRE(result.empty());
  }

  LIBSEMIGROUPS_TEST_CASE("Words", "018", "code coverage", "[wilo][quick]") {
    word_type first = 000_w;
    word_type last  = 1111_w;

    const_wilo_iterator it;
    it = cbegin_wilo(2, 5, first, last);
    REQUIRE(*it == word_type(000_w));
    REQUIRE(it->size() == 3);
    REQUIRE(*it++ == word_type(000_w));
    REQUIRE(*it == word_type(0000_w));

    auto it2 = it;
    REQUIRE(it == it2);
    it++;
    REQUIRE(it2 != it);
    REQUIRE(*it == word_type(0001_w));
    REQUIRE(*it2 == word_type(0000_w));

    swap(it, it2);
    REQUIRE(it2 != it);
    REQUIRE(*it2 == word_type(0001_w));
    REQUIRE(*it == word_type(0000_w));

    std::swap(it, it2);
    REQUIRE(it2 != it);
    REQUIRE(*it == word_type(0001_w));
    REQUIRE(*it2 == word_type(0000_w));
    it2++;
    REQUIRE(it == it2);
    REQUIRE(it++ == it2++);
    REQUIRE(it == it2);
    REQUIRE(++it == ++it2);
  }

  LIBSEMIGROUPS_TEST_CASE("Words", "019", "check count", "[words][quick]") {
    size_t const m = 27;
    size_t const n = 2;

    Words words;
    words.order(order::lex).letters(n).upper_bound(m + 1).min(1).max(m + 1);

    REQUIRE(std::distance(
                cbegin_wilo(n, m + 1, word_type({0}), word_type(m + 1, 0)),
                cend_wilo(n, m + 1, word_type({0}), word_type(m + 1, 0)))
            == 27);
    REQUIRE(words.upper_bound() == 28);
    REQUIRE(words.first() == 0_w);
    REQUIRE(words.last() == 0000000000000000000000000000_w);

    REQUIRE(words.count() == 27);
  }

  LIBSEMIGROUPS_TEST_CASE("Strings",
                          "020",
                          "lex | alphabet = a | min = 0 | max = 10",
                          "[lex][quick]") {
    Strings strings;

    strings.order(order::lex)
        .letters("a")
        .first("")
        .last("aaaaaaaaaa")
        .upper_bound(10);
    REQUIRE((strings | count()) == 10);
    REQUIRE((strings | to_vector())
            == std::vector<std::string>({"",
                                         "a",
                                         "aa",
                                         "aaa",
                                         "aaaa",
                                         "aaaaa",
                                         "aaaaaa",
                                         "aaaaaaa",
                                         "aaaaaaaa",
                                         "aaaaaaaaa"}));
    strings.letters("");
    REQUIRE((strings | count()) == 1);

    strings.letters("a").upper_bound(4).first("aa");
    REQUIRE((strings | to_vector()) == std::vector<std::string>({"aa", "aaa"}));
  }

  LIBSEMIGROUPS_TEST_CASE("Strings",
                          "021",
                          "lex | corner cases",
                          "[lex][quick]") {
    Strings strings;
    strings.order(order::lex)
        .letters("ab")
        .first("aaaaaaaaaa")
        .last("")
        .upper_bound(4);
    REQUIRE((strings | count()) == 0);

    strings.first("");
    REQUIRE((strings | count()) == 0);

    strings.first("a").last("bb").upper_bound(2);
    REQUIRE((strings | count()) == 2);
    REQUIRE((strings | to_vector()) == std::vector<std::string>({"a", "b"}));

    strings.upper_bound(1).first("").last("aaaaaaaaaaaa");
    REQUIRE((strings | to_vector()) == std::vector<std::string>({""}));
  }

  LIBSEMIGROUPS_TEST_CASE("Strings",
                          "022",
                          "lex | alphabet = ab | min = 1 | max = 4",
                          "[lex][quick]") {
    Strings strings;
    strings.letters("ab")
        .order(order::lex)
        .upper_bound(4)
        .first("a")
        .last("bbbbb");

    REQUIRE((strings | count()) == 14);
    REQUIRE((strings | to_vector())
            == std::vector<std::string>({"a",
                                         "aa",
                                         "aaa",
                                         "aab",
                                         "ab",
                                         "aba",
                                         "abb",
                                         "b",
                                         "ba",
                                         "baa",
                                         "bab",
                                         "bb",
                                         "bba",
                                         "bbb"}));

    REQUIRE((strings | to_words() | to_vector())
            == std::vector<word_type>({0_w,
                                       00_w,
                                       000_w,
                                       001_w,
                                       01_w,
                                       010_w,
                                       011_w,
                                       1_w,
                                       10_w,
                                       100_w,
                                       101_w,
                                       11_w,
                                       110_w,
                                       111_w}));
    REQUIRE(
        std::is_sorted(begin(strings), end(strings), LexicographicalCompare()));

    strings.letters("ba").first("b").last("aaaaa");
    REQUIRE((strings | count()) == 14);
    REQUIRE((strings | to_vector())
            == std::vector<std::string>({"b",
                                         "bb",
                                         "bbb",
                                         "bba",
                                         "ba",
                                         "bab",
                                         "baa",
                                         "a",
                                         "ab",
                                         "abb",
                                         "aba",
                                         "aa",
                                         "aab",
                                         "aaa"}));
  }

  LIBSEMIGROUPS_TEST_CASE("Strings",
                          "023",
                          "lex | alphabet = abc | min = 0 | max = 13",
                          "[lex][quick][no-valgrind]") {
    Strings strings;
    strings.order(order::lex)
        .letters("abc")
        .upper_bound(13)
        .first("")
        .last(std::string(13, 'c'));
    REQUIRE(number_of_words(3, 0, 13) == 797'161);
    REQUIRE(strings.count() == number_of_words(3, 0, 13));
    REQUIRE(
        std::is_sorted(begin(strings), end(strings), LexicographicalCompare()));
    REQUIRE((strings | count()) == 797'161);
  }

  LIBSEMIGROUPS_TEST_CASE("Strings",
                          "024",
                          "lex | code coverage",
                          "[lex][quick]") {
    auto    first = "aaa";
    auto    last  = "bbbb";
    Strings strings;

    strings.letters("ab").first(first).last(last).upper_bound(5).order(
        order::lex);

    auto it = begin(strings);
    REQUIRE(*it == "aaa");
    REQUIRE(it->size() == 3);
    REQUIRE(*++it == "aaaa");

    auto it2 = it;
    REQUIRE(*it == *it2);
    ++it;
    REQUIRE(*it2 != *it);
    REQUIRE(*it == "aaab");
    REQUIRE(*it2 == "aaaa");

    swap(it, it2);
    REQUIRE(it2 != it);
    REQUIRE(*it2 == "aaab");
    REQUIRE(*it == "aaaa");

    std::swap(it, it2);
    REQUIRE(it2 != it);
    REQUIRE(*it == "aaab");
    REQUIRE(*it2 == "aaaa");
    ++it2;
    REQUIRE(*it == *it2);
    REQUIRE(*(++it) == *(++it2));
    REQUIRE(*it == *it2);
    REQUIRE(*(++it) == *(++it2));
  }

  LIBSEMIGROUPS_TEST_CASE("Strings",
                          "025",
                          "shortlex | alphabet = a | min = 0 | max = 10",
                          "[shortlex][quick]") {
    Words words;
    words.letters(1).min(0).max(10);

    auto w = (words | to_strings("a"));
    REQUIRE((w | count()) == 10);
    REQUIRE((w | to_vector())
            == std::vector<std::string>({"",
                                         "a",
                                         "aa",
                                         "aaa",
                                         "aaaa",
                                         "aaaaa",
                                         "aaaaaa",
                                         "aaaaaaa",
                                         "aaaaaaaa",
                                         "aaaaaaaaa"}));
    words.min(2).max(4);
    REQUIRE((words | to_strings("b") | to_vector())
            == std::vector<std::string>({"bb", "bbb"}));
  }

  LIBSEMIGROUPS_TEST_CASE("Strings",
                          "026",
                          "shortlex | corner cases",
                          "[shortlex][quick]") {
    Strings strings;
    strings.last("").first("bbaaab");

    REQUIRE((strings | count()) == 0);

    strings.first("").last("");
    REQUIRE((strings | count()) == 0);

    strings.letters("ab").first("a").last("aa");
    REQUIRE((strings | count()) == 2);
    REQUIRE((strings | to_vector()) == std::vector<std::string>({"a", "b"}));

    strings.first("").last("bbaaab");
    REQUIRE((strings | to_vector())
            == std::vector<std::string>(
                {"",       "a",      "b",      "aa",     "ab",     "ba",
                 "bb",     "aaa",    "aab",    "aba",    "abb",    "baa",
                 "bab",    "bba",    "bbb",    "aaaa",   "aaab",   "aaba",
                 "aabb",   "abaa",   "abab",   "abba",   "abbb",   "baaa",
                 "baab",   "baba",   "babb",   "bbaa",   "bbab",   "bbba",
                 "bbbb",   "aaaaa",  "aaaab",  "aaaba",  "aaabb",  "aabaa",
                 "aabab",  "aabba",  "aabbb",  "abaaa",  "abaab",  "ababa",
                 "ababb",  "abbaa",  "abbab",  "abbba",  "abbbb",  "baaaa",
                 "baaab",  "baaba",  "baabb",  "babaa",  "babab",  "babba",
                 "babbb",  "bbaaa",  "bbaab",  "bbaba",  "bbabb",  "bbbaa",
                 "bbbab",  "bbbba",  "bbbbb",  "aaaaaa", "aaaaab", "aaaaba",
                 "aaaabb", "aaabaa", "aaabab", "aaabba", "aaabbb", "aabaaa",
                 "aabaab", "aababa", "aababb", "aabbaa", "aabbab", "aabbba",
                 "aabbbb", "abaaaa", "abaaab", "abaaba", "abaabb", "ababaa",
                 "ababab", "ababba", "ababbb", "abbaaa", "abbaab", "abbaba",
                 "abbabb", "abbbaa", "abbbab", "abbbba", "abbbbb", "baaaaa",
                 "baaaab", "baaaba", "baaabb", "baabaa", "baabab", "baabba",
                 "baabbb", "babaaa", "babaab", "bababa", "bababb", "babbaa",
                 "babbab", "babbba", "babbbb", "bbaaaa"}));
  }

  LIBSEMIGROUPS_TEST_CASE("Strings",
                          "027",
                          "shortlex | alphabet = ab | min = 1 | max = 4",
                          "[shortlex][quick]") {
    Strings strings;

    strings.letters("ab").first("a").last("aaaa");
    REQUIRE((strings | to_vector())
            == std::vector<std::string>({"a",
                                         "b",
                                         "aa",
                                         "ab",
                                         "ba",
                                         "bb",
                                         "aaa",
                                         "aab",
                                         "aba",
                                         "abb",
                                         "baa",
                                         "bab",
                                         "bba",
                                         "bbb"}));

    REQUIRE((strings | count()) == 14);
    REQUIRE(std::is_sorted(begin(strings), end(strings), ShortLexCompare()));

    strings.letters("ab").first("a").last("bbbbb");
    REQUIRE(std::is_sorted(begin(strings), end(strings), ShortLexCompare()));

    strings.letters("ba").first("b").last("bbbb");
    REQUIRE((strings | to_vector())
            == std::vector<std::string>({"b",
                                         "a",
                                         "bb",
                                         "ba",
                                         "ab",
                                         "aa",
                                         "bbb",
                                         "bba",
                                         "bab",
                                         "baa",
                                         "abb",
                                         "aba",
                                         "aab",
                                         "aaa"}));
  }

  LIBSEMIGROUPS_TEST_CASE("Strings",
                          "028",
                          "shortlex | alphabet = abc | min = 0 | max = 13",
                          "[shortlex][quick][no-valgrind]") {
    Strings strings;
    strings.letters("abc").max(13);
    REQUIRE((strings | count()) == number_of_words(3, 0, 13));
    REQUIRE(strings.count() == 797'161);
    REQUIRE(std::is_sorted(begin(strings), end(strings), ShortLexCompare()));
  }

  LIBSEMIGROUPS_TEST_CASE("Strings",
                          "029",
                          "shortlex | code coverage",
                          "[shortlex][quick]") {
    auto    first = "aaa";
    auto    last  = "bbbb";
    Strings strings;

    strings.letters("ab").first(first).last(last).upper_bound(5).order(
        order::shortlex);
    auto it = begin(strings);
    REQUIRE(*it == "aaa");
    REQUIRE(it->size() == 3);
    REQUIRE(*++it == "aab");

    auto it2 = it;
    REQUIRE(*it == *it2);
    ++it;
    REQUIRE(*it2 != *it);
    REQUIRE(*it == "aba");
    REQUIRE(*it2 == "aab");

    swap(it, it2);
    REQUIRE(it2 != it);
    REQUIRE(*it == "aab");
    REQUIRE(*it2 == "aba");

    std::swap(it, it2);
    REQUIRE(*it2 != *it);
    REQUIRE(*it == "aba");
    REQUIRE(*it2 == "aab");
    ++it2;
    REQUIRE(*it == *it2);
    REQUIRE(*(++it) == *(++it2));
    REQUIRE(*it == *it2);
    REQUIRE(*(++it) == *(++it2));
  }

}  // namespace libsemigroups
