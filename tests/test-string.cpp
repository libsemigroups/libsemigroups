// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021 James D. Mitchell + Maria Tsalakou
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

// Write tests for maximum_common_suffix

#include <string>  // for string

#include "catch.hpp"      // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/string.hpp"  // for is_prefix, is_suffix, maximum_common_suffix

namespace libsemigroups {
  LIBSEMIGROUPS_TEST_CASE("string",
                          "000",
                          "detail::is_suffix",
                          "[quick][string]") {
    REQUIRE(detail::is_suffix("ababaaa", ""));
    REQUIRE(detail::is_suffix("ababaaa", "a"));
    REQUIRE(detail::is_suffix("ababaaa", "aa"));
    REQUIRE(detail::is_suffix("ababaaa", "aaa"));
    REQUIRE(detail::is_suffix("ababaaa", "baaa"));
    REQUIRE(detail::is_suffix("ababaaa", "abaaa"));
    REQUIRE(detail::is_suffix("ababaaa", "babaaa"));
    REQUIRE(detail::is_suffix("ababaaa", "ababaaa"));
    REQUIRE(!detail::is_suffix("ababaaa", "bab"));
    REQUIRE(!detail::is_suffix("ababaaa", "xxxx"));
    REQUIRE(!detail::is_suffix("ababaaa", "babaaax"));
  }

  LIBSEMIGROUPS_TEST_CASE("string",
                          "001",
                          "detail::is_prefix",
                          "[quick][string]") {
    REQUIRE(detail::is_prefix("ababaaa", ""));
    REQUIRE(detail::is_prefix("ababaaa", "a"));
    REQUIRE(detail::is_prefix("ababaaa", "ab"));
    REQUIRE(detail::is_prefix("ababaaa", "aba"));
    REQUIRE(detail::is_prefix("ababaaa", "abab"));
    REQUIRE(detail::is_prefix("ababaaa", "ababa"));
    REQUIRE(detail::is_prefix("ababaaa", "ababaa"));
    REQUIRE(detail::is_prefix("ababaaa", "ababaaa"));
    REQUIRE(!detail::is_prefix("ababaaa", "bab"));
    REQUIRE(!detail::is_prefix("ababaaa", "xxxx"));
    REQUIRE(!detail::is_prefix("ababaaa", "ababaax"));
  }

  LIBSEMIGROUPS_TEST_CASE("string",
                          "002",
                          "detail::maximum_common_suffix",
                          "[quick][string]") {
    REQUIRE(detail::maximum_common_suffix("ababaaa", "asdadskjaldkjaa")
            == "aa");
    REQUIRE(detail::maximum_common_suffix("baaa", "baaa") == "baaa");
    REQUIRE(detail::maximum_common_suffix("baaa", "cccc") == "");
    REQUIRE(detail::maximum_common_suffix("baaabaaabaaabaaabaabaabaaa",
                                          "baaabaaabaabaabaaax")
            == "");
    REQUIRE(detail::maximum_common_suffix("baaabaaabaaabaaabaabaabaaa",
                                          "xbaaabaaabaabaabaaa")
            == "baaabaaabaabaabaaa");
  }
}  // namespace libsemigroups
