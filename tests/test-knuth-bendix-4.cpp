// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-2025 James D. Mitchell
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

// This file is the fourth of six that contains tests for the KnuthBendix
// classes. In a mostly vain attempt to speed up compilation the tests are
// split across 6 files as follows:
//
// 1: contains quick tests for KnuthBendix created from rules and all commented
//    out tests.
//
// 2: contains more quick tests for KnuthBendix created from rules
//
// 3: contains yet more quick tests for KnuthBendix created from rules
//
// 4: contains standard and extreme test for KnuthBendix created from rules
//
// 5: contains tests for KnuthBendix created from FroidurePin instances
//
// 6: contains tests for KnuthBendix.

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

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for AssertionHandler, oper...
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEMPLATE_TEST_CASE

#include "libsemigroups/constants.hpp"              // for operator==, operat...
#include "libsemigroups/exception.hpp"              // for LibsemigroupsExcep...
#include "libsemigroups/knuth-bendix.hpp"           // for KnuthBendix, norma...
#include "libsemigroups/order.hpp"                  // for shortlex_compare
#include "libsemigroups/paths.hpp"                  // for Paths
#include "libsemigroups/presentation-examples.hpp"  // for partition_mo
#include "libsemigroups/presentation.hpp"           // for add_rule, Presenta...
#include "libsemigroups/word-graph-helpers.hpp"     // for word_graph
#include "libsemigroups/word-graph.hpp"             // for WordGraph
#include "libsemigroups/word-range.hpp"             // for Inner, StringRange...

#include "libsemigroups/detail/report.hpp"  // for ReportGuard
#include "libsemigroups/detail/stl.hpp"     // for apply_permutation
#include "libsemigroups/detail/string.hpp"  // for random_string, operator<<

namespace libsemigroups {
  congruence_kind constexpr twosided = congruence_kind::twosided;

  using namespace rx;

  using RewriteTrie     = detail::RewriteTrie;
  using RewriteFromLeft = detail::RewriteFromLeft;

#define KNUTH_BENDIX_TYPES RewriteTrie, RewriteFromLeft

  ////////////////////////////////////////////////////////////////////////
  // Standard tests
  ////////////////////////////////////////////////////////////////////////

