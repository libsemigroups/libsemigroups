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

#include <type_traits>  // for is_default_constructible_v, is_copy_constructi...

#include "libsemigroups/presentation.hpp"  // for Presentation, presentation...

#include "libsemigroups/detail/knuth-bendix-backtrack-impl.hpp"  // for Knuth...

namespace libsemigroups {
  using literals::operator""_w;

  using KnuthBendixBacktrack = detail::KnuthBendixBacktrack;

  static_assert(!std::is_default_constructible_v<KnuthBendixBacktrack>);
  static_assert(std::is_copy_constructible_v<KnuthBendixBacktrack>);
  static_assert(std::is_move_constructible_v<KnuthBendixBacktrack>);
  static_assert(!std::is_copy_assignable_v<KnuthBendixBacktrack>);
  static_assert(!std::is_move_assignable_v<KnuthBendixBacktrack>);

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

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixBacktrack",
                          "002",
                          "constructors",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "a", "b");

    KnuthBendixBacktrack kbb(p, 100, 10);
    KnuthBendixBacktrack copy(kbb);
    REQUIRE(copy == kbb);
    REQUIRE(*copy == *kbb);
    REQUIRE(copy.operator->() != kbb.operator->());

    KnuthBendixBacktrack moved(std::move(copy));
    REQUIRE(moved == kbb);
    REQUIRE(*moved == *kbb);

    p.alphabet("abc");
    presentation::add_rule(p, "b", "c");
    REQUIRE(kbb->alphabet() == "ab");
    REQUIRE(kbb->rules == std::vector<std::string>{"a", "b"});
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixBacktrack",
                          "003",
                          "accessors and comparison",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "a", "b");

    KnuthBendixBacktrack kbb_1(p, 100, 10);
    KnuthBendixBacktrack kbb_2(p, 100, 10);
    KnuthBendixBacktrack kbb_3(p, 101, 10);

    auto const& presentation = *kbb_1;
    REQUIRE(kbb_1.operator->() == &presentation);
    REQUIRE(kbb_1->alphabet() == "ab");
    REQUIRE(kbb_1->rules == std::vector<std::string>{"a", "b"});

    REQUIRE(kbb_1 == kbb_2);
    REQUIRE(!(kbb_1 != kbb_2));
    REQUIRE(kbb_1 != kbb_3);
    REQUIRE(!(kbb_1 == kbb_3));

    // Forward iterator multi-pass guarantee
    kbb_1++;
    kbb_2++;
    REQUIRE(kbb_1 == kbb_2);
    bool res{((void) [](auto x) { ++x; }(kbb_1), *kbb_1) == *kbb_1};
    REQUIRE(res);

    auto const before = kbb_1++;
    REQUIRE(before == kbb_2);
    REQUIRE(before != kbb_1);
  }

}  // namespace libsemigroups
