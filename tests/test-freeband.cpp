//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021-2024 Reinis Cirpons + James Mitchell
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

#include <string>   // for tests with strings
#include <utility>  // for std::move

#include "Catch2-3.7.1/catch_amalgamated.hpp"  // for REQUIRE etc
#include "test-main.hpp"                       // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/freeband.hpp"    // for freeband_equal_to
#include "libsemigroups/types.hpp"       // for word_type
#include "libsemigroups/word-range.hpp"  // for literals

namespace libsemigroups {
  using literals::operator""_w;

  // The following contains useful test cases when checking the right, left,
  // count_sort and radix_sort functions.
  /*
  LIBSEMIGROUPS_TEST_CASE("Test right and left",
                          "000",
                          "",
                          "[freeband][quick]") {
    word_type w = {0, 0, 0, 0, 1, 1, 0, 0, 2};
    REQUIRE(test_right(w.cbegin(), w.cend(), 2)
            == word_type({7, 7, 7, 7, 7, 7, 8, 8, UNDEFINED}));
    w = {0, 0, 0, 0, 1, 1, 0, 0, 2};
    REQUIRE(test_right(w.cbegin(), w.cend(), 3)
            == word_type({8, 8, 8, 8, 8, 8, UNDEFINED, UNDEFINED, UNDEFINED}));
    w = {0, 0, 0, 1, 2, 1, 2, 1, 3, 1, 2, 1, 0, 1, 2, 3};
    REQUIRE(
        test_right(w.cbegin(), w.cend(), 2)
        == word_type(
            {3, 3, 3, 7, 7, 7, 7, 9, 9, 11, 11, 13, 13, 14, 15, UNDEFINED}));

    w = {0, 1, 2, 2, 3, 4, 0, 1, 5, 1};
    REQUIRE(
        test_left(w.cbegin(), w.cend(), 4)
        == word_type(
            {UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED, 0, 1, 2, 4, 5, 5}));

    w = {1, 5, 1, 0, 4, 3, 2, 2, 1, 0};
    detail::standardize(w);
    REQUIRE(
        test_right(w.cbegin(), w.cend(), 4)
        == word_type(
            {4, 4, 5, 7, 8, 9, UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED}));
  }

  LIBSEMIGROUPS_TEST_CASE("Test radix_sort", "001", "", "[freeband][quick]") {
    std::vector<word_type> level_edges = {{0, 0, 0, 0},
                                          {0, 1, 1, 0},
                                          {0, 2, 2, 0},
                                          {0, 0, 0, 0},
                                          {0, 1, 1, 0},
                                          {0, 2, 2, 0}};
    REQUIRE(test_radix_sort(level_edges, 3) == word_type({0, 1, 2, 0, 1, 2}));
    level_edges = std::vector<word_type>(
        6, word_type({UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED}));
    REQUIRE(test_radix_sort(level_edges, 3) == word_type({0, 0, 0, 0, 0, 0}));
    level_edges = {{0, 0, 0, 1},
                   {UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED},
                   {0, 0, 0, 1},
                   {0, 0, 0, 1},
                   {UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED},
                   {0, 0, 0, 1}};
    REQUIRE(test_radix_sort(level_edges, 8) == word_type({0, 1, 0, 0, 1, 0}));
    level_edges = {{1, 2, 0, 5},
                   {1, 2, 0, 5},
                   {1, 2, 0, 5},
                   {5, 3, 3, 5},
                   {8, 3, 3, 5},
                   {5, 3, 3, 5},
                   {8, 3, 3, 5},
                   {7, 2, 3, 5},
                   {10, 2, 3, 5},
                   {5, 0, 0, 4},
                   {8, 0, 0, 4},
                   {2, 2, 0, 4},
                   {1, 2, 0, 4},
                   {4, 3, 1, 9},
                   {UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED},
                   {UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED},
                   {UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED},
                   {UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED},
                   {UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED},
                   {UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED},
                   {1, 1, 0, 4},
                   {1, 1, 0, 5},
                   {1, 1, 0, 4},
                   {1, 1, 0, 5},
                   {5, 1, 2, 6},
                   {5, 1, 2, 7},
                   {5, 1, 3, 4},
                   {5, 1, 3, 5},
                   {5, 1, 2, 3},
                   {5, 1, 2, 2},
                   {5, 1, 0, 4},
                   {4, 3, 1, 9}};
    REQUIRE(test_radix_sort(level_edges, 4)
            == word_type({10, 10, 10, 14, 15, 14, 15, 12, 13, 2, 3,
                          7,  6,  18, 19, 19, 19, 19, 19, 19, 4, 9,
                          4,  9,  16, 17, 8,  11, 1,  0,  5,  18}));
  }
  */

  LIBSEMIGROUPS_TEST_CASE("freeband_equal_to", "002", "", "[freeband][quick]") {
    REQUIRE(freeband_equal_to({}, {}));
    REQUIRE(!freeband_equal_to(00_w, {}));
    REQUIRE(!freeband_equal_to({}, 0_w));
    REQUIRE(freeband_equal_to(00_w, 0_w));
    REQUIRE(!freeband_equal_to(01_w, 0_w));
    REQUIRE(freeband_equal_to(0123210_w, 012323210_w));
    REQUIRE(!freeband_equal_to(123_w, 012_w));
    REQUIRE(freeband_equal_to(142310_w, 14142310_w));
    REQUIRE(!freeband_equal_to(0123401234_w, 4321043210_w));
    REQUIRE(freeband_equal_to(0121012_w, 012_w));
    REQUIRE(freeband_equal_to("032154356329"_w, "032154356329032154356329"_w));
    REQUIRE(freeband_equal_to(012301_w,
                              0123322102102302132123210201020320122301_w));
    REQUIRE(freeband_equal_to(012101230121012_w,
                              01233221021023021321232102010203201223012_w));
    REQUIRE(freeband_equal_to(
        0123031321003221010103133333120100121231131211030102333002033312111201131220023122322322203123012223_w,
        0112333132112303332111003231232131213201122213110103030223223133322233020131310233321221110110301123_w));

    // Check the iterator version of the function works as intended
    std::string a = "abcdba";
    std::string b = "abcdcdba";
    REQUIRE(freeband_equal_to(a.begin(), a.end(), b.begin(), b.end()));
    a = "bcd";
    b = "abc";
    REQUIRE(!freeband_equal_to(a.begin(), a.end(), b.begin(), b.end()));
    a = "adbcZ";
    b = "adadbcZ";
    REQUIRE(freeband_equal_to(a.begin(), a.end(), b.begin(), b.end()));
    word_type w = 010_w, x = 0110_w;
    REQUIRE(freeband_equal_to(std::move(w), std::move(x)));
  }

}  // namespace libsemigroups
