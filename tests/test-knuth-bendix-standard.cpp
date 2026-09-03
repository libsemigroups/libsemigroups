// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-2026 James D. Mitchell
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

#define CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS

#include <algorithm>      // for next_permutation
#include <chrono>         // for milliseconds, seconds
#include <cmath>          // for pow
#include <cstddef>        // for size_t
#include <iostream>       // for string, operator<<, endl
#include <numeric>        // for iota
#include <string>         // for basic_string, char_traits
#include <unordered_set>  // for unordered_set
#include <utility>        // for move, operator==, pair
#include <vector>         // for vector, operator==

#include "Catch2-3.14.0/catch_amalgamated.hpp"  // for AssertionHandler, oper...
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEMPLATE_TEST_CASE

#include "libsemigroups/constants.hpp"  // for operator==, operat...
#include "libsemigroups/detail/rules.hpp"
#include "libsemigroups/exception.hpp"  // for LibsemigroupsExcep...
#include "libsemigroups/knuth-bendix-helpers.hpp"
#include "libsemigroups/knuth-bendix.hpp"           // for KnuthBendix, norma...
#include "libsemigroups/order.hpp"                  // for lenlex_cmp
#include "libsemigroups/paths.hpp"                  // for Paths
#include "libsemigroups/presentation-examples.hpp"  // for partition_mo
#include "libsemigroups/presentation.hpp"           // for add_rule, Presenta...
#include "libsemigroups/word-graph-helpers.hpp"     // for word_graph
#include "libsemigroups/word-graph.hpp"             // for WordGraph
#include "libsemigroups/word-range.hpp"             // for Inner, StringRange...
#include "libsemigroups/words-helpers.hpp"          // for literals

#include "libsemigroups/detail/report.hpp"  // for ReportGuard
#include "libsemigroups/detail/stl.hpp"     // for apply_permutation
#include "libsemigroups/detail/string.hpp"  // for random_string, operator<<

namespace libsemigroups {
  using std::literals::operator""s;

  using literals::operator""_w;

  congruence_kind constexpr twosided = congruence_kind::twosided;

  using namespace rx;

  using LenLexTrie = detail::RewritingSystemTrie<LenLexCmp>;
  using LenLexSet  = detail::RewritingSystemSet<LenLexCmp>;

  using RPOTrie = detail::RewritingSystemTrie<RPOCmp>;
  using RPOSet  = detail::RewritingSystemSet<RPOCmp>;

