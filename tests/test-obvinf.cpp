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

#include "catch_amalgamated.hpp"  // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "libsemigroups/exception.hpp"
#include "test-main.hpp"

#include "libsemigroups/obvinf.hpp"      // for IsObviouslyInfinite
#include "libsemigroups/word-range.hpp"  // for operator""_w

namespace libsemigroups {
  namespace {
    void require_true_if_eigen_enabled(IsObviouslyInfinite& ioi) {
#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
      REQUIRE(ioi.result());
#else
      REQUIRE(!ioi.result());
#endif
    }
  }  // namespace

  using namespace literals;

  LIBSEMIGROUPS_TEST_CASE_V3("ObviouslyInfinite",
                             "001",
                             "Multiple rule additions",
                             "[quick]") {
    IsObviouslyInfinite      ioi(3);
    std::vector<std::string> v
        = {"aababbaccabbc", "a", "aaabbbbaaabbbbacbbb", "bb"};

    REQUIRE_THROWS_AS(ioi.add_rules_no_checks("ab", v.cbegin(), v.cend()),
                      LibsemigroupsException);
    ioi.add_rules_no_checks("abc", v.cbegin(), v.cend());
    REQUIRE(ioi.result());
    v = {"cc", "bababab"};
    ioi.add_rules_no_checks("abc", v.cbegin(), v.cend());
    v = {"bbbaaaaabbbaaaaabbbaaaaabbbcccbbbbbbbbb",
         "bcbab",
         "aaababaabbbccc",
         "aa",
         "",
         "aaaaaaabbbbbbbbbc"};
    ioi.add_rules_no_checks("abc", v.cbegin(), v.cend());

    require_true_if_eigen_enabled(ioi);

    v = {"a", "aa"};
    ioi.add_rules_no_checks("abc", v.cbegin(), v.cend());
    REQUIRE(!ioi.result());
    v = {"b", "bbaa", "caa", "ccbbbaa"};
    ioi.add_rules_no_checks("abc", v.cbegin(), v.cend());
    REQUIRE(!ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE_V3(
      "ObviouslyInfinite",
      "002",
      "A power of the generator 'b' does not occur on its own in any relation",
      "[quick]") {
    IsObviouslyInfinite      ioi(2);
    std::vector<std::string> v = {"ab", "a", "aba", "ba"};
    ioi.add_rules_no_checks("ab", v.cbegin(), v.cend());
    REQUIRE(ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE_V3(
      "ObviouslyInfinite",
      "003",
      "Preserves the number of occurrences of the generator 'a'",
      "[quick]") {
    IsObviouslyInfinite      ioi(2);
    std::vector<std::string> v = {"aba", "aa", "bb", "b", "abab", "abbba"};
    ioi.add_rules_no_checks("ab", v.cbegin(), v.cend());
    REQUIRE(ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE_V3("ObviouslyInfinite",
                             "004",
                             "Less relations than generators",
                             "[quick]") {
    IsObviouslyInfinite      ioi(3);
    std::vector<std::string> v = {"aba", "bc", "ca", "b"};
    ioi.add_rules_no_checks("abc", v.cbegin(), v.cend());
    REQUIRE(ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE_V3("ObviouslyInfinite",
                             "005",
                             "Relations preserve length",
                             "[quick]") {
    IsObviouslyInfinite      ioi(3);
    std::vector<std::string> v
        = {"aaa", "bbc", "cccc", "bcba", "bb", "cb", "cba", "bbc"};
    ioi.add_rules_no_checks("abc", v.cbegin(), v.cend());
    REQUIRE(ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE_V3("ObviouslyInfinite",
                             "006",
                             "Matrix has non empty kernel",
                             "[quick]") {
    IsObviouslyInfinite    ioi(2);
    std::vector<word_type> vv = {"aa"_w, "bba"_w, "bbaa"_w, "bbbbbb"_w};
    ioi.add_rules_no_checks(vv.cbegin(), vv.cend());

    require_true_if_eigen_enabled(ioi);

    ioi.init(2);
    std::vector<std::string> v = {"aa", "bba", "bbaa", "bbbbbb"};
    ioi.add_rules_no_checks("ab", v.cbegin(), v.cend());

    require_true_if_eigen_enabled(ioi);
  }

  LIBSEMIGROUPS_TEST_CASE_V3("ObviouslyInfinite",
                             "007",
                             "Free product of trivial semigroups",
                             "[quick]") {
    IsObviouslyInfinite      ioi(2);
    std::vector<std::string> v = {"a", "aa", "b", "bb"};
    ioi.add_rules_no_checks("ab", v.cbegin(), v.cend());
    REQUIRE(ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE_V3("ObviouslyInfinite",
                             "008",
                             "Another free product",
                             "[quick]") {
    IsObviouslyInfinite      ioi(5);
    std::vector<std::string> v
        = {"a", "aa", "b", "bb", "abe", "eee", "dc", "c", "cc", "ddd"};
    ioi.add_rules_no_checks("abcde", v.cbegin(), v.cend());
    REQUIRE(ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE_V3("ObviouslyInfinite",
                             "009",
                             "Infinite but not obviously so",
                             "[quick]") {
    IsObviouslyInfinite      ioi(2);
    std::vector<std::string> v = {"a", "abb", "b", "baa"};
    ioi.add_rules_no_checks("ab", v.cbegin(), v.cend());
    REQUIRE(!ioi.result());
    // Currently the test does not pass, but the semigroup
    // is infinite! Contains (ab)^n for all n.
  }

  LIBSEMIGROUPS_TEST_CASE_V3("ObviouslyInfinite",
                             "010",
                             "Finite semigroup",
                             "[quick]") {
    IsObviouslyInfinite      ioi(3);
    std::vector<std::string> v
        = {"a", "aa", "b", "bb", "", "cc", "ac", "cb", "abab", "ab"};
    ioi.add_rules_no_checks("abc", v.cbegin(), v.cend());
    REQUIRE(!ioi.result());
    // This is a presentation for a finite semigroup so
    // we should never detect it as obviously infinite
  }

  LIBSEMIGROUPS_TEST_CASE_V3("ObviouslyInfinite",
                             "011",
                             "Multiple rule additions",
                             "[quick][integer-alphabet]") {
    IsObviouslyInfinite    ioi(3);
    auto                   w = 00_w;
    std::vector<word_type> v
        = {0010110220112_w, 0_w, 0001111000111102111_w, 11_w};
    ioi.add_rules_no_checks(v.cbegin(), v.cend());
    REQUIRE(ioi.result());
    v = {22_w, 1010101_w};
    ioi.add_rules_no_checks(v.cbegin(), v.cend());
    require_true_if_eigen_enabled(ioi);

    v = {111000001110000011100000111222111111111_w,
         12101_w,
         00010100111222_w,
         00_w,
         {},
         00000001111111112_w};
    ioi.add_rules_no_checks(v.cbegin(), v.cend());
    require_true_if_eigen_enabled(ioi);
    v = {0_w, 00_w};
    ioi.add_rules_no_checks(v.cbegin(), v.cend());
    REQUIRE(!ioi.result());
    v = {1_w, 1100_w, 200_w, 2211100_w};
    ioi.add_rules_no_checks(v.cbegin(), v.cend());
    REQUIRE(!ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE_V3(
      "ObviouslyInfinite",
      "012",
      "A power of the generator 'b' does not occur on its own in any relation",
      "[quick][integer-alphabet]") {
    IsObviouslyInfinite    ioi(2);
    std::vector<word_type> v = {01_w, 0_w, 010_w, 10_w};
    ioi.add_rules_no_checks(v.cbegin(), v.cend());
    REQUIRE(ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE_V3(
      "ObviouslyInfinite",
      "013",
      "Preserves the number of occurrences of the generator 'a'",
      "[quick][integer-alphabet]") {
    IsObviouslyInfinite    ioi(2);
    std::vector<word_type> v = {010_w, 00_w, 11_w, 1_w, 0101_w, 01110_w};
    ioi.add_rules_no_checks(v.cbegin(), v.cend());
    REQUIRE(ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE_V3("ObviouslyInfinite",
                             "014",
                             "Less relations than generators",
                             "[quick][integer-alphabet]") {
    IsObviouslyInfinite    ioi(3);
    std::vector<word_type> v = {010_w, 12_w, 20_w, 1_w};
    ioi.add_rules_no_checks(v.cbegin(), v.cend());
    REQUIRE(ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE_V3("ObviouslyInfinite",
                             "015",
                             "Relations preserve length",
                             "[quick][integer-alphabet]") {
    IsObviouslyInfinite    ioi(3);
    std::vector<word_type> v
        = {000_w, 112_w, 2222_w, 1210_w, 11_w, 21_w, 210_w, 112_w};
    ioi.add_rules_no_checks(v.cbegin(), v.cend());
    REQUIRE(ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE_V3("ObviouslyInfinite",
                             "016",
                             "Matrix has non empty kernel",
                             "[quick][integer-alphabet]") {
    IsObviouslyInfinite    ioi(2);
    std::vector<word_type> v = {00_w, 110_w, 1100_w, 111111_w};
    ioi.add_rules_no_checks(v.cbegin(), v.cend());
    require_true_if_eigen_enabled(ioi);
  }

  LIBSEMIGROUPS_TEST_CASE_V3("ObviouslyInfinite",
                             "017",
                             "Free product of trivial semigroups",
                             "[quick][integer-alphabet]") {
    IsObviouslyInfinite    ioi(2);
    std::vector<word_type> v = {0_w, 00_w, 1_w, 11_w};
    ioi.add_rules_no_checks(v.cbegin(), v.cend());
    REQUIRE(ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE_V3("ObviouslyInfinite",
                             "018",
                             "Another free product",
                             "[quick][integer-alphabet]") {
    IsObviouslyInfinite    ioi(5);
    std::vector<word_type> v
        = {0_w, 00_w, 1_w, 11_w, 014_w, 444_w, 32_w, 2_w, 22_w, 333_w};
    ioi.add_rules_no_checks(v.cbegin(), v.cend());
    REQUIRE(ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE_V3("ObviouslyInfinite",
                             "019",
                             "Infinite but not obviously so",
                             "[quick][integer-alphabet]") {
    IsObviouslyInfinite    ioi(2);
    std::vector<word_type> v = {0_w, 011_w, 1_w, 100_w};
    ioi.add_rules_no_checks(v.cbegin(), v.cend());
    REQUIRE(!ioi.result());
    // Currently the test does not pass, but the semigroup
    // is infinite! Contains (ab)^n for all n.
  }

  LIBSEMIGROUPS_TEST_CASE_V3("ObviouslyInfinite",
                             "020",
                             "Finite semigroup",
                             "[quick][integer-alphabet]") {
    IsObviouslyInfinite    ioi(3);
    std::vector<word_type> v
        = {0_w, 00_w, 1_w, 11_w, {}, 22_w, 02_w, 21_w, 0101_w, 01_w};
    ioi.add_rules_no_checks(v.cbegin(), v.cend());
    REQUIRE(!ioi.result());
    // This is a presentation for a finite semigroup so
    // we should never detect it as obviously infinite
  }
}  // namespace libsemigroups
