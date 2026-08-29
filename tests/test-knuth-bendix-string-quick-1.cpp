// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2026 James D. Mitchell
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

// TODO(later)
// * The other examples from Sims' book (Chap.s 5 and 6) which use
//   reduction orderings different from lenlex

#define CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS

#include <algorithm>    // for fill
#include <chrono>       // for milliseconds
#include <cstddef>      // for size_t
#include <string>       // for basic_string, operator==
#include <type_traits>  // for is_default_constructible_v
#include <utility>      // for move
#include <vector>       // for vector, operator==

#include "Catch2-3.14.0/catch_amalgamated.hpp"  // for AssertionHandler, ope...
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEMPLATE_TEST_CASE

#include "libsemigroups/constants.hpp"           // for operator==, operator!=
#include "libsemigroups/exception.hpp"           // for LibsemigroupsException
#include "libsemigroups/knuth-bendix.hpp"        // for KnuthBendix, normal_f...
#include "libsemigroups/obvinf.hpp"              // for is_obviously_infinite
#include "libsemigroups/order.hpp"               // for LenLexCmp
#include "libsemigroups/paths.hpp"               // for Paths
#include "libsemigroups/presentation.hpp"        // for add_rule, Presentation
#include "libsemigroups/ranges.hpp"              // for equal
#include "libsemigroups/to-froidure-pin.hpp"     // for to<FroidurePin>
#include "libsemigroups/types.hpp"               // for word_type
#include "libsemigroups/word-graph-helpers.hpp"  // for word_graph
#include "libsemigroups/word-graph.hpp"          // for WordGraph

#include "libsemigroups/detail/report.hpp"  // for ReportGuard

namespace libsemigroups {
  using std::literals::operator""s;

  using namespace rx;

  congruence_kind constexpr twosided = congruence_kind::twosided;

  using LenLexTrie = detail::RewritingSystemTrie<LenLexCmp>;
  using LenLexSet  = detail::RewritingSystemSet<LenLexCmp>;

  using RPOTrie = detail::RewritingSystemTrie<RPOCmp>;
  using RPOSet  = detail::RewritingSystemSet<RPOCmp>;

  using RevRPOTrie = detail::RewritingSystemTrie<RevRPOCmp>;
  using RevRPOSet  = detail::RewritingSystemSet<RevRPOCmp>;

#define REWRITING_SYSTEM_TYPES LenLexTrie, LenLexSet, RevRPOTrie, RevRPOSet

