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
#include <vector>       // for vector

#include "libsemigroups/adapters.hpp"      // for ReturnFalse
#include "libsemigroups/presentation.hpp"  // for Presentation, presentation...
#include "libsemigroups/to-word.hpp"       // for ToWord
#include "libsemigroups/word-range-class.hpp"  // for WordRange

#include "libsemigroups/detail/knuth-bendix-backtrack-impl.hpp"  // for Knuth...
#include "libsemigroups/detail/rewriting-system.hpp"  // for RewritingSystemTrie

namespace libsemigroups {
  using literals::operator""_w;

  template <typename = Default, bool = false>
  struct NoOrder : public ReturnFalse {
    void init() const noexcept {}
  };

  using KnuthBendixBacktrack = detail::KnuthBendixBacktrack;
  using std::literals::operator""s;

  // Assert that the forward iterator requirements are met
  static_assert(std::is_default_constructible_v<KnuthBendixBacktrack>,
                "forward iterator requires default-constructible");
  static_assert(std::is_copy_constructible_v<KnuthBendixBacktrack>,
                "forward iterator requires copy-constructible");
  static_assert(std::is_move_constructible_v<KnuthBendixBacktrack>,
                "forward iterator requires move-constructible");
  static_assert(std::is_copy_assignable_v<KnuthBendixBacktrack>,
                "forward iterator requires copy-assignable");
  static_assert(std::is_move_assignable_v<KnuthBendixBacktrack>,
                "forward iterator requires move-assignable");
  static_assert(std::is_destructible_v<KnuthBendixBacktrack>,
                "forward iterator requires destructible");

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixBacktrack",
                          "000",
                          "simple test 0",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("abc"s);
    presentation::add_rule(p, "baa"s, "c"s);
    presentation::add_rule(p, "aba"s, "cc"s);

    KnuthBendixBacktrack       kbb(p, 100, 50);
    KnuthBendixBacktrack const end
        = detail::end_knuth_bendix_backtrack(p, 100, 50);

    std::vector<std::vector<std::string>> expected_rules{
        {"baa",
         "c",
         "aba",
         "cc",
         "ac",
         "cca",
         "bcccca",
         "cba",
         "abcc",
         "ccba",
         "bcccccc",
         "cbcc"},
        {"baa",
         "c",
         "aba",
         "cc",
         "ac",
         "cca",
         "cba",
         "bcccca",
         "abcc",
         "cbcccca",
         "bccccaa",
         "cc",
         "bcccccc",
         "cbcc",
         "abcbcccca",
         "ccbcc",
         "cbcbcccca",
         "bcccccbcccca",
         "abcbcccccbcccca",
         "ccbccbcc",
         "cbcccccbcccccbcccca",
         "ccbcbcccccbcccca",
         "bcccccbcccccbcccca",
         "cbcbcccccbcccca"},
        {"baa",
         "c",
         "aba",
         "cc",
         "ac",
         "cca",
         "cba",
         "bcccca",
         "abcc",
         "cbcccca",
         "bccccaa",
         "cc",
         "bcccccc",
         "cbcc",
         "abcbcccca",
         "ccbcc",
         "bcccccbcccca",
         "cbcbcccca"},
    };

    size_t index = 0;

    while (kbb != end) {
      REQUIRE(kbb->rules == expected_rules[index]);
      ++kbb;
      ++index;
    }

    REQUIRE(index == 3);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixBacktrack",
                          "001",
                          "simple test 1",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("ab"s);
    presentation::add_rule(p, "a"s, "b"s);
    KnuthBendixBacktrack kbb(p, 100, 10);

    Presentation<std::string> expected;
    expected.alphabet("ab"s);
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
    p.alphabet("ab"s);
    presentation::add_rule(p, "a"s, "b"s);

    KnuthBendixBacktrack kbb(p, 100, 10);
    KnuthBendixBacktrack copy(kbb);
    REQUIRE(copy == kbb);
    REQUIRE(*copy == *kbb);
    REQUIRE(copy.operator->() != kbb.operator->());

    KnuthBendixBacktrack moved(std::move(copy));
    REQUIRE(moved == kbb);
    REQUIRE(*moved == *kbb);

