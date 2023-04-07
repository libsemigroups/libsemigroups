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
// 6: contains tests for KnuthBendix using word_type presentations

// #define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER

#include <string>  // for string
#include <vector>  // for vector

#include "catch.hpp"      // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/constants.hpp"         // for POSITIVE_INFINITY
#include "libsemigroups/knuth-bendix-new.hpp"  // for KnuthBendix, operator<<
#include "libsemigroups/obvinf.hpp"
#include "libsemigroups/ranges.hpp"  // for ???
#include "libsemigroups/report.hpp"  // for ReportGuard
#include "libsemigroups/types.hpp"   // for word_type

#include <rx/ranges.hpp>

namespace libsemigroups {

  using namespace rx;

  using literals::operator""_w;

  struct LibsemigroupsException;

  constexpr bool REPORT = false;

  using rule_type = KnuthBendix::rule_type;

  namespace {
    struct weird_cmp {
      constexpr bool operator()(rule_type const& x,
                                rule_type const& y) const noexcept {
        return shortlex_compare(x.first, y.first)
               || (x.first == y.first && shortlex_compare(x.second, y.second));
      }
    };
  }  // namespace

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "050",
                          "Chapter 11, Lemma 1.8 (q = 6, r = 5) in NR "
                          "(infinite)",
                          "[knuth-bendix][quick]") {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("ABCabc");
    p.contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "cC", "");
    presentation::add_rule(p, "Cc", "");
    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "abaBaBabaBab", "");

    KnuthBendix kb(p);
    REQUIRE(!kb.confluent());
    kb.run();
    REQUIRE(kb.number_of_active_rules() == 16);
    REQUIRE(kb.confluent());
    REQUIRE(kb.size() == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(6).count() == 1206);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(2).max(3) | to_strings(p.alphabet()) | to_vector())
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

  LIBSEMIGROUPS_TEST_CASE(
      "KnuthBendix",
      "051",
      "Chapter 11, Section 2 (q = 6, r = 2, alpha = abaabba) in NR (size 4)",
      "[knuth-bendix][quick]") {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("ab");

    presentation::add_rule(p, "aaa", "a");
    presentation::add_rule(p, "bbbbbbb", "b");
    presentation::add_rule(p, "abaabba", "bb");
    KnuthBendix kb(p);

    REQUIRE(!kb.confluent());
    kb.run();
    REQUIRE(kb.number_of_active_rules() == 4);
    REQUIRE(kb.confluent());
    REQUIRE(kb.size() == 4);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(POSITIVE_INFINITY).count()
            == 4);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(1).max(10) | to_strings(p.alphabet()) | to_vector())
            == std::vector<std::string>({"a", "b", "aa", "ab"}));
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "052",
                          "Chapter 8, Theorem 4.2 in NR (infinite) ",
                          "[knuth-bendix][quick]") {
    auto rg = ReportGuard(REPORT);

    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "aaa", "a");
    presentation::add_rule(p, "bbbb", "b");
    presentation::add_rule(p, "bababababab", "b");
    presentation::add_rule(p, "baab", "babbbab");

    KnuthBendix kb(p);
    REQUIRE(!kb.confluent());
    kb.run();
    REQUIRE(kb.number_of_active_rules() == 8);
    REQUIRE(kb.confluent());

    REQUIRE(kb.size() == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(POSITIVE_INFINITY).count()
            == POSITIVE_INFINITY);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(1).max(4) | to_strings(p.alphabet()) | to_vector())
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
                          "equal_to fp semigroup",
                          "[quick][knuth-bendix]") {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("abc");

    presentation::add_rule(p, "ab", "ba");
    presentation::add_rule(p, "ac", "ca");
    presentation::add_rule(p, "aa", "a");
    presentation::add_rule(p, "ac", "a");
    presentation::add_rule(p, "ca", "a");
    presentation::add_rule(p, "bb", "bb");
    presentation::add_rule(p, "bc", "cb");
    presentation::add_rule(p, "bbb", "b");
    presentation::add_rule(p, "bc", "b");
    presentation::add_rule(p, "cb", "b");
    presentation::add_rule(p, "a", "b");

    KnuthBendix kb(p);
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
                          "equal_to free semigroup",
                          "[quick][knuth-bendix]") {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet(2);

    REQUIRE(p.alphabet() == std::string({0, 1}));

    KnuthBendix kb(p);
    REQUIRE(!kb.equal_to({0}, {1}));
    REQUIRE(kb.equal_to({0}, {0}));
    REQUIRE(kb.equal_to({0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}));
    REQUIRE(kb.size() == POSITIVE_INFINITY);

    auto nf = knuth_bendix::normal_forms(kb).min(1).max(6);
    REQUIRE(nf.count() == 62);

    Strings s;
    s.letters(p.alphabet()).min(1).max(6);
    REQUIRE(equal(s, nf | to_strings(p.alphabet())));
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "055",
                          "from GAP smalloverlap gap/test.gi (infinite)",
                          "[quick][knuth-bendix][smalloverlap]") {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("abcdefg");

    presentation::add_rule(p, "abcd", "ce");
    presentation::add_rule(p, "df", "dg");

    KnuthBendix kb(p);
    REQUIRE(is_obviously_infinite(kb));
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
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(6).count() == 17'921);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(1).max(2) | to_strings(p.alphabet()) | to_vector())
            == std::vector<std::string>({"a", "b", "c", "d", "e", "f", "g"}));
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "056",
                          "from GAP smalloverlap gap/test.gi:49 (infinite)",
                          "[quick][knuth-bendix][smalloverlap]") {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("abcdefgh");

    presentation::add_rule(p, "abcd", "ce");
    presentation::add_rule(p, "df", "hd");

    KnuthBendix kb(p);
    REQUIRE(is_obviously_infinite(kb));
    REQUIRE(kb.confluent());

    REQUIRE(kb.equal_to("abchd", "abcdf"));
    REQUIRE(!kb.equal_to("abchf", "abcdf"));
    REQUIRE(kb.equal_to("abchd", "abchd"));
    REQUIRE(kb.equal_to("abchdf", "abchhd"));
    // Test cases (4) and (5)
    REQUIRE(kb.equal_to("abchd", "cef"));
    REQUIRE(kb.equal_to("cef", "abchd"));

    REQUIRE(kb.size() == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(6).count() == 35'199);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE(
        (nf.min(1).max(2) | to_strings(p.alphabet()) | to_vector())
        == std::vector<std::string>({"a", "b", "c", "d", "e", "f", "g", "h"}));
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "057",
                          "from GAP smalloverlap gap/test.gi:63 (infinite)",
                          "[quick][knuth-bendix][smalloverlap]") {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("abcdefgh");

    presentation::add_rule(p, "afh", "bgh");
    presentation::add_rule(p, "hc", "d");

    KnuthBendix kb(p);
    REQUIRE(is_obviously_infinite(kb));
    REQUIRE(!kb.confluent());

    // Test case (6)
    REQUIRE(kb.equal_to("afd", "bgd"));

    kb.run();
    REQUIRE(kb.number_of_active_rules() == 3);
    REQUIRE(kb.size() == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(6).count() == 34'819);

    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE(
        (nf.min(1).max(2) | to_strings(p.alphabet()) | to_vector())
        == std::vector<std::string>({"a", "b", "c", "d", "e", "f", "g", "h"}));
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "058",
                          "from GAP smalloverlap gap/test.gi:70 (infinite)",
                          "[quick][knuth-bendix][smalloverlap]") {
    auto rg = ReportGuard(REPORT);
    // The following permits a more complex test of case (6), which also
    // involves using the case (2) code to change the prefix being
    //  looked for:
    Presentation<std::string> p;
    p.alphabet("abcdefghij");

    presentation::add_rule(p, "afh", "bgh");
    presentation::add_rule(p, "hc", "de");
    presentation::add_rule(p, "ei", "j");

    KnuthBendix kb(p);
    REQUIRE(is_obviously_infinite(kb));
    REQUIRE(!kb.confluent());

    REQUIRE(kb.equal_to("afdj", "bgdj"));
    REQUIRE_THROWS_AS(kb.equal_to("xxxxxxxxxxxxxxxxxxxxxxx", "b"),
                      LibsemigroupsException);

    kb.run();
    REQUIRE(kb.number_of_active_rules() == 5);
    REQUIRE(kb.size() == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(6).count() == 102'255);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(1).max(2) | to_strings(p.alphabet()) | to_vector())
            == std::vector<std::string>(
                {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j"}));
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "059",
                          "from GAP smalloverlap gap/test.gi:77 (infinite)",
                          "[quick][knuth-bendix][smalloverlap][no-valgrind]") {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("abcdefghijkl");

    presentation::add_rule(p, "afh", "bgh");
    presentation::add_rule(p, "hc", "de");
    presentation::add_rule(p, "ei", "j");
    presentation::add_rule(p, "fhk", "ghl");

    KnuthBendix kb(p);

    REQUIRE(is_obviously_infinite(kb));
    REQUIRE(!kb.confluent());

    REQUIRE(kb.equal_to("afdj", "bgdj"));

    kb.run();
    REQUIRE(kb.number_of_active_rules() == 7);
    REQUIRE(kb.size() == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(6).count() == 255'932);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(POSITIVE_INFINITY).count()
            == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(6).count() == 255'932);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(1).max(2) | to_strings(p.alphabet()) | to_vector())
            == std::vector<std::string>(
                {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l"}));
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "060",
                          "from GAP smalloverlap gap/test.gi:85 (infinite)",
                          "[quick][knuth-bendix][smalloverlap]") {
    auto rg = ReportGuard(REPORT);

    Presentation<std::string> p;
    p.alphabet("cab");  // runs forever with a different order
    presentation::add_rule(p, "aabc", "acba");

    KnuthBendix kb(p);
    REQUIRE(is_obviously_infinite(kb));
    REQUIRE(kb.confluent());  // Confirmed with GAP

    REQUIRE(!kb.equal_to("a", "b"));
    REQUIRE(kb.equal_to("aabcabc", "aabccba"));

    kb.run();
    REQUIRE(kb.number_of_active_rules() == 1);
    REQUIRE(kb.size() == POSITIVE_INFINITY);
    REQUIRE((kb.active_rules() | to_vector())
            == std::vector<rule_type>({{"aabc", "acba"}}));
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(6).count() == 356);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "061",
                          "Von Dyck (2,3,7) group (infinite)",
                          "[quick][knuth-bendix][kbmag]") {
    auto rg = ReportGuard(REPORT);

    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("ABabc");
    presentation::add_inverse_rules(p, "abABc");
    presentation::add_rule(p, "aaaa", "AAA");
    presentation::add_rule(p, "bb", "B");
    presentation::add_rule(p, "BA", "c");

    KnuthBendix kb(p);

    REQUIRE(!kb.confluent());
    kb.run();

    REQUIRE(kb.number_of_active_rules() == 30);
    REQUIRE(kb.confluent());
    REQUIRE(!kb.equal_to("a", "b"));
    REQUIRE(!kb.equal_to("aabcabc", "aabccba"));

    REQUIRE(kb.size() == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(6).count() == 88);
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(POSITIVE_INFINITY).count()
            == POSITIVE_INFINITY);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(0).max(2) | to_strings(p.alphabet()) | to_vector())
            == std::vector<std::string>({"", "A", "B", "a", "b", "c"}));
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "062",
                          "Von Dyck (2,3,7) group - different "
                          "presentation (infinite)",
                          "[no-valgrind][quick][knuth-bendix][kbmag]") {
    auto rg = ReportGuard(REPORT);

    Presentation<std::string> p;
    p.alphabet("abcAB");

    presentation::add_rule(p, "aaaa", "AAA");
    presentation::add_rule(p, "bb", "B");
    presentation::add_rule(p, "abababa", "BABABAB");
    presentation::add_rule(p, "BA", "c");

    KnuthBendix kb(p);
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
      "rewriting system from KnuthBendixCongruenceByPairs 08",
      "[quick][knuth-bendix][kbmag]") {
    auto rg = ReportGuard(REPORT);

    Presentation<std::string> p;
    p.alphabet("abc");

    presentation::add_rule(p, "bbbbbbb", "b");
    presentation::add_rule(p, "ccccc", "c");
    presentation::add_rule(p, "bccba", "bccb");
    presentation::add_rule(p, "bccbc", "bccb");
    presentation::add_rule(p, "bbcbca", "bbcbc");
    presentation::add_rule(p, "bbcbcb", "bbcbc");

    KnuthBendix kb(p);
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

    REQUIRE((kb.active_rules() | sort(weird_cmp()) | to_vector())
            == std::vector<rule_type>({{"bcbca", "bcbc"},
                                       {"bcbcb", "bcbc"},
                                       {"bcbcc", "bcbc"},
                                       {"bccba", "bccb"},
                                       {"bccbb", "bccb"},
                                       {"bccbc", "bccb"},
                                       {"ccccc", "c"},
                                       {"bbbbbbb", "b"}}));

    REQUIRE(kb.size() == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(6).count() == 356);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(POSITIVE_INFINITY).count()
            == POSITIVE_INFINITY);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(1).max(2) | to_strings(p.alphabet()) | to_vector())
            == std::vector<std::string>({"a", "b", "c"}));
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "064",
                          "rewriting system from Congruence 20",
                          "[quick][knuth-bendix]") {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("ab");

    presentation::add_rule(p, "aaa", "a");
    presentation::add_rule(p, "ab", "ba");
    presentation::add_rule(p, "aa", "a");

    KnuthBendix kb(p);
    kb.run();

    REQUIRE(kb.equal_to("abbbbbbbbbbbbbb", "aabbbbbbbbbbbbbb"));
    REQUIRE(kb.size() == POSITIVE_INFINITY);
  }

  // 2-generator free abelian group (with this ordering KB terminates - but
  // no all)
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "065",
                          "(from kbmag/standalone/kb_data/ab2)",
                          "[quick][knuth-bendix][kbmag][shortlex]") {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("aAbB");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "AaBb");
    presentation::add_rule(p, "Bab", "a");

    KnuthBendix kb(p);

    REQUIRE(!kb.confluent());
    kb.run();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 8);

    REQUIRE(kb.equal_to("Bab", "a"));
    REQUIRE(kb.size() == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(6).count() == 61);
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(POSITIVE_INFINITY).count()
            == POSITIVE_INFINITY);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(0).max(4) | to_strings(p.alphabet()) | to_vector())
            == std::vector<std::string>(
                {"",    "a",   "A",   "b",   "B",   "aa",  "ab",  "aB",  "AA",
                 "Ab",  "AB",  "bb",  "BB",  "aaa", "aab", "aaB", "abb", "aBB",
                 "AAA", "AAb", "AAB", "Abb", "ABB", "bbb", "BBB"}));
  }

  // This group is actually D_22 (although it wasn't meant to be). All
  // generators are unexpectedly involutory. knuth_bendix does not terminate
  // with the commented out ordering, terminates almost immediately with the
  // uncommented order.
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "066",
                          "(from kbmag/standalone/kb_data/d22) (1 / 3)"
                          "(infinite)",
                          "[quick][knuth-bendix][kbmag][shortlex]") {
    auto rg = ReportGuard(REPORT);

    // Presentation<std::string> p;
    // p.alphabet("aAbBcCdDyYfF");

    Presentation<std::string> p;
    p.alphabet("ABCDYFabcdyf");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "abcdyfABCDYF");

    presentation::add_rule(p, "aCAd", "");
    presentation::add_rule(p, "bfBY", "");
    presentation::add_rule(p, "cyCD", "");
    presentation::add_rule(p, "dFDa", "");
    presentation::add_rule(p, "ybYA", "");
    presentation::add_rule(p, "fCFB", "");

    KnuthBendix kb(p);
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

    REQUIRE((kb.active_rules() | sort(weird_cmp()) | to_vector())
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
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(3).count() == 17);
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(POSITIVE_INFINITY).count()
            == 22);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(0).max(4) | to_strings(p.alphabet()) | to_vector())
            == std::vector<std::string>({"",    "A",   "B",   "C",  "D",  "Y",
                                         "F",   "AB",  "AC",  "AD", "AY", "AF",
                                         "BA",  "BD",  "BY",  "CY", "DB", "ABA",
                                         "ABD", "ABY", "ACY", "ADB"}));
  }

  // No generators - no anything!
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "067",
                          "(from kbmag/standalone/kb_data/degen1)",
                          "[quick][knuth-bendix][kbmag][shortlex]") {
    auto rg = ReportGuard(REPORT);

    KnuthBendix kb;

    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 0);
    REQUIRE(kb.size() == 0);
    REQUIRE(kb.gilman_digraph().number_of_nodes() == 0);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(0).max(4) | to_vector()).empty());
  }

  //
  //     // Symmetric group S_4
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "068",
                          "(from kbmag/standalone/kb_data/s4)",
                          "[quick][knuth-bendix][kbmag][shortlex]") {
    auto rg = ReportGuard(REPORT);

    Presentation<std::string> p;
    p.alphabet("abB");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "aBb");

    presentation::add_rule(p, "bb", "B");
    presentation::add_rule(p, "BaBa", "abab");

    KnuthBendix kb(p);
    REQUIRE(!kb.confluent());

    kb.knuth_bendix_by_overlap_length();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 11);
    REQUIRE(kb.size() == 24);
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(6).count() == 23);
    REQUIRE(knuth_bendix::normal_forms(kb).min(6).max(7).count() == 1);
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(POSITIVE_INFINITY).count()
            == 24);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(0).max(7) | to_strings(p.alphabet()) | to_vector())
            == std::vector<std::string>(
                {"",     "a",    "b",     "B",     "ab",    "aB",
                 "ba",   "Ba",   "aba",   "aBa",   "bab",   "baB",
                 "Bab",  "BaB",  "abab",  "abaB",  "aBab",  "aBaB",
                 "baBa", "Baba", "abaBa", "aBaba", "baBab", "abaBab"}));
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "069",
                          "fp semigroup (infinite)",
                          "[quick][knuth-bendix]") {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet(3);
    presentation::add_rule(p, {0, 1}, {1, 0});
    presentation::add_rule(p, {0, 2}, {2, 0});
    presentation::add_rule(p, {0, 0}, {0});
    presentation::add_rule(p, {0, 2}, {0});
    presentation::add_rule(p, {2, 0}, {0});
    presentation::add_rule(p, {1, 1}, {1, 1});
    presentation::add_rule(p, {1, 2}, {2, 1});
    presentation::add_rule(p, {1, 1, 1}, {1});
    presentation::add_rule(p, {1, 2}, {1});
    presentation::add_rule(p, {2, 1}, {1});
    presentation::add_rule(p, {0}, {1});

    KnuthBendix kb(p);
    REQUIRE(kb.confluent());

    REQUIRE(kb.equal_to({0, 0}, {0}));
    REQUIRE(kb.equal_to({1, 1}, {1, 1}));
    REQUIRE(kb.equal_to({1, 2}, {2, 1}));
    REQUIRE(kb.equal_to({1, 0}, {2, 2, 0, 1, 2}));
    REQUIRE(kb.equal_to({2, 1}, {1, 1, 1, 2}));
    REQUIRE(!kb.equal_to({1, 0}, {2}));
    REQUIRE(kb.size() == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "070",
                          "Chapter 11, Section 1 (q = 4, r = 3) in NR(size 86)",
                          "[knuth-bendix][quick]") {
    auto rg = ReportGuard(REPORT);

    Presentation<std::string> p;
    p.alphabet("ab");

    presentation::add_rule(p, "aaa", "a");
    presentation::add_rule(p, "bbbbb", "b");
    presentation::add_rule(p, "abbbabb", "bba");

    KnuthBendix kb(p);
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
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(POSITIVE_INFINITY).count()
            == 86);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "KnuthBendix",
      "071",
      "Chapter 11, Section 1 (q = 8, r = 5) in NR (size 746)",
      "[no-valgrind][knuth-bendix][quick]") {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("ab");

    presentation::add_rule(p, "aaa", "a");
    presentation::add_rule(p, "bbbbbbbbb", "b");
    presentation::add_rule(p, "abbbbbabb", "bba");

    KnuthBendix kb(p);
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

    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(POSITIVE_INFINITY).count()
            == 746);
  }

  // See KBFP 07 also.
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "072",
                          "Chapter 7, Theorem 3.9 in NR (size 240)",
                          "[no-valgrind][knuth-bendix][quick]") {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("ab");

    presentation::add_rule(p, "aaa", "a");
    presentation::add_rule(p, "bbbb", "b");
    presentation::add_rule(p, "abbba", "aa");
    presentation::add_rule(p, "baab", "bb");
    presentation::add_rule(p, "aabababababa", "aa");

    KnuthBendix kb(p);
    REQUIRE(!kb.confluent());
    kb.run();
    REQUIRE(kb.number_of_active_rules() == 24);
    REQUIRE(kb.confluent());
    REQUIRE(kb.size() == 240);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(POSITIVE_INFINITY).count()
            == 240);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "073",
                          "F(2, 5) - Chapter 9, Section 1 in NR (size 11)",
                          "[knuth-bendix][quick]") {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("abcde");

    presentation::add_rule(p, "ab", "c");
    presentation::add_rule(p, "bc", "d");
    presentation::add_rule(p, "cd", "e");
    presentation::add_rule(p, "de", "a");
    presentation::add_rule(p, "ea", "b");

    KnuthBendix kb(p);
    REQUIRE(!kb.confluent());
    kb.run();
    REQUIRE(kb.number_of_active_rules() == 24);
    REQUIRE(kb.confluent());
    REQUIRE(kb.size() == 11);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE(nf.min(1).max(POSITIVE_INFINITY).count() == 11);
    REQUIRE(
        (nf | to_strings(p.alphabet()) | to_vector())
        == std::vector<std::string>(
            {"a", "b", "c", "d", "e", "aa", "ac", "ad", "bb", "be", "aad"}));
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "074",
                          "F(2, 6) - Chapter 9, Section 1 in NR",
                          "[knuth-bendix][quick]") {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("abcdef");
    p.contains_empty_word(true);

    presentation::add_rule(p, "ab", "");
    presentation::add_rule(p, "bc", "d");
    presentation::add_rule(p, "cd", "e");
    presentation::add_rule(p, "de", "f");
    presentation::add_rule(p, "ef", "a");
    presentation::add_rule(p, "fa", "b");

    KnuthBendix kb(p);
    REQUIRE(!kb.confluent());
    kb.run();
    REQUIRE(kb.number_of_active_rules() == 35);
    REQUIRE(kb.confluent());
    REQUIRE(kb.size() == 12);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE(nf.min(0).max(POSITIVE_INFINITY).count() == 12);
    REQUIRE(
        (nf | to_strings(p.alphabet()) | to_vector())
        == std::vector<std::string>(
            {"", "a", "b", "c", "d", "e", "f", "aa", "ac", "ae", "bd", "df"}));
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "075",
                          "Chapter 10, Section 4 in NR (infinite)",
                          "[knuth-bendix][quick]") {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("abc");

    presentation::add_rule(p, "aaaa", "a");
    presentation::add_rule(p, "bbbb", "b");
    presentation::add_rule(p, "cccc", "c");
    presentation::add_rule(p, "abab", "aaa");
    presentation::add_rule(p, "bcbc", "bbb");

    KnuthBendix kb(p);
    REQUIRE(!kb.confluent());
    kb.run();
    REQUIRE(kb.number_of_active_rules() == 31);
    REQUIRE(kb.confluent());
    REQUIRE(kb.size() == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(POSITIVE_INFINITY).count()
            == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(10).count() == 8'823);
  }

  // Note: the fourth relator in NR's thesis incorrectly has exponent 3, it
  // should be 2. With exponent 3, the presentation defines the trivial group,
  // with exponent of 2, it defines the symmetric group as desired.
  LIBSEMIGROUPS_TEST_CASE(
      "KnuthBendix",
      "076",
      "Sym(5) from Chapter 3, Proposition 1.1 in NR (size 120)",
      "[no-valgrind][knuth-bendix][quick]") {
    auto rg = ReportGuard(REPORT);

    Presentation<std::string> p;
    p.alphabet("ABab");
    p.contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bbbbb", "");
    presentation::add_rule(p, "babababa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "BabBab", "");
    presentation::add_rule(p, "aBBabbaBBabb", "");
    presentation::add_rule(p, "aBBBabbbaBBBabbb", "");
    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");

    KnuthBendix kb(p);
    REQUIRE(!kb.confluent());

    kb.run();
    REQUIRE(kb.number_of_active_rules() == 36);
    REQUIRE(kb.confluent());
    REQUIRE(kb.size() == 120);
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(POSITIVE_INFINITY).count()
            == 120);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(0).max(4) | to_strings(p.alphabet()) | to_vector())
            == std::vector<std::string>({"",    "A",   "B",   "b",   "AB",
                                         "Ab",  "BA",  "BB",  "bA",  "bb",
                                         "ABA", "ABB", "AbA", "Abb", "BAB",
                                         "BAb", "BBA", "bAB", "bAb", "bbA"}));
  }

  LIBSEMIGROUPS_TEST_CASE(
      "KnuthBendix",
      "077",
      "SL(2, 7) from Chapter 3, Proposition 1.5 in NR (size 336)",
      "[no-valgrind][quick][knuth-bendix]") {
    auto rg = ReportGuard(REPORT);

    Presentation<std::string> p;
    p.alphabet("abAB");
    p.contains_empty_word(true);

    presentation::add_rule(p, "aaaaaaa", "");
    presentation::add_rule(p, "bb", "ababab");
    presentation::add_rule(p, "bb", "aaaabaaaabaaaabaaaab");
    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");

    KnuthBendix kb(p);
    REQUIRE(!kb.confluent());

    kb.run();
    REQUIRE(kb.number_of_active_rules() == 152);
    REQUIRE(kb.confluent());
    REQUIRE(kb.size() == 336);
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(POSITIVE_INFINITY).count()
            == 336);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE(
        (nf.min(0).max(4) | to_strings(p.alphabet()) | to_vector())
        == std::vector<std::string>(
            {"",    "a",   "b",   "A",   "B",   "aa",  "ab",  "aB",  "ba",
             "bb",  "bA",  "Ab",  "AA",  "AB",  "Ba",  "BA",  "aaa", "aab",
             "aaB", "aba", "abb", "abA", "aBa", "aBA", "baa", "bab", "baB",
             "bbA", "bAA", "Aba", "AAb", "AAA", "AAB", "ABa", "Baa", "BAA"}));
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "078",
                          "bicyclic monoid (infinite)",
                          "[knuth-bendix][quick]") {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("ab");
    p.contains_empty_word(true);

    presentation::add_rule(p, "ab", "");

    KnuthBendix kb(p);
    REQUIRE(kb.confluent());
    kb.run();
    REQUIRE(kb.number_of_active_rules() == 1);
    REQUIRE(kb.confluent());
    REQUIRE(is_obviously_infinite(kb));
    REQUIRE(kb.size() == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(10).count() == 55);

    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(0).max(4) | to_strings(p.alphabet()) | to_vector())
            == std::vector<std::string>(
                {"", "a", "b", "aa", "ba", "bb", "aaa", "baa", "bba", "bbb"}));
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "079",
                          "plactic monoid of degree 2 (infinite)",
                          "[knuth-bendix][quick]") {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("abc");
    p.contains_empty_word(true);

    presentation::add_rule(p, "aba", "baa");
    presentation::add_rule(p, "bba", "bab");
    presentation::add_rule(p, "ac", "");
    presentation::add_rule(p, "ca", "");
    presentation::add_rule(p, "bc", "");
    presentation::add_rule(p, "cb", "");

    KnuthBendix kb(p);
    REQUIRE(!kb.confluent());

    kb.run();
    REQUIRE(kb.number_of_active_rules() == 3);
    REQUIRE(kb.confluent());
    REQUIRE(kb.size() == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(10).count() == 19);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE(
        (nf.min(0).max(4) | to_strings(p.alphabet()) | to_vector())
        == std::vector<std::string>({"", "a", "c", "aa", "cc", "aaa", "ccc"}));
  }

  LIBSEMIGROUPS_TEST_CASE(
      "KnuthBendix",
      "080",
      "example before Chapter 7, Proposition 1.1 in NR (infinite)",
      "[knuth-bendix][quick]") {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("ab");

    presentation::add_rule(p, "aa", "a");
    presentation::add_rule(p, "bb", "b");

    KnuthBendix kb(p);
    REQUIRE(kb.confluent());
    kb.run();
    REQUIRE(kb.number_of_active_rules() == 2);
    REQUIRE(kb.confluent());
    REQUIRE(kb.size() == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(10).count() == 18);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(1).max(4) | to_strings(p.alphabet()) | to_vector())
            == std::vector<std::string>({"a", "b", "ab", "ba", "aba", "bab"}));
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "081",
                          "Chapter 7, Theorem 3.6 in NR (size 243)",
                          "[knuth-bendix][quick]") {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("ab");

    presentation::add_rule(p, "aaa", "a");
    presentation::add_rule(p, "bbbb", "b");
    presentation::add_rule(p, "ababababab", "aa");

    KnuthBendix kb(p);
    REQUIRE(!kb.confluent());

    kb.run();
    REQUIRE(kb.number_of_active_rules() == 12);
    REQUIRE(kb.confluent());
    REQUIRE(kb.size() == 243);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(POSITIVE_INFINITY).count()
            == 243);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(1).max(4) | to_strings(p.alphabet()) | to_vector())
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
                          "finite semigroup (size 99)",
                          "[knuth-bendix][quick]") {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("ab");

    presentation::add_rule(p, "aaa", "a");
    presentation::add_rule(p, "bbbb", "b");
    presentation::add_rule(p, "abababab", "aa");

    KnuthBendix kb(p);
    REQUIRE(!kb.confluent());

    kb.run();
    REQUIRE(kb.number_of_active_rules() == 9);
    REQUIRE(kb.confluent());
    REQUIRE(kb.size() == 99);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(POSITIVE_INFINITY).count()
            == 99);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(1).max(4) | to_strings(p.alphabet()) | to_vector())
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
      "[fail]") {
    Presentation<std::string> p;
    p.alphabet("bABa");
    presentation::add_inverse_rules(p, "BabeA");
    presentation::add_rule(p, "Abba", "BB");
    presentation::add_rule(p, "Baab", "AA");

    KnuthBendix kb(p);
    kb.knuth_bendix_by_overlap_length();

    REQUIRE(kb.size() == POSITIVE_INFINITY);
  }

}  // namespace libsemigroups
