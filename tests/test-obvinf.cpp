// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020 Reinis Cirpons
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

// TODO(later):
// 1. add more tests

#include <iostream>

#include "libsemigroups/obvinf.hpp"

#include "catch.hpp"  // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "test-main.hpp"

namespace libsemigroups {
  // TODO(v2): uncomment these tests or remove them
  /*LIBSEMIGROUPS_TEST_CASE("ObviouslyInfinite",
                          "001",
                          "Multiple rule additions",
                          "[quick]") {
    detail::IsObviouslyInfinite<char, std::string> ioi(3);
    std::vector<std::string>                       v
        = {"aababbaccabbc", "a", "aaabbbbaaabbbbacbbb", "bb"};
    ioi.add_rules(v.cbegin(), v.cend());
    REQUIRE(ioi.result());
    v = {"cc", "bababab"};
    ioi.add_rules(v.cbegin(), v.cend());
    REQUIRE(ioi.result());
    v = {"bbbaaaaabbbaaaaabbbaaaaabbbcccbbbbbbbbb",
         "bcbab",
         "aaababaabbbccc",
         "aa",
         "",
         "aaaaaaabbbbbbbbbc"};
    ioi.add_rules(v.cbegin(), v.cend());
    REQUIRE(ioi.result());
    v = {"a", "aa"};
    ioi.add_rules(v.cbegin(), v.cend());
    REQUIRE(!ioi.result());
    v = {"b", "bbaa", "caa", "ccbbbaa"};
    ioi.add_rules(v.cbegin(), v.cend());
    REQUIRE(!ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE(
      "ObviouslyInfinite",
      "002",
      "A power of the generator 'b' does not occur on its own in any relation",
      "[quick]") {
    detail::IsObviouslyInfinite<char, std::string> ioi(2);
    std::vector<std::string>                       v = {"ab", "a", "aba", "ba"};
    ioi.add_rules(v.cbegin(), v.cend());
    REQUIRE(ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE(
      "ObviouslyInfinite",
      "003",
      "Preserves the number of occurences of the generator 'a'",
      "[quick]") {
    detail::IsObviouslyInfinite<char, std::string> ioi(2);
    std::vector<std::string> v = {"aba", "aa", "bb", "b", "abab", "abbba"};
    ioi.add_rules(v.cbegin(), v.cend());
    REQUIRE(ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE("ObviouslyInfinite",
                          "004",
                          "Less relations than generators",
                          "[quick]") {
    detail::IsObviouslyInfinite<char, std::string> ioi(3);
    std::vector<std::string>                       v = {"aba", "bc", "ca", "b"};
    ioi.add_rules(v.cbegin(), v.cend());
    REQUIRE(ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE("ObviouslyInfinite",
                          "005",
                          "Relations preserve length",
                          "[quick]") {
    detail::IsObviouslyInfinite<char, std::string> ioi(3);
    std::vector<std::string>                       v
        = {"aaa", "bbc", "cccc", "bcba", "bb", "cb", "cba", "bbc"};
    ioi.add_rules(v.cbegin(), v.cend());
    REQUIRE(ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE("ObviouslyInfinite",
                          "006",
                          "Matrix has non empty kernel",
                          "[quick]") {
    detail::IsObviouslyInfinite<char, std::string> ioi(2);
    std::vector<std::string> v = {"aa", "bba", "bbaa", "bbbbbb"};
    ioi.add_rules(v.cbegin(), v.cend());
    REQUIRE(ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE("ObviouslyInfinite",
                          "007",
                          "Free product of trivial semigroups",
                          "[quick]") {
    detail::IsObviouslyInfinite<char, std::string> ioi(2);
    std::vector<std::string>                       v = {"a", "aa", "b", "bb"};
    ioi.add_rules(v.cbegin(), v.cend());
    REQUIRE(ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE("ObviouslyInfinite",
                          "008",
                          "Another free product",
                          "[quick]") {
    detail::IsObviouslyInfinite<char, std::string> ioi(5);
    std::vector<std::string>                       v
        = {"a", "aa", "b", "bb", "abe", "eee", "dc", "c", "cc", "ddd"};
    ioi.add_rules(v.cbegin(), v.cend());
    REQUIRE(ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE("ObviouslyInfinite",
                          "009",
                          "Infinite but not obviously so",
                          "[quick]") {
    detail::IsObviouslyInfinite<char, std::string> ioi(2);
    std::vector<std::string>                       v = {"a", "abb", "b", "baa"};
    ioi.add_rules(v.cbegin(), v.cend());
    REQUIRE(!ioi.result());
    // Currently the test does not pass, but the semigroup
    // is infinite! Contains (ab)^n for all n.
  }

  LIBSEMIGROUPS_TEST_CASE("ObviouslyInfinite",
                          "010",
                          "Finite semigroup",
                          "[quick]") {
    detail::IsObviouslyInfinite<char, std::string> ioi(3);
    std::vector<std::string>                       v
        = {"a", "aa", "b", "bb", "", "cc", "ac", "cb", "abab", "ab"};
    ioi.add_rules(v.cbegin(), v.cend());
    REQUIRE(!ioi.result());
    // This is a presentation for a finite semigroup so
    // we should never detect it as obviously infinite
  }*/

