// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-2023 James D. Mitchell
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
// 6: contains tests for congruence::KnuthBendix.

#include <algorithm>                           // for next_permutation
#include <chrono>                              // for milliseconds, seconds
#include <cmath>                               // for pow
#include <cstddef>                             // for size_t
#include <iostream>                            // for string, operator<<, endl
#include <numeric>                             // for iota
#include <string>                              // for basic_string, char_traits
#include <unordered_map>                       // for operator==
#include <unordered_set>                       // for unordered_set
#include <utility>                             // for move, operator==, pair
#include <vector>                              // for vector, operator==
                                               //
#include "catch.hpp"                           // for AssertionHandler, oper...
#include "test-main.hpp"                       // for LIBSEMIGROUPS_TEST_CASE
                                               //
#include "libsemigroups/constants.hpp"         // for operator==, operator!=
#include "libsemigroups/digraph-helper.hpp"    // for is_acyclic
#include "libsemigroups/digraph.hpp"           // for ActionDigraph
#include "libsemigroups/exception.hpp"         // for LibsemigroupsException
#include "libsemigroups/iterator.hpp"          // for operator+
#include "libsemigroups/knuth-bendix-new.hpp"  // for KnuthBendix, normal_forms
#include "libsemigroups/order.hpp"             // for shortlex_compare
#include "libsemigroups/paths.hpp"             // for Paths
#include "libsemigroups/present.hpp"           // for add_rule, Presentation
#include "libsemigroups/report.hpp"            // for ReportGuard
#include "libsemigroups/stl.hpp"               // for apply_permutation
#include "libsemigroups/string.hpp"            // for random_string, operator<<
#include "libsemigroups/words.hpp"             // for Inner, Strings, to_str...

#include "rx/ranges.hpp"  // for operator|, to_vector

namespace libsemigroups {
  congruence_kind constexpr twosided = congruence_kind::twosided;

  using namespace rx;

  using rule_type = KnuthBendix::rule_type;

  ////////////////////////////////////////////////////////////////////////
  // Standard tests
  ////////////////////////////////////////////////////////////////////////

  // Takes approx. 2s
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "084",
                          "Example 6.6 in Sims (with limited overlap lengths)",
                          "[standard][knuth-bendix]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("abc");

    presentation::add_rule(p, "aa", "");

    presentation::add_rule(p, "bc", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "ababababababab", "");
    presentation::add_rule(p, "abacabacabacabacabacabacabacabac", "");
    KnuthBendix kb(twosided, p);

    REQUIRE(!kb.confluent());

    // In Sims it says to use 44 here, but that doesn't seem to work.
    kb.max_overlap(45);
    // Avoid checking confluence since this is very slow, essentially takes
    // the same amount of time as running Knuth-Bendix 13.
    kb.check_confluence_interval(LIMIT_MAX);

    kb.run();
    REQUIRE(kb.number_of_active_rules() == 1'026);
    // REQUIRE(kb.confluent());
    // REQUIRE(kb.size() == 10752);
  }

