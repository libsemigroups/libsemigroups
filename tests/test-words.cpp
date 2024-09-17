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

#include <algorithm>              // for find, is_sorted, sort
#include <cstddef>                // for size_t
#include <iterator>               // for distance
#include <utility>                // for swap
#include <vector>                 // for vector
                                  //
#include "catch_amalgamated.hpp"  // for REQUIRE etc
#include "libsemigroups/exception.hpp"
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/order.hpp"   // for number_of_words
#include "libsemigroups/ranges.hpp"  // for equals
#include "libsemigroups/types.hpp"   // for word_type
#include "libsemigroups/words.hpp"   // for number_of_words

#include "libsemigroups/detail/word-iterators.hpp"  // for const_wilo_iterator

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
    REQUIRE(3 == number_of_words(1, 1, 4));
  }

  LIBSEMIGROUPS_TEST_CASE("ToWord", "001", "", "[quick]") {
    {
      ToWord toword("BCA");
      REQUIRE(!toword.empty());
      REQUIRE(toword("BCABACB") == 0120210_w);
      REQUIRE(toword("B") == 0_w);
      REQUIRE(toword("C") == 1_w);
      REQUIRE(toword("A") == 2_w);

      REQUIRE_THROWS_AS(toword.init("aa"), LibsemigroupsException);
      REQUIRE_THROWS_AS(toword.init("XX"), LibsemigroupsException);
      REQUIRE_THROWS_AS(toword.init(std::string(256, 'a')),
                        LibsemigroupsException);

      REQUIRE(toword("BCABACB") == 0120210_w);
      REQUIRE(toword("B") == 0_w);
      REQUIRE(toword("C") == 1_w);
      REQUIRE(toword("A") == 2_w);
      REQUIRE_THROWS_AS(toword("z"), LibsemigroupsException);
    }
    {
      ToWord toword("bac");
      REQUIRE(toword("bac") == 012_w);
      REQUIRE(toword("bababbbcbcbaac") == 01010002020112_w);
      ToString tostring("bac");
      REQUIRE(tostring(toword("bababbbcbcbaac")) == "bababbbcbcbaac");
      REQUIRE(toword(tostring(01010002020112_w)) == 01010002020112_w);
    }
    std::string output;
    ToString    tostring("bac");
    tostring(output, 012101_w);
    REQUIRE(output == "bacaba");
  }

  LIBSEMIGROUPS_TEST_CASE("operator\"\" _w",
                          "002",
                          "literal",
                          "[quick][WordRange]") {
    REQUIRE(0120210_w == word_type({0, 1, 2, 0, 2, 1, 0}));
    REQUIRE(0_w == word_type({0}));
    REQUIRE(1_w == word_type({1}));
    REQUIRE(2_w == word_type({2}));
    // Require ""s to avoid interpretation as bad octal.
    REQUIRE("08"_w == word_type({0, 8}));

    // The other mode of behaviour
    REQUIRE("ab"_w == word_type({0, 1}));
    REQUIRE("zz"_w == word_type({25, 25}));
    REQUIRE_THROWS_AS("\n"_w, LibsemigroupsException);
    REQUIRE_THROWS_AS("0a"_w, LibsemigroupsException);
    REQUIRE_THROWS_AS("a0"_w, LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("WordRange",
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

    WordRange words;
    words.first(first).last(last);
    REQUIRE(words.count() == 0);
    words.alphabet_size(2);
    REQUIRE(words.count() == 14);
  }

  LIBSEMIGROUPS_TEST_CASE("WordRange",
                          "004",
                          "corner cases",
                          "[wislo][quick]") {
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

  LIBSEMIGROUPS_TEST_CASE("WordRange",
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

  LIBSEMIGROUPS_TEST_CASE("WordRange",
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

  LIBSEMIGROUPS_TEST_CASE("WordRange",
                          "007",
                          "lex + sort",
                          "[wislo][quick][no-valgrind][no-coverage]") {
    word_type first = {};
    word_type last(13, 2);
    auto      w = std::vector<word_type>(cbegin_wilo(3, 13, first, last),
                                    cend_wilo(3, 13, first, last));
    std::sort(w.begin(), w.end(), ShortLexCompare());
    REQUIRE(w.size() == number_of_words(3, 0, 13));
    REQUIRE(w.size() == 797161);
    REQUIRE(std::is_sorted(w.cbegin(), w.cend(), ShortLexCompare()));
  }

  LIBSEMIGROUPS_TEST_CASE("WordRange",
                          "008",
                          "code coverage",
                          "[wislo][quick]") {
    word_type first = 000_w;
    word_type last  = 0000_w;

    detail::const_wislo_iterator it;
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

    detail::const_wislo_iterator it3(cbegin_wislo(2, first, last));
    it3 = cbegin_wislo(2, first, last);
    REQUIRE(*it3 == word_type(000_w));
    REQUIRE(it->size() == 3);
    REQUIRE(*it3++ == word_type(000_w));
    REQUIRE(*it3 == word_type(001_w));
  }
  LIBSEMIGROUPS_TEST_CASE("WordRange", "009", "corner cases", "[wilo][quick]") {
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

  LIBSEMIGROUPS_TEST_CASE("WordRange",
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

  LIBSEMIGROUPS_TEST_CASE("WordRange",
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

  LIBSEMIGROUPS_TEST_CASE("WordRange",
                          "012",
                          "letters: 3, min: 0, max: 10",
                          "[wilo][quick][no-valgrind]") {
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

  LIBSEMIGROUPS_TEST_CASE("WordRange",
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

  LIBSEMIGROUPS_TEST_CASE("WordRange",
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

  LIBSEMIGROUPS_TEST_CASE("WordRange",
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

  LIBSEMIGROUPS_TEST_CASE("WordRange",
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

  LIBSEMIGROUPS_TEST_CASE("WordRange",
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

  LIBSEMIGROUPS_TEST_CASE("WordRange",
                          "018",
                          "code coverage",
                          "[wilo][quick]") {
    word_type first = 000_w;
    word_type last  = 1111_w;

    detail::const_wilo_iterator it;
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

  LIBSEMIGROUPS_TEST_CASE("WordRange", "019", "check count", "[words][quick]") {
    using words::pow;
    size_t const m = 27;
    size_t const n = 2;

    WordRange words;
    words.order(Order::lex)
        .alphabet_size(n)
        .upper_bound(m + 1)
        .min(1)
        .max(m + 1);
    REQUIRE(words.get() == 0_w);
    words.next();
    REQUIRE(words.get() == 00_w);

    REQUIRE(std::distance(
                cbegin_wilo(n, m + 1, word_type({0}), word_type(m + 1, 0)),
                cend_wilo(n, m + 1, word_type({0}), word_type(m + 1, 0)))
            == 27);
    REQUIRE(words.upper_bound() == 28);
    REQUIRE(words.first() == 0_w);
    REQUIRE(words.last() == pow(0_w, 28));
    REQUIRE(words.count() == 26);

    REQUIRE_THROWS_AS(words.order(Order::none), LibsemigroupsException);
    REQUIRE_THROWS_AS(words.order(Order::recursive), LibsemigroupsException);

    WordRange copy;
    copy.operator=(words);
    REQUIRE(copy.get() == 00_w);
    copy.next();
    REQUIRE(copy.get() == 000_w);
    words.next();

    REQUIRE(equal(words, copy));
    REQUIRE(copy.upper_bound() == 28);
    REQUIRE(copy.first() == 0_w);
    REQUIRE(copy.last() == pow(0_w, 28));
    REQUIRE(copy.count() == 25);

    WordRange move;
    move.operator=(std::move(words));
    REQUIRE(equal(copy, move));
    REQUIRE(move.upper_bound() == 28);
    REQUIRE(move.first() == 0_w);
    REQUIRE(move.last() == pow(0_w, 28));
    REQUIRE(move.count() == 25);
    REQUIRE(move.alphabet_size() == 2);

    WordRange more;
    REQUIRE(more.at_end());
    REQUIRE_NOTHROW(more.next());
    REQUIRE(more.get() == ""_w);
    REQUIRE(more.alphabet_size() == 0);
    REQUIRE(more.order() == Order::shortlex);
    REQUIRE(more.is_finite);
    REQUIRE(more.is_idempotent);
    REQUIRE(more.size_hint() == 0);
    REQUIRE(more.count() == 0);
    REQUIRE(equal(more, move.init()));

    WordRange swap;
    swap.alphabet_size(3).first("abc"_w).last("abcbcbcbcb"_w);
    std::swap(swap, more);
    REQUIRE(equal(move, swap));
    REQUIRE(swap.upper_bound() == move.upper_bound());
    REQUIRE(swap.first() == move.first());
    REQUIRE(swap.last() == move.last());
    REQUIRE(swap.count() == move.count());
    REQUIRE(swap.alphabet_size() == move.alphabet_size());
  }

  LIBSEMIGROUPS_TEST_CASE("StringRange",
                          "020",
                          "lex | alphabet = a | min = 0 | max = 10",
                          "[lex][quick]") {
    StringRange strings;

    strings.order(Order::lex)
        .alphabet("a")
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
    strings.alphabet("");
    REQUIRE((strings | count()) == 1);

    strings.alphabet("a").upper_bound(4).first("aa");
    REQUIRE((strings | to_vector()) == std::vector<std::string>({"aa", "aaa"}));
  }

  LIBSEMIGROUPS_TEST_CASE("StringRange",
                          "021",
                          "lex | corner cases",
                          "[lex][quick]") {
    StringRange strings;
    strings.order(Order::lex)
        .alphabet("ab")
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

  LIBSEMIGROUPS_TEST_CASE("StringRange",
                          "022",
                          "lex | alphabet = ab | min = 1 | max = 4",
                          "[lex][quick]") {
    StringRange strings;
    strings.alphabet("ab")
        .order(Order::lex)
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

    REQUIRE((strings | ToWord("ab") | to_vector())
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
    REQUIRE(is_sorted(strings, LexicographicalCompare()));

    strings.alphabet("ba").first("b").last("aaaaa");
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

  LIBSEMIGROUPS_TEST_CASE("StringRange",
                          "023",
                          "lex | alphabet = abc | min = 0 | max = 13",
                          "[lex][quick][no-valgrind]") {
    StringRange strings;
    strings.order(Order::lex)
        .alphabet("abc")
        .upper_bound(13)
        .first("")
        .last(std::string(13, 'c'));
    REQUIRE(number_of_words(3, 0, 13) == 797'161);
    REQUIRE(strings.count() == number_of_words(3, 0, 13));
    REQUIRE(is_sorted(strings, LexicographicalCompare()));
    REQUIRE((strings | count()) == 797'161);
  }

  LIBSEMIGROUPS_TEST_CASE("StringRange",
                          "024",
                          "lex | code coverage",
                          "[lex][quick]") {
    auto        first = "aaa";
    auto        last  = "bbbb";
    StringRange strings;

    strings.alphabet("ab").first(first).last(last).upper_bound(5).order(
        Order::lex);

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

    REQUIRE(*it2 == "aaaa");
    REQUIRE(*it == "aaab");

    REQUIRE(*it == "aaab");
    REQUIRE(*it2 == "aaaa");
    ++it2;
    REQUIRE(*it == *it2);
    REQUIRE(*(++it) == *(++it2));
    REQUIRE(*it == *it2);
    REQUIRE(*(++it) == *(++it2));
  }

  LIBSEMIGROUPS_TEST_CASE("ToString",
                          "025",
                          "shortlex | alphabet = a | min = 0 | max = 10",
                          "[shortlex][quick]") {
    WordRange words;
    words.alphabet_size(1).min(0).max(10);

    auto w = (words | ToString("a"));
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
    REQUIRE((words | ToString("b") | to_vector())
            == std::vector<std::string>({"bb", "bbb"}));
  }

  LIBSEMIGROUPS_TEST_CASE("StringRange",
                          "026",
                          "shortlex | corner cases",
                          "[shortlex][quick]") {
    StringRange strings;
    // TODO first and last throw if they contain letters not in the alphabet
    strings.alphabet("ab").last("").first("bbaaab");

    REQUIRE((strings | count()) == 0);

    strings.first("").last("");
    REQUIRE((strings | count()) == 0);

    strings.alphabet("ab").first("a").last("aa");
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

  LIBSEMIGROUPS_TEST_CASE("StringRange",
                          "027",
                          "shortlex | alphabet = ab | min = 1 | max = 4",
                          "[shortlex][quick]") {
    StringRange strings;

    strings.alphabet("ab").first("a").last("aaaa");
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
    REQUIRE(is_sorted(strings, ShortLexCompare()));

    strings.alphabet("ab").first("a").last("bbbbb");
    REQUIRE(is_sorted(strings, ShortLexCompare()));

    strings.alphabet("ba").first("b").last("bbbb");
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

  LIBSEMIGROUPS_TEST_CASE("StringRange",
                          "028",
                          "shortlex | alphabet = abc | min = 0 | max = 13",
                          "[shortlex][quick][no-valgrind]") {
    StringRange strings;
    strings.alphabet("abc").max(13);
    REQUIRE((strings | count()) == number_of_words(3, 0, 13));
    REQUIRE(strings.count() == 797'161);
    REQUIRE(is_sorted(strings, ShortLexCompare()));
  }

  LIBSEMIGROUPS_TEST_CASE("StringRange",
                          "029",
                          "shortlex | code coverage",
                          "[shortlex][quick]") {
    auto        first = "aaa";
    auto        last  = "bbbb";
    StringRange strings;

    strings.alphabet("ab").first(first).last(last).upper_bound(5).order(
        Order::shortlex);
    auto it = begin(strings);
    REQUIRE(*it == "aaa");
    REQUIRE(it->size() == 3);
    REQUIRE(*++it == "aab");

    auto it2 = it;
    REQUIRE(*it == *it2);
    ++it;
    REQUIRE(*it == "aba");
    REQUIRE(*it2 == "aab");

    REQUIRE(*it == "aba");
    REQUIRE(*it2 == "aab");

    REQUIRE(*it == "aba");
    REQUIRE(*it2 == "aab");
    ++it2;
    REQUIRE(*it == *it2);
    REQUIRE(*(++it) == *(++it2));
    REQUIRE(*it == *it2);
    REQUIRE(*(++it) == *(++it2));
  }

  LIBSEMIGROUPS_TEST_CASE("StringRange", "030", "code coverage", "[words]") {
    using words::pow;

    size_t const m = 27;

    StringRange strings;
    strings.order(Order::lex)
        .alphabet("ab")
        .upper_bound(m + 1)
        .min(1)
        .max(m + 1);
    REQUIRE(strings.get() == "a");
    strings.next();
    REQUIRE(strings.get() == "aa");

    REQUIRE(strings.upper_bound() == 28);
    REQUIRE(strings.first() == "a");
    REQUIRE(strings.last() == pow("a", 28));
    REQUIRE(strings.count() == 26);

    REQUIRE_THROWS_AS(strings.alphabet("aba"), LibsemigroupsException);

    REQUIRE_THROWS_AS(strings.order(Order::none), LibsemigroupsException);
    REQUIRE_THROWS_AS(strings.order(Order::recursive), LibsemigroupsException);

    StringRange copy;
    copy.operator=(strings);
    REQUIRE(copy.get() == "aa");
    copy.next();
    REQUIRE(copy.get() == "aaa");
    strings.next();

    REQUIRE(equal(strings, copy));
    REQUIRE(copy.upper_bound() == 28);
    REQUIRE(copy.first() == "a");
    REQUIRE(copy.last() == pow("a", 28));
    REQUIRE(copy.count() == 25);

    StringRange move;
    move.operator=(std::move(strings));
    REQUIRE(equal(copy, move));
    REQUIRE(move.upper_bound() == 28);
    REQUIRE(move.first() == "a");
    REQUIRE(move.last() == pow("a", 28));
    REQUIRE(move.count() == 25);
    REQUIRE(move.alphabet() == "ab");

    StringRange more;
    REQUIRE(more.at_end());
    REQUIRE_NOTHROW(more.next());
    REQUIRE(more.get() == "");
    REQUIRE(more.alphabet() == "");
    REQUIRE(more.order() == Order::shortlex);
    REQUIRE(more.is_finite);
    REQUIRE(more.is_idempotent);
    REQUIRE(more.size_hint() == 0);
    REQUIRE(more.count() == 0);
    REQUIRE(equal(more, move.init()));

    StringRange move2(std::move(copy));
    REQUIRE(equal(move, move2));
    REQUIRE(move2.upper_bound() == 28);
    REQUIRE(move2.first() == "a");
    REQUIRE(move2.last() == pow("a", 28));
    REQUIRE(move2.count() == 25);
    REQUIRE(move2.alphabet() == "ab");

    StringRange swap;
    swap.alphabet("abc").first("abc").last("abcbcbcbcb");
    std::swap(swap, move2);
    REQUIRE(equal(move, swap));
    REQUIRE(swap.upper_bound() == 28);
    REQUIRE(swap.first() == "a");
    REQUIRE(swap.last() == pow("a", 28));
    REQUIRE(swap.count() == 25);
    REQUIRE(swap.alphabet() == "ab");

    for (auto const& s : swap) {
      REQUIRE(s.size() > 0);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("WordRange", "031", "parsing", "[shortlex][quick]") {
    REQUIRE("cd(ab)^2ef"_p == "cdababef");
    REQUIRE("cd((ab)^2)^4ef"_p == "cdababababababababef");
    REQUIRE("cd((ab)^2)^4(ef)^2"_p == "cdababababababababefef");
    REQUIRE("a^16"_p == "aaaaaaaaaaaaaaaa");
    REQUIRE("a^16cd^10((ab)^2)^4(ef)^2"_p
            == "aaaaaaaaaaaaaaaacddddddddddababababababababefef");
    REQUIRE("X^3(yx^2)"_p == "XXXyxx");
    REQUIRE("b(aX)^3x"_p == "baXaXaXx");
    REQUIRE("((a)b^2y)^10"_p == "abbyabbyabbyabbyabbyabbyabbyabbyabbyabby");

    REQUIRE("()"_p == "");
    REQUIRE("y^0"_p == "");
    REQUIRE(""_p == "");
    REQUIRE("a"_p == "a");

    REQUIRE_THROWS_AS("a*a*b*bc"_p, LibsemigroupsException);
    REQUIRE("           "_p == "");

    // TODO the error message for the block of lines aren't very good
    REQUIRE_THROWS_AS("(a*b)^3*b"_p, LibsemigroupsException);
    REQUIRE_THROWS_AS("(a*b)^3*bc"_p, LibsemigroupsException);
    REQUIRE_THROWS_AS("(2^2)"_p, LibsemigroupsException);
    REQUIRE_THROWS_AS("2*2"_p, LibsemigroupsException);

    REQUIRE_THROWS_AS("(()()()((((())()())"_p, LibsemigroupsException);
    REQUIRE_THROWS_AS("("_p, LibsemigroupsException);
    REQUIRE_THROWS_AS("(^2)"_p, LibsemigroupsException);
    REQUIRE_THROWS_AS("(a^)"_p, LibsemigroupsException);
    REQUIRE_THROWS_AS("(a^a)"_p, LibsemigroupsException);
    REQUIRE_THROWS_AS("(a^^a)"_p, LibsemigroupsException);
    REQUIRE_THROWS_AS("^"_p, LibsemigroupsException);
    REQUIRE_THROWS_AS("*"_p, LibsemigroupsException);
    REQUIRE_THROWS_AS("*2"_p, LibsemigroupsException);
    REQUIRE_THROWS_AS("a*"_p, LibsemigroupsException);
    REQUIRE_THROWS_AS("*b"_p, LibsemigroupsException);
    REQUIRE_THROWS_AS("2*"_p, LibsemigroupsException);
    REQUIRE("22"_p == "22");

    REQUIRE_THROWS_AS("a^"_p, LibsemigroupsException);
    REQUIRE_THROWS_AS("^y"_p, LibsemigroupsException);
    REQUIRE_THROWS_AS("1^1"_p, LibsemigroupsException);
    REQUIRE_THROWS_AS("&^1"_p, LibsemigroupsException);
    REQUIRE_THROWS_AS("a^16cd^10((ab)^2)^4(!f)^2"_p, LibsemigroupsException);
    REQUIRE("((ab)^3xx)^2"_p == "abababxxabababxx");
    REQUIRE(""_p == "");
    REQUIRE_THROWS_AS(")"_p, LibsemigroupsException);
    REQUIRE_THROWS_AS("xy)"_p, LibsemigroupsException);
    REQUIRE_THROWS_AS("(ab)^2xy^7)"_p, LibsemigroupsException);
    REQUIRE_THROWS_AS("((ab)()"_p, LibsemigroupsException);
    REQUIRE_THROWS_AS("("_p, LibsemigroupsException);
    REQUIRE_THROWS_AS("\n"_p, LibsemigroupsException);
    REQUIRE_THROWS_AS("-"_p, LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("WordRange",
                          "032",
                          "operator+",
                          "[quick][word_functions]") {
    using namespace literals;
    using words::operator+;
    word_type w = 01_w;
    word_type v = 2_w;
    REQUIRE((w + v) == 012_w);
    REQUIRE((w + v + w) == 01201_w);

    REQUIRE((010_w + 2_w) == 0102_w);
    REQUIRE((0_w + ""_w) == 0_w);
    REQUIRE((""_w + 0_w) == 0_w);

    REQUIRE(w + 7 == 017_w);
    REQUIRE(7 + w == 701_w);
  }

  LIBSEMIGROUPS_TEST_CASE("WordRange",
                          "033",
                          "operator+=",
                          "[quick][word_functions]") {
    using namespace literals;
    using words::operator+=;
    word_type w = 123_w;
    word_type v = 345_w;
    w += v;
    REQUIRE(w == 123345_w);
    word_type t = word_type({});
    w += t;
    REQUIRE(w == 123345_w);

    w = 01_w;
    w += 2_w;
    REQUIRE(w == 012_w);

    w += 7;
    REQUIRE(w == 0127_w);
    7 += w;
    REQUIRE(w == 70127_w);
  }

  LIBSEMIGROUPS_TEST_CASE("WordRange",
                          "034",
                          "pow",
                          "[quick][word_functions]") {
    using namespace literals;
    using words::pow;
    word_type w = 01_w;
    REQUIRE(pow(w, 0) == ""_w);
    REQUIRE(pow(w, 1) == w);
    REQUIRE(pow(w, 2) == 0101_w);
    REQUIRE(pow(pow(w, 2), 3) == 010101010101_w);
    REQUIRE(pow(0_w, 1'000'000) == word_type(1'000'000, 0));
    REQUIRE(pow({0, 1}, 3) == 010101_w);

    REQUIRE(pow("ab", 2) == "abab");
    REQUIRE(pow("a", 5) == "aaaaa");
  }

  LIBSEMIGROUPS_TEST_CASE("WordRange",
                          "035",
                          "pow_inplace",
                          "[quick][word_functions][no-coverage][no-valgrind]") {
    using namespace literals;
    using words::pow_inplace;
    word_type w = 01_w;
    pow_inplace(w, 0);
    REQUIRE(w == word_type({}));

    word_type u = 01_w;
    pow_inplace(u, 1);
    REQUIRE(u == 01_w);
    pow_inplace(u, 2);
    REQUIRE(u == 0101_w);
    pow_inplace(u, 3);
    REQUIRE(u == 010101010101_w);

    for (size_t i = 0; i <= 1'000'000; i += 10'000) {
      word_type v = 0_w;
      pow_inplace(v, i);
      REQUIRE(v == word_type(i, 0));
    }

    std::string x = "ab";
    pow_inplace(x, 2);
    REQUIRE(x == "abab");

    std::string a = "a";
    pow_inplace(a, 5);
    REQUIRE(a == "aaaaa");
  }

  LIBSEMIGROUPS_TEST_CASE("WordRange",
                          "036",
                          "prod",
                          "[quick][word_functions]") {
    using namespace literals;
    using words::pow;
    using words::prod;
    word_type eps = 012345_w;
    REQUIRE(prod(012345_w, 1, 6, 2) == 135_w);
    REQUIRE(prod(012345_w, 0, 6, 1) == 012345_w);
    REQUIRE(prod(012345_w, 5, 0, -1) == 54321_w);
    REQUIRE(prod(012345_w, 5, 3, 1) == ""_w);
    REQUIRE(prod(012345_w, 3, 10, -1) == ""_w);

    REQUIRE(prod(1245_w, 0, 8, 3) == 154_w);
    REQUIRE(prod(01_w, 0, 0, 1) == ""_w);

    REQUIRE(prod("abcdef", 0, 6, 2) == "ace");

    REQUIRE_THROWS_AS(prod(""_w, 0, 1, 1), LibsemigroupsException);

    REQUIRE(prod(""_w, 0, 0, 1) == ""_w);
    REQUIRE(prod(0_w, 1, 1, -1) == ""_w);

    REQUIRE_THROWS_AS(prod({0, 1}, 0, 1, 0), LibsemigroupsException);

    REQUIRE(prod(012345_w, 2, -1, -1) == 210_w);
    REQUIRE(prod(012345_w, -1, -2, -1) == 5_w);
    REQUIRE(prod(0123_w, 0, 16, 3) == 032103_w);

    REQUIRE(prod(0123_w, 16) == pow(0123_w, 4));
    // Test for the function that uses an initializer_list of letters
    REQUIRE(prod({0, 1, 2, 3}, 16) == pow(0123_w, 4));
    REQUIRE(prod(0123_w, -16) == ""_w);
    REQUIRE(prod(012345_w, -1, -2, -1) == 5_w);
    REQUIRE(prod(012345_w, -10, -2, 1) == 23450123_w);

    REQUIRE(prod({010_w, 232_w}, 0, 4, 1) == 010232010232_w);
    REQUIRE(prod({010_w, 232_w}, 4) == 010232010232_w);

    REQUIRE(prod({"aba", "xyz"}, 0, 4, 1) == "abaxyzabaxyz");
    REQUIRE(prod({"aba", "xyz"}, 4) == "abaxyzabaxyz");
    REQUIRE(prod("aba", 4) == "abaa");
  }

  LIBSEMIGROUPS_TEST_CASE("random_word", "037", "", "[quick]") {
    auto w = random_word(10, 3);
    REQUIRE(w.size() == 10);
    REQUIRE(
        std::all_of(w.begin(), w.end(), [](auto const& x) { return x < 3; }));
    REQUIRE_THROWS_AS(random_word(10, 0), LibsemigroupsException);

    REQUIRE_THROWS_AS(random_string("", 5, 6), LibsemigroupsException);
    REQUIRE_THROWS_AS(random_string("abc", 6, 6), LibsemigroupsException);
    REQUIRE_THROWS_AS(random_strings("", 10, 5, 6), LibsemigroupsException);
    REQUIRE_THROWS_AS(random_strings("abc", 10, 6, 6), LibsemigroupsException);
    auto s = random_strings("abc", 100, 3, 5);
    REQUIRE((s | rx::count()) == 100);
    auto result = s | rx::all_of([](auto const& str) {
                    return str.size() >= 3 && str.size() < 5;
                  });
    REQUIRE(result);
  }

  LIBSEMIGROUPS_TEST_CASE("human_readable_index",
                          "038",
                          "check all chars",
                          "[quick]") {
    using words::human_readable_index;
    using words::human_readable_letter;
    std::array<uint8_t,
               1 + std::numeric_limits<char>::max()
                   - std::numeric_limits<char>::min()>
        result;
    std::iota(result.begin(), result.end(), 0);
    auto expected = result;

    std::for_each(result.begin(), result.end(), [](auto& val) {
      val = words::human_readable_index(val);
    });
    std::for_each(result.begin(), result.end(), [](auto& val) {
      val = human_readable_letter<>(val);
    });
    REQUIRE(result == expected);

    std::for_each(result.begin(), result.end(), [](auto& val) {
      val = human_readable_letter<>(val);
    });
    std::for_each(result.begin(), result.end(), [](auto& val) {
      val = words::human_readable_index(val);
    });
    REQUIRE(result == expected);
  }

  LIBSEMIGROUPS_TEST_CASE("to_word", "039", "", "[quick]") {
    ToWord to_word;
    REQUIRE(to_word("abc") == 012_w);
    REQUIRE(to_word("ABC") == word_type({26, 27, 28}));
  }

  LIBSEMIGROUPS_TEST_CASE("ToWord", "040", "code coverage", "[quick]") {
    StringRange strings;
    strings.alphabet("ab").first("a").last("bbbb");

    REQUIRE((strings | ToWord(strings.alphabet()) | to_vector())
            == std::vector<word_type>(
                {0_w,    1_w,    00_w,   01_w,   10_w,   11_w,   000_w,  001_w,
                 010_w,  011_w,  100_w,  101_w,  110_w,  111_w,  0000_w, 0001_w,
                 0010_w, 0011_w, 0100_w, 0101_w, 0110_w, 0111_w, 1000_w, 1001_w,
                 1010_w, 1011_w, 1100_w, 1101_w, 1110_w}));

    ToWord to_words("ba");
    REQUIRE((strings | to_words | to_vector())
            == std::vector<word_type>(
                {1_w,    0_w,    11_w,   10_w,   01_w,   00_w,   111_w,  110_w,
                 101_w,  100_w,  011_w,  010_w,  001_w,  000_w,  1111_w, 1110_w,
                 1101_w, 1100_w, 1011_w, 1010_w, 1001_w, 1000_w, 0111_w, 0110_w,
                 0101_w, 0100_w, 0011_w, 0010_w, 0001_w}));

    ToWord copy(to_words);
    REQUIRE(equal(strings | to_words, strings | copy));

    ToWord move(std::move(copy));
    REQUIRE(equal(strings | to_words, strings | move));

    move.operator=(to_words);
    REQUIRE(equal(strings | to_words, strings | move));

    copy = move;
    move.operator=(std::move(copy));
    REQUIRE(equal(strings | to_words, strings | move));

    REQUIRE((std::move(strings) | to_words | to_vector())
            == std::vector<word_type>(
                {1_w,    0_w,    11_w,   10_w,   01_w,   00_w,   111_w,  110_w,
                 101_w,  100_w,  011_w,  010_w,  001_w,  000_w,  1111_w, 1110_w,
                 1101_w, 1100_w, 1011_w, 1010_w, 1001_w, 1000_w, 0111_w, 0110_w,
                 0101_w, 0100_w, 0011_w, 0010_w, 0001_w}));
  }

  LIBSEMIGROUPS_TEST_CASE("ToString", "041", "code coverage", "[quick]") {
    using words::pow;

    WordRange words;
    words.alphabet_size(2).first(0_w).last(pow(1_w, 3));

    REQUIRE((words | ToString("ba") | to_vector())
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
                                         "aab"}));

    ToString to_string("xy");
    REQUIRE((words | to_string | to_vector())
            == std::vector<std::string>({"x",
                                         "y",
                                         "xx",
                                         "xy",
                                         "yx",
                                         "yy",
                                         "xxx",
                                         "xxy",
                                         "xyx",
                                         "xyy",
                                         "yxx",
                                         "yxy",
                                         "yyx"}));

    ToString copy(to_string);
    REQUIRE(equal(words | to_string, words | copy));

    ToString move(std::move(copy));
    REQUIRE(equal(words | to_string, words | move));

    move.operator=(to_string);
    REQUIRE(equal(words | to_string, words | move));

    copy = move;
    move.operator=(std::move(copy));
    REQUIRE(equal(words | to_string, words | move));

    REQUIRE((std::move(words) | to_string | to_vector())
            == std::vector<std::string>({"x",
                                         "y",
                                         "xx",
                                         "xy",
                                         "yx",
                                         "yy",
                                         "xxx",
                                         "xxy",
                                         "xyx",
                                         "xyy",
                                         "yxx",
                                         "yxy",
                                         "yyx"}));
    using words::pow;
    words.alphabet_size(10).first(pow(0_w, 100)).last(pow(1_w, 1'000));
    for (auto const& s :
         words | ToString("abcdefghij") | skip_n(1'000) | take(1)) {
      REQUIRE(s
              == "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                 "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabaaa");
    }
  }

  LIBSEMIGROUPS_TEST_CASE("WordRange", "042", "empty iterator", "[quick]") {
    using words::pow;

    WordRange words;
    words.alphabet_size(1).first("01"_w).last("11"_w);
    REQUIRE(words.at_end());
    REQUIRE(words.count() == 0);
    REQUIRE((words | to_vector()) == std::vector<word_type>({}));

    words.init();
    words.alphabet_size(0).min(2).max(5);
    REQUIRE(words.at_end());
    REQUIRE(words.count() == 0);
    REQUIRE((words | to_vector()) == std::vector<word_type>({}));
  }

  LIBSEMIGROUPS_TEST_CASE("ToWord", "043", "alphabet", "[quick]") {
    ToWord to_word("BAaC1");
    REQUIRE(to_word.alphabet() == "BAaC1");

    ToWord to_word_2(to_word);
    REQUIRE(to_word_2.alphabet() == "BAaC1");

    ToWord to_word_3(std::move(to_word_2));
    REQUIRE(to_word.alphabet() == "BAaC1");
    REQUIRE(to_word_3.alphabet() == "BAaC1");

    std::string big_alphabet(256, '\0');
    std::iota(big_alphabet.begin(), big_alphabet.end(), 0);
    ToWord big_converter(big_alphabet);
    REQUIRE(big_converter.alphabet() == big_alphabet);
    big_converter.init(detail::chars_in_human_readable_order());
    REQUIRE(big_converter.alphabet()
            == detail::chars_in_human_readable_order());
  }

  LIBSEMIGROUPS_TEST_CASE("ToString", "044", "alphabet", "[quick]") {
    ToString to_string("BAaC1");
    REQUIRE(to_string.alphabet() == "BAaC1");

    ToString to_string_2(to_string);
    REQUIRE(to_string_2.alphabet() == "BAaC1");

    ToString to_string_3(std::move(to_string_2));
    REQUIRE(to_string.alphabet() == "BAaC1");
    REQUIRE(to_string_3.alphabet() == "BAaC1");

    std::string big_alphabet(256, '\0');
    std::iota(big_alphabet.begin(), big_alphabet.end(), 0);
    ToString big_converter(big_alphabet);
    REQUIRE(big_converter.alphabet() == big_alphabet);
    big_converter.init(detail::chars_in_human_readable_order());
    REQUIRE(big_converter.alphabet()
            == detail::chars_in_human_readable_order());
  }

  LIBSEMIGROUPS_TEST_CASE("WordRange", "045", "doxygen examples", "[quick]") {
    // cbegin_wilo
    {
      REQUIRE(std::vector<word_type>(cbegin_wilo(2, 3, {0}, {1, 1, 1}),
                                     cend_wilo(2, 3, {0}, {1, 1, 1}))
              == std::vector<word_type>(
                  {{0}, {0, 0}, {0, 1}, {1}, {1, 0}, {1, 1}}));
    }
    // cbegin_wislo
    {
      REQUIRE(std::vector<word_type>(cbegin_wislo(2, {0}, {0, 0, 0}),
                                     cend_wislo(2, {0}, {0, 0, 0}))
              == std::vector<word_type>(
                  {{0}, {1}, {0, 0}, {0, 1}, {1, 0}, {1, 1}}));
    }
    // ToWord
    {
      ToWord toword("bac");
      REQUIRE(toword("bac") == word_type({0, 1, 2}));
      REQUIRE(toword("bababbbcbc")
              == word_type({0, 1, 0, 1, 0, 0, 0, 2, 0, 2}));

      toword.init();
      REQUIRE(toword("bac") == word_type({1, 0, 2}));
    }
    // ToWord combinator
    {
      StringRange strings;
      strings.alphabet("ab").first("a").last("bbbb");
      auto words = (strings | ToWord("ba"));
      REQUIRE((words | to_vector())
              == std::vector<word_type>({1_w,    0_w,    11_w,   10_w,   01_w,
                                         00_w,   111_w,  110_w,  101_w,  100_w,
                                         011_w,  010_w,  001_w,  000_w,  1111_w,
                                         1110_w, 1101_w, 1100_w, 1011_w, 1010_w,
                                         1001_w, 1000_w, 0111_w, 0110_w, 0101_w,
                                         0100_w, 0011_w, 0010_w, 0001_w}));
    }
    // ToString
    {
      ToString tostring("bac");
      REQUIRE(tostring(word_type({1, 0, 2})) == "abc");
      REQUIRE(tostring(word_type({0, 1, 1, 0, 1, 1, 0, 2})) == "baabaabc");

      tostring.init();
      REQUIRE(tostring(word_type({1, 0, 2})) == "bac");
    }
    // ToString combinator
    {
      WordRange words;
      words.alphabet_size(1).min(0).max(10);

      auto strings = (words | ToString("a"));
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
    }
    // Literals
    {
      REQUIRE(012_w == word_type({0, 1, 2}));
      REQUIRE("abc"_w == word_type({0, 1, 2}));
      REQUIRE("(ab)^3"_p == "ababab");
    }
    {
      using namespace words;
      REQUIRE(pow("a", 5) == "aaaaa");
      REQUIRE(01_w + 2 == 012_w);
      REQUIRE(01_w + 01_w == 0101_w);
      REQUIRE(prod(0123_w, 0, 16, 3) == 032103_w);

      word_type w = 012345_w;
      REQUIRE(prod(w, 0, 5, 2) == word_type({0, 2, 4}));
      REQUIRE(prod(w, 1, 9, 2) == word_type({1, 3, 5, 1}));
      REQUIRE(prod("abcde", 4, 1, -1) == "edc");
      REQUIRE(prod({"aba", "xyz"}, 0, 4, 1) == "abaxyzabaxyz");
    }
  }

  LIBSEMIGROUPS_TEST_CASE("WordRange",
                          "046",
                          "to_human_readable_repr",
                          "[quick]") {
    WordRange wr;
    wr.min(0).max(1).alphabet_size(4);
    REQUIRE(to_human_readable_repr(wr, 120)
            == "<WordRange of length 1 between [] and [0] with letters in "
               "[0, 4) in shortlex order>");
    wr.max(10);
    REQUIRE(to_human_readable_repr(wr)
            == "<WordRange of length 349525 with letters in [0, 4) in shortlex "
               "order>");
  }

  LIBSEMIGROUPS_TEST_CASE("ToWord",
                          "047",
                          "to_human_readable_repr",
                          "[quick]") {
    ToWord to_word("BAc2w");
    REQUIRE(to_human_readable_repr(to_word)
            == "<ToWord object with alphabet \"BAc2w\">");
  }

  LIBSEMIGROUPS_TEST_CASE("StringRange",
                          "048",
                          "to_human_readable_repr",
                          "[quick]") {
    StringRange sr;
    sr.min(3).max(5).alphabet("bcd");
    REQUIRE(to_human_readable_repr(sr, 120)
            == "<StringRange of length 108 between \"bbb\" and \"bbbbb\" with "
               "letters in "
               "\"bcd\" in shortlex order>");
    sr.max(20);
    REQUIRE(to_human_readable_repr(sr)
            == "<StringRange of length 1743392187 in shortlex order>");
  }

  LIBSEMIGROUPS_TEST_CASE("ToString",
                          "049",
                          "to_human_readable_repr",
                          "[quick]") {
    ToString to_string("BAc2w");
    REQUIRE(to_human_readable_repr(to_string)
            == "<ToString object with alphabet \"BAc2w\">");
  }
}  // namespace libsemigroups
