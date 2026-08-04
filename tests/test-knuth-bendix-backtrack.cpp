// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 Joseph Edwards
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

// This file contains tests for the class KnuthBendixBacktrack

#include "Catch2-3.14.0/catch_amalgamated.hpp"  // for AssertionHandler, oper...
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEMPLATE_TEST_CASE

#include "libsemigroups/presentation.hpp"  // for add_rule

#include "libsemigroups/detail/knuth-bendix-backtrack-impl.hpp"  // for ValueGuard::Va...

namespace libsemigroups {
  using literals::operator""_w;

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixBacktrack",
                          "000",
                          "simple test 0",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("abc");
    presentation::add_rule(p, "baa", "c");
    presentation::add_rule(p, "aba", "cc");

    detail::KnuthBendixBacktrack kbb(p, 100, 20);

    Presentation<std::string> expected;
    expected.alphabet("abc");
    expected.rules = {"bccccccba",
                      "cbccba",
                      "bcccccca",
                      "cbcca",
                      "bcccca",
                      "cba",
                      "bcccccc",
                      "cbcc",
                      "ac",
                      "cca",
                      "abcc",
                      "ccba",
                      "aba",
                      "cc",
                      "baa",
                      "c"};

    Presentation<std::string> output(*kbb);
    presentation::sort_rules(output);
    presentation::sort_rules(expected);
    REQUIRE(output == expected);

    ++kbb;
    expected.rules = {"abcbcccccbcccca",
                      "ccbccbcc",
                      "bcccccbcccccbcccca",
                      "cbcbcccccbcccca",
                      "abcbcccca",
                      "ccbcc",
                      "cbcbcccca",
                      "bcccccbcccca",
                      "bccccaa",
                      "cc",
                      "cba",
                      "bcccca",
                      "bcccccc",
                      "cbcc",
                      "ac",
                      "cca",
                      "abcc",
                      "ccba",
                      "aba",
                      "cc",
                      "baa",
                      "c"};
    output         = *kbb;
    presentation::sort_rules(output);
    presentation::sort_rules(expected);
    REQUIRE(output == expected);

    ++kbb;
    expected.rules = {"abcbcccca",
                      "ccbcc",
                      "bcccccbcccca",
                      "cbcbcccca",
                      "bccccaa",
                      "cc",
                      "cba",
                      "bcccca",
                      "bcccccc",
                      "cbcc",
                      "ac",
                      "cca",
                      "abcc",
                      "ccba",
                      "aba",
                      "cc",
                      "baa",
                      "c"};
    output         = *kbb;
    presentation::sort_rules(output);
    presentation::sort_rules(expected);
    REQUIRE(output == expected);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixBacktrack",
                          "001",
                          "simple test 1",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "a", "b");
    detail::KnuthBendixBacktrack kbb(p, 100, 10);

    Presentation<std::string> expected;
    expected.alphabet("ab");
    expected.rules = {"a", "b"};

    REQUIRE(*kbb == expected);

    ++kbb;
    expected.rules = {"b", "a"};
    REQUIRE(*kbb == expected);
  }

}  // namespace libsemigroups