  static_assert(std::is_default_constructible_v<
                KnuthBendix<std::string, LenLexTrie, LenLexCmp>>);
  static_assert(
      std::is_default_constructible_v<detail::KnuthBendixImpl<LenLexTrie>>);
  static_assert(
      std::is_same_v<typename KnuthBendix<std::string>::reduction_order,
                     LenLexCmp<Default, false>>);
  static_assert(
      std::is_same_v<typename detail::KnuthBendixImpl<>::reduction_order,
                     LenLexCmp<Default, false>>);

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "000",
                                   "confluent fp semigroup 1 (infinite)",
                                   "[quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abc"s);
    p.rules = {"ab", "ba", "ac", "ca",  "aa", "a",  "ac", "a",  "ca", "a", "bb",
               "bb", "bc", "cb", "bbb", "b",  "bc", "b",  "cb", "b",  "a", "b"};

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!knuth_bendix::is_reduced(kb));

    REQUIRE(kb.rewriting_system().number_of_rules() == 10);
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(knuth_bendix::reduce(kb, "ca") == "a");
    REQUIRE(knuth_bendix::reduce(kb, "ac") == "a");
    REQUIRE(knuth_bendix::contains(kb, "ca", "a"));
    REQUIRE(knuth_bendix::contains(kb, "ac", "a"));
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    REQUIRE(is_obviously_infinite(kb));

    auto nf = knuth_bendix::normal_forms(kb).min(1).max(4);

    REQUIRE((nf | to_vector())
            == std::vector<std::string>({"a", "c", "cc", "ccc", "cccc"}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "001",
                                   "confluent fp semigroup 2 (infinite)",
                                   "[quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abc"s);
    presentation::add_rule_no_checks(p, "ac"s, "ca"s);
    presentation::add_rule_no_checks(p, "aa"s, "a"s);
    presentation::add_rule_no_checks(p, "ac"s, "a"s);
    presentation::add_rule_no_checks(p, "ca"s, "a"s);
    presentation::add_rule_no_checks(p, "bb"s, "bb"s);
    presentation::add_rule_no_checks(p, "bc"s, "cb"s);
    presentation::add_rule_no_checks(p, "bbb"s, "b"s);
    presentation::add_rule_no_checks(p, "bc"s, "b"s);
    presentation::add_rule_no_checks(p, "cb"s, "b"s);
    presentation::add_rule_no_checks(p, "a"s, "b"s);

    KnuthBendix<std::string, TestType> kb(twosided, p);

    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.rewriting_system().number_of_rules() == 4);
    REQUIRE(is_obviously_infinite(kb));
    auto nf = knuth_bendix::normal_forms(kb).min(1).max(4);

    REQUIRE((nf | to_vector())
            == std::vector<std::string>({"a", "c", "cc", "ccc", "cccc"}));
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "002",
                                   "confluent fp semigroup 3 (infinite)",
                                   "[quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("012"s);
    presentation::add_rule_no_checks(p, "01"s, "10"s);
    presentation::add_rule_no_checks(p, "02"s, "20"s);
    presentation::add_rule_no_checks(p, "00"s, "0"s);
    presentation::add_rule_no_checks(p, "02"s, "0"s);
    presentation::add_rule_no_checks(p, "20"s, "0"s);
    presentation::add_rule_no_checks(p, "11"s, "11"s);
    presentation::add_rule_no_checks(p, "12"s, "21"s);
    presentation::add_rule_no_checks(p, "111"s, "1"s);
    presentation::add_rule_no_checks(p, "12"s, "1"s);
    presentation::add_rule_no_checks(p, "21"s, "1"s);
    presentation::add_rule_no_checks(p, "0"s, "1"s);

    KnuthBendix<std::string, TestType> kb(twosided, p);

    REQUIRE(kb.rewriting_system().number_of_rules() == 10);
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.rewriting_system().number_of_rules() == 4);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);

    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(1).max(1) | to_vector())
            == std::vector<std::string>({"0", "2"}));

    REQUIRE((nf.min(1).max(11) | to_vector())
            == std::vector<std::string>({"0",
                                         "2",
                                         "22",
                                         "222",
                                         "2222",
                                         "22222",
                                         "222222",
                                         "2222222",
                                         "22222222",
                                         "222222222",
                                         "2222222222",
                                         "22222222222"}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "003",
                                   "non-confluent example wikipedia",
                                   "[quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    using order = typename TestType::reduction_order;

    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("01"s);
    presentation::add_rule_no_checks(p, "000"s, ""s);
    presentation::add_rule_no_checks(p, "111"s, ""s);
    presentation::add_rule_no_checks(p, "010101"s, ""s);

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(kb.presentation().alphabet() == "01");
    REQUIRE(!kb.rewriting_system().confluent());
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 4);
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);

    auto nf    = knuth_bendix::normal_forms(kb);
    auto found = (kb.active_rules() | rx::sort() | rx::to_vector());

    REQUIRE(std::all_of(found.begin(), found.end(), [](auto const& rule) {
      return order()(rule.second, rule.first);
    }));
    REQUIRE(is_sorted(nf.min(0).max(4), LenLexCmp{}));

    if constexpr (std::is_same_v<order, LenLexCmp<Default, false>>) {
      REQUIRE(
          found
          == std::vector<std::pair<std::string, std::string>>(
              {{"000", ""}, {"1010", "0011"}, {"1100", "0101"}, {"111", ""}}));
      REQUIRE((nf.min(0).max(4) | to_vector())
              == std::vector<std::string>(
                  {"",     "0",    "1",    "00",   "01",   "10",   "11",
                   "001",  "010",  "011",  "100",  "101",  "110",  "0010",
                   "0011", "0100", "0101", "0110", "1001", "1011", "1101"}));
    } else {
      REQUIRE(
          found
          == std::vector<std::pair<std::string, std::string>>(
              {{"000", ""}, {"0011", "1010"}, {"0101", "1100"}, {"111", ""}}));
      REQUIRE((nf.min(0).max(4) | to_vector())
              == std::vector<std::string>(
                  {"",     "0",    "1",    "00",   "01",   "10",   "11",
                   "001",  "010",  "011",  "100",  "101",  "110",  "0010",
                   "0100", "0110", "1001", "1010", "1011", "1100", "1101"}));
    }

    REQUIRE((nf.min(0).max(10) | all_of([&kb](auto const& w) {
               return knuth_bendix::reduce(kb, w) == w;
             })));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "004",
                                   "Example 5.1 in Sims (infinite)",
                                   "[quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    using order = typename TestType::reduction_order;
    auto rg     = ReportGuard(false);

    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("abcd"s);
    presentation::add_rule_no_checks(p, "ab"s, ""s);
    presentation::add_rule_no_checks(p, "ba"s, ""s);
    presentation::add_rule_no_checks(p, "cd"s, ""s);
    presentation::add_rule_no_checks(p, "dc"s, ""s);
    presentation::add_rule_no_checks(p, "ca"s, "ac"s);

    KnuthBendix<std::string, TestType> kb(twosided, p);

    REQUIRE(!kb.rewriting_system().confluent());
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 8);
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);

    auto nf = knuth_bendix::normal_forms(kb);
    if constexpr (std::is_same_v<order, LenLexCmp<Default, false>>) {
      REQUIRE((nf.min(0).max(4) | to_vector())
              == std::vector<std::string>(  // codespell:begin-ignore
                  {"",     "a",    "b",    "c",    "d",    "aa",   "ac",
                   "ad",   "bb",   "bc",   "bd",   "cc",   "dd",   "aaa",
                   "aac",  "aad",  "acc",  "add",  "bbb",  "bbc",  "bbd",
                   "bcc",  "bdd",  "ccc",  "ddd",  "aaaa", "aaac", "aaad",
                   "aacc", "aadd", "accc", "addd", "bbbb", "bbbc", "bbbd",
                   "bbcc", "bbdd", "bccc", "bddd", "cccc", "dddd"}));
      // codespell:end-ignore
    }
    REQUIRE((nf.min(0).max(6) | all_of([&kb](auto const& w) {
               return knuth_bendix::reduce(kb, w) == w;
             })));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "005",
                                   "Example 5.1 in Sims (infinite) x 2",
                                   "[quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    using order = typename TestType::reduction_order;
    auto rg     = ReportGuard(false);

    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("aAbB"s);
    presentation::add_inverse_rules(p, "AaBb"s);
    presentation::add_rule_no_checks(p, "ba"s, "ab"s);

    KnuthBendix<std::string, TestType> kb(twosided, p);

    REQUIRE(!kb.rewriting_system().confluent());
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 8);
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);

    auto nf = knuth_bendix::normal_forms(kb);

    if constexpr (std::is_same_v<order, LenLexCmp<Default, false>>) {
      REQUIRE((nf.min(0).max(4) | to_vector())
              == std::vector<std::string>(
                  {"",     "a",    "A",    "b",    "B",    "aa",   "ab",
                   "aB",   "AA",   "Ab",   "AB",   "bb",   "BB",   "aaa",
                   "aab",  "aaB",  "abb",  "aBB",  "AAA",  "AAb",  "AAB",
                   "Abb",  "ABB",  "bbb",  "BBB",  "aaaa", "aaab", "aaaB",
                   "aabb", "aaBB", "abbb", "aBBB", "AAAA", "AAAb", "AAAB",
                   "AAbb", "AABB", "Abbb", "ABBB", "bbbb", "BBBB"}));
    }
    REQUIRE((nf.min(0).max(6) | all_of([&kb](auto const& w) {
               return knuth_bendix::reduce(kb, w) == w;
             })));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "006",
                                   "Example 5.3 in Sims",
                                   "[quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    using order = typename TestType::reduction_order;
    auto rg     = ReportGuard(false);

    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("ab"s);
    presentation::add_rule_no_checks(p, "aa"s, ""s);
    presentation::add_rule_no_checks(p, "bbb"s, ""s);
    presentation::add_rule_no_checks(p, "ababab"s, ""s);

    KnuthBendix<std::string, TestType> kb(twosided, p);

    REQUIRE(!kb.rewriting_system().confluent());
    kb.run();
    auto nf = knuth_bendix::normal_forms(kb);

    if constexpr (std::is_same_v<order, LenLexCmp<Default, false>>) {
      REQUIRE(kb.rewriting_system().number_of_rules() == 6);
      REQUIRE((nf | to_vector())
              == std::vector<std::string>({"",
                                           "a",
                                           "b",
                                           "ab",
                                           "ba",
                                           "bb",
                                           "aba",
                                           "abb",
                                           "bab",
                                           "bba",
                                           "babb",
                                           "bbab"}));
    } else {
      REQUIRE(kb.rewriting_system().number_of_rules() == 4);
    }

    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.number_of_classes() == 12);
    REQUIRE(nf.count() == 12);

    REQUIRE((nf.min(0).max(6) | all_of([&kb](auto const& w) {
               return knuth_bendix::reduce(kb, w) == w;
             })));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "007",
                                   "Example 5.4 in Sims",
                                   "[quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    using order = typename TestType::reduction_order;
    auto rg     = ReportGuard(false);

    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("Bab"s);
    presentation::add_rule_no_checks(p, "aa"s, ""s);
    presentation::add_rule_no_checks(p, "bB"s, ""s);
    presentation::add_rule_no_checks(p, "bbb"s, ""s);
    presentation::add_rule_no_checks(p, "ababab"s, ""s);

    KnuthBendix<std::string, TestType> kb(twosided, p);

    REQUIRE(!kb.rewriting_system().confluent());
    kb.run();
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.number_of_classes() == 12);

    auto nf = knuth_bendix::normal_forms(kb).min(1).max(5);
    REQUIRE(nf.size_hint() == 11);

    if constexpr (std::is_same_v<order, LenLexCmp<Default, false>>) {
      REQUIRE(kb.rewriting_system().number_of_rules() == 11);
      REQUIRE((nf | to_vector())
              == std::vector<std::string>({"B",
                                           "a",
                                           "b",
                                           "Ba",
                                           "aB",
                                           "ab",
                                           "ba",
                                           "BaB",
                                           "Bab",
                                           "aBa",
                                           "baB"}));
    } else {
      REQUIRE(kb.rewriting_system().number_of_rules() == 5);
    }
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "008",
                                   "Example 6.4 in Sims",
                                   "[quick][knuth-bendix][no-valgrind]",
                                   REWRITING_SYSTEM_TYPES) {
    using order = typename TestType::reduction_order;
    auto rg     = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abc"s);
    p.contains_empty_word(true);

    presentation::add_rule_no_checks(p, "aa"s, ""s);
    presentation::add_rule_no_checks(p, "bc"s, ""s);
    presentation::add_rule_no_checks(p, "bbb"s, ""s);
    presentation::add_rule_no_checks(p, "ababababababab"s, ""s);
    presentation::add_rule_no_checks(p, "abacabacabacabac"s, ""s);

    KnuthBendix<std::string, TestType> kb(twosided, p);

    REQUIRE(!kb.rewriting_system().confluent());
    REQUIRE(!is_obviously_infinite(kb));
    // REQUIRE(!kb.is_obviously_finite());
    kb.run();
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(knuth_bendix::reduce(kb, "cc") == "b");
    REQUIRE(knuth_bendix::reduce(kb, "ccc") == "");
    REQUIRE(kb.number_of_classes() == 168);

    auto S = to<FroidurePin>(kb);
    REQUIRE(S.size() == 168);

    if constexpr (std::is_same_v<order, LenLexCmp<Default, false>>) {
      REQUIRE(S.generator(2).word() == "c");
      REQUIRE(kb.rewriting_system().number_of_rules() == 40);
      auto nf = knuth_bendix::normal_forms(kb).min(1).max(4);
      REQUIRE((nf | to_vector())
              == std::vector<std::string>(
                  {"a",    "b",    "c",    "ab",   "ac",   "ba",   "ca",
                   "aba",  "aca",  "bab",  "bac",  "cab",  "cac",  "abab",
                   "abac", "acab", "acac", "baba", "baca", "caba", "caca"}));
    } else {
      REQUIRE(S.generator(2).word() == "bb");
      REQUIRE(kb.rewriting_system().number_of_rules() == 17);
    }
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "009",
                                   "random example",
                                   "[quick][knuth-bendix][no-valgrind]",
                                   REWRITING_SYSTEM_TYPES) {
    using order = typename TestType::reduction_order;
    auto rg     = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("012"s);

    presentation::add_rule_no_checks(p, "000"s, "2"s);
    presentation::add_rule_no_checks(p, "111"s, "2"s);
    presentation::add_rule_no_checks(p, "010101"s, "2"s);
    presentation::add_identity_rules(p, '2');

    KnuthBendix<std::string, TestType> kb(twosided, p);

    REQUIRE(!kb.rewriting_system().confluent());
    kb.run();
    auto& wg = kb.gilman_graph();

    if constexpr (std::is_same_v<order, LenLexCmp<Default, false>>) {
      REQUIRE(kb.rewriting_system().number_of_rules() == 9);
      REQUIRE(wg.number_of_nodes() == 9);
      REQUIRE(wg.number_of_edges() == 13);
      auto fp = to<FroidurePin>(kb);
      fp.enumerate(100);

      auto expected = froidure_pin::current_normal_forms(fp);

      Paths paths(wg);
      paths.source(0).min(1).max(fp.current_max_word_length());

      REQUIRE(equal(expected, paths));

      auto nf = knuth_bendix::normal_forms(kb).min(1).max(4);
      REQUIRE((nf | to_vector())
              == std::vector<std::string>(
                  {"0",    "1",    "2",    "00",   "01",   "10",   "11",
                   "001",  "010",  "011",  "100",  "101",  "110",  "0010",
                   "0011", "0100", "0101", "0110", "1001", "1011", "1101"}));
    } else {
      REQUIRE(kb.rewriting_system().number_of_rules() == 7);
      REQUIRE(wg.number_of_nodes() == 11);
      REQUIRE(wg.number_of_edges() == 16);
    }
    REQUIRE(kb.rewriting_system().confluent());

    REQUIRE(!word_graph::is_acyclic(wg));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "010",
                                   "SL(2, 7) from Chap. 3, Prop. 1.5 in NR",
                                   "[quick][knuth-bendix][no-valgrind]",
                                   REWRITING_SYSTEM_TYPES) {
    using order = typename TestType::reduction_order;
    auto rg     = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abAB"s);
    p.contains_empty_word(true);

    presentation::add_rule_no_checks(p, "aaaaaaa"s, ""s);
    presentation::add_rule_no_checks(p, "bb"s, "ababab"s);
    presentation::add_rule_no_checks(p, "bb"s, "aaaabaaaabaaaabaaaab"s);
    presentation::add_rule_no_checks(p, "aA"s, ""s);
    presentation::add_rule_no_checks(p, "Aa"s, ""s);
    presentation::add_rule_no_checks(p, "bB"s, ""s);
    presentation::add_rule_no_checks(p, "Bb"s, ""s);

    KnuthBendix<std::string, TestType> kb(twosided, p);

    REQUIRE(!kb.rewriting_system().confluent());

    kb.run();
    auto& wg = kb.gilman_graph();
    REQUIRE(word_graph::is_acyclic(wg));
    Paths paths(wg);
    paths.source(0).min(0).max(13);

    if constexpr (std::is_same_v<order, LenLexCmp<Default, false>>) {
      REQUIRE(paths.count() == 336);
      REQUIRE(kb.rewriting_system().number_of_rules() == 152);
      REQUIRE(wg.number_of_nodes() == 232);
      REQUIRE(wg.number_of_edges() == 265);
    } else {
      REQUIRE(paths.count() == 244);
      REQUIRE(kb.rewriting_system().number_of_rules() == 36);
      REQUIRE(wg.number_of_nodes() == 80);
      REQUIRE(wg.number_of_edges() == 93);
    }
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.number_of_classes() == 336);

    // Test copy constructor
    auto T = to<FroidurePin>(kb);
    auto S = froidure_pin::copy_closure(T, {T.generator(0)});

    REQUIRE(S.size() == 336);
    // 5 because S is generated as semigroup by 5 generators, while p is a
    // monoid presentation
    REQUIRE(S.number_of_generators() == 5);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "011",
                                   "F(2, 5) - Chap. 9, Sec. 1 in NR",
                                   "[knuth-bendix][quick]",
                                   REWRITING_SYSTEM_TYPES) {
    using order                  = typename TestType::reduction_order;
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abcde"s);

    presentation::add_rule_no_checks(p, "ab"s, "c"s);
    presentation::add_rule_no_checks(p, "bc"s, "d"s);
    presentation::add_rule_no_checks(p, "cd"s, "e"s);
    presentation::add_rule_no_checks(p, "de"s, "a"s);
    presentation::add_rule_no_checks(p, "ea"s, "b"s);
    KnuthBendix<std::string, TestType> kb(twosided, p);

    REQUIRE(!kb.rewriting_system().confluent());
    kb.run();
    auto& wg = kb.gilman_graph();
    auto  nf = knuth_bendix::normal_forms(kb).min(0).max(4);

    if constexpr (std::is_same_v<order, LenLexCmp<Default, false>>) {
      REQUIRE(kb.rewriting_system().number_of_rules() == 24);
      REQUIRE(nf.count() == 12);
      REQUIRE(wg.number_of_nodes() == 8);
      REQUIRE(wg.number_of_edges() == 11);
      REQUIRE(word_graph::is_acyclic(wg));
    } else {
      REQUIRE(kb.rewriting_system().number_of_rules() == 5);
      REQUIRE(nf.count() == 5);
      REQUIRE(wg.number_of_nodes() == 12);
      REQUIRE(wg.number_of_edges() == 11);
      REQUIRE(word_graph::is_acyclic(wg));
    }
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.number_of_classes() == 11);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "012",
                                   "Reinis example 1",
                                   "[quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abc"s);

    presentation::add_rule_no_checks(p, "a"s, "abb"s);
    presentation::add_rule_no_checks(p, "b"s, "baa"s);
    KnuthBendix<std::string, TestType> kb(twosided, p);

    REQUIRE(!kb.rewriting_system().confluent());
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 4);

    auto& wg = kb.gilman_graph();
    REQUIRE(wg.number_of_nodes() == 7);
    REQUIRE(wg.number_of_edges() == 17);
    REQUIRE(!word_graph::is_acyclic(wg));
    Paths paths(wg);
    paths.source(0).min(0).max(9);
    REQUIRE(paths.count() == 13'044);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "013",
                                   "redundant_rule (std::string)",
                                   "[quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abc"s);
    presentation::add_rule(p, "a"s, "abb"s);
    presentation::add_rule(p, "b"s, "baa"s);
    presentation::add_rule(p, "c"s, "abbabababaaababababab"s);

    auto it = knuth_bendix::redundant_rule(p, std::chrono::milliseconds(100));
    REQUIRE(it == p.rules.cend());

    presentation::add_rule(p, "b"s, "baa"s);
    it = knuth_bendix::redundant_rule(p, std::chrono::milliseconds(100));
    REQUIRE(it != p.rules.cend());
    REQUIRE(*it == "b");
    REQUIRE(*(it + 1) == "baa");
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "015",
                                   "constructors/init for finished",
                                   "[quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    using order = typename TestType::reduction_order;
    auto rg     = ReportGuard(false);

    Presentation<std::string> p1;
    p1.contains_empty_word(true);
    p1.alphabet("abcd"s);
    presentation::add_rule_no_checks(p1, "ab"s, ""s);
    presentation::add_rule_no_checks(p1, "ba"s, ""s);
    presentation::add_rule_no_checks(p1, "cd"s, ""s);
    presentation::add_rule_no_checks(p1, "dc"s, ""s);
    presentation::add_rule_no_checks(p1, "ca"s, "ac"s);

    Presentation<std::string> p2;
    p2.contains_empty_word(true);
    p2.alphabet("01"s);
    presentation::add_rule_no_checks(p2, "000"s, ""s);
    presentation::add_rule_no_checks(p2, "111"s, ""s);
    presentation::add_rule_no_checks(p2, "010101"s, ""s);

    KnuthBendix<std::string, TestType> kb1(twosided, p1);
    REQUIRE(!kb1.rewriting_system().confluent());
    REQUIRE(!kb1.finished());
    kb1.run();
    REQUIRE(kb1.rewriting_system().confluent());

    if constexpr (std::is_same_v<order, LenLexCmp<Default, false>>) {
      REQUIRE(knuth_bendix::reduce(kb1, "abababbdbcbdbabdbdb") == "bbbbbbddd");
    } else {
      REQUIRE(knuth_bendix::reduce(kb1, "abababbdbcbdbabdbdb") == "dddbbbbbb");
    }

    kb1.init(twosided, p2);
    REQUIRE(!kb1.rewriting_system().confluent());
    REQUIRE(!kb1.finished());
    REQUIRE(kb1.presentation() == p2);
    kb1.run();
    REQUIRE(kb1.finished());
    REQUIRE(kb1.rewriting_system().confluent());
    REQUIRE(kb1.rewriting_system().confluent_known());

    kb1.init(twosided, p1);
    REQUIRE(!kb1.rewriting_system().confluent());
    REQUIRE(!kb1.finished());
    REQUIRE(kb1.presentation() == p1);
    kb1.run();
    REQUIRE(kb1.finished());
    REQUIRE(kb1.rewriting_system().confluent());
    REQUIRE(kb1.rewriting_system().confluent_known());
    if constexpr (std::is_same_v<order, LenLexCmp<Default, false>>) {
      REQUIRE(knuth_bendix::reduce(kb1, "abababbdbcbdbabdbdb") == "bbbbbbddd");
    } else {
      REQUIRE(knuth_bendix::reduce(kb1, "abababbdbcbdbabdbdb") == "dddbbbbbb");
    }

    KnuthBendix<std::string, TestType> kb2(std::move(kb1));
    REQUIRE(kb2.rewriting_system().confluent());
    REQUIRE(kb2.rewriting_system().confluent_known());
    REQUIRE(kb2.finished());
    if constexpr (std::is_same_v<order, LenLexCmp<Default, false>>) {
      REQUIRE(knuth_bendix::reduce(kb2, "abababbdbcbdbabdbdb") == "bbbbbbddd");
    } else {
      REQUIRE(knuth_bendix::reduce(kb2, "abababbdbcbdbabdbdb") == "dddbbbbbb");
    }

    kb1 = std::move(kb2);
    REQUIRE(kb1.rewriting_system().confluent());
    REQUIRE(kb1.rewriting_system().confluent_known());
    REQUIRE(kb1.finished());
    if constexpr (std::is_same_v<order, LenLexCmp<Default, false>>) {
      REQUIRE(knuth_bendix::reduce(kb1, "abababbdbcbdbabdbdb") == "bbbbbbddd");
    } else {
      REQUIRE(knuth_bendix::reduce(kb1, "abababbdbcbdbabdbdb") == "dddbbbbbb");
    }

    kb1.init(twosided, std::move(p1));
    REQUIRE(!kb1.rewriting_system().confluent());
    REQUIRE(!kb1.finished());
    kb1.run();
    REQUIRE(kb1.finished());
    REQUIRE(kb1.rewriting_system().confluent());
    REQUIRE(kb1.rewriting_system().confluent_known());
    if constexpr (std::is_same_v<order, LenLexCmp<Default, false>>) {
      REQUIRE(knuth_bendix::reduce(kb1, "abababbdbcbdbabdbdb") == "bbbbbbddd");
    } else {
      REQUIRE(knuth_bendix::reduce(kb1, "abababbdbcbdbabdbdb") == "dddbbbbbb");
    }
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "016",
                                   "constructors/init for partially run",
                                   "[quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    using literals::operator""_w;

    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("abc"s);

    presentation::add_rule_no_checks(p, "aa"s, ""s);
    presentation::add_rule_no_checks(p, "bc"s, ""s);
    presentation::add_rule_no_checks(p, "bbb"s, ""s);
    presentation::add_rule_no_checks(p, "ababababababab"s, ""s);
    presentation::add_rule_no_checks(
        p, "abacabacabacabacabacabacabacabac"s, ""s);

    KnuthBendix<std::string, TestType> kb1(twosided, p);
    REQUIRE(!kb1.rewriting_system().confluent());
    REQUIRE(!kb1.finished());
    kb1.run_for(std::chrono::milliseconds(10));
    REQUIRE(!kb1.rewriting_system().confluent());
    REQUIRE(!kb1.finished());

    kb1.init(twosided, p);
    REQUIRE(!kb1.rewriting_system().confluent());
    REQUIRE(!kb1.finished());
    REQUIRE(kb1.presentation() == p);
    kb1.run_for(std::chrono::milliseconds(10));
    REQUIRE(!kb1.rewriting_system().confluent());
    REQUIRE(!kb1.finished());

    KnuthBendix<std::string, TestType> kb2(kb1);
    REQUIRE(!kb2.rewriting_system().confluent());
    REQUIRE(!kb2.finished());
    REQUIRE(kb2.presentation() == p);
    REQUIRE(kb1.rewriting_system().number_of_rules()
            == kb2.rewriting_system().number_of_rules());
    kb2.run_for(std::chrono::milliseconds(10));
    REQUIRE(!kb2.rewriting_system().confluent());
    REQUIRE(!kb2.finished());

    size_t const M = kb2.rewriting_system().number_of_rules();
    kb1            = std::move(kb2);
    REQUIRE(kb1.rewriting_system().number_of_rules() == M);
    REQUIRE(!kb1.finished());

    kb1.init(twosided, p);
    knuth_bendix::add_generating_pair(kb1, "ab", "ba");
    REQUIRE(kb1.number_of_generating_pairs() == 1);
    REQUIRE(kb1.generating_pairs() == std::vector<std::string>({"ab", "ba"}));
    REQUIRE(kb1.internal_generating_pairs().size() == 2);

    kb1.init(twosided, p);
    REQUIRE(kb1.number_of_generating_pairs() == 0);
    REQUIRE(kb1.internal_generating_pairs().size() == 0);
    REQUIRE(kb1.generating_pairs().size() == 0);

    knuth_bendix::add_generating_pair(kb1, "ab", "ba");

    REQUIRE(kb1.number_of_generating_pairs() == 1);
    REQUIRE(kb1.internal_generating_pairs().size() == 2);
    REQUIRE(kb1.generating_pairs().size() == 2);

    kb1.init();

    REQUIRE(kb1.number_of_generating_pairs() == 0);
    REQUIRE(kb1.internal_generating_pairs().size() == 0);
    REQUIRE(kb1.generating_pairs().size() == 0);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "017",
                                   "non-trivial classes",
                                   "[quick][knuth-bendix]",
                                   RevRPOTrie) {
    using order = typename TestType::reduction_order;

    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abc"s);
    presentation::add_rule_no_checks(p, "ab"s, "ba"s);
    presentation::add_rule_no_checks(p, "ac"s, "ca"s);
    presentation::add_rule_no_checks(p, "aa"s, "a"s);
    presentation::add_rule_no_checks(p, "ac"s, "a"s);
    presentation::add_rule_no_checks(p, "ca"s, "a"s);
    presentation::add_rule_no_checks(p, "bc"s, "cb"s);
    presentation::add_rule_no_checks(p, "bbb"s, "b"s);
    presentation::add_rule_no_checks(p, "bc"s, "b"s);
    presentation::add_rule_no_checks(p, "cb"s, "b"s);

    KnuthBendix<std::string, TestType> kb1(twosided, p);

    presentation::add_rule_no_checks(p, "a"s, "b"s);

    KnuthBendix<std::string, TestType> kb2(twosided, p);

    REQUIRE(knuth_bendix::contains(kb2, "a", "b"));
    REQUIRE(knuth_bendix::contains(kb2, "a", "ba"));
    REQUIRE(knuth_bendix::contains(kb2, "a", "bb"));
    REQUIRE(knuth_bendix::contains(kb2, "a", "bab"));

    REQUIRE(knuth_bendix::contains(kb2, "a", "b"));
    REQUIRE(knuth_bendix::contains(kb2, "a", "ab"));
    REQUIRE(knuth_bendix::contains(kb2, "a", "bb"));
    REQUIRE(knuth_bendix::contains(kb2, "a", "abb"));

    if constexpr (std::is_same_v<order, LenLexCmp<Default, false>>) {
      REQUIRE(knuth_bendix::reduce(kb2, "b") == "a");
      REQUIRE(knuth_bendix::reduce(kb2, "ab") == "a");
      REQUIRE(knuth_bendix::reduce(kb2, "bb") == "a");
      REQUIRE(knuth_bendix::reduce(kb2, "abb") == "a");
      REQUIRE(knuth_bendix::reduce(kb2, "a") == "a");

      auto nf = knuth_bendix::normal_forms(kb1).min(0).max(8);
      REQUIRE(nf.count() == 14);
      REQUIRE((nf | rx::to_vector())
              == std::vector<std::string>({"",
                                           "a",
                                           "b",
                                           "c",
                                           "ab",
                                           "bb",
                                           "cc",
                                           "abb",
                                           "ccc",
                                           "cccc",
                                           "ccccc",
                                           "cccccc",
                                           "ccccccc",
                                           "cccccccc"}));
      REQUIRE(knuth_bendix::partition(kb2, nf)
              == std::vector<std::vector<std::string>>(
                  {{"a", "b", "ab", "bb", "abb"},
                   {"c"},
                   {"cc"},
                   {"ccc"},
                   {"cccc"},
                   {"ccccc"},
                   {"cccccc"},
                   {"ccccccc"},
                   {"cccccccc"}}));

      REQUIRE(knuth_bendix::non_trivial_classes(kb1, kb2)
              == std::vector<std::vector<std::string>>(
                  {{"b", "ab", "bb", "abb", "a"}}));
    } else {
      REQUIRE(knuth_bendix::reduce(kb2, "b") == "a");
      REQUIRE(knuth_bendix::reduce(kb2, "ba") == "a");
      REQUIRE(knuth_bendix::reduce(kb2, "bb") == "a");
      REQUIRE(knuth_bendix::reduce(kb2, "bba") == "a");
      REQUIRE(knuth_bendix::reduce(kb2, "a") == "a");

      auto nf = knuth_bendix::normal_forms(kb1).min(0).max(8);
      REQUIRE(nf.count() == 14);
      REQUIRE((nf | rx::to_vector())
              == std::vector<std::string>({"",
                                           "a",
                                           "b",
                                           "c",
                                           "ba",
                                           "bb",
                                           "cc",
                                           "bba",
                                           "ccc",
                                           "cccc",
                                           "ccccc",
                                           "cccccc",
                                           "ccccccc",
                                           "cccccccc"}));
      REQUIRE(knuth_bendix::partition(kb2, nf)
              == std::vector<std::vector<std::string>>(
                  {{"a", "b", "ba", "bb", "bba"},
                   {"c"},
                   {"cc"},
                   {"ccc"},
                   {"cccc"},
                   {"ccccc"},
                   {"cccccc"},
                   {"ccccccc"},
                   {"cccccccc"}}));
      REQUIRE(knuth_bendix::non_trivial_classes(kb1, kb2)
              == std::vector<std::vector<std::string>>(
                  {{"b", "ba", "bb", "bba", "a"}}));
    }
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "018",
                                   "non-trivial classes x 2",

                                   "[quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abc"s);
    presentation::add_rule_no_checks(p, "ab"s, "ba"s);
    presentation::add_rule_no_checks(p, "ac"s, "ca"s);
    presentation::add_rule_no_checks(p, "aa"s, "a"s);
    presentation::add_rule_no_checks(p, "ac"s, "a"s);
    presentation::add_rule_no_checks(p, "ca"s, "a"s);
    presentation::add_rule_no_checks(p, "bc"s, "cb"s);
    presentation::add_rule_no_checks(p, "bbb"s, "b"s);
    presentation::add_rule_no_checks(p, "bc"s, "b"s);
    presentation::add_rule_no_checks(p, "cb"s, "b"s);

    KnuthBendix<std::string, TestType> kb1(twosided, p);
    REQUIRE(kb1.number_of_classes() == POSITIVE_INFINITY);

    presentation::add_rule_no_checks(p, "b"s, "c"s);

    KnuthBendix<std::string, TestType> kb2(twosided, p);
    REQUIRE(kb2.number_of_classes() == 2);

    REQUIRE_THROWS_AS(knuth_bendix::non_trivial_classes(kb1, kb2),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "019",
                                   "non-trivial classes x 3",
                                   "[quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    using order = typename TestType::reduction_order;

    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abc"s);
    presentation::add_rule_no_checks(p, "ab"s, "ba"s);
    presentation::add_rule_no_checks(p, "ac"s, "ca"s);
    presentation::add_rule_no_checks(p, "aa"s, "a"s);
    presentation::add_rule_no_checks(p, "ac"s, "a"s);
    presentation::add_rule_no_checks(p, "ca"s, "a"s);
    presentation::add_rule_no_checks(p, "bc"s, "cb"s);
    presentation::add_rule_no_checks(p, "bbb"s, "b"s);
    presentation::add_rule_no_checks(p, "bc"s, "b"s);
    presentation::add_rule_no_checks(p, "cb"s, "b"s);

    KnuthBendix<std::string, TestType> kb1(twosided, p);

    presentation::add_rule_no_checks(p, "bb"s, "a"s);

    KnuthBendix<std::string, TestType> kb2(twosided, p);

    if constexpr (std::is_same_v<order, LenLexCmp<Default, false>>) {
      auto nf = knuth_bendix::normal_forms(kb1).min(0).max(8);
      REQUIRE(nf.count() == 14);
      REQUIRE((nf | rx::to_vector())
              == std::vector<std::string>({"",
                                           "a",
                                           "b",
                                           "c",
                                           "ab",
                                           "bb",
                                           "cc",
                                           "abb",
                                           "ccc",
                                           "cccc",
                                           "ccccc",
                                           "cccccc",
                                           "ccccccc",
                                           "cccccccc"}));
      REQUIRE(knuth_bendix::partition(kb2, nf)
              == std::vector<std::vector<std::string>>({{"a", "bb", "abb"},
                                                        {"b", "ab"},
                                                        {"c"},
                                                        {"cc"},
                                                        {"ccc"},
                                                        {"cccc"},
                                                        {"ccccc"},
                                                        {"cccccc"},
                                                        {"ccccccc"},
                                                        {"cccccccc"}}));
      REQUIRE(knuth_bendix::non_trivial_classes(kb1, kb2)
              == std::vector<std::vector<std::string>>(
                  {{"ab", "b"}, {"bb", "abb", "a"}}));
    } else {
      auto nf = knuth_bendix::normal_forms(kb1).min(0).max(8);
      REQUIRE(nf.count() == 14);
      REQUIRE((nf | rx::to_vector())
              == std::vector<std::string>({"",
                                           "a",
                                           "b",
                                           "c",
                                           "ba",
                                           "bb",
                                           "cc",
                                           "bba",
                                           "ccc",
                                           "cccc",
                                           "ccccc",
                                           "cccccc",
                                           "ccccccc",
                                           "cccccccc"}));
      REQUIRE(knuth_bendix::partition(kb2, nf)
              == std::vector<std::vector<std::string>>({{"a", "bb", "bba"},
                                                        {"b", "ba"},
                                                        {"c"},
                                                        {"cc"},
                                                        {"ccc"},
                                                        {"cccc"},
                                                        {"ccccc"},
                                                        {"cccccc"},
                                                        {"ccccccc"},
                                                        {"cccccccc"}}));
      REQUIRE(knuth_bendix::non_trivial_classes(kb1, kb2)
              == std::vector<std::vector<std::string>>(
                  {{"ba", "b"}, {"bb", "bba", "a"}}));
    }
  }

  // This example verifies the nilpotence of the group using the Sims
  // algorithm. The original presentation was <a,b| [b,a,b], [b,a,a,a,a],
  // [b,a,a,a,b,a,a] >. (where [] mean left-normed commutators). The
  // presentation here was derived by first applying the NQA to find the
  // maximal nilpotent quotient, and then introducing new generators for
  // the PCP generators. It is essential for success that reasonably low
  // values of the maxstoredlen parameter are given.
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "994",
                                   "kbmag/verifynilp",
                                   "[quick][knuth-bendix][kbmag][recursive]",
                                   RevRPOTrie,
                                   RevRPOSet) {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("hHgGfFyYdDcCbBaA"s).contains_empty_word(true);

    // presentation::add_inverse_rules(p, "HhGgFfYyDdCcBbAa");
    presentation::add_rule(p, "BAba"s, "c"s);
    presentation::add_rule(p, "CAca"s, "d"s);
    presentation::add_rule(p, "DAda"s, "y"s);
    presentation::add_rule(p, "YByb"s, "f"s);
    presentation::add_rule(p, "FAfa"s, "g"s);
    presentation::add_rule(p, "ga"s, "ag"s);
    presentation::add_rule(p, "GBgb"s, "h"s);
    presentation::add_rule(p, "cb"s, "bc"s);
    presentation::add_rule(p, "ya"s, "ay"s);

    KnuthBendix<std::string, TestType> kb(congruence_kind::twosided, p);

    kb.run();
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.rewriting_system().number_of_rules() == 9);

    REQUIRE(knuth_bendix::contains(kb, "BAba", "c"));
    REQUIRE(knuth_bendix::contains(kb, "CAca", "d"));
    REQUIRE(knuth_bendix::contains(kb, "DAda", "y"));
    REQUIRE(knuth_bendix::contains(kb, "YByb", "f"));
    REQUIRE(knuth_bendix::contains(kb, "FAfa", "g"));
    REQUIRE(knuth_bendix::contains(kb, "ga", "ag"));
    REQUIRE(knuth_bendix::contains(kb, "GBgb", "h"));
    REQUIRE(knuth_bendix::contains(kb, "cb", "bc"));
    REQUIRE(knuth_bendix::contains(kb, "ya", "ay"));
    REQUIRE(
        (kb.active_rules() | rx::sort() | rx::to_vector())
        == std::vector<std::pair<std::string, std::string>>({{"BAba", "c"},
                                                             {"CAca", "d"},
                                                             {"DAda", "y"},
                                                             {"FAfa", "g"},
                                                             {"GBgb", "h"},
                                                             {"YByb", "f"},
                                                             {"cb", "bc"},
                                                             {"ga", "ag"},
                                                             {"ya", "ay"}}));
  }

  // //  A nonhopfian group
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "996",
                                   "kbmag/nonhopf",
                                   "[quick][knuth-bendix][kbmag][recursive]",
                                   RevRPOTrie,
                                   RevRPOSet) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;

    p.contains_empty_word(true).alphabet("aAbB"s);
    presentation::add_inverse_rules(p, "AaBb"s);
    presentation::add_rule(p, "Baab"s, "aaa"s);

    KnuthBendix<std::string, TestType> kb(twosided, p);

    kb.run();
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.rewriting_system().number_of_rules() == 8);

    REQUIRE(knuth_bendix::contains(kb, "Baab", "aaa"));
    REQUIRE(
        (kb.active_rules() | rx::sort() | rx::to_vector())
        == std::vector<std::pair<std::string, std::string>>({{"AB", "aaBAA"},
                                                             {"Aa", ""},
                                                             {"Ab", "abAAA"},
                                                             {"Bb", ""},
                                                             {"aA", ""},
                                                             {"aaaB", "Baa"},
                                                             {"aab", "baaa"},
                                                             {"bB", ""}}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "997",
                                   "kbmag/freenilpc3",
                                   "[quick][knuth-bendix][kbmag][recursive]",
                                   RevRPOTrie,
                                   RevRPOSet) {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("eEdDcCbBaA"s).contains_empty_word(true);

    presentation::add_inverse_rules(p, "EeDdCcBbAa"s);
    presentation::add_rule(p, "BAba"s, "c"s);
    presentation::add_rule(p, "CAca"s, "d"s);
    presentation::add_rule(p, "CBcb"s, "e"s);
    presentation::add_rule(p, "da"s, "ad"s);
    presentation::add_rule(p, "ea"s, "ae"s);
    presentation::add_rule(p, "db"s, "bd"s);
    presentation::add_rule(p, "eb"s, "be"s);

    KnuthBendix<std::string, TestType> kb(twosided, p);

    REQUIRE(!kb.rewriting_system().confluent());
    kb.run();
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.rewriting_system().number_of_rules() == 50);

    REQUIRE(knuth_bendix::contains(kb, "BAba", "c"));
    REQUIRE(knuth_bendix::contains(kb, "CAca", "d"));
    REQUIRE(knuth_bendix::contains(kb, "CBcb", "e"));
    REQUIRE(knuth_bendix::contains(kb, "da", "ad"));
    REQUIRE(knuth_bendix::contains(kb, "ea", "ae"));
    REQUIRE(knuth_bendix::contains(kb, "db", "bd"));
    REQUIRE(knuth_bendix::contains(kb, "eb", "be"));

    auto found = (kb.active_rules() | rx::to_vector());
    std::sort(found.begin(), found.end());
    // the following is from KBMAG
    std::vector<std::pair<std::string, std::string>> expected
        = {{"eE", ""},     {"Ee", ""},     {"dD", ""},    {"Dd", ""},
           {"cC", ""},     {"Cc", ""},     {"bB", ""},    {"Bb", ""},
           {"aA", ""},     {"Aa", ""},     {"da", "ad"},  {"ea", "ae"},
           {"db", "bd"},   {"eb", "be"},   {"ba", "abc"}, {"BA", "ABcDE"},
           {"ca", "acd"},  {"CA", "ACd"},  {"cb", "bce"}, {"CB", "BCe"},
           {"Da", "aD"},   {"dA", "Ad"},   {"Ea", "aE"},  {"eA", "Ae"},
           {"Db", "bD"},   {"dB", "Bd"},   {"Eb", "bE"},  {"eB", "Be"},
           {"DA", "AD"},   {"EA", "AE"},   {"DB", "BD"},  {"EB", "BE"},
           {"Ba", "aBCe"}, {"Ca", "aCD"},  {"cA", "AcD"}, {"Cb", "bCE"},
           {"cB", "BcE"},  {"ed", "de"},   {"ED", "DE"},  {"eD", "De"},
           {"Ed", "dE"},   {"bA", "AbCd"}, {"dc", "cd"},  {"Dc", "cD"},
           {"dC", "Cd"},   {"DC", "CD"},   {"ec", "ce"},  {"Ec", "cE"},
           {"eC", "Ce"},   {"EC", "CE"}};
    std::sort(expected.begin(), expected.end());

    REQUIRE(found == expected);
  }

  // Free nilpotent group of rank 2 and class 2
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "998",
                                   "kbmag/nilp2",
                                   "[quick][knuth-bendix][kbmag][recursive]",
                                   RevRPOTrie,
                                   RevRPOSet) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("cCbBaA"s).contains_empty_word(true);
    presentation::add_inverse_rules(p, "CcBbAa"s);
    presentation::add_rule_no_checks(p, "ba"s, "abc"s);
    presentation::add_rule_no_checks(p, "ca"s, "ac"s);
    presentation::add_rule_no_checks(p, "cb"s, "bc"s);

    KnuthBendix<std::string, TestType> kb(twosided, p);
    // REQUIRE(kb.rewriting_system().confluent());

    kb.run();
    REQUIRE(kb.rewriting_system().confluent());

    REQUIRE(kb.rewriting_system().number_of_rules() == 18);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "048",
                                   "order of pending rules is important",
                                   "[quick][knuth-bendix][kbmag]",
                                   LenLexSet,
                                   LenLexTrie) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abc"s).contains_empty_word(true);

    SECTION("order #1") {
      presentation::add_rule(p, "bbac"s, ""s);
      presentation::add_rule(p, "abbacca"s, ""s);
      presentation::add_rule(p, "bacc"s, ""s);

      KnuthBendix<std::string, TestType> kb(twosided, p);
      kb.rewriting_system().sort_pending_rules_by(nullptr);
      REQUIRE((kb.active_rules() | rx::sort() | rx::to_vector())
              == std::vector<std::pair<std::string, std::string>>(
                  {{"aba", ""}, {"bacc", ""}, {"bbac", ""}}));
    }
    SECTION("order #2") {
      presentation::add_rule(p, "bacc"s, ""s);
      presentation::add_rule(p, "abbacca"s, ""s);
      presentation::add_rule(p, "bbac"s, ""s);

      KnuthBendix<std::string, TestType> kb(twosided, p);
      kb.rewriting_system().sort_pending_rules_by(nullptr);
      REQUIRE((kb.active_rules() | rx::sort() | rx::to_vector())
              == std::vector<std::pair<std::string, std::string>>(
                  {{"aca", ""}, {"bacc", ""}, {"bbac", ""}}));
    }
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "112",
                                   "Sorouhesh",
                                   "[quick][knuth-bendix][kbmag]",
                                   LenLexSet,
                                   LenLexTrie) {
    using words::pow;
    using order     = typename TestType::reduction_order;
    auto         rg = ReportGuard(false);
    size_t const n  = 2;
    size_t const q  = 11;

    Presentation<std::string> p;
    p.alphabet("ab"s);
    presentation::add_rule(p, pow("a", std::pow(5, n)), "a"s);
    presentation::add_rule(p, "aba"s, "b"s);
    presentation::add_rule(p, "ab"s, pow("b", q) + "a");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.rewriting_system().confluent());

    kb.run();
    if constexpr (std::is_same_v<order, LenLexCmp<Default, false>>) {
      REQUIRE(kb.rewriting_system().number_of_rules() == 7);
    } else {
      REQUIRE(kb.rewriting_system().number_of_rules() == 4);
    }
    REQUIRE(kb.number_of_classes() == size_t(std::pow(5, n)) + 4 * q - 5);
    REQUIRE(knuth_bendix::reduce(kb, "aabb") == "aabb");
    REQUIRE(knuth_bendix::reduce(kb, "aabbaabb") == "bbbb");
    REQUIRE(knuth_bendix::reduce(kb, "aabbaabbaabb") == "aabbbbbb");
    REQUIRE(knuth_bendix::reduce(kb, "aabbaabbaabbaabb") == "bbbbbbbb");
    REQUIRE(knuth_bendix::reduce(kb, "aabbaabbaabbaabbaabb") == "aabbbbbbbbbb");
    auto nf = knuth_bendix::normal_forms(kb).min(1);
    REQUIRE((nf | to_vector())
            == std::vector<std::string>({"a",
                                         "b",
                                         "aa",
                                         "ab",
                                         "ba",
                                         "bb",
                                         "aaa",
                                         "aab",
                                         "abb",
                                         "bab",
                                         "bbb",
                                         "aaaa",
                                         "aabb",
                                         "abbb",
                                         "babb",
                                         "bbbb",
                                         "aaaaa",
                                         "aabbb",
                                         "abbbb",
                                         "babbb",
                                         "bbbbb",
                                         "aaaaaa",
                                         "aabbbb",
                                         "abbbbb",
                                         "babbbb",
                                         "bbbbbb",
                                         "aaaaaaa",
                                         "aabbbbb",
                                         "abbbbbb",
                                         "babbbbb",
                                         "bbbbbbb",
                                         "aaaaaaaa",
                                         "aabbbbbb",
                                         "abbbbbbb",
                                         "babbbbbb",
                                         "bbbbbbbb",
                                         "aaaaaaaaa",
                                         "aabbbbbbb",
                                         "abbbbbbbb",
                                         "babbbbbbb",
                                         "bbbbbbbbb",
                                         "aaaaaaaaaa",
                                         "aabbbbbbbb",
                                         "abbbbbbbbb",
                                         "babbbbbbbb",
                                         "bbbbbbbbbb",
                                         "aaaaaaaaaaa",
                                         "aabbbbbbbbb",
                                         "abbbbbbbbbb",
                                         "babbbbbbbbb",
                                         "aaaaaaaaaaaa",
                                         "aabbbbbbbbbb",
                                         "aaaaaaaaaaaaa",
                                         "aaaaaaaaaaaaaa",
                                         "aaaaaaaaaaaaaaa",
                                         "aaaaaaaaaaaaaaaa",
                                         "aaaaaaaaaaaaaaaaa",
                                         "aaaaaaaaaaaaaaaaaa",
                                         "aaaaaaaaaaaaaaaaaaa",
                                         "aaaaaaaaaaaaaaaaaaaa",
                                         "aaaaaaaaaaaaaaaaaaaaa",
                                         "aaaaaaaaaaaaaaaaaaaaaa",
                                         "aaaaaaaaaaaaaaaaaaaaaaa",
                                         "aaaaaaaaaaaaaaaaaaaaaaaa"}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "115",
                                   "Konovalov",
                                   "[quick][knuth-bendix]",
                                   LenLexSet,
                                   LenLexTrie) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("abAB"s);
    presentation::add_rule(p, "Abba"s, "BB"s);
    presentation::add_rule(p, "Baab"s, "AA"s);

    KnuthBendix<std::string, TestType> k(twosided, p);
    k.run();
    REQUIRE(k.number_of_classes() == POSITIVE_INFINITY);
  }

}  // namespace libsemigroups