  LIBSEMIGROUPS_TEST_CASE("ObviouslyInfinite",
                          "011",
                          "Multiple rule additions",
                          "[quick][integer-alphabet]") {
    detail::IsObviouslyInfinite ioi(3);
    std::vector<word_type>      v
        = {{0, 0, 1, 0, 1, 1, 0, 2, 2, 0, 1, 1, 2},
           {0},
           {0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 2, 1, 1, 1},
           {1, 1}};
    ioi.add_rules(v.cbegin(), v.cend());
    REQUIRE(ioi.result());
    v = {{2, 2}, {1, 0, 1, 0, 1, 0, 1}};
    ioi.add_rules(v.cbegin(), v.cend());
#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
    REQUIRE(ioi.result());
#else
    REQUIRE(!ioi.result());
#endif

    v = {{1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0,
          0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1},
         {1, 2, 1, 0, 1},
         {0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 2, 2, 2},
         {0, 0},
         {},
         {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2}};
    ioi.add_rules(v.cbegin(), v.cend());
#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
    REQUIRE(ioi.result());
#else
    REQUIRE(!ioi.result());
#endif
    v = {{0}, {0, 0}};
    ioi.add_rules(v.cbegin(), v.cend());
    REQUIRE(!ioi.result());
    v = {{1}, {1, 1, 0, 0}, {2, 0, 0}, {2, 2, 1, 1, 1, 0, 0}};
    ioi.add_rules(v.cbegin(), v.cend());
    REQUIRE(!ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE(
      "ObviouslyInfinite",
      "012",
      "A power of the generator 'b' does not occur on its own in any relation",
      "[quick][integer-alphabet]") {
    detail::IsObviouslyInfinite ioi(2);
    std::vector<word_type>      v = {{0, 1}, {0}, {0, 1, 0}, {1, 0}};
    ioi.add_rules(v.cbegin(), v.cend());
    REQUIRE(ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE(
      "ObviouslyInfinite",
      "013",
      "Preserves the number of occurences of the generator 'a'",
      "[quick][integer-alphabet]") {
    detail::IsObviouslyInfinite ioi(2);
    std::vector<word_type>      v
        = {{0, 1, 0}, {0, 0}, {1, 1}, {1}, {0, 1, 0, 1}, {0, 1, 1, 1, 0}};
    ioi.add_rules(v.cbegin(), v.cend());
    REQUIRE(ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE("ObviouslyInfinite",
                          "014",
                          "Less relations than generators",
                          "[quick][integer-alphabet]") {
    detail::IsObviouslyInfinite ioi(3);
    std::vector<word_type>      v = {{0, 1, 0}, {1, 2}, {2, 0}, {1}};
    ioi.add_rules(v.cbegin(), v.cend());
    REQUIRE(ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE("ObviouslyInfinite",
                          "015",
                          "Relations preserve length",
                          "[quick][integer-alphabet]") {
    detail::IsObviouslyInfinite ioi(3);
    std::vector<word_type>      v = {{0, 0, 0},
                                {1, 1, 2},
                                {2, 2, 2, 2},
                                {1, 2, 1, 0},
                                {1, 1},
                                {2, 1},
                                {2, 1, 0},
                                {1, 1, 2}};
    ioi.add_rules(v.cbegin(), v.cend());
    REQUIRE(ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE("ObviouslyInfinite",
                          "016",
                          "Matrix has non empty kernel",
                          "[quick][integer-alphabet]") {
    detail::IsObviouslyInfinite ioi(2);
    std::vector<word_type>      v
        = {{0, 0}, {1, 1, 0}, {1, 1, 0, 0}, {1, 1, 1, 1, 1, 1}};
    ioi.add_rules(v.cbegin(), v.cend());
#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
    REQUIRE(ioi.result());
#else
    REQUIRE(!ioi.result());
#endif
  }

  LIBSEMIGROUPS_TEST_CASE("ObviouslyInfinite",
                          "017",
                          "Free product of trivial semigroups",
                          "[quick][integer-alphabet]") {
    detail::IsObviouslyInfinite ioi(2);
    std::vector<word_type>      v = {{0}, {0, 0}, {1}, {1, 1}};
    ioi.add_rules(v.cbegin(), v.cend());
    REQUIRE(ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE("ObviouslyInfinite",
                          "018",
                          "Another free product",
                          "[quick][integer-alphabet]") {
    detail::IsObviouslyInfinite ioi(5);
    std::vector<word_type>      v = {{0},
                                {0, 0},
                                {1},
                                {1, 1},
                                {0, 1, 4},
                                {4, 4, 4},
                                {3, 2},
                                {2},
                                {2, 2},
                                {3, 3, 3}};
    ioi.add_rules(v.cbegin(), v.cend());
    REQUIRE(ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE("ObviouslyInfinite",
                          "019",
                          "Infinite but not obviously so",
                          "[quick][integer-alphabet]") {
    detail::IsObviouslyInfinite ioi(2);
    std::vector<word_type>      v = {{0}, {0, 1, 1}, {1}, {1, 0, 0}};
    ioi.add_rules(v.cbegin(), v.cend());
    REQUIRE(!ioi.result());
    // Currently the test does not pass, but the semigroup
    // is infinite! Contains (ab)^n for all n.
  }

  LIBSEMIGROUPS_TEST_CASE("ObviouslyInfinite",
                          "020",
                          "Finite semigroup",
                          "[quick][integer-alphabet]") {
    detail::IsObviouslyInfinite ioi(3);
    std::vector<word_type>      v = {{0},
                                {0, 0},
                                {1},
                                {1, 1},
                                {},
                                {2, 2},
                                {0, 2},
                                {2, 1},
                                {0, 1, 0, 1},
                                {0, 1}};
    ioi.add_rules(v.cbegin(), v.cend());
    REQUIRE(!ioi.result());
    // This is a presentation for a finite semigroup so
    // we should never detect it as obviously infinite
  }
}  // namespace libsemigroups
