// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020 James D. Mitchell
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

// This file is the third of six that contains tests for the KnuthBendix
// classes. In a mostly vain attempt to speed up compilation the tests are
// split across 6 files as follows:
//
// 1: contains quick tests for fpsemigroup::KnuthBendix created from rules and
//    all commented out tests.
//
// 2: contains more quick tests for fpsemigroup::KnuthBendix created from rules
//
// 3: contains yet more quick tests for fpsemigroup::KnuthBendix created from
//    rules
//
// 4: contains standard and extreme test for fpsemigroup::KnuthBendix created
//    from rules
//
// 5: contains tests for fpsemigroup::KnuthBendix created from FroidurePin
//    instances
//
// 6: contains tests for congruence::KnuthBendix.

// #define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER
#include <iostream>  // for ostringstream

#include <string>  // for string
#include <vector>  // for vector

#include "catch.hpp"      // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/constants.hpp"     // for POSITIVE_INFINITY
#include "libsemigroups/knuth-bendix.hpp"  // for KnuthBendix, operator<<
#include "libsemigroups/report.hpp"        // for ReportGuard
#include "libsemigroups/siso.hpp"          // for cbegin_sislo
#include "libsemigroups/types.hpp"         // for word_type

namespace libsemigroups {
  struct LibsemigroupsException;
  constexpr bool REPORT = false;

  using rule_type = fpsemigroup::KnuthBendix::rule_type;

  namespace fpsemigroup {
    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "050",
        "(fpsemi) Chapter 11, Lemma 1.8 (q = 6, r = 5) in NR "
        "(infinite)",
        "[knuth-bendix][fpsemigroup][fpsemi][quick]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("ABCabc");

