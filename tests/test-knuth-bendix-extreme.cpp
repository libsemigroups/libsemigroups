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

// This file contains all of the Knuth-Bendix tests tagged "extreme".

#define CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS

#include <algorithm>    // for find, all_of
#include <chrono>       // for seconds
#include <complex>      // for operator*, ope...
#include <cstddef>      // for size_t
#include <iterator>     // for back_inserter
#include <list>         // for operator!=
#include <numeric>      // for accumulate, iota
#include <string>       // for basic_string
#include <tuple>        // for get
#include <type_traits>  // for is_same_v
#include <utility>      // for pair, forward
#include <vector>       // for vector, operat...

#include "Catch2-3.14.0/catch_amalgamated.hpp"  // for AssertionHandler, oper...
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEMPLATE_TEST_CASE

#include "libsemigroups/cong-common-helpers.hpp"    // for reduce
#include "libsemigroups/constants.hpp"              // for operator==
#include "libsemigroups/knuth-bendix-class.hpp"     // for KnuthBendix
#include "libsemigroups/knuth-bendix-helpers.hpp"   // for normal_forms
#include "libsemigroups/obvinf.hpp"                 // for is_obviously_i...
#include "libsemigroups/order.hpp"                  // for LenLexCmp
#include "libsemigroups/paths-count.hpp"            // for count
#include "libsemigroups/paths.hpp"                  // for cbegin_pilo
#include "libsemigroups/presentation-examples.hpp"  // for full_transform...
#include "libsemigroups/presentation.hpp"           // for add_rule
#include "libsemigroups/ranges.hpp"                 // for operator|
#include "libsemigroups/todd-coxeter-helpers.hpp"   // for normal_forms
#include "libsemigroups/types.hpp"                  // for word_type, con...
#include "libsemigroups/word-graph-helpers.hpp"     // for is_acyclic
#include "libsemigroups/word-graph-view.hpp"        // for WordGraphView:...
#include "libsemigroups/word-graph.hpp"             // for WordGraph::target
#include "libsemigroups/word-range.hpp"             // for StringRange

#include "libsemigroups/detail/cong-common-class.hpp"  // for CongruenceComm...
#include "libsemigroups/detail/eigen.hpp"              // for eigen
#include "libsemigroups/detail/fmt.hpp"                // for fmt
#include "libsemigroups/detail/iterator.hpp"           // for operator+
#include "libsemigroups/detail/knuth-bendix-impl.hpp"  // for KnuthBendixImp...
#include "libsemigroups/detail/path-iterators.hpp"     // for const_pilo_ite...
#include "libsemigroups/detail/print.hpp"              // for to_printable
#include "libsemigroups/detail/report.hpp"             // for report_default
#include "libsemigroups/detail/rewriting-system.hpp"   // for RewritingSyste...
#include "libsemigroups/detail/rules.hpp"        // for reorder, rev_rpo_cmp
#include "libsemigroups/detail/string.hpp"       // for group_digits
#include "libsemigroups/detail/timer.hpp"        // for string_time
#include "libsemigroups/detail/value-guard.hpp"  // for ValueGuard::Va...

namespace libsemigroups {
  using literals::operator""_w;

  congruence_kind constexpr twosided = congruence_kind::twosided;

  using namespace rx;

  using LenLexTrie = detail::RewritingSystemTrie<LenLexCmp>;
  using LenLexSet  = detail::RewritingSystemSet<LenLexCmp>;
  using RPOTrie    = detail::RewritingSystemTrie<RevRPOCmp>;
  using RPOSet     = detail::RewritingSystemSet<RevRPOCmp>;

#define REWRITING_SYSTEM_TYPES LenLexTrie, RPOTrie

  ////////////////////////////////////////////////////////////////////////
  // Extreme tests
  ////////////////////////////////////////////////////////////////////////