  // Takes approx. 2s
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "100",
                                   "Sims Ex. 6.6 (limited overlap lengths)",
                                   "[standard][knuth-bendix]",
                                   KNUTH_BENDIX_TYPES) {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("abc");

    presentation::add_rule(p, "aa", "");

    presentation::add_rule(p, "bc", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "ababababababab", "");
    presentation::add_rule(p, "abacabacabacabacabacabacabacabac", "");
    KnuthBendix<std::string, TestType> kb(twosided, p);

    REQUIRE(!kb.confluent());

    // In Sims it says to use 44 here, but that doesn't seem to work.
    kb.max_overlap(45);
    // Avoid checking confluence since this is very slow, essentially takes
    // the same amount of time as running Knuth-Bendix 13.
    kb.check_confluence_interval(LIMIT_MAX);

    kb.run();
    REQUIRE(kb.number_of_active_rules() == 1'026);
    // REQUIRE(kb.confluent());
    // REQUIRE(kb.number_of_classes() == 10752);
  }

  // Takes approx. 2s
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "101",
                                   "kbmag/standalone/kb_data/funny3",
                                   "[standard][knuth-bendix][kbmag][shortlex]",
                                   KNUTH_BENDIX_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("aAbBcC");

    presentation::add_inverse_rules(p, "AaBbCc");

    presentation::add_rule(p, "aaa", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "ccc", "");
    presentation::add_rule(p, "ABa", "BaB");
    presentation::add_rule(p, "bcB", "cBc");
    presentation::add_rule(p, "caC", "aCa");
    presentation::add_rule(p, "abcABCabcABCabcABC", "");
    presentation::add_rule(p, "BcabCABcabCABcabCA", "");
    presentation::add_rule(p, "cbACBacbACBacbACBa", "");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.confluent());

    knuth_bendix::by_overlap_length(kb);
    // kb.run() // also works, but is slower
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 8);
    REQUIRE(kb.number_of_classes() == 3);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf | to_vector()) == std::vector<std::string>({"", "a", "A"}));
  }

  ////////////////////////////////////////////////////////////////////////
  // Extreme tests
  ////////////////////////////////////////////////////////////////////////

  // Fibonacci group F(2,7) - order 29 - works better with largish tidyint
  // Takes approx. 10s
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE(
      "KnuthBendix",
      "102",
      "kbmag/standalone/kb_data/f27) (finite) (2 / 2",
      "[extreme][knuth-bendix][kbmag][shortlex]",
      KNUTH_BENDIX_TYPES) {
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
    REQUIRE(!kb.confluent());

    kb.run();
    // knuth_bendix::by_overlap_length(kb);
    REQUIRE(kb.finished());
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 194);
    REQUIRE(kb.number_of_classes() == 29);
  }

  // Mathieu group M_11
  // Takes approx. 58s (majority in checking confluence)
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "103",
                                   "kbmag/standalone/kb_data/m11",
                                   "[extreme][knuth-bendix][kbmag][shortlex]",
                                   KNUTH_BENDIX_TYPES) {
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
    REQUIRE(!kb.confluent());

    knuth_bendix::by_overlap_length(kb);
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 1'731);
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
    std::sort(ntc[0].begin(), ntc[0].end(), ShortLexCompare());

    REQUIRE(ntc[0]
            == (knuth_bendix::normal_forms(kb) | rx::sort(ShortLexCompare())
                | rx::to_vector()));
  }

  // Weyl group E8 (all gens involutory).
  // Takes approx. 5s for KnuthBendix
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "104",
                                   "kbmag/standalone/kb_data/e8",
                                   "[extreme][knuth-bendix][kbmag][shortlex]",
                                   KNUTH_BENDIX_TYPES) {
    auto rg = ReportGuard(true);

    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("abcdefgh");

    presentation::add_inverse_rules(p, "abcdefgh");
    presentation::add_rule(p, "bab", "aba");
    presentation::add_rule(p, "ca", "ac");
    presentation::add_rule(p, "da", "ad");
    presentation::add_rule(p, "ea", "ae");
    presentation::add_rule(p, "fa", "af");
    presentation::add_rule(p, "ga", "ag");
    presentation::add_rule(p, "ha", "ah");
    presentation::add_rule(p, "cbc", "bcb");
    presentation::add_rule(p, "db", "bd");
    presentation::add_rule(p, "eb", "be");
    presentation::add_rule(p, "fb", "bf");
    presentation::add_rule(p, "gb", "bg");
    presentation::add_rule(p, "hb", "bh");
    presentation::add_rule(p, "dcd", "cdc");
    presentation::add_rule(p, "ece", "cec");
    presentation::add_rule(p, "fc", "cf");
    presentation::add_rule(p, "gc", "cg");
    presentation::add_rule(p, "hc", "ch");
    presentation::add_rule(p, "ed", "de");
    presentation::add_rule(p, "fd", "df");
    presentation::add_rule(p, "gd", "dg");
    presentation::add_rule(p, "hd", "dh");
    presentation::add_rule(p, "fef", "efe");
    presentation::add_rule(p, "ge", "eg");
    presentation::add_rule(p, "he", "eh");
    presentation::add_rule(p, "gfg", "fgf");
    presentation::add_rule(p, "hf", "fh");
    presentation::add_rule(p, "hgh", "ghg");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.confluent());
    knuth_bendix::by_overlap_length(kb);
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 192);
    REQUIRE(kb.gilman_graph().number_of_nodes() == 332);
    REQUIRE(kb.gilman_graph().number_of_edges() == 533);
    REQUIRE(kb.number_of_classes() == 696'729'600);
  }

  // Second of BHN's series of increasingly complicated presentations of 1.
  // Works quickest with large value of tidyint
  // Takes > 1m (knuth_bendix), didn't run to the end
  // Takes approx. 6s (knuth_bendix_by_overlap_length)
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "105",
                                   "kbmag/standalone/kb_data/degen4b",
                                   "[extreme][knuth-bendix][kbmag][shortlex]",
                                   KNUTH_BENDIX_TYPES) {
    auto rg = ReportGuard(true);

    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("aAbBcC");
    presentation::add_inverse_rules(p, "AaBbCc");
    presentation::add_rule(p, "bbABaBcbCCAbaBBccBCbccBCb", "");
    presentation::add_rule(p, "ccBCbCacAABcbCCaaCAcaaCAc", "");
    presentation::add_rule(p, "aaCAcAbaBBCacAAbbABabbABa", "");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.confluent());
    // kb.run();
    knuth_bendix::by_overlap_length(kb);
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 6);
    REQUIRE(kb.number_of_classes() == 1);
  }

  // Two generator presentation of Fibonacci group F(2,7) - order 29. Large
  // value of tidyint works better.
  // Takes approx. 12s (knuth_bendix_by_overlap_length)
  // Takes > 19s (knuth_bendix), didn't run to the end
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "106",
                                   "kbmag/standalone/kb_data/f27_2gen",
                                   "[extreme][knuth-bendix][kbmag][shortlex]",
                                   KNUTH_BENDIX_TYPES) {
    auto rg = ReportGuard(true);

    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("aAbB");
    presentation::add_inverse_rules(p, "AaBb");
    presentation::add_rule(p, "bababbababbabbababbab", "a");
    presentation::add_rule(p, "abbabbababbaba", "b");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.confluent());

    knuth_bendix::by_overlap_length(kb);
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 19);
    REQUIRE(kb.number_of_classes() == 29);
  }

  // Takes approx. 1m8s
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "107",
                                   "Example 6.6 in Sims",
                                   "[extreme][knuth-bendix]",
                                   KNUTH_BENDIX_TYPES) {
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
    REQUIRE(!kb.confluent());
    kb.run();
    // knuth_bendix::by_overlap_length(kb);
    REQUIRE(kb.number_of_active_rules() == 1'026);
    // REQUIRE(kb.confluent());
    // REQUIRE(kb.number_of_classes() == 10'752);
  }

  // Fibonacci group F(2,7) - without inverses
  // Takes approx. 13s
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE(
      "KnuthBendix",
      "108",
      "kbmag/standalone/kb_data/f27) (infinite) (1 / 2",
      "[extreme][knuth-bendix][kbmag][shortlex]",
      KNUTH_BENDIX_TYPES) {
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    p.alphabet("aAbBcCdDyYfFgG");

    presentation::add_rule(p, "ab", "c");
    presentation::add_rule(p, "bc", "d");
    presentation::add_rule(p, "cd", "y");
    presentation::add_rule(p, "dy", "f");
    presentation::add_rule(p, "yf", "g");
    presentation::add_rule(p, "fg", "a");
    presentation::add_rule(p, "ga", "b");

    KnuthBendix<std::string, TestType> kb(twosided, p);

    REQUIRE(!kb.confluent());

    knuth_bendix::by_overlap_length(kb);
    // Fails to terminate, or is very slow, with knuth_bendix
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 47);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // An extension of 2^6 be L32
  // Takes approx. 1m7s
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "109",
                                   "kbmag/standalone/kb_data/l32ext",
                                   "[extreme][knuth-bendix][kbmag][shortlex]",
                                   KNUTH_BENDIX_TYPES) {
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
    REQUIRE(!kb.confluent());
    kb.run();

    // knuth_bendix::by_overlap_length(kb);
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 1'026);
    REQUIRE(kb.number_of_classes() == 10'752);

    auto& ad = kb.gilman_graph();
    REQUIRE(ad.number_of_nodes() == 6'021);
    REQUIRE(ad.number_of_edges() == 7'435);
    REQUIRE(v4::word_graph::is_acyclic(ad));
    REQUIRE(number_of_paths(ad, 0, 0, 100) == 10'752);
  }

  ////////////////////////////////////////////////////////////////////////
  // Tests that fail
  ////////////////////////////////////////////////////////////////////////

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "110",
                                   "Ceitin's undecidable word problem example",
                                   "[fail][knuth-bendix]",
                                   KNUTH_BENDIX_TYPES) {
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("abcde");
    presentation::add_rule(p, "ac", "ca");
    presentation::add_rule(p, "ad", "da");
    presentation::add_rule(p, "bc", "cb");
    presentation::add_rule(p, "bd", "db");
    presentation::add_rule(p, "eca", "ce");
    presentation::add_rule(p, "edb", "de");
    presentation::add_rule(p, "cca", "ccae");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    kb.run();  // I guess this shouldn't work, and indeed it doesn't!
  }

  // kbmag/standalone/kb_data/verifynilp
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "111",
                                   "kbmag/standalone/kb_data/verifynilp",
                                   "[fail][knuth-bendix][kbmag][shortlex]",
                                   KNUTH_BENDIX_TYPES) {
    auto                rg    = ReportGuard();
    std::string         lphbt = "hHgGfFyYdDcCbBaA";
    std::string         invrs = "HhGgFfYyDdCcBbAa";
    std::vector<size_t> perm(lphbt.size(), 0);
    std::iota(perm.begin(), perm.end(), 0);

    do {
      detail::apply_permutation(lphbt, invrs, perm);

      Presentation<std::string> p;
      p.contains_empty_word(true);
      p.alphabet(lphbt);

      presentation::add_inverse_rules(p, invrs);

      presentation::add_rule(p, "BAba", "c");
      presentation::add_rule(p, "CAca", "d");
      presentation::add_rule(p, "DAda", "y");
      presentation::add_rule(p, "YByb", "f");
      presentation::add_rule(p, "FAfa", "g");
      presentation::add_rule(p, "ga", "ag");
      presentation::add_rule(p, "GBgb", "h");
      presentation::add_rule(p, "cb", "bc");
      presentation::add_rule(p, "ya", "ay");

      KnuthBendix<std::string, TestType> kb(twosided, p);

      REQUIRE(!kb.confluent());

      knuth_bendix::by_overlap_length(kb);
      if (kb.confluent()) {
        REQUIRE(kb.number_of_active_rules() == 0);
        REQUIRE(kb.number_of_classes() == 0);
        break;
      }
    } while (std::next_permutation(perm.begin(), perm.end()));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "112",
                                   "Sorouhesh",
                                   "[quick][knuth-bendix][kbmag][shortlex]",
                                   KNUTH_BENDIX_TYPES) {
    using words::pow;
    auto         rg = ReportGuard(false);
    size_t const n  = 2;
    size_t const q  = 11;

    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, pow("a", std::pow(5, n)), "a");
    presentation::add_rule(p, "aba", "b");
    presentation::add_rule(p, "ab", pow("b", q) + "a");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.confluent());

    kb.run();
    REQUIRE(kb.number_of_active_rules() == 7);
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

  namespace {
    std::string swap_a_and_b(std::string const& w) {
      std::string result;
      for (auto l : w) {
        if (l == 'a') {
          result += "b";
        } else {
          result += "a";
        }
      }
      return result;
    }

    std::unordered_set<std::string>& get_set() {
      static std::unordered_set<std::string> set;
      return set;
    }

    template <typename T>
    void register_relation(T const& it1, T const& it2, size_t& nr) {
      auto tmp = it1 + "#" + it2;
      auto u   = swap_a_and_b(it1);
      auto v   = swap_a_and_b(it2);
      if (shortlex_compare(u, v)) {
        get_set().insert(u + "#" + v);
      } else {
        get_set().insert(v + "#" + u);
      }
      std::cout << it1 << " = " << it2 << std::endl;
      nr++;
    }
  }  // namespace

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE(
      "KnuthBendix",
      "113",
      "all 2-generated 1-relation semigroups 1 to 10",
      "[fail][knuth-bendix][xxx]",
      KNUTH_BENDIX_TYPES) {
    auto rg = ReportGuard(false);

    StringRange lhss;
    lhss.alphabet("ab").min(1).max(11);
    REQUIRE((lhss | count()) == 2'046);

    StringRange rhss;
    rhss.alphabet("ab").max(11);

    size_t total_c4 = 0;
    size_t total    = 0;

    for (auto const& lhs : lhss) {
      rhss.first(lhs);
      for (auto const& rhs : rhss | skip_n(1)) {
        auto tmp = lhs + "#" + rhs;
        if (get_set().insert(tmp).second) {
          bool try_again = false;
          {
            Presentation<std::string> p;
            p.contains_empty_word(true);
            p.alphabet("ab");
            presentation::add_rule(p, lhs, rhs);
            KnuthBendix<std::string, TestType> k(twosided, p);
            k.run_for(std::chrono::milliseconds(10));
            if (k.confluent()) {
              register_relation(lhs, rhs, total_c4);
            } else {
              try_again = true;
            }
          }
          if (try_again) {
            Presentation<std::string> p;
            p.contains_empty_word(true);
            p.alphabet("ba");
            presentation::add_rule(p, lhs, rhs);
            KnuthBendix<std::string, TestType> k(twosided, p);
            k.run_for(std::chrono::milliseconds(10));
            if (k.confluent()) {
              register_relation(lhs, rhs, total_c4);
            }
          }
        }
      }
    }
    REQUIRE(total_c4 == 471'479);
    REQUIRE(total == 2'092'035);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "114",
                                   "hard 2-generated 1-relation monoid",
                                   "[fail][knuth-bendix][xxx2]",
                                   KNUTH_BENDIX_TYPES) {
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("abc");
    presentation::add_rule(p, "a", "cc");
    presentation::add_rule(p, "c", "bab");

    KnuthBendix<std::string, TestType> k(twosided, p);
    k.run();
    // knuth_bendix::by_overlap_length(k);
    using rule_type = typename decltype(k)::rule_type;
    REQUIRE(k.active_rules().get() == rule_type({"", ""}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "115",
                                   "Konovalov",
                                   "[quick][knuth-bendix]",
                                   KNUTH_BENDIX_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("abAB");
    presentation::add_rule(p, "Abba", "BB");
    presentation::add_rule(p, "Baab", "AA");

    KnuthBendix<std::string, TestType> k(twosided, p);
    k.run();
    REQUIRE(k.number_of_classes() == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE(
      "KnuthBendix",
      "116",
      "https://math.stackexchange.com/questions/2649807",
      "[knuth-bendix][fail]",
      KNUTH_BENDIX_TYPES) {
    do {
      std::string lphbt = "abcABC";
      std::string invrs = "ABCabc";
      std::cout << std::string(72, '#') << std::endl;

      Presentation<std::string> p;
      p.contains_empty_word(true);
      p.alphabet(lphbt);

      presentation::add_inverse_rules(p, invrs);

      presentation::add_rule(p, "aa", "");
      presentation::add_rule(p, "bbbbbbbbbbb", "");
      presentation::add_rule(p, "cc", "");
      presentation::add_rule(p, "abababab", "");
      presentation::add_rule(p, "abbabbabbabbabbabb", "");
      presentation::add_rule(p, "abbabaBabaBBabbaB", "");
      presentation::add_rule(p, "acacac", "");
      presentation::add_rule(p, "bcbc", "");

      for (size_t i = 1; i < 3; ++i) {
        auto lhs = random_string(lphbt, 100);
        auto rhs = random_string(lphbt, 100);
        presentation::add_rule(p, lhs, rhs);
        std::cout << "trying rule " << lhs << " -> " << rhs << std::endl;
      }
      KnuthBendix<std::string, TestType> k(twosided, p);
      k.run_for(std::chrono::seconds(1));
      if (k.confluent()) {
        size_t const N = k.number_of_classes();
        std::cout << "k.number_of_classes() == " << N << std::endl;
        if (k.number_of_classes() == POSITIVE_INFINITY) {
          break;
        }
      }
    } while (true);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "117",
                                   "example with undecidable word problem",
                                   "[knuth-bendix][extreme]",
                                   KNUTH_BENDIX_TYPES) {
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("ab");
    presentation::add_rule(p, "abaabb", "bbaaba");
    presentation::add_rule(p, "aababba", "bbaaaba");
    presentation::add_rule(p, "abaaabb", "abbabaa");
    presentation::add_rule(p, "bbbaabbaaba", "bbbaabbaaaa");
    presentation::add_rule(p, "aaaabbaaba", "bbaaaa");
    KnuthBendix<std::string, TestType> k(twosided, p);
    k.run_for(std::chrono::seconds(1));
    REQUIRE(!k.finished());
  }

}  // namespace libsemigroups
