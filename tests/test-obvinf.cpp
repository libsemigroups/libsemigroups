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

// TODO(now):
// 1. add more tests 

#include "libsemigroups/obvinf.hpp"

#include "catch.hpp"  // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "test-main.hpp"

namespace libsemigroups {

  LIBSEMIGROUPS_TEST_CASE("ObviouslyInfinite",
                          "001",
                          "Fails to have free occurence of generator 'a', otherwise has full rank matrix.",
                          "[quick]") {
  detail::IsObviouslyInfinite<char, std::string> ioi(2);
  std::vector<std::string> v = {"ab", "a", "aba", "ba"};
  ioi.add_rules(v.cbegin(), v.cend());
  REQUIRE(ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE("ObviouslyInfinite",
                          "002",
                          "description",
                          "[quick]") {
  detail::IsObviouslyInfinite<char, std::string> ioi(2);
  std::vector<std::string> v = {"aba", "aa", "bb", "b", "abab", "abbba"};
  ioi.add_rules(v.cbegin(), v.cend());
  REQUIRE(ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE("ObviouslyInfinite",
                          "003",
                          "description",
                          "[quick]") {
  detail::IsObviouslyInfinite<char, std::string> ioi(3);
  std::vector<std::string> v = {"aba", "bc", "ca", "b"};
  ioi.add_rules(v.cbegin(), v.cend());
  REQUIRE(ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE("ObviouslyInfinite",
                          "004",
                          "Matrix has non empty kernel",
                          "[quick]") {
  detail::IsObviouslyInfinite<char, std::string> ioi(2);
  std::vector<std::string> v = {"aa", "bba", "bbaa", "bbbbbb"};
  ioi.add_rules(v.cbegin(), v.cend());
  REQUIRE(ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE("ObviouslyInfinite",
                          "005",
                          "Free product of trivial semigroups",
                          "[quick]") {
  detail::IsObviouslyInfinite<char, std::string> ioi(2);
  std::vector<std::string> v = {"a", "aa", "b", "bb"};
  ioi.add_rules(v.cbegin(), v.cend());
  REQUIRE(ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE("ObviouslyInfinite",
                          "006",
                          "Another free product",
                          "[quick]") {
  detail::IsObviouslyInfinite<char, std::string> ioi(5);
  std::vector<std::string> v = {"a", "aa", "b", "bb", "abe", "eee", 
                                "dc", "c", "cc", "ddd"};
  ioi.add_rules(v.cbegin(), v.cend());
  REQUIRE(ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE("ObviouslyInfinite",
                          "007",
                          "Infinite but not obviously so",
                          "[quick]") {
  detail::IsObviouslyInfinite<char, std::string> ioi(2);
  std::vector<std::string> v = {"a", "abb", "b", "baa"};
  ioi.add_rules(v.cbegin(), v.cend());
  REQUIRE(!ioi.result());
  // Currently the test does not pass, but the semigroup
  // is infinite! Contains (ab)^n for all n.
  }

  LIBSEMIGROUPS_TEST_CASE("ObviouslyInfinite",
                          "008",
                          "Test multiple rule additions",
                          "[quick]") {
  detail::IsObviouslyInfinite<char, std::string> ioi(3);
  std::vector<std::string> v = {"aababbaccabbc", "a", "aaabbbbaaabbbbacbbb", "bb"};
  ioi.add_rules(v.cbegin(), v.cend());
  REQUIRE(ioi.result());
  v = {"cc", "bababab"};
  ioi.add_rules(v.cbegin(), v.cend());
  REQUIRE(ioi.result());
  v = {"bbbaaaaabbbaaaaabbbaaaaabbbcccbbbbbbbbb", "bcbab",
       "aaababaabbbccc", "aa", "", "aaaaaaabbbbbbbbbc"};
  ioi.add_rules(v.cbegin(), v.cend());
  REQUIRE(ioi.result());
  v = {"a", "aa"};
  ioi.add_rules(v.cbegin(), v.cend());
  REQUIRE(!ioi.result());
  v = {"b", "bbaa", "caa", "ccbbbaa"};
  ioi.add_rules(v.cbegin(), v.cend());
  REQUIRE(!ioi.result());
  }

  LIBSEMIGROUPS_TEST_CASE("ObviouslyInfinite",
                          "009",
                          "Preserves length",
                          "[quick]") {
  detail::IsObviouslyInfinite<char, std::string> ioi(3);
  std::vector<std::string> v = {"aaa", "bbc", "cccc", "bcba", "bb", "cb", "cba", "bbc"};
  ioi.add_rules(v.cbegin(), v.cend());
  REQUIRE(ioi.result());
  }
}  // namespace libsemigroups