  // Fibonacci group F(2,7) - order 29 - works better with largish tidyint
  // [102]: KnuthBendix: kbmag/standalone/kb_data/f27 - RPOTrie
  // -- plain Knuth-Bendix ......4.681s
  // -- by_overlap_length ......5.677s
  // [102]: KnuthBendix: kbmag/standalone/kb_data/f27 - LenLexTrie
  // -- plain Knuth-Bendix .....14.223s
  // -- by_overlap_length ......4.301s
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "102",
                                   "kbmag/standalone/kb_data/f27",
                                   "[extreme][knuth-bendix][kbmag]",
                                   REWRITING_SYSTEM_TYPES) {
    auto rg = ReportGuard(true);

    Presentation<std::string> p;
    p.alphabet("aAbBcCdDyYfFgG");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "AaBbCcDdYyFfGg");
    presentation::add_rule(p, "ab", "c");
    presentation::add_rule(p, "bc", "d");
    presentation::add_rule(p, "cd", "y");
    presentation::add_rule(p, "dy", "f");
    presentation::add_rule(p, "yf", "g");
    presentation::add_rule(p, "fg", "a");
    presentation::add_rule(p, "ga", "b");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    kb.rewriting_system().use_new_rule_trie([](auto const& rws) {
      return rws.pending_rules().size() < rws.active_rules().size();
    });
    REQUIRE(!kb.rewriting_system().confluent());
    SECTION("plain Knuth-Bendix") {
      kb.run();
    }
    SECTION("by_overlap_length") {
      knuth_bendix::by_overlap_length(kb);
    }
    REQUIRE(kb.finished());
    REQUIRE(kb.rewriting_system().confluent());

    if constexpr (std::is_same_v<TestType, RPOTrie>) {
      REQUIRE(kb.rewriting_system().number_of_rules() == 14);
    } else {
      REQUIRE(kb.rewriting_system().number_of_rules() == 194);
    }

    REQUIRE(kb.number_of_classes() == 29);
  }

  // Mathieu group M_11
  // [103]: KnuthBendix: kbmag/standalone/kb_data/m11 - LenLexTrie ......6.062s
  // [103]: KnuthBendix: kbmag/standalone/kb_data/m11 - RPOTrie ......2.913s
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "103",
                                   "kbmag/standalone/kb_data/m11",
                                   "[extreme][knuth-bendix][kbmag]",
                                   REWRITING_SYSTEM_TYPES) {
    auto rg = ReportGuard(true);

    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("abB");

    presentation::add_inverse_rules(p, "aBb");
    presentation::add_rule(p, "BB", "bb");
    presentation::add_rule(p, "BaBaBaBaBaB", "abababababa");
    presentation::add_rule(p, "bbabbabba", "abbabbabb");
    presentation::add_rule(p, "aBaBababaBabaBBaBab", "");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.rewriting_system().confluent());

    knuth_bendix::by_overlap_length(kb);
    REQUIRE(kb.rewriting_system().confluent());
    if constexpr (std::is_same_v<TestType, RPOTrie>) {
      REQUIRE(kb.rewriting_system().number_of_rules() == 741);
    } else {
      REQUIRE(kb.rewriting_system().number_of_rules() == 1'731);
    }
    REQUIRE(kb.number_of_classes() == 7'920);
    REQUIRE(knuth_bendix::reduce(kb, "") == "");

    presentation::add_rule(p, "a", "");
    presentation::add_rule(p, "a", "b");
    presentation::add_rule(p, "B", "a");

    KnuthBendix<std::string, TestType> kb2(twosided, p);
    REQUIRE(kb2.number_of_classes() == 1);

    auto ntc = knuth_bendix::non_trivial_classes(kb, kb2);
    REQUIRE(ntc.size() == 1);
    REQUIRE(ntc[0].size() == 7'920);
    REQUIRE(std::find(ntc[0].cbegin(), ntc[0].cend(), "") != ntc[0].cend());
    std::sort(ntc[0].begin(), ntc[0].end(), LenLexCmp());

    REQUIRE(ntc[0]
            == (knuth_bendix::normal_forms(kb) | rx::sort(LenLexCmp())
                | rx::to_vector()));
  }

  // Second of BHN's series of increasingly complicated presentations of 1.
  // Works quickest with large value of tidyint
  // [105]: KnuthBendix: kbmag/standalone/kb_data/degen4b - LenLexTrie
  // ......2.600s
  // [105]: KnuthBendix: kbmag/standalone/kb_data/degen4b - RPOTrie
  // .......790ms
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "105",
                                   "kbmag/standalone/kb_data/degen4b",
                                   "[extreme][knuth-bendix][kbmag]",
                                   REWRITING_SYSTEM_TYPES) {
    auto rg = ReportGuard(true);

    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("aAbBcC");
    presentation::add_inverse_rules(p, "AaBbCc");
    presentation::add_rule(p, "bbABaBcbCCAbaBBccBCbccBCb", "");
    presentation::add_rule(p, "ccBCbCacAABcbCCaaCAcaaCAc", "");
    presentation::add_rule(p, "aaCAcAbaBBCacAAbbABabbABa", "");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.rewriting_system().confluent());
    // kb.run();
    knuth_bendix::by_overlap_length(kb);
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.rewriting_system().number_of_rules() == 6);
    REQUIRE(kb.number_of_classes() == 1);
  }

  // Two generator presentation of Fibonacci group F(2,7) - order 29. Large
  // value of tidyint works better.
  // [106]: KnuthBendix: kbmag/standalone/kb_data/f27_2gen - LenLexTrie
  // ......8.602s
  // [106]: KnuthBendix: kbmag/standalone/kb_data/f27_2gen - RPOTrie
  // ......3.956s
  // Takes > 19s (knuth_bendix), didn't run to the end
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "106",
                                   "kbmag/standalone/kb_data/f27_2gen",
                                   "[extreme][knuth-bendix][kbmag]",
                                   REWRITING_SYSTEM_TYPES) {
    auto rg = ReportGuard(true);

    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("aAbB");
    presentation::add_inverse_rules(p, "AaBb");
    presentation::add_rule(p, "bababbababbabbababbab", "a");
    presentation::add_rule(p, "abbabbababbaba", "b");

    KnuthBendix<std::string, TestType> kb(twosided, p);

    kb.rewriting_system().use_new_rule_trie([](auto const& rws) {
      return rws.pending_rules().size() < rws.active_rules().size();
    });
    REQUIRE(!kb.rewriting_system().confluent());

    knuth_bendix::by_overlap_length(kb);
    REQUIRE(kb.rewriting_system().confluent());
    if constexpr (std::is_same_v<TestType, RPOTrie>) {
      REQUIRE(kb.rewriting_system().number_of_rules() == 4);
    } else {
      REQUIRE(kb.rewriting_system().number_of_rules() == 19);
    }
    REQUIRE(kb.number_of_classes() == 29);
  }

  // [107]: KnuthBendix: Example 6.6 in Sims - LenLexTrie
  // -- plain Knuth-Bendix ......6.405s
  // -- by overlap length ......5.439s
  // [107]: KnuthBendix: Example 6.6 in Sims - RPOTrie
  // -- plain Knuth-Bendix ......2.691s
  // -- by overlap length ......2.483s
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "107",
                                   "Example 6.6 in Sims",
                                   "[extreme][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("abc");

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bc", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "ababababababab", "");
    presentation::add_rule(p, "abacabacabacabacabacabacabacabac", "");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.rewriting_system().confluent());
    SECTION("plain Knuth-Bendix") {
      kb.run();
    }
    SECTION("by overlap length") {
      knuth_bendix::by_overlap_length(kb);
    }
    if constexpr (std::is_same_v<TestType, RPOTrie>) {
      REQUIRE(kb.rewriting_system().number_of_rules() == 408);
    } else {
      REQUIRE(kb.rewriting_system().number_of_rules() == 1'026);
    }
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.number_of_classes() == 10'752);
  }

  // [108]: KnuthBendix: kbmag/standalone/kb_data/f27 - LenLexTrie ......3.072s
  // [108]: KnuthBendix: kbmag/standalone/kb_data/f27 - RPOTrie .....20.083s
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "108",
                                   "kbmag/standalone/kb_data/f27monoid",
                                   "[extreme][knuth-bendix][kbmag]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    p.alphabet("abcdyfg");

    presentation::add_rule(p, "ab", "c");
    presentation::add_rule(p, "bc", "d");
    presentation::add_rule(p, "cd", "y");
    presentation::add_rule(p, "dy", "f");
    presentation::add_rule(p, "yf", "g");
    presentation::add_rule(p, "fg", "a");
    presentation::add_rule(p, "ga", "b");

    KnuthBendix<std::string, TestType> kb(twosided, p);

    REQUIRE(!kb.rewriting_system().confluent());
    kb.rewriting_system().use_new_rule_trie([](auto const& rws) {
      return rws.pending_rules().size() < rws.active_rules().size();
    });

    knuth_bendix::by_overlap_length(kb);
    // Fails to terminate, or is very slow, with knuth_bendix
    REQUIRE(kb.rewriting_system().confluent());
    if constexpr (std::is_same_v<TestType, RPOTrie>) {
      REQUIRE(kb.rewriting_system().number_of_rules() == 7);
    } else {
      REQUIRE(kb.rewriting_system().number_of_rules() == 47);
    }
    REQUIRE(kb.number_of_classes() == 29);
  }

  // monoid presentation of F(2,7) - should produce a monoid of length 30
  // which is the same as the group, together with the empty word. This
  // is a very difficult calculation indeed, however.
  //
  // KBMAG does not seem to terminate when SHORTLEX order is used.
  //
  // [999]: KnuthBendix: kbmag/f27monoid - RPOTrie .....29.123s
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "999",
                                   "kbmag/f27monoid",
                                   "[extreme][knuth-bendix][kbmag]",
                                   RPOTrie) {
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    p.alphabet("abcdefg");
    presentation::add_rule_no_checks(p, "ab", "c");
    presentation::add_rule_no_checks(p, "bc", "d");
    presentation::add_rule_no_checks(p, "cd", "e");
    presentation::add_rule_no_checks(p, "de", "f");
    presentation::add_rule_no_checks(p, "ef", "g");
    presentation::add_rule_no_checks(p, "fg", "a");
    presentation::add_rule_no_checks(p, "ga", "b");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    kb.rewriting_system().use_new_rule_trie([](auto const& rws) {
      return rws.pending_rules().size() < rws.active_rules().size();
    });
    REQUIRE(!kb.rewriting_system().confluent());

    kb.run();
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.rewriting_system().number_of_rules() == 7);
  }

  // An extension of 2^6 be L32
  // [109]: KnuthBendix: kbmag/standalone/kb_data/l32ext - LenLexTrie
  // -- plain Knuth-Bendix ......6.502s
  // -- by overlap length ......5.480s
  // [109]: KnuthBendix: kbmag/standalone/kb_data/l32ext - RPOTrie
  // -- plain Knuth-Bendix ......2.713s
  // -- by overlap length ......2.622s
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "109",
                                   "kbmag/standalone/kb_data/l32ext",
                                   "[extreme][knuth-bendix][kbmag]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("abB");

    presentation::add_inverse_rules(p, "aBb");

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "BB", "b");
    presentation::add_rule(p, "BaBaBaB", "abababa");
    presentation::add_rule(p, "aBabaBabaBabaBab", "BabaBabaBabaBaba");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.rewriting_system().confluent());

    SECTION("plain Knuth-Bendix") {
      kb.run();
    }
    SECTION("by overlap length") {
      knuth_bendix::by_overlap_length(kb);
    }

    REQUIRE(kb.rewriting_system().confluent());
    if constexpr (std::is_same_v<TestType, RPOTrie>) {
      REQUIRE(kb.rewriting_system().number_of_rules() == 408);
    } else {
      REQUIRE(kb.rewriting_system().number_of_rules() == 1'026);
    }
    REQUIRE(kb.number_of_classes() == 10'752);

    auto& wg = kb.gilman_graph();
    REQUIRE(v4::word_graph::is_acyclic(wg));
  }

  // [117]: KnuthBendix: example with undecidable word problem - LenLexTrie
  // .....10.056s
  //  [117]: KnuthBendix: example with undecidable word problem - RPOTrie
  //  .....10.052s
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "117",
                                   "example with undecidable word problem",
                                   "[knuth-bendix][extreme]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("ab");
    presentation::add_rule(p, "abaabb", "bbaaba");
    presentation::add_rule(p, "aababba", "bbaaaba");
    presentation::add_rule(p, "abaaabb", "abbabaa");
    presentation::add_rule(p, "bbbaabbaaba", "bbbaabbaaaa");
    presentation::add_rule(p, "aaaabbaaba", "bbaaaa");
    KnuthBendix<std::string, TestType> k(twosided, p);
    k.run_for(std::chrono::seconds(10));
    REQUIRE(!k.finished());
  }

  // [146]: KnuthBendix: process millions of pending rules - LenLexTrie
  // -- sorted by lhs_rev_lex_cmp ......5.979s
  // -- sorted by lhs_lex_cmp ......6.223s
  // -- sorted by rev_rpo_cmp ......6.279s
  // [146]: KnuthBendix: process millions of pending rules - RPOTrie
  // -- sorted by lhs_rev_lex_cmp ......5.810s
  // -- sorted by lhs_lex_cmp ......6.241s
  // -- sorted by rev_rpo_cmp ......6.300s
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "146",
                                   "process millions of pending rules",
                                   "[knuth-bendix][extreme]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("abAB");

    auto rules = StringRange().alphabet(p.alphabet()).min(11).max(12);
    p.rules    = rules | rx::to_vector();
    presentation::add_rule(p, "aaabbb", "aabb");
    KnuthBendix<std::string, TestType> k(twosided, p);
    REQUIRE(k.rewriting_system().number_of_rules() == 2'097'153);

    SECTION("sorted by lhs_rev_lex_cmp") {
      k.rewriting_system().sort_pending_rules_by(detail::lhs_rev_lex_cmp);
      k.rewriting_system().reduce();
      REQUIRE(k.rewriting_system().number_of_rules() == 2'041'465);
    }
    SECTION("sorted by lhs_lex_cmp") {
      k.rewriting_system().sort_pending_rules_by(detail::lhs_lex_cmp);
      k.rewriting_system().reduce();
      REQUIRE(k.rewriting_system().number_of_rules() == 2'045'649);
    }
    SECTION("sorted by rev_rpo_cmp") {
      k.rewriting_system().sort_pending_rules_by(detail::rev_rpo_cmp);
      k.rewriting_system().reduce();
      REQUIRE(k.rewriting_system().number_of_rules() == 2'041'466);
    }
    REQUIRE(k.rewriting_system().pending_rules().size() == 0);

    for (auto const& [i, rule] : rx::enumerate(k.active_rules())) {
      REQUIRE(std::pair(i, knuth_bendix::reduce_no_run(k, rule.first))
              == std::pair(i, rule.second));
      REQUIRE(std::pair(i, knuth_bendix::reduce_no_run(k, rule.second))
              == std::pair(i, rule.second));
    }
  }

  // This example verifies the nilpotence of the group using the Sims
  // algorithm. The original presentation was <a,b| [b,a,a,a],
  // [b^-1,a,a,a], [a,b,b,b], [a^-1,b,b,b], [a,a*b,a*b,a*b],
  // [a^-1,a*b,a*b,a*b] >. (where [] mean left-normed commutators. The
  // presentation here was derived by first applying the NQA to find the
  // maximal nilpotent quotient, and then introducing new generators for
  // the PCP generators.
  // [932]: KnuthBendix: kbmag/heinnilp - RPOTrie
  // -- using a separate trie for new rules .....15.710s
  // -- NOT using a separate trie for new rules ......9.897s
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "932",
                                   "kbmag/heinnilp",
                                   "[extreme][knuth-bendix][kbmag][recursive]",
                                   RPOTrie) {
    auto rg = ReportGuard(true);

    Presentation<std::string> p;
    p.alphabet("fFyYdDcCbBaA");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "FfYyDdCcBbAa");
    presentation::add_rule(p, "BAba", "c");
    presentation::add_rule(p, "CAca", "d");
    presentation::add_rule(p, "CBcb", "y");
    presentation::add_rule(p, "DBdb", "f");
    presentation::add_rule(p, "cBCb", "bcBC");
    presentation::add_rule(p, "babABaBA", "abABaBAb");
    presentation::add_rule(p, "cBACab", "abcBAC");
    presentation::add_rule(p, "BabABBAbab", "aabABBAb");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.rewriting_system().confluent());
    kb.rewriting_system().settings().reduction_threshold = 1024;
    kb.rewriting_system().sort_pending_rules_by(detail::rev_rpo_cmp);
    SECTION("using a separate trie for new rules") {
      kb.rewriting_system().use_new_rule_trie([](auto const&) { return true; });
    }
    SECTION("NOT using a separate trie for new rules") {
      kb.rewriting_system().use_new_rule_trie(
          [](auto const&) { return false; });
    }

    kb.run();
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.rewriting_system().number_of_rules() == 72);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    auto rules1 = (kb.active_rules() | rx::to_vector());
    std::sort(rules1.begin(), rules1.end());
    REQUIRE(
        rules1
        == std::vector<std::pair<std::string, std::string>>(
            {{"Aa", ""},      {"BA", "ABcDYF"}, {"Ba", "aBCy"}, {"Bb", ""},
             {"CA", "ACdff"}, {"CB", "BCy"},    {"Ca", "aCD"},  {"Cb", "bCY"},
             {"Cc", ""},      {"DA", "ADFF"},   {"DB", "BDf"},  {"DC", "CD"},
             {"Da", "aDff"},  {"Db", "bDF"},    {"Dc", "cD"},   {"Dd", ""},
             {"FA", "AF"},    {"FB", "BF"},     {"FC", "CF"},   {"FD", "DF"},
             {"FY", "YF"},    {"Fa", "aF"},     {"Fb", "bF"},   {"Fc", "cF"},
             {"Fd", "dF"},    {"Ff", ""},       {"Fy", "yF"},   {"YA", "AYf"},
             {"YB", "BY"},    {"YC", "CY"},     {"YD", "DY"},   {"Ya", "aYF"},
             {"Yb", "bY"},    {"Yc", "cY"},     {"Yd", "dY"},   {"Yy", ""},
             {"aA", ""},      {"bA", "AbCdff"}, {"bB", ""},     {"ba", "abc"},
             {"cA", "AcDFF"}, {"cB", "BcY"},    {"cC", ""},     {"ca", "acd"},
             {"cb", "bcy"},   {"dA", "Adff"},   {"dB", "BdF"},  {"dC", "Cd"},
             {"dD", ""},      {"da", "adFF"},   {"db", "bdf"},  {"dc", "cd"},
             {"fA", "Af"},    {"fB", "Bf"},     {"fC", "Cf"},   {"fD", "Df"},
             {"fF", ""},      {"fY", "Yf"},     {"fa", "af"},   {"fb", "bf"},
             {"fc", "cf"},    {"fd", "df"},     {"fy", "yf"},   {"yA", "AyF"},
             {"yB", "By"},    {"yC", "Cy"},     {"yD", "Dy"},   {"yY", ""},
             {"ya", "ayf"},   {"yb", "by"},     {"yc", "cy"},   {"yd", "dy"}}));

    // NOTE: rev_rpo_cmp (and all the other orders) use the numerical
    // value of the letters in the alphabet as the order on the alphabet, in
    // this example, the order on the alphabet is "fFyYdDcCbBaA" which is
    // not numerical order, hence the contorsions below.
    // TODO: make it so that we don't have the contortions below, using the yet
    // to be implemented Alphabet objects

    v4::ToWord to_word(p.alphabet());
    auto       rules2
        = (rx::iterator_range(rules1.begin(), rules1.end())
           | rx::transform([&to_word](auto const& rule) {
               return std::pair(to_word(rule.first), to_word(rule.second));
             })
           | rx::to_vector());
    REQUIRE(std::all_of(rules2.begin(), rules2.end(), [](auto const& rule) {
      return rev_rpo_cmp(rule.second, rule.first);
    }));
  }

  // [144]: KnuthBendix: process pending rules x3 - LenLexTrie .....25.231s
  // [144]: KnuthBendix: process pending rules x3 - RPOTrie .....25.169s
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "144",
                                   "process pending rules x3",
                                   "[extreme][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                    rg = ReportGuard(true);
    Presentation<word_type> p;
    p.alphabet(2);
    p.contains_empty_word(true);

    WordRange wr;
    wr.alphabet_size(2).min(23).max(24);
    REQUIRE(wr.count() == 8'388'608);
    for (auto const& word : wr) {
      presentation::add_rule_no_checks(p, word, ""_w);
    }
    REQUIRE(presentation::length(p) == 192'937'984);

    KnuthBendix<word_type, TestType> kb(twosided, p);
    kb.rewriting_system().reduce();
    REQUIRE(kb.rewriting_system().number_of_rules() == wr.count());
  }

  // [118]: KnuthBendix: process pending rules x1 - LenLexTrie .....22.597s
  // [118]: KnuthBendix: process pending rules x1 - RPOTrie .....22.983s
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "118",
                                   "process pending rules x1",
                                   "[extreme][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                    rg = ReportGuard(true);
    Presentation<word_type> p;
    p.alphabet(2);
    p.contains_empty_word(true);

    WordRange wr;
    wr.alphabet_size(2).min(23).max(24);
    for (auto const& word : wr) {
      presentation::add_rule(p, word, ""_w);
    }

    KnuthBendix<word_type, TestType> kb(twosided, p);
    kb.rewriting_system().reduce();
    REQUIRE(kb.rewriting_system().number_of_rules() == wr.count());
  }

  // [139]: KnuthBendix: partial_transformation_monoid5 - LenLexTrie
  // ......6.295s
  // [139]: KnuthBendix: partial_transformation_monoid5 - RPOTrie
  // .......262ms
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "139",
                                   "partial_transformation_monoid5",
                                   "[extreme][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    auto rg = ReportGuard(true);

    size_t n = 5;
    auto   p = presentation::examples::partial_transformation_monoid_Shu60(n);

    KnuthBendix<word_type, TestType> kb(twosided, p);
    REQUIRE(!is_obviously_infinite(kb));
    REQUIRE(kb.number_of_classes() == 7'776);
  }

}  // namespace libsemigroups