  // Takes approx. 2s
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "085",
                          "(from kbmag/standalone/kb_data/funny3)",
                          "[standard][knuth-bendix][kbmag][shortlex]") {
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

    KnuthBendix kb(twosided, p);
    REQUIRE(!kb.confluent());

    kb.knuth_bendix_by_overlap_length();
    // kb.run() // also works, but is slower
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 8);
    REQUIRE(kb.size() == 3);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf | to_strings(p.alphabet()) | to_vector())
            == std::vector<std::string>({"", "a", "A"}));
  }

  ////////////////////////////////////////////////////////////////////////
  // Extreme tests
  ////////////////////////////////////////////////////////////////////////

  // Fibonacci group F(2,7) - order 29 - works better with largish tidyint
  // Takes approx. 10s
  LIBSEMIGROUPS_TEST_CASE(
      "KnuthBendix",
      "086",
      "(from kbmag/standalone/kb_data/f27) (finite) (2 / 2)",
      "[extreme][knuth-bendix][kbmag][shortlex]") {
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

    KnuthBendix kb(twosided, p);
    REQUIRE(!kb.confluent());

    kb.knuth_bendix_by_overlap_length();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 194);
    REQUIRE(kb.size() == 29);
  }

  // Mathieu group M_11
  // Takes approx. 58s (majority in checking confluence)
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "087",
                          "(from kbmag/standalone/kb_data/m11)",
                          "[extreme][knuth-bendix][kbmag][shortlex]") {
    auto rg = ReportGuard(true);

    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("abB");

    presentation::add_inverse_rules(p, "aBb");
    presentation::add_rule(p, "BB", "bb");
    presentation::add_rule(p, "BaBaBaBaBaB", "abababababa");
    presentation::add_rule(p, "bbabbabba", "abbabbabb");
    presentation::add_rule(p, "aBaBababaBabaBBaBab", "");

    KnuthBendix kb(twosided, p);
    REQUIRE(!kb.confluent());

    kb.knuth_bendix_by_overlap_length();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 1'731);
    REQUIRE(kb.size() == 7'920);

    presentation::add_rule(p, "a", "");
    presentation::add_rule(p, "a", "b");
    presentation::add_rule(p, "B", "a");

    KnuthBendix kb2(twosided, p);
    REQUIRE(kb2.size() == 1);
    REQUIRE(knuth_bendix::non_trivial_classes(kb, kb2)
            == std::vector<std::vector<std::string>>(
                {knuth_bendix::normal_forms(kb) | to_strings(p.alphabet())
                 | rx::to_vector()}));
  }

  // Weyl group E8 (all gens involutory).
  // Takes approx. 5s for KnuthBendix
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "088",
                          "(from kbmag/standalone/kb_data/e8)",
                          "[extreme][knuth-bendix][kbmag][shortlex]") {
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

    KnuthBendix kb(twosided, p);
    REQUIRE(!kb.confluent());
    kb.knuth_bendix_by_overlap_length();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 192);
    REQUIRE(kb.gilman_digraph().number_of_nodes() == 332);
    REQUIRE(kb.gilman_digraph().number_of_edges() == 533);
    REQUIRE(kb.size() == 696'729'600);
  }

  // Second of BHN's series of increasingly complicated presentations of 1.
  // Works quickest with large value of tidyint
  // Takes > 1m (knuth_bendix), didn't run to the end
  // Takes approx. 6s (knuth_bendix_by_overlap_length)
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "089",
                          "(from kbmag/standalone/kb_data/degen4b)",
                          "[extreme][knuth-bendix][kbmag][shortlex]") {
    auto rg = ReportGuard(true);

    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("aAbBcC");
    presentation::add_inverse_rules(p, "AaBbCc");
    presentation::add_rule(p, "bbABaBcbCCAbaBBccBCbccBCb", "");
    presentation::add_rule(p, "ccBCbCacAABcbCCaaCAcaaCAc", "");
    presentation::add_rule(p, "aaCAcAbaBBCacAAbbABabbABa", "");

    KnuthBendix kb(twosided, p);
    REQUIRE(!kb.confluent());
    // kb.run();
    kb.knuth_bendix_by_overlap_length();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 6);
    REQUIRE(kb.size() == 1);
  }

  // Two generator presentation of Fibonacci group F(2,7) - order 29. Large
  // value of tidyint works better.
  // Takes approx. 12s (knuth_bendix_by_overlap_length)
  // Takes > 19s (knuth_bendix), didn't run to the end
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "090",
                          "(from kbmag/standalone/kb_data/f27_2gen)",
                          "[extreme][knuth-bendix][kbmag][shortlex]") {
    auto rg = ReportGuard(true);

    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("aAbB");
    presentation::add_inverse_rules(p, "AaBb");
    presentation::add_rule(p, "bababbababbabbababbab", "a");
    presentation::add_rule(p, "abbabbababbaba", "b");

    KnuthBendix kb(twosided, p);
    REQUIRE(!kb.confluent());

    kb.knuth_bendix_by_overlap_length();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 19);
    REQUIRE(kb.size() == 29);
  }

  // Takes approx. 1m8s
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "091",
                          "Example 6.6 in Sims",
                          "[extreme][knuth-bendix]") {
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("abc");

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bc", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "ababababababab", "");
    presentation::add_rule(p, "abacabacabacabacabacabacabacabac", "");

    KnuthBendix kb(twosided, p);
    REQUIRE(!kb.confluent());
    kb.knuth_bendix_by_overlap_length();
    REQUIRE(kb.number_of_active_rules() == 1'026);
    REQUIRE(kb.confluent());
    REQUIRE(kb.size() == 10'752);
  }

  // Fibonacci group F(2,7) - without inverses
  // Takes approx. 13s
  LIBSEMIGROUPS_TEST_CASE(
      "KnuthBendix",
      "092",
      "(from kbmag/standalone/kb_data/f27) (infinite) (1 / 2)",
      "[extreme][knuth-bendix][kbmag][shortlex]") {
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

    KnuthBendix kb(twosided, p);

    REQUIRE(!kb.confluent());

    kb.knuth_bendix_by_overlap_length();
    // Fails to terminate, or is very slow, with knuth_bendix
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 47);
    REQUIRE(kb.size() == POSITIVE_INFINITY);
  }

  // An extension of 2^6 be L32
  // Takes approx. 1m7s
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "093",
                          "(from kbmag/standalone/kb_data/l32ext)",
                          "[extreme][knuth-bendix][kbmag][shortlex]") {
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("abB");

    presentation::add_inverse_rules(p, "aBb");

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "BB", "b");
    presentation::add_rule(p, "BaBaBaB", "abababa");
    presentation::add_rule(p, "aBabaBabaBabaBab", "BabaBabaBabaBaba");

    KnuthBendix kb(twosided, p);
    REQUIRE(!kb.confluent());

    kb.knuth_bendix_by_overlap_length();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 1'026);
    REQUIRE(kb.size() == 10'752);

    auto& ad = kb.gilman_digraph();
    REQUIRE(ad.number_of_nodes() == 6'021);
    REQUIRE(ad.number_of_edges() == 7'435);
    REQUIRE(action_digraph_helper::is_acyclic(ad));
    REQUIRE(number_of_paths(ad, 0, 0, 100) == 10'752);
  }

  ////////////////////////////////////////////////////////////////////////
  // Tests that fail
  ////////////////////////////////////////////////////////////////////////

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "094",
                          "Ceitin's undecidable word problem example",
                          "[fail][knuth-bendix]") {
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

    KnuthBendix kb(twosided, p);
    kb.run();  // I guess this shouldn't work, and indeed it doesn't!
  }

  // kbmag/standalone/kb_data/verifynilp
  LIBSEMIGROUPS_TEST_CASE(
      "KnuthBendix",
      "095",
      "(KnuthBendix 050 again) (from kbmag/standalone/kb_data/verifynilp)",
      "[fail][knuth-bendix][kbmag][shortlex]") {
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

      KnuthBendix kb(twosided, p);

      REQUIRE(!kb.confluent());

      kb.knuth_bendix_by_overlap_length();
      if (kb.confluent()) {
        REQUIRE(kb.number_of_active_rules() == 0);
        REQUIRE(kb.size() == 0);
        break;
      }
    } while (std::next_permutation(perm.begin(), perm.end()));
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "096",
                          "Sorouhesh",
                          "[quick][knuth-bendix][kbmag][shortlex]") {
    using presentation::pow;
    auto         rg = ReportGuard(false);
    size_t const n  = 2;
    size_t const q  = 11;

    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, pow("a", std::pow(5, n)), "a");
    presentation::add_rule(p, "aba", "b");
    presentation::add_rule(p, "ab", pow("b", q) + "a");

    KnuthBendix kb(twosided, p);
    REQUIRE(!kb.confluent());

    kb.run();
    REQUIRE(kb.number_of_active_rules() == 7);
    REQUIRE(kb.size() == size_t(std::pow(5, n)) + 4 * q - 5);
    REQUIRE(kb.normal_form("aabb") == "aabb");
    REQUIRE(kb.normal_form("aabbaabb") == "bbbb");
    REQUIRE(kb.normal_form("aabbaabbaabb") == "aabbbbbb");
    REQUIRE(kb.normal_form("aabbaabbaabbaabb") == "bbbbbbbb");
    REQUIRE(kb.normal_form("aabbaabbaabbaabbaabb") == "aabbbbbbbbbb");
    auto nf = knuth_bendix::normal_forms(kb).min(1);
    REQUIRE((nf | to_strings(p.alphabet()) | to_vector())
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

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "015",
                          "all 2-generated 1-relation semigroups 1 to 10",
                          "[extreme][kambites][xxx]") {
    auto rg = ReportGuard(false);

    Strings lhss;
    lhss.letters("ab").min(1).max(11);
    REQUIRE((lhss | count()) == 2'046);

    Strings rhss;
    rhss.letters("ab").max(11);

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
            KnuthBendix k(twosided, p);
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
            KnuthBendix k(twosided, p);
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

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "016",
                          "hard 2-generated 1-relation semigroups 1 to 10",
                          "[extreme][kambites][xxx2]") {
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("bac");
    presentation::add_rule(p, "a", "cc");
    presentation::add_rule(p, "c", "bab");

    KnuthBendix k(twosided, p);
    k.knuth_bendix_by_overlap_length();
    std::cout << (k.active_rules() | to_vector()) << std::endl;
    REQUIRE(k.active_rules().get() == rule_type({"", ""}));
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "017",
                          "Konovalov",
                          "[extreme][knuthbendix][xxx3]") {
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("abAB");
    presentation::add_rule(p, "Abba", "BB");
    presentation::add_rule(p, "Baab", "AA");

    KnuthBendix k(twosided, p);
    k.run();
    REQUIRE(k.size() == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "018",
                          "https://math.stackexchange.com/questions/2649807",
                          "[knuthbendix][fail]") {
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
        auto lhs = detail::random_string(lphbt, 100);
        auto rhs = detail::random_string(lphbt, 100);
        presentation::add_rule(p, lhs, rhs);
        std::cout << "trying rule " << lhs << " -> " << rhs << std::endl;
      }
      KnuthBendix k(twosided, p);
      k.run_for(std::chrono::seconds(1));
      if (k.confluent()) {
        size_t const N = k.size();
        std::cout << "k.size() == " << N << std::endl;
        if (k.size() == POSITIVE_INFINITY) {
          break;
        }
      }
    } while (true);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "120",
                          "example with undecidable word problem",
                          "[knuthbendix][fail]") {
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("ab");
    presentation::add_rule(p, "abaabb", "bbaaba");
    presentation::add_rule(p, "aababba", "bbaaaba");
    presentation::add_rule(p, "abaaabb", "abbabaa");
    presentation::add_rule(p, "bbbaabbaaba", "bbbaabbaaaa");
    presentation::add_rule(p, "aaaabbaaba", "bbaaaa");
    KnuthBendix k(twosided, p);
    k.run_for(std::chrono::seconds(10));
    REQUIRE(k.finished());
  }

}  // namespace libsemigroups
