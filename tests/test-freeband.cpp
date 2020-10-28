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

#include <random>  // for mt19937, random_device

#include "catch.hpp"      // for REQUIRE etc
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/constants.hpp"
#include "libsemigroups/freeband.hpp"

namespace libsemigroups {

  word_type random_word(size_t length, size_t nr_letters) {
    static std::random_device               rd;
    static std::mt19937                     gen(rd());
    std::uniform_int_distribution<uint64_t> dist(0, nr_letters);
    word_type                               out;
    for (size_t i = 0; i < length; ++i) {
      out.push_back(dist(gen));
    }
    return out;
  }

  LIBSEMIGROUPS_TEST_CASE("freeband_equal_to", "001", "", "[freeband][quick]") {
    word_type w = {0, 0, 0, 0, 1, 1, 0, 0, 2};
    REQUIRE(right(w.cbegin(), w.cend(), 2)
            == word_type({7, 7, 7, 7, 7, 7, 8, 8, UNDEFINED}));
    w = {0, 0, 0, 0, 1, 1, 0, 0, 2};
    REQUIRE(right(w.cbegin(), w.cend(), 3)
            == word_type({8, 8, 8, 8, 8, 8, UNDEFINED, UNDEFINED, UNDEFINED}));
    w = {0, 0, 0, 1, 2, 1, 2, 1, 3, 1, 2, 1, 0, 1, 2, 3};
    REQUIRE(
        right(w.cbegin(), w.cend(), 2)
        == word_type(
            {3, 3, 3, 7, 7, 7, 7, 9, 9, 11, 11, 13, 13, 14, 15, UNDEFINED}));

    w = {0, 1, 2, 2, 3, 4, 0, 1, 5, 1};
    REQUIRE(
        left(w.cbegin(), w.cend(), 4)
        == word_type(
            {UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED, 0, 1, 2, 4, 5, 5}));

    w = {1, 5, 1, 0, 4, 3, 2, 2, 1, 0};
    standardize(w);
    REQUIRE(
        right(w.cbegin(), w.cend(), 4)
        == word_type(
            {4, 4, 5, 7, 8, 9, UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED}));
  }

  LIBSEMIGROUPS_TEST_CASE("Test radix_sort", "002", "", "[freeband][quick]") {
    std::vector<word_type> level_edges = {{0, 0, 0, 0},
                                          {0, 1, 1, 0},
                                          {0, 2, 2, 0},
                                          {0, 0, 0, 0},
                                          {0, 1, 1, 0},
                                          {0, 2, 2, 0}};
    REQUIRE(radix_sort(level_edges, 3) == word_type({0, 1, 2, 0, 1, 2}));
    level_edges = std::vector<word_type>(
        6, word_type({UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED}));
    REQUIRE(radix_sort(level_edges, 3) == word_type({0, 0, 0, 0, 0, 0}));
    level_edges = {{0, 0, 0, 1},
                   {UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED},
                   {0, 0, 0, 1},
                   {0, 0, 0, 1},
                   {UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED},
                   {0, 0, 0, 1}};
    REQUIRE(radix_sort(level_edges, 8) == word_type({0, 1, 0, 0, 1, 0}));
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
    REQUIRE(radix_sort(level_edges, 4)
            == word_type({10, 10, 10, 14, 15, 14, 15, 12, 13, 2, 3,
                          7,  6,  18, 19, 19, 19, 19, 19, 19, 4, 9,
                          4,  9,  16, 17, 8,  11, 1,  0,  5,  18}));
  }

  LIBSEMIGROUPS_TEST_CASE("freeband_equal_to", "004", "", "[freeband][quick]") {
    REQUIRE(freeband_equal_to({0, 0}, {0}));
    REQUIRE(!freeband_equal_to({0, 1}, {0}));
    REQUIRE(
        freeband_equal_to({0, 1, 2, 3, 2, 1, 0}, {0, 1, 2, 3, 2, 3, 2, 1, 0}));
    REQUIRE(!freeband_equal_to({1, 2, 3}, {0, 1, 2}));
    REQUIRE(freeband_equal_to({1, 4, 2, 3, 10}, {1, 4, 1, 4, 2, 3, 10}));
    REQUIRE(!freeband_equal_to({0, 1, 2, 3, 4, 0, 1, 2, 3, 4},
                               {4, 3, 2, 1, 0, 4, 3, 2, 1, 0}));
    REQUIRE(freeband_equal_to({0, 1, 2, 1, 0, 1, 2}, {0, 1, 2}));
    REQUIRE(freeband_equal_to({0, 3, 2, 1, 5, 4, 3, 5, 6, 3, 2, 9},
                              {0, 3, 2, 1, 5, 4, 3, 5, 6, 3, 2, 9,
                               0, 3, 2, 1, 5, 4, 3, 5, 6, 3, 2, 9}));
    REQUIRE(freeband_equal_to({0, 1, 2, 3, 0, 1},
                              {0, 1, 2, 3, 3, 2, 2, 1, 0, 2, 1, 0, 2, 3,
                               0, 2, 1, 3, 2, 1, 2, 3, 2, 1, 0, 2, 0, 1,
                               0, 2, 0, 3, 2, 0, 1, 2, 2, 3, 0, 1}));
    REQUIRE(freeband_equal_to({0, 1, 2, 1, 0, 1, 2, 3, 0, 1, 2, 1, 0, 1, 2},
                              {0, 1, 2, 3, 3, 2, 2, 1, 0, 2, 1, 0, 2, 3,
                               0, 2, 1, 3, 2, 1, 2, 3, 2, 1, 0, 2, 0, 1,
                               0, 2, 0, 3, 2, 0, 1, 2, 2, 3, 0, 1, 2}));
    REQUIRE(freeband_equal_to(
        {0, 1, 2, 3, 0, 3, 1, 3, 2, 1, 0, 0, 3, 2, 2, 1, 0, 1, 0, 1,
         0, 3, 1, 3, 3, 3, 3, 3, 1, 2, 0, 1, 0, 0, 1, 2, 1, 2, 3, 1,
         1, 3, 1, 2, 1, 1, 0, 3, 0, 1, 0, 2, 3, 3, 3, 0, 0, 2, 0, 3,
         3, 3, 1, 2, 1, 1, 1, 2, 0, 1, 1, 3, 1, 2, 2, 0, 0, 2, 3, 1,
         2, 2, 3, 2, 2, 3, 2, 2, 2, 0, 3, 1, 2, 3, 0, 1, 2, 2, 2, 3},
        {0, 1, 1, 2, 3, 3, 3, 1, 3, 2, 1, 1, 2, 3, 0, 3, 3, 3, 2, 1,
         1, 1, 0, 0, 3, 2, 3, 1, 2, 3, 2, 1, 3, 1, 2, 1, 3, 2, 0, 1,
         1, 2, 2, 2, 1, 3, 1, 1, 0, 1, 0, 3, 0, 3, 0, 2, 2, 3, 2, 2,
         3, 1, 3, 3, 3, 2, 2, 2, 3, 3, 0, 2, 0, 1, 3, 1, 3, 1, 0, 2,
         3, 3, 3, 2, 1, 2, 2, 1, 1, 1, 0, 1, 1, 0, 3, 0, 1, 1, 2, 3}));
  }

}  // namespace libsemigroups