  using RevRPOTrie = detail::RewritingSystemTrie<RevRPOCmp>;
  using RevRPOSet  = detail::RewritingSystemSet<RevRPOCmp>;

#define REWRITING_SYSTEM_TYPES LenLexTrie, LenLexSet, RevRPOTrie, RevRPOSet

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "065",
                                   "sigma sylvester monoid x 2",
                                   "[knuth-bendix][standard]",
                                   REWRITING_SYSTEM_TYPES) {
    using namespace literals;
    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_idempotent_rules_no_checks(p, 01_w);
    using words::operator+;
    WordRange    words;
    words.alphabet_size(2).min(0).max(6);
    size_t n = 2;
    for (size_t a = 0; a < n - 1; ++a) {
      for (size_t b = a; b < n - 1; ++b) {
        for (size_t c = b + 1; c < n; ++c) {
          for (auto& u : words) {
            for (auto& v : words) {
              for (auto& w : words) {
                presentation::add_rule(
                    p, u + a + c + v + b + w, u + c + a + v + b + w);
              }
            }
          }
        }
      }
    }
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);
    presentation::remove_trivial_rules(p);
    p.contains_empty_word(true);
    std::reverse(p.rules.begin(), p.rules.end());

    KnuthBendix<word_type, TestType> kb(twosided, p);

    auto S = to<FroidurePin>(kb);
    REQUIRE(S.contains_one());
    REQUIRE(S.size() == kb.number_of_classes());
    REQUIRE(S.number_of_idempotents() == 5);
    REQUIRE(kb.number_of_classes() == 6);
  }

  // Takes approx. 2s
  // RevRPOTrie + RevRPOSet return different numbers of rules at the end.
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "100",
                                   "Sims Ex. 6.6 (limited overlap lengths)",
                                   "[standard][knuth-bendix]",
                                   RevRPOTrie,
                                   LenLexTrie) {
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("abc"s);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bc"s, ""s);
    presentation::add_rule(p, "bbb"s, ""s);
    presentation::add_rule(p, "ababababababab"s, ""s);
    presentation::add_rule(p, "abacabacabacabacabacabacabacabac"s, ""s);

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(kb.overlap_policy() == decltype(kb)::options::overlap::ABC);

    REQUIRE(!kb.rewriting_system().confluent());

    using order = typename TestType::reduction_order;
    if constexpr (std::is_same_v<order, LenLexCmp<Default, false>>) {
      // In Sims it says to use 44 here, but that doesn't seem to work.
      kb.max_overlap(45);
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 1'026);
    } else if (std::is_same_v<order, RevRPOCmp<Default, false>>) {
      kb.max_overlap(56);
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 407);
    }
  }

  // Takes approx. 2s, is very slow with RevRPO
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "101",
                                   "kbmag/standalone/kb_data/funny3",
                                   "[standard][knuth-bendix][kbmag]",
                                   LenLexSet,
                                   LenLexTrie) {
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("aAbBcC"s);

    presentation::add_inverse_rules(p, "AaBbCc"s);

    presentation::add_rule(p, "aaa"s, ""s);
    presentation::add_rule(p, "bbb"s, ""s);
    presentation::add_rule(p, "ccc"s, ""s);
    presentation::add_rule(p, "ABa"s, "BaB"s);
    presentation::add_rule(p, "bcB"s, "cBc"s);
    presentation::add_rule(p, "caC"s, "aCa"s);
    presentation::add_rule(p, "abcABCabcABCabcABC"s, ""s);
    presentation::add_rule(p, "BcabCABcabCABcabCA"s, ""s);
    presentation::add_rule(p, "cbACBacbACBacbACBa"s, ""s);

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.rewriting_system().confluent());
    REQUIRE(kb.overlap_policy() == decltype(kb)::options::overlap::ABC);

    kb.rewriting_system().settings().reduction_threshold = 200;
    knuth_bendix::by_overlap_length(kb);
    // kb.run() // also works, but is slower
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.rewriting_system().number_of_rules() == 8);
    REQUIRE(kb.number_of_classes() == 3);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf | to_vector()) == std::vector<std::string>({"", "a", "A"}));
  }

  // Weyl group E8 (all gens involutory).
  // Takes approx. 5s for KnuthBendix
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "104",
                                   "kbmag/standalone/kb_data/e8",
                                   "[standard][knuth-bendix][kbmag]",
                                   LenLexTrie,
                                   RevRPOTrie) {
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("abcdefgh"s);

    presentation::add_inverse_rules(p, "abcdefgh"s);
    presentation::add_rule(p, "bab"s, "aba"s);
    presentation::add_rule(p, "ca"s, "ac"s);
    presentation::add_rule(p, "da"s, "ad"s);
    presentation::add_rule(p, "ea"s, "ae"s);
    presentation::add_rule(p, "fa"s, "af"s);
    presentation::add_rule(p, "ga"s, "ag"s);
    presentation::add_rule(p, "ha"s, "ah"s);
    presentation::add_rule(p, "cbc"s, "bcb"s);
    presentation::add_rule(p, "db"s, "bd"s);
    presentation::add_rule(p, "eb"s, "be"s);
    presentation::add_rule(p, "fb"s, "bf"s);
    presentation::add_rule(p, "gb"s, "bg"s);
    presentation::add_rule(p, "hb"s, "bh"s);
    presentation::add_rule(p, "dcd"s, "cdc"s);
    presentation::add_rule(p, "ece"s, "cec"s);
    presentation::add_rule(p, "fc"s, "cf"s);
    presentation::add_rule(p, "gc"s, "cg"s);
    presentation::add_rule(p, "hc"s, "ch"s);
    presentation::add_rule(p, "ed"s, "de"s);
    presentation::add_rule(p, "fd"s, "df"s);
    presentation::add_rule(p, "gd"s, "dg"s);
    presentation::add_rule(p, "hd"s, "dh"s);
    presentation::add_rule(p, "fef"s, "efe"s);
    presentation::add_rule(p, "ge"s, "eg"s);
    presentation::add_rule(p, "he"s, "eh"s);
    presentation::add_rule(p, "gfg"s, "fgf"s);
    presentation::add_rule(p, "hf"s, "fh"s);
    presentation::add_rule(p, "hgh"s, "ghg"s);

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.rewriting_system().confluent());
    knuth_bendix::by_overlap_length(kb);
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.rewriting_system().number_of_rules() == 192);
    REQUIRE(kb.number_of_classes() == 696'729'600);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "140",
                                   "full_transformation_monoid Iwahori",
                                   "[standard][knuth-bendix]",
                                   LenLexTrie,
                                   RevRPOTrie) {
    size_t n = 5;
    auto   p = presentation::examples::full_transformation_monoid_II74(n);
    KnuthBendix<word_type, TestType> kb(twosided, p);
    REQUIRE(!is_obviously_infinite(kb));
    kb.run();
    if constexpr (std::is_same_v<TestType, RevRPOTrie>) {
      REQUIRE(kb.rewriting_system().number_of_rules() == 230);
    } else {
      REQUIRE(kb.rewriting_system().number_of_rules() == 1'162);
    }
    REQUIRE(kb.number_of_classes() == 3'125);
  }

  LIBSEMIGROUPS_TEST_CASE("TietzeExplorer",
                          "148",
                          "bababbbabba=a",
                          "[knuth-bendix][standard][tietze-explorer]") {
    using namespace knuth_bendix;
    fmt::print("\n");
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("ab"s);
    p.rules = {"bababbbabba", "a"};

    KnuthBendix kb(congruence_kind::twosided, p);

    TietzeExplorer dora(kb);

    dora.depth_max(1).depth_min(1).run_each_for(std::chrono::milliseconds(10));

    auto result = dora.result();

    REQUIRE(result.has_value());
    REQUIRE(result.value().presentation().alphabet() == "abc");
    REQUIRE((result.value().active_rules() | rx::sort() | rx::to_vector())
            == std::vector<std::pair<std::string, std::string>>(
                {{"ababaa", "bccca"},
                 {"abababa", "cca"},
                 {"abababba", "bcca"},
                 {"abababbc", "bccc"},
                 {"abababc", "ccc"},
                 {"ababac", "bcccc"},
                 {"ababbba", "bcc"},
                 {"bbaa", "abba"},
                 {"bbabaa", "ababba"},
                 {"bbababa", "abbbaba"},
                 {"bbababba", "abbbabba"},
                 {"bbababbc", "abbba"},
                 {"bbababc", "abbbabc"},
                 {"bbabac", "ababbc"},
                 {"bbabca", "aba"},
                 {"bbabcc", "abc"},
                 {"bbac", "abbc"},
                 {"bbca", "a"},
                 {"bbcc", "c"},
                 {"caa", "aca"},
                 {"caba", "abca"},
                 {"cabba", "aa"},
                 {"cabbba", "ababbc"},
                 {"cabbc", "ac"},
                 {"cabc", "abcc"},
                 {"cac", "acc"},
                 {"cba", "bca"},
                 {"cbba", "a"},
                 {"cbc", "bcc"}}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "150",
                                   "baabbbaba=a",
                                   "[standard][tietze-explorer]",
                                   RevRPOTrie) {
    fmt::print("\n");
    Presentation<std::string> p;
    p.alphabet("ab"s);
    p.contains_empty_word(true);
    presentation::add_rule(p, "baabbbaba"s, "a"s);

    KnuthBendix<std::string, TestType> kb(twosided, p);
    knuth_bendix::TietzeExplorer       dora(kb);
    auto result = dora.number_of_threads(5).depth_max(2).result();

    using rule_type = typename decltype(kb)::rule_type;
    REQUIRE(result.has_value());
    REQUIRE(result.value().rewriting_system().confluent());
    REQUIRE(result.value().presentation().alphabet() == "dabc");
    REQUIRE(
        result.value().presentation().rules
        == std::vector<std::string>({"ca", "a", "c", "baadb", "d", "bbba"}));

    REQUIRE((result.value().active_rules() | rx::sort() | rx::to_vector())
            == std::vector<rule_type>({{"ba", "dada"},
                                       {"bdaa", "daddaa"},
                                       {"bdada", "daddada"},
                                       {"bdaddaa", "ddaaddada"},
                                       {"bdaddada", "d"},
                                       {"c", "dadaadb"},
                                       {"dadaaa", "aaddaa"},
                                       {"dadaada", "aaddada"},
                                       {"dadaaddaa", "adaaddada"},
                                       {"dadaaddada", "a"}}));

    REQUIRE(dora.finished());
    dora.run();  // for code coverage
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "154",
                          "TietzeExplorer::run_for",
                          "[standard][tietze-explorer]") {
    using literals::operator""_w;

    Presentation<word_type> p;
    p.alphabet(2);
    p.contains_empty_word(true);
    presentation::add_rule(p, "baaabaaa"_w, "aba"_w);

    KnuthBendix kb(twosided, p);

    knuth_bendix::TietzeExplorer dora(kb);

    dora.run_for(std::chrono::milliseconds(10));
    dora.run_for(std::chrono::milliseconds(10));
    dora.run_for(std::chrono::milliseconds(10));
  }

}  // namespace libsemigroups