    p.alphabet("abc"s);
    presentation::add_rule(p, "b"s, "c"s);
    REQUIRE(kbb->alphabet() == "ab");
    REQUIRE(kbb->rules == std::vector<std::string>{"a", "b"});
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixBacktrack",
                          "003",
                          "accessors and comparison",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("ab"s);
    presentation::add_rule(p, "a"s, "b"s);

    KnuthBendixBacktrack kbb_1(p, 100, 10);
    KnuthBendixBacktrack kbb_2(p, 100, 10);

    auto const& presentation = *kbb_1;
    REQUIRE(kbb_1.operator->() == &presentation);
    REQUIRE(kbb_1->alphabet() == "ab");
    REQUIRE(kbb_1->rules == std::vector<std::string>{"a", "b"});

    REQUIRE(kbb_1 == kbb_2);
    REQUIRE(!(kbb_1 != kbb_2));

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

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixBacktrack",
                          "004",
                          "confluence",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("abc"s);
    presentation::add_rule(p, "baa"s, "c"s);
    presentation::add_rule(p, "aba"s, "cc"s);

    v4::ToWord to_word("abc");

    KnuthBendixBacktrack kbb = detail::begin_knuth_bendix_backtrack(p, 100, 50);
    KnuthBendixBacktrack end = detail::end_knuth_bendix_backtrack(p, 100, 50);
    detail::RewritingSystemTrie<NoOrder> rws;

    while (kbb != end) {
      rws.init();
      rws.increase_alphabet_size_by(3);
      for (size_t i = 0; i < kbb->rules.size(); i += 2) {
        detail::rewriting_system::add_rule(
            rws, to_word(kbb->rules[i]), to_word(kbb->rules[i + 1]));
      }
      REQUIRE(rws.confluent());
      ++kbb;
    }
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixBacktrack",
                          "005",
                          "one relation monoids",
                          "[extreme]") {
    std::string const first_word("a");
    std::string const last_word("aaaaaaaa");

    v4::WordRange<std::string> lhss;
    lhss.order(LenLexCmp(Alphabet("ab"s))).first(first_word).last(last_word);
    v4::WordRange<std::string> rhss;

    Presentation<std::string> p;
    p.alphabet("ab"s);
    p.contains_empty_word(true);

    KnuthBendixBacktrack       kbb;
    KnuthBendixBacktrack const end
        = detail::end_knuth_bendix_backtrack(p, 20, 10);

    size_t total   = 0;
    size_t success = 0;

    for (auto const& lhs : lhss) {
      rhss.order(LenLexCmp(Alphabet("ab"s))).first(lhs).last(last_word);
      for (auto const& rhs : rhss) {
        ++total;
        p.rules = {rhs, lhs};
        kbb     = KnuthBendixBacktrack(p, 20, 10);
        if (kbb != end) {
          ++success;
        }
      }
    }
    REQUIRE(total == 32385);
    REQUIRE(success == 27407);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixBacktrack",
                          "006",
                          "confluence x2",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("abcd"s);
    presentation::add_rule(p, "ab"s, "d"s);
    presentation::add_rule(p, "bc"s, "d"s);

    v4::ToWord to_word("abcd");

    KnuthBendixBacktrack kbb = detail::begin_knuth_bendix_backtrack(p, 100, 50);
    KnuthBendixBacktrack end = detail::end_knuth_bendix_backtrack(p, 100, 50);
    detail::RewritingSystemTrie<NoOrder> rws;

    while (kbb != end) {
      rws.init();
      rws.increase_alphabet_size_by(4);
      for (size_t i = 0; i < kbb->rules.size(); i += 2) {
        detail::rewriting_system::add_rule(
            rws, to_word(kbb->rules[i]), to_word(kbb->rules[i + 1]));
      }
      REQUIRE(rws.confluent());
      ++kbb;
    }
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixBacktrack",
                          "007",
                          "empty words",
                          "[quick]") {
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("a"s);
    presentation::add_rule(p, "a"s, ""s);

    KnuthBendixBacktrack       kbb(p, 100, 20);
    KnuthBendixBacktrack const end
        = detail::end_knuth_bendix_backtrack(p, 100, 20);

    size_t index = 0;
    while (kbb != end) {
      REQUIRE(kbb->rules == std::vector<std::string>{"a"s, ""s});
      ++kbb;
      ++index;
    }

    REQUIRE(index == 1);
  }

}  // namespace libsemigroups