      kb.add_rule("aA", "");
      kb.add_rule("Aa", "");
      kb.add_rule("bB", "");
      kb.add_rule("Bb", "");
      kb.add_rule("cC", "");
      kb.add_rule("Cc", "");
      kb.add_rule("aa", "");
      kb.add_rule("bbb", "");
      kb.add_rule("abaBaBabaBab", "");

      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.number_of_active_rules() == 16);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == POSITIVE_INFINITY);
      REQUIRE(kb.number_of_normal_forms(0, 6) == 1206);
      REQUIRE(std::vector<std::string>(kb.cbegin_normal_forms(2, 3),
                                       kb.cend_normal_forms())
              == std::vector<std::string>({"AB",
                                           "AC",
                                           "Ab",
                                           "Ac",
                                           "BA",
                                           "BC",
                                           "Bc",
                                           "CA",
                                           "CB",
                                           "CC",
                                           "Cb",
                                           "bA",
                                           "bC",
                                           "bc",
                                           "cA",
                                           "cB",
                                           "cb",
                                           "cc"}));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "051",
                            "(fpsemi) Chapter 11, Section 2 (q = 6, r = 2, "
                            "alpha = "
                            "abaabba) in NR (size 4)",
                            "[knuth-bendix][fpsemigroup][fpsemi][quick]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("ab");

      kb.add_rule("aaa", "a");
      kb.add_rule("bbbbbbb", "b");
      kb.add_rule("abaabba", "bb");

      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.number_of_active_rules() == 4);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 4);
      REQUIRE(kb.number_of_normal_forms(0, POSITIVE_INFINITY) == 4);
      REQUIRE(std::vector<std::string>(kb.cbegin_normal_forms(0, 10),
                                       kb.cend_normal_forms())
              == std::vector<std::string>({"a", "b", "aa", "ab"}));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "052",
                            "(fpsemi) Chapter 8, Theorem 4.2 in NR (infinite)",
                            "[knuth-bendix][fpsemigroup][fpsemi][quick]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("ab");

      kb.add_rule("aaa", "a");
      kb.add_rule("bbbb", "b");
      kb.add_rule("bababababab", "b");
      kb.add_rule("baab", "babbbab");

      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.number_of_active_rules() == 8);
      REQUIRE(kb.confluent());

      REQUIRE(kb.size() == POSITIVE_INFINITY);
      REQUIRE(kb.number_of_normal_forms(0, POSITIVE_INFINITY)
              == POSITIVE_INFINITY);
      REQUIRE(std::vector<std::string>(kb.cbegin_normal_forms(0, 4),
                                       kb.cend_normal_forms())
              == std::vector<std::string>({"a",
                                           "b",
                                           "aa",
                                           "ab",
                                           "ba",
                                           "bb",
                                           "aab",
                                           "aba",
                                           "abb",
                                           "baa",
                                           "bab",
                                           "bba",
                                           "bbb"}));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "053",
                            "(fpsemi) equal_to fp semigroup",
                            "[quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("abc");

      kb.add_rule("ab", "ba");
      kb.add_rule("ac", "ca");
      kb.add_rule("aa", "a");
      kb.add_rule("ac", "a");
      kb.add_rule("ca", "a");
      kb.add_rule("bb", "bb");
      kb.add_rule("bc", "cb");
      kb.add_rule("bbb", "b");
      kb.add_rule("bc", "b");
      kb.add_rule("cb", "b");
      kb.add_rule("a", "b");

      REQUIRE(kb.equal_to("aa", "a"));
      REQUIRE(kb.equal_to("bb", "bb"));
      REQUIRE(kb.equal_to("bc", "cb"));
      REQUIRE(kb.equal_to("ba", "ccabc"));
      REQUIRE(kb.equal_to("cb", "bbbc"));
      REQUIRE(!kb.equal_to("ba", "c"));
      REQUIRE(kb.size() == POSITIVE_INFINITY);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "054",
                            "(fpsemi) equal_to free semigroup",
                            "[quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet(2);
      REQUIRE(!kb.equal_to(word_type({0}), word_type({1})));
      REQUIRE(kb.equal_to(word_type({0}), word_type({0})));
      REQUIRE(kb.equal_to(word_type({0, 0, 0, 0, 0, 0, 0}),
                          word_type({0, 0, 0, 0, 0, 0, 0})));
      REQUIRE(kb.size() == POSITIVE_INFINITY);
      REQUIRE(kb.number_of_normal_forms(0, 6) == 62);
      REQUIRE(
          std::distance(kb.cbegin_normal_forms(0, 6), kb.cend_normal_forms())
          == 62);
      REQUIRE(std::equal(kb.cbegin_normal_forms(0, 6),
                         kb.cend_normal_forms(),
                         cbegin_sislo(kb.alphabet(),
                                      kb.alphabet(0),
                                      std::string(6, kb.alphabet(1)[0]))));
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "055",
        "(fpsemi) from GAP smalloverlap gap/test.gi (infinite)",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][smalloverlap]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("abcdefg");

      kb.add_rule("abcd", "ce");
      kb.add_rule("df", "dg");

      REQUIRE(kb.is_obviously_infinite());
      REQUIRE(!kb.confluent());

      REQUIRE(kb.equal_to("dfabcdf", "dfabcdg"));
      REQUIRE(kb.equal_to("abcdf", "ceg"));
      REQUIRE(kb.equal_to("abcdf", "cef"));

      kb.run();
      REQUIRE(kb.number_of_active_rules() == 3);
      REQUIRE(kb.confluent());
      REQUIRE(kb.equal_to("dfabcdf", "dfabcdg"));
      REQUIRE(kb.equal_to("abcdf", "ceg"));
      REQUIRE(kb.equal_to("abcdf", "cef"));

      REQUIRE(kb.size() == POSITIVE_INFINITY);
      REQUIRE(kb.number_of_normal_forms(0, 6) == 17921);
      REQUIRE(
          std::distance(kb.cbegin_normal_forms(0, 6), kb.cend_normal_forms())
          == 17921);
      REQUIRE(std::vector<std::string>(kb.cbegin_normal_forms(0, 2),
                                       kb.cend_normal_forms())
              == std::vector<std::string>({"a", "b", "c", "d", "e", "f", "g"}));
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "056",
        "(fpsemi) from GAP smalloverlap gap/test.gi:49 (infinite)",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][smalloverlap]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("abcdefgh");

      kb.add_rule("abcd", "ce");
      kb.add_rule("df", "hd");

      REQUIRE(kb.is_obviously_infinite());
      REQUIRE(kb.confluent());

      REQUIRE(kb.equal_to("abchd", "abcdf"));
      REQUIRE(!kb.equal_to("abchf", "abcdf"));
      REQUIRE(kb.equal_to("abchd", "abchd"));
      REQUIRE(kb.equal_to("abchdf", "abchhd"));
      // Test cases (4) and (5)
      REQUIRE(kb.equal_to("abchd", "cef"));
      REQUIRE(kb.equal_to("cef", "abchd"));

      REQUIRE(kb.size() == POSITIVE_INFINITY);
      REQUIRE(kb.number_of_normal_forms(0, 6) == 35199);
      REQUIRE(
          std::distance(kb.cbegin_normal_forms(0, 6), kb.cend_normal_forms())
          == 35199);
      REQUIRE(std::vector<std::string>(kb.cbegin_normal_forms(0, 2),
                                       kb.cend_normal_forms())
              == std::vector<std::string>(
                  {"a", "b", "c", "d", "e", "f", "g", "h"}));
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "057",
        "(fpsemi) from GAP smalloverlap gap/test.gi:63 (infinite)",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][smalloverlap]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("abcdefgh");

      kb.add_rule("afh", "bgh");
      kb.add_rule("hc", "d");

      REQUIRE(kb.is_obviously_infinite());
      REQUIRE(!kb.confluent());

      // Test case (6)
      REQUIRE(kb.equal_to("afd", "bgd"));

      kb.run();
      REQUIRE(kb.number_of_active_rules() == 3);
      REQUIRE(kb.size() == POSITIVE_INFINITY);
      REQUIRE(kb.number_of_normal_forms(0, 6) == 34819);
      REQUIRE(
          std::distance(kb.cbegin_normal_forms(0, 6), kb.cend_normal_forms())
          == 34819);
      REQUIRE(std::vector<std::string>(kb.cbegin_normal_forms(0, 2),
                                       kb.cend_normal_forms())
              == std::vector<std::string>(
                  {"a", "b", "c", "d", "e", "f", "g", "h"}));
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "058",
        "(fpsemi) from GAP smalloverlap gap/test.gi:70 (infinite)",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][smalloverlap]") {
      auto rg = ReportGuard(REPORT);
      // The following permits a more complex test of case (6), which also
      // involves using the case (2) code to change the prefix being looked for:
      KnuthBendix kb;
      kb.set_alphabet("abcdefghij");

      kb.add_rule("afh", "bgh");
      kb.add_rule("hc", "de");
      kb.add_rule("ei", "j");

      REQUIRE(kb.is_obviously_infinite());
      REQUIRE(!kb.confluent());

      REQUIRE(kb.equal_to("afdj", "bgdj"));
      REQUIRE_THROWS_AS(!kb.equal_to("xxxxxxxxxxxxxxxxxxxxxxx", "b"),
                        LibsemigroupsException);

      kb.run();
      REQUIRE(kb.number_of_active_rules() == 5);
      REQUIRE(kb.size() == POSITIVE_INFINITY);
      REQUIRE(kb.number_of_normal_forms(0, 6) == 102255);
      REQUIRE(
          std::distance(kb.cbegin_normal_forms(0, 6), kb.cend_normal_forms())
          == 102255);
      REQUIRE(std::vector<std::string>(kb.cbegin_normal_forms(0, 2),
                                       kb.cend_normal_forms())
              == std::vector<std::string>(
                  {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j"}));
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "059",
        "(fpsemi) from GAP smalloverlap gap/test.gi:77 (infinite)",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][smalloverlap][no-"
        "valgrind]") {
      auto rg = ReportGuard(REPORT);
      // A slightly more complicated presentation for testing case (6), in which
      // the max piece suffixes of the first two relation words no longer agree
      // (since fh and gh are now pieces).
      KnuthBendix kb;
      kb.set_alphabet("abcdefghijkl");

      kb.add_rule("afh", "bgh");
      kb.add_rule("hc", "de");
      kb.add_rule("ei", "j");
      kb.add_rule("fhk", "ghl");

      REQUIRE(kb.is_obviously_infinite());
      REQUIRE(!kb.confluent());

      REQUIRE(kb.equal_to("afdj", "bgdj"));

      kb.run();
      REQUIRE(kb.number_of_active_rules() == 7);
      REQUIRE(kb.size() == POSITIVE_INFINITY);
      REQUIRE(kb.number_of_normal_forms(0, 6) == 255932);
      REQUIRE(kb.number_of_normal_forms(0, POSITIVE_INFINITY)
              == POSITIVE_INFINITY);
      REQUIRE(
          std::distance(kb.cbegin_normal_forms(0, 6), kb.cend_normal_forms())
          == 255932);
      REQUIRE(
          std::vector<std::string>(kb.cbegin_normal_forms(0, 2),
                                   kb.cend_normal_forms())
          == std::vector<std::string>(
              {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l"}));
      // The following is for comparison with the Kambites class.
      // size_t N = std::distance(cbegin_sislo("abcdefghijkl", "a", "bgdk"),
      //                          cend_sislo("abcdefghijkl", "a", "bgdk"));
      // REQUIRE(N == 4522);
      // for (auto it1 = cbegin_sislo("abcdefghijkl", "a", "bgdk");
      //      it1 != cend_sislo("abcdefghijkl", "a", "bgdk");
      //      ++it1) {
      //   for (auto it2 = cbegin_sislo("abcdefghijkl", "a", "bgdk"); it2 !=
      //   it1;
      //        ++it2) {
      //     if (kb.equal_to(*it1, *it2)) {
      //       N--;
      //       break;
      //     }
      //   }
      // }
      // REQUIRE(N == 4392);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "060",
        "(fpsemi) from GAP smalloverlap gap/test.gi:85 (infinite)",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][smalloverlap]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("cab");  // runs forever with a different order

      kb.add_rule("aabc", "acba");

      REQUIRE(kb.is_obviously_infinite());
      REQUIRE(kb.confluent());  // Confirmed with GAP

      REQUIRE(!kb.equal_to("a", "b"));
      REQUIRE(kb.equal_to("aabcabc", "aabccba"));

      kb.run();
      REQUIRE(kb.number_of_active_rules() == 1);
      REQUIRE(kb.size() == POSITIVE_INFINITY);
      REQUIRE(kb.active_rules() == std::vector<rule_type>({{"aabc", "acba"}}));
      REQUIRE(kb.number_of_normal_forms(0, 6) == 356);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "061",
        "(fpsemi) Von Dyck (2,3,7) group (infinite)",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][kbmag]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("ABabc");
      kb.set_identity("");
      kb.set_inverses("abABc");

      kb.add_rule("aaaa", "AAA");
      kb.add_rule("bb", "B");
      kb.add_rule("BA", "c");

      REQUIRE(!kb.confluent());
      kb.run();

      REQUIRE(kb.number_of_active_rules() == 30);
      REQUIRE(kb.confluent());
      REQUIRE(!kb.equal_to("a", "b"));
      REQUIRE(!kb.equal_to("aabcabc", "aabccba"));

      REQUIRE(kb.size() == POSITIVE_INFINITY);
      REQUIRE(kb.number_of_normal_forms(0, 6) == 88);
      REQUIRE(kb.number_of_normal_forms(0, POSITIVE_INFINITY)
              == POSITIVE_INFINITY);
      REQUIRE(
          std::distance(kb.cbegin_normal_forms(0, 6), kb.cend_normal_forms())
          == 88);
      REQUIRE(std::vector<std::string>(kb.cbegin_normal_forms(0, 2),
                                       kb.cend_normal_forms())
              == std::vector<std::string>({"", "A", "B", "a", "b", "c"}));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "062",
                            "(fpsemi) Von Dyck (2,3,7) group - different "
                            "presentation (infinite)",
                            "[no-valgrind][quick][knuth-bendix][fpsemigroup]["
                            "fpsemi][kbmag]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("abcAB");

      kb.add_rule("aaaa", "AAA");
      kb.add_rule("bb", "B");
      kb.add_rule("abababa", "BABABAB");
      kb.add_rule("BA", "c");

      REQUIRE(!kb.confluent());
      kb.overlap_policy(KnuthBendix::options::overlap::MAX_AB_BC);
      kb.max_rules(100);
      kb.run();
      REQUIRE(kb.number_of_active_rules() == 101);
      kb.run();
      REQUIRE(kb.number_of_active_rules() == 101);
      kb.max_rules(250);
      kb.run();
      REQUIRE(kb.number_of_active_rules() == 259);
      // kb.max_rules(POSITIVE_INFINITY);
      // kb.run();
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "063",
        "(fpsemi) rewriting system from KnuthBendixCongruenceByPairs 08",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][kbmag]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("abc");

      kb.add_rule("bbbbbbb", "b");
      kb.add_rule("ccccc", "c");
      kb.add_rule("bccba", "bccb");
      kb.add_rule("bccbc", "bccb");
      kb.add_rule("bbcbca", "bbcbc");
      kb.add_rule("bbcbcb", "bbcbc");

      REQUIRE(!kb.confluent());
      REQUIRE(kb.number_of_active_rules() == 6);
      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.number_of_active_rules() == 8);

      REQUIRE(kb.equal_to("bbbbbbb", "b"));
      REQUIRE(kb.equal_to("ccccc", "c"));
      REQUIRE(kb.equal_to("bccba", "bccb"));
      REQUIRE(kb.equal_to("bccbc", "bccb"));
      REQUIRE(kb.equal_to("bcbca", "bcbc"));
      REQUIRE(kb.equal_to("bcbcb", "bcbc"));
      REQUIRE(kb.equal_to("bcbcc", "bcbc"));
      REQUIRE(kb.equal_to("bccbb", "bccb"));
      REQUIRE(kb.equal_to("bccb", "bccbb"));
      REQUIRE(!kb.equal_to("aaaa", "bccbb"));

      std::vector<rule_type> rules = kb.active_rules();
      REQUIRE(rules[0] == rule_type("bcbca", "bcbc"));
      REQUIRE(rules[1] == rule_type("bcbcb", "bcbc"));
      REQUIRE(rules[2] == rule_type("bcbcc", "bcbc"));
      REQUIRE(rules[3] == rule_type("bccba", "bccb"));
      REQUIRE(rules[4] == rule_type("bccbb", "bccb"));
      REQUIRE(rules[5] == rule_type("bccbc", "bccb"));
      REQUIRE(rules[6] == rule_type("ccccc", "c"));
      REQUIRE(rules[7] == rule_type("bbbbbbb", "b"));

      REQUIRE(kb.size() == POSITIVE_INFINITY);
      REQUIRE(kb.number_of_normal_forms(0, 6) == 356);
      REQUIRE(kb.number_of_normal_forms(0, POSITIVE_INFINITY)
              == POSITIVE_INFINITY);
      REQUIRE(
          std::distance(kb.cbegin_normal_forms(0, 6), kb.cend_normal_forms())
          == 356);
      REQUIRE(std::vector<std::string>(kb.cbegin_normal_forms(0, 2),
                                       kb.cend_normal_forms())
              == std::vector<std::string>({"a", "b", "c"}));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "064",
                            "(fpsemi) rewriting system from Congruence 20",
                            "[quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("ab");

      kb.add_rule("aaa", "a");
      kb.add_rule("ab", "ba");
      kb.add_rule("aa", "a");
      kb.run();

      REQUIRE(kb.equal_to("abbbbbbbbbbbbbb", "aabbbbbbbbbbbbbb"));
      REQUIRE(kb.size() == POSITIVE_INFINITY);
    }

    // 2-generator free abelian group (with this ordering KB terminates - but
    // no all)
    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "065",
        "(fpsemi) (from kbmag/standalone/kb_data/ab2)",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][kbmag][shortlex]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("aAbB");
      kb.set_identity("");
      kb.set_inverses("AaBb");

      kb.add_rule("Bab", "a");

      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.number_of_active_rules() == 8);

      REQUIRE(kb.equal_to("Bab", "a"));
      REQUIRE(kb.size() == POSITIVE_INFINITY);
      REQUIRE(kb.number_of_normal_forms(0, 6) == 61);
      REQUIRE(kb.number_of_normal_forms(0, POSITIVE_INFINITY)
              == POSITIVE_INFINITY);
      REQUIRE(
          std::distance(kb.cbegin_normal_forms(0, 6), kb.cend_normal_forms())
          == 61);
      REQUIRE(std::vector<std::string>(kb.cbegin_normal_forms(0, 4),
                                       kb.cend_normal_forms())
              == std::vector<std::string>({"",    "a",   "A",   "b",   "B",
                                           "aa",  "ab",  "aB",  "AA",  "Ab",
                                           "AB",  "bb",  "BB",  "aaa", "aab",
                                           "aaB", "abb", "aBB", "AAA", "AAb",
                                           "AAB", "Abb", "ABB", "bbb", "BBB"}));
    }

    // This group is actually D_22 (although it wasn't meant to be). All
    // generators are unexpectedly involutory.
    // knuth_bendix does not terminate with the commented out ordering,
    // terminates almost immediately with the uncommented order.
    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "066",
        "(fpsemi) (from kbmag/standalone/kb_data/d22) (1 / 3)"
        "(infinite)",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][kbmag][shortlex]") {
      auto rg = ReportGuard(REPORT);

      // KnuthBendix kb;
      // kb.set_alphabet("aAbBcCdDyYfF");

      KnuthBendix kb;
      kb.set_alphabet("ABCDYFabcdyf");
      kb.set_identity("");
      kb.set_inverses("abcdyfABCDYF");

      kb.add_rule("aCAd", "");
      kb.add_rule("bfBY", "");
      kb.add_rule("cyCD", "");
      kb.add_rule("dFDa", "");
      kb.add_rule("ybYA", "");
      kb.add_rule("fCFB", "");
      REQUIRE(!kb.confluent());

      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.confluent());
      REQUIRE(kb.number_of_active_rules() == 41);

      REQUIRE(kb.equal_to("bfBY", ""));
      REQUIRE(kb.equal_to("cyCD", ""));
      REQUIRE(kb.equal_to("ybYA", ""));
      REQUIRE(kb.equal_to("fCFB", ""));
      REQUIRE(kb.equal_to("CAd", "dFD"));
      REQUIRE(kb.equal_to("FDa", "aCA"));
      REQUIRE(kb.equal_to("adFD", ""));
      REQUIRE(kb.equal_to("daCA", ""));

      REQUIRE(
          kb.active_rules()
          == std::vector<rule_type>(
              {{"a", "A"},    {"b", "B"},     {"c", "C"},     {"d", "D"},
               {"f", "F"},    {"y", "Y"},     {"AA", ""},     {"BB", ""},
               {"BC", "AB"},  {"BF", "Ay"},   {"CA", "AD"},   {"CB", "BA"},
               {"CC", ""},    {"CD", "AF"},   {"CF", "BY"},   {"DA", "AC"},
               {"DC", "CY"},  {"DD", ""},     {"DF", "AD"},   {"DY", "BD"},
               {"FA", "CY"},  {"FB", "BY"},   {"FC", "Ay"},   {"FD", "DA"},
               {"FF", "AA"},  {"FY", "BA"},   {"YA", "BY"},   {"YB", "BF"},
               {"YC", "CD"},  {"YD", "DB"},   {"YF", "AB"},   {"YY", ""},
               {"BAB", "C"},  {"BAC", "AYd"}, {"BAD", "ABA"}, {"BAF", "ADY"},
               {"BAY", "F"},  {"BDB", "ACY"}, {"DBA", "ADY"}, {"DBD", "Y"},
               {"DBY", "ADB"}}));

      REQUIRE(kb.size() == 22);
      REQUIRE(kb.number_of_normal_forms(0, 3) == 17);
      REQUIRE(kb.number_of_normal_forms(0, POSITIVE_INFINITY) == 22);
      REQUIRE(
          std::distance(kb.cbegin_normal_forms(0, 6), kb.cend_normal_forms())
          == 22);
      REQUIRE(std::vector<std::string>(kb.cbegin_normal_forms(0, 4),
                                       kb.cend_normal_forms())
              == std::vector<std::string>(
                  {"",   "A",   "B",   "C",   "D",   "Y",  "F",  "AB",
                   "AC", "AD",  "AY",  "AF",  "BA",  "BD", "BY", "CY",
                   "DB", "ABA", "ABD", "ABY", "ACY", "ADB"}));
    }

    // No generators - no anything!
    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "067",
        "(fpsemi) (from kbmag/standalone/kb_data/degen1)",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][kbmag][shortlex]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;

      REQUIRE(kb.confluent());
      REQUIRE_THROWS_AS(kb.run(), LibsemigroupsException);
      REQUIRE(kb.confluent());
      REQUIRE(kb.number_of_active_rules() == 0);
      REQUIRE(kb.size() == 0);
      REQUIRE_THROWS_AS(kb.cbegin_normal_forms(0, POSITIVE_INFINITY),
                        LibsemigroupsException);
    }

    // Symmetric group S_4
    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "068",
        "(fpsemi) (from kbmag/standalone/kb_data/s4)",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][kbmag][shortlex]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("abB");
      kb.set_identity("");
      kb.set_inverses("aBb");

      kb.add_rule("bb", "B");
      kb.add_rule("BaBa", "abab");

      REQUIRE(!kb.confluent());

      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.confluent());
      REQUIRE(kb.number_of_active_rules() == 11);
      REQUIRE(kb.size() == 24);
      REQUIRE(kb.number_of_normal_forms(0, 6) == 23);
      REQUIRE(kb.number_of_normal_forms(6, 7) == 1);
      REQUIRE(kb.number_of_normal_forms(0, POSITIVE_INFINITY) == 24);
      REQUIRE(
          std::distance(kb.cbegin_normal_forms(0, 6), kb.cend_normal_forms())
          == 23);
      REQUIRE(std::vector<std::string>(kb.cbegin_normal_forms(0, 7),
                                       kb.cend_normal_forms())
              == std::vector<std::string>(
                  {"",     "a",    "b",     "B",     "ab",    "aB",
                   "ba",   "Ba",   "aba",   "aBa",   "bab",   "baB",
                   "Bab",  "BaB",  "abab",  "abaB",  "aBab",  "aBaB",
                   "baBa", "Baba", "abaBa", "aBaba", "baBab", "abaBab"}));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "069",
                            "(fpsemi) fp semigroup (infinite)",
                            "[quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet(3);
      kb.add_rule({0, 1}, {1, 0});
      kb.add_rule({0, 2}, {2, 0});
      kb.add_rule({0, 0}, {0});
      kb.add_rule({0, 2}, {0});
      kb.add_rule({2, 0}, {0});
      kb.add_rule({1, 1}, {1, 1});
      kb.add_rule({1, 2}, {2, 1});
      kb.add_rule({1, 1, 1}, {1});
      kb.add_rule({1, 2}, {1});
      kb.add_rule({2, 1}, {1});
      kb.add_rule({0}, {1});

      REQUIRE(kb.confluent());

      REQUIRE(kb.equal_to(word_type({0, 0}), word_type({0})));
      REQUIRE(kb.equal_to(word_type({1, 1}), word_type({1, 1})));
      REQUIRE(kb.equal_to(word_type({1, 2}), word_type({2, 1})));
      REQUIRE(kb.equal_to(word_type({1, 0}), word_type({2, 2, 0, 1, 2})));
      REQUIRE(kb.equal_to(word_type({2, 1}), word_type({1, 1, 1, 2})));
      REQUIRE(!kb.equal_to(word_type({1, 0}), word_type({2})));
      REQUIRE(kb.size() == POSITIVE_INFINITY);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "070",
                            "(fpsemi) Chapter 11, Section 1 (q = 4, r = 3) "
                            "in NR (size 86)",
                            "[knuth-bendix][fpsemigroup][fpsemi][quick]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("ab");

      kb.add_rule("aaa", "a");
      kb.add_rule("bbbbb", "b");
      kb.add_rule("abbbabb", "bba");

      REQUIRE(!kb.confluent());
      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.number_of_active_rules() == 20);
      REQUIRE(kb.confluent());

      // Check that rewrite to a non-pointer argument does not rewrite its
      // argument
      std::string w = "aaa";
      REQUIRE(kb.rewrite(w) == "a");
      REQUIRE(w == "aaa");

      // defining relations
      REQUIRE(kb.rewrite("aaa") == kb.rewrite("a"));
      REQUIRE(kb.rewrite("bbbbb") == kb.rewrite("b"));
      REQUIRE(kb.rewrite("abbbabb") == kb.rewrite("bba"));

      // consequential relations (Chapter 11, Lemma 1.1 in NR)
      REQUIRE(kb.rewrite("babbbb") == kb.rewrite("ba"));
      REQUIRE(kb.rewrite("baabbbb") == kb.rewrite("baa"));
      REQUIRE(kb.rewrite("aabbbbbbbbbba") == kb.rewrite("bbbbbbbbbba"));
      REQUIRE(kb.rewrite("babbbbbbbbaa") == kb.rewrite("babbbbbbbb"));
      REQUIRE(kb.rewrite("baabbbbbbaa") == kb.rewrite("baabbbbbb"));
      REQUIRE(kb.rewrite("bbbbaabbbbaa") == kb.rewrite("bbbbaa"));
      REQUIRE(kb.rewrite("bbbaa") == kb.rewrite("baabb"));
      REQUIRE(kb.rewrite("abbbaabbba") == kb.rewrite("bbbbaa"));

      REQUIRE(kb.size() == 86);
      REQUIRE(kb.number_of_normal_forms(0, POSITIVE_INFINITY) == 86);
      REQUIRE(std::distance(kb.cbegin_normal_forms(0, POSITIVE_INFINITY),
                            kb.cend_normal_forms())
              == 86);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "071",
        "(fpsemi) Chapter 11, Section 1 (q = 8, r = 5) in NR "
        "(size 746)",
        "[no-valgrind][knuth-bendix][fpsemigroup][fpsemi][quick]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("ab");

      kb.add_rule("aaa", "a");
      kb.add_rule("bbbbbbbbb", "b");
      kb.add_rule("abbbbbabb", "bba");

      // kb.clear_stack_interval(0);

      REQUIRE(!kb.confluent());
      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.number_of_active_rules() == 105);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 746);

      // defining relations
      REQUIRE(kb.rewrite("aaa") == kb.rewrite("a"));
      REQUIRE(kb.rewrite("bbbbbbbbb") == kb.rewrite("b"));
      REQUIRE(kb.rewrite("abbbbbabb") == kb.rewrite("bba"));

      // consequential relations (Chapter 11, Lemma 1.1 in NR)
      REQUIRE(kb.rewrite("babbbbbbbb") == kb.rewrite("ba"));
      REQUIRE(kb.rewrite("baabbbbbbbb") == kb.rewrite("baa"));
      REQUIRE(kb.rewrite("aabbbbbbbbbbbba") == kb.rewrite("bbbbbbbbbbbba"));
      REQUIRE(kb.rewrite("babbbbbbbbbbaa") == kb.rewrite("babbbbbbbbbb"));
      REQUIRE(kb.rewrite("baabbbbbbbbaa") == kb.rewrite("baabbbbbbbb"));
      REQUIRE(kb.rewrite("bbbbbbbbaabbbbbbbbaa") == kb.rewrite("bbbbbbbbaa"));
      REQUIRE(kb.rewrite("bbbaa") == kb.rewrite("baabb"));
      REQUIRE(kb.rewrite("abbbbbaabbbbba") == kb.rewrite("bbbbbbbbaa"));

      REQUIRE(kb.number_of_normal_forms(0, POSITIVE_INFINITY) == 746);
      REQUIRE(std::distance(kb.cbegin_normal_forms(0, POSITIVE_INFINITY),
                            kb.cend_normal_forms())
              == 746);
    }

    // See KBFP 07 also.
    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "072",
        "(fpsemi) Chapter 7, Theorem 3.9 in NR (size 240)",
        "[no-valgrind][knuth-bendix][fpsemigroup][fpsemi][quick]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("ab");

      kb.add_rule("aaa", "a");
      kb.add_rule("bbbb", "b");
      kb.add_rule("abbba", "aa");
      kb.add_rule("baab", "bb");
      kb.add_rule("aabababababa", "aa");

      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.number_of_active_rules() == 24);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 240);
      REQUIRE(kb.number_of_normal_forms(0, POSITIVE_INFINITY) == 240);
      REQUIRE(std::distance(kb.cbegin_normal_forms(0, POSITIVE_INFINITY),
                            kb.cend_normal_forms())
              == 240);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "073",
                            "(fpsemi) F(2, 5) - Chapter 9, Section 1 in NR "
                            "(size 11)",
                            "[knuth-bendix][fpsemigroup][fpsemi][quick]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("abcde");

      kb.add_rule("ab", "c");
      kb.add_rule("bc", "d");
      kb.add_rule("cd", "e");
      kb.add_rule("de", "a");
      kb.add_rule("ea", "b");

      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.number_of_active_rules() == 24);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 11);
      REQUIRE(kb.number_of_normal_forms(0, POSITIVE_INFINITY) == 11);
      REQUIRE(std::distance(kb.cbegin_normal_forms(0, POSITIVE_INFINITY),
                            kb.cend_normal_forms())
              == 11);
      REQUIRE(
          std::vector<std::string>(kb.cbegin_normal_forms(0, POSITIVE_INFINITY),
                                   kb.cend_normal_forms())
          == std::vector<std::string>(
              {"a", "b", "c", "d", "e", "aa", "ac", "ad", "bb", "be", "aad"}));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "074",
                            "(fpsemi) F(2, 6) - Chapter 9, Section 1 in NR",
                            "[knuth-bendix][fpsemigroup][fpsemi][quick]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("abcdef");

      kb.add_rule("ab", "");
      kb.add_rule("bc", "d");
      kb.add_rule("cd", "e");
      kb.add_rule("de", "f");
      kb.add_rule("ef", "a");
      kb.add_rule("fa", "b");

      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.number_of_active_rules() == 35);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 12);
      REQUIRE(kb.number_of_normal_forms(0, POSITIVE_INFINITY) == 12);
      REQUIRE(std::distance(kb.cbegin_normal_forms(0, POSITIVE_INFINITY),
                            kb.cend_normal_forms())
              == 12);
      REQUIRE(
          std::vector<std::string>(kb.cbegin_normal_forms(0, POSITIVE_INFINITY),
                                   kb.cend_normal_forms())
          == std::vector<std::string>({"",
                                       "a",
                                       "b",
                                       "c",
                                       "d",
                                       "e",
                                       "f",
                                       "aa",
                                       "ac",
                                       "ae",
                                       "bd",
                                       "df"}));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "075",
                            "(fpsemi) Chapter 10, Section 4 in NR (infinite)",
                            "[knuth-bendix][fpsemigroup][fpsemi][quick]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("abc");

      kb.add_rule("aaaa", "a");
      kb.add_rule("bbbb", "b");
      kb.add_rule("cccc", "c");
      kb.add_rule("abab", "aaa");
      kb.add_rule("bcbc", "bbb");

      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.number_of_active_rules() == 31);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == POSITIVE_INFINITY);
      REQUIRE(kb.number_of_normal_forms(0, POSITIVE_INFINITY)
              == POSITIVE_INFINITY);
      REQUIRE(kb.number_of_normal_forms(0, 10) == 8823);
    }

    // Note: the fourth relator in NR's thesis incorrectly has exponent 3, it
    // should be 2. With exponent 3, the presentation defines the trivial
    // group, with exponent of 2, it defines the symmetric group as desired.
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "076",
                            "(fpsemi) Sym(5) from Chapter 3, Proposition "
                            "1.1 in NR "
                            "(size 120)",
                            "[no-valgrind][knuth-bendix][fpsemi][quick]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("ABab");

      kb.add_rule("aa", "");
      kb.add_rule("bbbbb", "");
      kb.add_rule("babababa", "");
      kb.add_rule("bB", "");
      kb.add_rule("Bb", "");
      kb.add_rule("BabBab", "");
      kb.add_rule("aBBabbaBBabb", "");
      kb.add_rule("aBBBabbbaBBBabbb", "");
      kb.add_rule("aA", "");
      kb.add_rule("Aa", "");

      REQUIRE(!kb.confluent());

      kb.run();
      REQUIRE(kb.number_of_active_rules() == 36);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 120);
      REQUIRE(kb.number_of_normal_forms(0, POSITIVE_INFINITY) == 120);
      REQUIRE(std::distance(kb.cbegin_normal_forms(0, POSITIVE_INFINITY),
                            kb.cend_normal_forms())
              == 120);
      REQUIRE(std::vector<std::string>(kb.cbegin_normal_forms(0, 4),
                                       kb.cend_normal_forms())
              == std::vector<std::string>({"",    "A",   "B",   "b",   "AB",
                                           "Ab",  "BA",  "BB",  "bA",  "bb",
                                           "ABA", "ABB", "AbA", "Abb", "BAB",
                                           "BAb", "BBA", "bAB", "bAb", "bbA"}));
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "077",
        "(fpsemi) SL(2, 7) from Chapter 3, Proposition "
        "1.5 in NR "
        "(size 336)",
        "[no-valgrind][quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("abAB");

      kb.add_rule("aaaaaaa", "");
      kb.add_rule("bb", "ababab");
      kb.add_rule("bb", "aaaabaaaabaaaabaaaab");
      kb.add_rule("aA", "");
      kb.add_rule("Aa", "");
      kb.add_rule("bB", "");
      kb.add_rule("Bb", "");

      REQUIRE(!kb.confluent());

      kb.run();
      REQUIRE(kb.number_of_active_rules() == 152);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 336);
      REQUIRE(kb.number_of_normal_forms(0, POSITIVE_INFINITY) == 336);
      REQUIRE(std::distance(kb.cbegin_normal_forms(0, POSITIVE_INFINITY),
                            kb.cend_normal_forms())
              == 336);
      REQUIRE(
          std::vector<std::string>(kb.cbegin_normal_forms(0, 4),
                                   kb.cend_normal_forms())
          == std::vector<std::string>(
              {"",    "a",   "b",   "A",   "B",   "aa",  "ab",  "aB",  "ba",
               "bb",  "bA",  "Ab",  "AA",  "AB",  "Ba",  "BA",  "aaa", "aab",
               "aaB", "aba", "abb", "abA", "aBa", "aBA", "baa", "bab", "baB",
               "bbA", "bAA", "Aba", "AAb", "AAA", "AAB", "ABa", "Baa", "BAA"}));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "078",
                            "(fpsemi) bicyclic monoid (infinite)",
                            "[knuth-bendix][fpsemigroup][fpsemi][quick]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("ab");

      kb.add_rule("ab", "");

      REQUIRE(kb.confluent());
      kb.run();
      REQUIRE(kb.number_of_active_rules() == 1);
      REQUIRE(kb.confluent());
      REQUIRE(kb.is_obviously_infinite());
      REQUIRE(kb.size() == POSITIVE_INFINITY);
      REQUIRE(kb.number_of_normal_forms(0, 10) == 55);
      REQUIRE(
          std::distance(kb.cbegin_normal_forms(0, 10), kb.cend_normal_forms())
          == 55);
      REQUIRE(
          std::vector<std::string>(kb.cbegin_normal_forms(0, 4),
                                   kb.cend_normal_forms())
          == std::vector<std::string>(
              {"", "a", "b", "aa", "ba", "bb", "aaa", "baa", "bba", "bbb"}));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "079",
                            "(fpsemi) plactic monoid of degree 2 (infinite)",
                            "[knuth-bendix][fpsemigroup][fpsemi][quick]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("abc");

      kb.add_rule("aba", "baa");
      kb.add_rule("bba", "bab");
      kb.add_rule("ac", "");
      kb.add_rule("ca", "");
      kb.add_rule("bc", "");
      kb.add_rule("cb", "");

      REQUIRE(!kb.confluent());

      kb.run();
      REQUIRE(kb.number_of_active_rules() == 3);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == POSITIVE_INFINITY);
      REQUIRE(kb.number_of_normal_forms(0, 10) == 19);
      REQUIRE(
          std::distance(kb.cbegin_normal_forms(0, 10), kb.cend_normal_forms())
          == 19);
      REQUIRE(std::vector<std::string>(kb.cbegin_normal_forms(0, 4),
                                       kb.cend_normal_forms())
              == std::vector<std::string>(
                  {"", "a", "c", "aa", "cc", "aaa", "ccc"}));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "080",
                            "(fpsemi) example before Chapter 7, Proposition "
                            "1.1 in NR (infinite)",
                            "[knuth-bendix][fpsemigroup][fpsemi][quick]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("ab");

      kb.add_rule("aa", "a");
      kb.add_rule("bb", "b");

      REQUIRE(kb.confluent());
      kb.run();
      REQUIRE(kb.number_of_active_rules() == 2);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == POSITIVE_INFINITY);
      REQUIRE(kb.number_of_normal_forms(0, 10) == 18);
      REQUIRE(
          std::distance(kb.cbegin_normal_forms(0, 10), kb.cend_normal_forms())
          == 18);
      REQUIRE(
          std::vector<std::string>(kb.cbegin_normal_forms(0, 4),
                                   kb.cend_normal_forms())
          == std::vector<std::string>({"a", "b", "ab", "ba", "aba", "bab"}));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "081",
                            "(fpsemi) Chapter 7, Theorem 3.6 in NR (size 243)",
                            "[knuth-bendix][fpsemigroup][fpsemi][quick]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("ab");

      kb.add_rule("aaa", "a");
      kb.add_rule("bbbb", "b");
      kb.add_rule("ababababab", "aa");

      REQUIRE(!kb.confluent());

      kb.run();
      REQUIRE(kb.number_of_active_rules() == 12);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 243);
      REQUIRE(kb.number_of_normal_forms(0, POSITIVE_INFINITY) == 243);
      REQUIRE(std::distance(kb.cbegin_normal_forms(0, POSITIVE_INFINITY),
                            kb.cend_normal_forms())
              == 243);
      REQUIRE(std::vector<std::string>(kb.cbegin_normal_forms(0, 4),
                                       kb.cend_normal_forms())
              == std::vector<std::string>({"a",
                                           "b",
                                           "aa",
                                           "ab",
                                           "ba",
                                           "bb",
                                           "aab",
                                           "aba",
                                           "abb",
                                           "baa",
                                           "bab",
                                           "bba",
                                           "bbb"}));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "082",
                            "(fpsemi) finite semigroup (size 99)",
                            "[knuth-bendix][fpsemigroup][fpsemi][quick]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("ab");

      kb.add_rule("aaa", "a");
      kb.add_rule("bbbb", "b");
      kb.add_rule("abababab", "aa");

      REQUIRE(!kb.confluent());

      kb.run();
      REQUIRE(kb.number_of_active_rules() == 9);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 99);
      REQUIRE(kb.number_of_normal_forms(0, POSITIVE_INFINITY) == 99);
      REQUIRE(std::distance(kb.cbegin_normal_forms(0, POSITIVE_INFINITY),
                            kb.cend_normal_forms())
              == 99);
      REQUIRE(std::vector<std::string>(kb.cbegin_normal_forms(0, 4),
                                       kb.cend_normal_forms())
              == std::vector<std::string>({"a",
                                           "b",
                                           "aa",
                                           "ab",
                                           "ba",
                                           "bb",
                                           "aab",
                                           "aba",
                                           "abb",
                                           "baa",
                                           "bab",
                                           "bba",
                                           "bbb"}));
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "998",
        "Giles Gardam in \"A counterexample to the unit conjecture for group "
        "rings\" (https://arxiv.org/abs/2102.11818)",
        "[fpsemigroup][fail]") {
      KnuthBendix k;
      k.set_alphabet("bABea");
      k.set_identity("e");
      k.set_inverses("BabeA");
      k.add_rule("Abba", "BB");
      k.add_rule("Baab", "AA");
      k.knuth_bendix_by_overlap_length();

      REQUIRE(k.size() == POSITIVE_INFINITY);
    }
  }  // namespace fpsemigroup

}  // namespace libsemigroups
