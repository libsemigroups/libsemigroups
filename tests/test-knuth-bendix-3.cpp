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

#include <cstddef>  // for size_t
#include <iosfwd>   // for string
#include <string>   // for allocator, basic_string
#include <utility>  // for move
#include <vector>   // for vector, operator==

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for operator""_catch_sr
#include "test-main.hpp"  // for  LIBSEMIGROUPS_TEMPLATE_TEST_CASE

#include "libsemigroups/constants.hpp"     // for operator==, PositiveIn...
#include "libsemigroups/exception.hpp"     // for LibsemigroupsException
#include "libsemigroups/knuth-bendix.hpp"  // for KnuthBendix, normal_forms
#include "libsemigroups/obvinf.hpp"        // for is_obviously_infinite
#include "libsemigroups/order.hpp"         // for shortlex_compare
#include "libsemigroups/paths.hpp"         // for Paths
#include "libsemigroups/presentation-examples.hpp"  // for Inner, ToString, Str...
#include "libsemigroups/presentation.hpp"        // for add_rule, Presentation
#include "libsemigroups/ranges.hpp"              // for equal
#include "libsemigroups/word-graph-helpers.hpp"  // for WordGraph helpers
#include "libsemigroups/word-graph.hpp"          // for WordGraph
#include "libsemigroups/word-range.hpp"          // for Inner, ToString, Str...

#include "libsemigroups/detail/report.hpp"  // for ReportGuard

#include "libsemigroups/ranges.hpp"  // for operator|, to_vector

namespace libsemigroups {
  congruence_kind constexpr twosided = congruence_kind::twosided;

  using namespace rx;
  using literals::operator""_w;

  using rule_type = detail::KnuthBendixImpl<>::rule_type;

  struct LibsemigroupsException;

  using RewriteTrie     = detail::RewriteTrie;
  using RewriteFromLeft = detail::RewriteFromLeft;

#define KNUTH_BENDIX_TYPES RewriteTrie, RewriteFromLeft

  namespace {
    struct weird_cmp {
      bool operator()(rule_type const& x, rule_type const& y) const noexcept {
        return shortlex_compare(x.first, y.first)
               || (x.first == y.first && shortlex_compare(x.second, y.second));
      }
    };
  }  // namespace

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "066",
                                   "Chap. 11, Lem. 1.8 (q = 6, r = 5) in NR",
                                   "[knuth-bendix][quick]",
                                   KNUTH_BENDIX_TYPES) {
    auto                      rg = ReportGuard(false);
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

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.confluent());
    kb.run();
    REQUIRE(kb.number_of_active_rules() == 16);
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(6).count() == 1206);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(2).max(3) | to_vector())
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

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "067",
                                   "NR Chap. 11, §2 (q=6, r=2, \u03B1=abaabba)",
                                   "[knuth-bendix][quick]",
                                   KNUTH_BENDIX_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("ab");

    presentation::add_rule(p, "aaa", "a");
    presentation::add_rule(p, "bbbbbbb", "b");
    presentation::add_rule(p, "abaabba", "bb");
    KnuthBendix<std::string, TestType> kb(twosided, p);

    REQUIRE(!kb.confluent());
    kb.run();
    REQUIRE(kb.number_of_active_rules() == 4);
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_classes() == 4);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(POSITIVE_INFINITY).count()
            == 4);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(1).max(10) | to_vector())
            == std::vector<std::string>({"a", "b", "aa", "ab"}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "068",
                                   "Chap. 8, Thm. 4.2 in NR ",
                                   "[knuth-bendix][quick]",
                                   KNUTH_BENDIX_TYPES) {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "aaa", "a");
    presentation::add_rule(p, "bbbb", "b");
    presentation::add_rule(p, "bababababab", "b");
    presentation::add_rule(p, "baab", "babbbab");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.confluent());
    kb.run();
    REQUIRE(kb.number_of_active_rules() == 8);
    REQUIRE(kb.confluent());

    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(POSITIVE_INFINITY).count()
            == POSITIVE_INFINITY);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(1).max(4) | to_vector())
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

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "069",
                                   "equal_to fp semigroup",
                                   "[quick][knuth-bendix]",
                                   KNUTH_BENDIX_TYPES) {
    auto                      rg = ReportGuard(false);
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

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(knuth_bendix::contains(kb, "aa", "a"));
    REQUIRE(knuth_bendix::contains(kb, "bb", "bb"));
    REQUIRE(knuth_bendix::contains(kb, "bc", "cb"));
    REQUIRE(knuth_bendix::contains(kb, "ba", "ccabc"));
    REQUIRE(knuth_bendix::contains(kb, "cb", "bbbc"));
    REQUIRE(!knuth_bendix::contains(kb, "ba", "c"));
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "070",
                                   "equal_to free semigroup",
                                   "[quick][knuth-bendix]",
                                   KNUTH_BENDIX_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet(2);

    REQUIRE(p.alphabet() == "ab");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!knuth_bendix::contains(kb, "a", "b"));
    REQUIRE(knuth_bendix::contains(kb, "a", "a"));
    REQUIRE(knuth_bendix::contains(kb, "aaaaaaa", "aaaaaaa"));
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);

    auto nf = knuth_bendix::normal_forms(kb).min(1).max(6);
    REQUIRE(nf.count() == 62);

    StringRange s;
    s.alphabet(p.alphabet()).min(1).max(6);
    REQUIRE(equal(s, nf));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "071",
                                   "gap/smalloverlap/gap/test.gi",
                                   "[quick][knuth-bendix][smalloverlap]",
                                   KNUTH_BENDIX_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abcdefg");

    presentation::add_rule(p, "abcd", "ce");
    presentation::add_rule(p, "df", "dg");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(is_obviously_infinite(kb));
    REQUIRE(!kb.confluent());

    REQUIRE(knuth_bendix::contains(kb, "dfabcdf", "dfabcdg"));
    REQUIRE(knuth_bendix::contains(kb, "abcdf", "ceg"));
    REQUIRE(knuth_bendix::contains(kb, "abcdf", "cef"));

    kb.run();
    REQUIRE(kb.number_of_active_rules() == 3);
    REQUIRE(kb.confluent());
    REQUIRE(knuth_bendix::contains(kb, "dfabcdf", "dfabcdg"));
    REQUIRE(knuth_bendix::contains(kb, "abcdf", "ceg"));
    REQUIRE(knuth_bendix::contains(kb, "abcdf", "cef"));

    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(6).count() == 17'921);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(1).max(2) | to_vector())
            == std::vector<std::string>({"a", "b", "c", "d", "e", "f", "g"}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "072",
                                   "gap/smalloverlap/gap/test.gi:49",
                                   "[quick][knuth-bendix][smalloverlap]",
                                   KNUTH_BENDIX_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abcdefgh");

    presentation::add_rule(p, "abcd", "ce");
    presentation::add_rule(p, "df", "hd");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    // kb.process_pending_rules();
    REQUIRE(is_obviously_infinite(kb));
    REQUIRE(kb.confluent());

    REQUIRE(knuth_bendix::contains(kb, "abchd", "abcdf"));
    REQUIRE(!knuth_bendix::contains(kb, "abchf", "abcdf"));
    REQUIRE(knuth_bendix::contains(kb, "abchd", "abchd"));
    REQUIRE(knuth_bendix::contains(kb, "abchdf", "abchhd"));
    // Test cases (4) and (5)
    REQUIRE(knuth_bendix::contains(kb, "abchd", "cef"));
    REQUIRE(knuth_bendix::contains(kb, "cef", "abchd"));

    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(6).count() == 35'199);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE(
        (nf.min(1).max(2) | to_vector())
        == std::vector<std::string>({"a", "b", "c", "d", "e", "f", "g", "h"}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "073",
                                   "gap/smalloverlap/gap/test.gi:63",
                                   "[quick][knuth-bendix][smalloverlap]",
                                   KNUTH_BENDIX_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abcdefgh");

    presentation::add_rule(p, "afh", "bgh");
    presentation::add_rule(p, "hc", "d");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(is_obviously_infinite(kb));
    REQUIRE(!kb.confluent());

    // Test case (6)
    REQUIRE(knuth_bendix::contains(kb, "afd", "bgd"));

    kb.run();
    REQUIRE(kb.number_of_active_rules() == 3);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(6).count() == 34'819);

    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE(
        (nf.min(1).max(2) | to_vector())
        == std::vector<std::string>({"a", "b", "c", "d", "e", "f", "g", "h"}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "074",
                                   "gap/smalloverlap/gap/test.gi:70",
                                   "[quick][knuth-bendix][smalloverlap]",
                                   KNUTH_BENDIX_TYPES) {
    auto rg = ReportGuard(false);
    // The following permits a more complex test of case (6), which also
    // involves using the case (2) code to change the prefix being
    //  looked for:
    Presentation<std::string> p;
    p.alphabet("abcdefghij");

    presentation::add_rule(p, "afh", "bgh");
    presentation::add_rule(p, "hc", "de");
    presentation::add_rule(p, "ei", "j");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(is_obviously_infinite(kb));
    REQUIRE(!kb.confluent());

    REQUIRE(knuth_bendix::contains(kb, "afdj", "bgdj"));
    REQUIRE_THROWS_AS(
        knuth_bendix::contains(kb, "xxxxxxxxxxxxxxxxxxxxxxx", "b"),
        LibsemigroupsException);

    kb.run();
    REQUIRE(kb.number_of_active_rules() == 5);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(6).count() == 102'255);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(1).max(2) | to_vector())
            == std::vector<std::string>(
                {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j"}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE(
      "KnuthBendix",
      "075",
      "gap/smalloverlap/gap/test.gi:77",
      "[quick][knuth-bendix][smalloverlap][no-valgrind]",
      KNUTH_BENDIX_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abcdefghijkl");

    presentation::add_rule(p, "afh", "bgh");
    presentation::add_rule(p, "hc", "de");
    presentation::add_rule(p, "ei", "j");
    presentation::add_rule(p, "fhk", "ghl");

    KnuthBendix<std::string, TestType> kb(twosided, p);

    REQUIRE(is_obviously_infinite(kb));
    REQUIRE(!kb.confluent());

    REQUIRE(knuth_bendix::contains(kb, "afdj", "bgdj"));

    kb.run();
    REQUIRE(kb.number_of_active_rules() == 7);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(6).count() == 255'932);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(POSITIVE_INFINITY).count()
            == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(6).count() == 255'932);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(1).max(2) | to_vector())
            == std::vector<std::string>(
                {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l"}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "076",
                                   "gap/pkg/smalloverlap/gap/test.gi:85",
                                   "[quick][knuth-bendix][smalloverlap]",
                                   KNUTH_BENDIX_TYPES) {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("cab");  // runs forever with a different order
    presentation::add_rule(p, "aabc", "acba");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    // kb.process_pending_rules();
    REQUIRE(is_obviously_infinite(kb));
    REQUIRE(kb.confluent());  // Confirmed with GAP

    REQUIRE(!knuth_bendix::contains(kb, "a", "b"));
    REQUIRE(knuth_bendix::contains(kb, "aabcabc", "aabccba"));

    kb.run();
    REQUIRE(kb.number_of_active_rules() == 1);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    REQUIRE((kb.active_rules() | to_vector())
            == std::vector<rule_type>({{"aabc", "acba"}}));
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(6).count() == 356);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "077",
                                   "Von Dyck (2,3,7) group",
                                   "[quick][knuth-bendix][kbmag]",
                                   KNUTH_BENDIX_TYPES) {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("ABabc");
    presentation::add_inverse_rules(p, "abABc");
    presentation::add_rule(p, "aaaa", "AAA");
    presentation::add_rule(p, "bb", "B");
    presentation::add_rule(p, "BA", "c");

    KnuthBendix<std::string, TestType> kb(twosided, p);

    REQUIRE(!kb.confluent());
    kb.run();

    REQUIRE(kb.number_of_active_rules() == 30);
    REQUIRE(kb.confluent());
    REQUIRE(!knuth_bendix::contains(kb, "a", "b"));
    REQUIRE(!knuth_bendix::contains(kb, "aabcabc", "aabccba"));

    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(6).count() == 88);
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(POSITIVE_INFINITY).count()
            == POSITIVE_INFINITY);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(0).max(2) | to_vector())
            == std::vector<std::string>({"", "A", "B", "a", "b", "c"}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "078",
                                   "Von Dyck (2,3,7) group - alternate",
                                   "[no-valgrind][quick][knuth-bendix][kbmag]",
                                   KNUTH_BENDIX_TYPES) {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abcAB");

    presentation::add_rule(p, "aaaa", "AAA");
    presentation::add_rule(p, "bb", "B");
    presentation::add_rule(p, "abababa", "BABABAB");
    presentation::add_rule(p, "BA", "c");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.confluent());
    kb.overlap_policy(
        KnuthBendix<std::string, TestType>::options::overlap::MAX_AB_BC);
    kb.max_rules(100);
    kb.run();
    REQUIRE(kb.number_of_active_rules() > 100);
    auto old = kb.number_of_active_rules();
    kb.run();
    REQUIRE(kb.number_of_active_rules() == old);
    kb.max_rules(250);
    kb.run();
    REQUIRE(kb.number_of_active_rules() > 250);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "079",
                                   "rewriting system from another test",
                                   "[quick][knuth-bendix][kbmag]",
                                   KNUTH_BENDIX_TYPES) {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abc");

    presentation::add_rule(p, "bbbbbbb", "b");
    presentation::add_rule(p, "ccccc", "c");
    presentation::add_rule(p, "bccba", "bccb");
    presentation::add_rule(p, "bccbc", "bccb");
    presentation::add_rule(p, "bbcbca", "bbcbc");
    presentation::add_rule(p, "bbcbcb", "bbcbc");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    // kb.process_pending_rules();
    REQUIRE(!kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 6);
    kb.run();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 8);

    REQUIRE(knuth_bendix::contains(kb, "bbbbbbb", "b"));
    REQUIRE(knuth_bendix::contains(kb, "ccccc", "c"));
    REQUIRE(knuth_bendix::contains(kb, "bccba", "bccb"));
    REQUIRE(knuth_bendix::contains(kb, "bccbc", "bccb"));
    REQUIRE(knuth_bendix::contains(kb, "bcbca", "bcbc"));
    REQUIRE(knuth_bendix::contains(kb, "bcbcb", "bcbc"));
    REQUIRE(knuth_bendix::contains(kb, "bcbcc", "bcbc"));
    REQUIRE(knuth_bendix::contains(kb, "bccbb", "bccb"));
    REQUIRE(knuth_bendix::contains(kb, "bccb", "bccbb"));
    REQUIRE(!knuth_bendix::contains(kb, "aaaa", "bccbb"));

    REQUIRE((kb.active_rules() | sort(weird_cmp()) | to_vector())
            == std::vector<rule_type>({{"bcbca", "bcbc"},
                                       {"bcbcb", "bcbc"},
                                       {"bcbcc", "bcbc"},
                                       {"bccba", "bccb"},
                                       {"bccbb", "bccb"},
                                       {"bccbc", "bccb"},
                                       {"ccccc", "c"},
                                       {"bbbbbbb", "b"}}));

    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(6).count() == 356);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(POSITIVE_INFINITY).count()
            == POSITIVE_INFINITY);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(1).max(2) | to_vector())
            == std::vector<std::string>({"a", "b", "c"}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "080",
                                   "rewriting system from Congruence 20",
                                   "[quick][knuth-bendix]",
                                   KNUTH_BENDIX_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("ab");

    presentation::add_rule(p, "aaa", "a");
    presentation::add_rule(p, "ab", "ba");
    presentation::add_rule(p, "aa", "a");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    kb.run();

    REQUIRE(knuth_bendix::contains(kb, "abbbbbbbbbbbbbb", "aabbbbbbbbbbbbbb"));
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // 2-generator free abelian group (with this ordering KB terminates - but
  // no all)
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "081",
                                   "(from kbmag/standalone/kb_data/ab2)",
                                   "[quick][knuth-bendix][kbmag][shortlex]",
                                   KNUTH_BENDIX_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("aAbB");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "AaBb");
    presentation::add_rule(p, "Bab", "a");

    KnuthBendix<std::string, TestType> kb(twosided, p);

    REQUIRE(!kb.confluent());
    kb.run();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 8);

    REQUIRE(knuth_bendix::contains(kb, "Bab", "a"));
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(6).count() == 61);
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(POSITIVE_INFINITY).count()
            == POSITIVE_INFINITY);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(0).max(4) | to_vector())
            == std::vector<std::string>(
                {"",    "a",   "A",   "b",   "B",   "aa",  "ab",  "aB",  "AA",
                 "Ab",  "AB",  "bb",  "BB",  "aaa", "aab", "aaB", "abb", "aBB",
                 "AAA", "AAb", "AAB", "Abb", "ABB", "bbb", "BBB"}));
  }

  // This group is actually D_22 (although it wasn't meant to be). All
  // generators are unexpectedly involutory. knuth_bendix does not terminate
  // with the commented out ordering, terminates almost immediately with the
  // uncommented order.
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "082",
                                   "kbmag/standalone/kb_data/d22",
                                   "[quick][knuth-bendix][kbmag][shortlex]",
                                   KNUTH_BENDIX_TYPES) {
    auto rg = ReportGuard(false);

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

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.confluent());

    knuth_bendix::by_overlap_length(kb);
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 41);

    REQUIRE(knuth_bendix::contains(kb, "bfBY", ""));
    REQUIRE(knuth_bendix::contains(kb, "cyCD", ""));
    REQUIRE(knuth_bendix::contains(kb, "ybYA", ""));
    REQUIRE(knuth_bendix::contains(kb, "fCFB", ""));
    REQUIRE(knuth_bendix::contains(kb, "CAd", "dFD"));
    REQUIRE(knuth_bendix::contains(kb, "FDa", "aCA"));
    REQUIRE(knuth_bendix::contains(kb, "adFD", ""));
    REQUIRE(knuth_bendix::contains(kb, "daCA", ""));

    //     REQUIRE((kb.active_rules() | sort(weird_cmp()) | to_vector())
    //             == std::vector<rule_type>(
    //                 {{"a", "A"},    {"b", "B"},     {"c", "C"},     {"d",
    //                 "D"},
    //                  {"f", "F"},    {"y", "Y"},     {"AA", ""},     {"BB",
    //                  ""},
    //                  {"BC", "AB"},  {"BF", "Ay"},   {"CA", "AD"},   {"CB",
    //                  "BA"},
    //                  {"CC", ""},    {"CD", "AF"},   {"CF", "BY"},   {"DA",
    //                  "AC"},
    //                  {"DC", "CY"},  {"DD", ""},     {"DF", "AD"},   {"DY",
    //                  "BD"},
    //                  {"FA", "CY"},  {"FB", "BY"},   {"FC", "Ay"},   {"FD",
    //                  "DA"},
    //                  {"FF", "AA"},  {"FY", "BA"},   {"YA", "BY"},   {"YB",
    //                  "BF"},
    //                  {"YC", "CD"},  {"YD", "DB"},   {"YF", "AB"},   {"YY",
    //                  ""},
    //                  {"BAB", "C"},  {"BAC", "AYd"}, {"BAD", "ABA"}, {"BAF",
    //                  "ADY"},
    //                  {"BAY", "F"},  {"BDB", "ACY"}, {"DBA", "ADY"}, {"DBD",
    //                  "Y"},
    //                  {"DBY", "ADB"}}));

    REQUIRE(kb.number_of_classes() == 22);
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(3).count() == 17);
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(POSITIVE_INFINITY).count()
            == 22);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(0).max(4) | to_vector())
            == std::vector<std::string>({"",    "A",   "B",   "C",  "D",  "Y",
                                         "F",   "AB",  "AC",  "AD", "AY", "AF",
                                         "BA",  "BD",  "BY",  "CY", "DB", "ABA",
                                         "ABD", "ABY", "ACY", "ADB"}));
  }

  // No generators - no anything!
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "083",
                                   "(from kbmag/standalone/kb_data/degen1)",
                                   "[quick][knuth-bendix][kbmag][shortlex]",
                                   KNUTH_BENDIX_TYPES) {
    auto rg = ReportGuard(false);

    KnuthBendix<std::string, TestType> kb;

    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 0);
    REQUIRE(kb.number_of_classes() == 0);
    REQUIRE(kb.gilman_graph().number_of_nodes() == 0);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(0).max(4) | to_vector()).empty());
    REQUIRE(nf.min(0).max(4).count() == 0);
  }

  // Symmetric group S_4
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "084",
                                   "(from kbmag/standalone/kb_data/s4)",
                                   "[quick][knuth-bendix][kbmag][shortlex]",
                                   KNUTH_BENDIX_TYPES) {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abB");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "aBb");

    presentation::add_rule(p, "bb", "B");
    presentation::add_rule(p, "BaBa", "abab");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.confluent());

    knuth_bendix::by_overlap_length(kb);
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 11);
    REQUIRE(kb.number_of_classes() == 24);
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(6).count() == 23);
    REQUIRE(knuth_bendix::normal_forms(kb).min(6).max(7).count() == 1);
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(POSITIVE_INFINITY).count()
            == 24);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(0).max(7) | to_vector())
            == std::vector<std::string>(
                {"",     "a",    "b",     "B",     "ab",    "aB",
                 "ba",   "Ba",   "aba",   "aBa",   "bab",   "baB",
                 "Bab",  "BaB",  "abab",  "abaB",  "aBab",  "aBaB",
                 "baBa", "Baba", "abaBa", "aBaba", "baBab", "abaBab"}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "085",
                                   "fp semigroup",
                                   "[quick][knuth-bendix]",
                                   KNUTH_BENDIX_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet({0, 1, 2});
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

    KnuthBendix<std::string, TestType> kb(twosided, p);
    // kb.process_pending_rules();
    REQUIRE(kb.confluent());

    REQUIRE(knuth_bendix::contains(kb, {0, 0}, {0}));
    REQUIRE(knuth_bendix::contains(kb, {1, 1}, {1, 1}));
    REQUIRE(knuth_bendix::contains(kb, {1, 2}, {2, 1}));
    REQUIRE(knuth_bendix::contains(kb, {1, 0}, {2, 2, 0, 1, 2}));
    REQUIRE(knuth_bendix::contains(kb, {2, 1}, {1, 1, 1, 2}));
    REQUIRE(!knuth_bendix::contains(kb, {1, 0}, {2}));
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "086",
                                   "Chap. 11, Sec. 1 (q = 4, r = 3) in NR",
                                   "[knuth-bendix][quick]",
                                   KNUTH_BENDIX_TYPES) {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("ab");

    presentation::add_rule(p, "aaa", "a");
    presentation::add_rule(p, "bbbbb", "b");
    presentation::add_rule(p, "abbbabb", "bba");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.confluent());
    knuth_bendix::by_overlap_length(kb);
    REQUIRE(kb.number_of_active_rules() == 20);
    REQUIRE(kb.confluent());

    // Check that rewrite to a non-pointer argument does not rewrite its
    // argument
    std::string w = "aaa";
    REQUIRE(knuth_bendix::reduce_no_run(kb, w) == "a");
    REQUIRE(w == "aaa");

    // defining relations
    REQUIRE(knuth_bendix::reduce_no_run(kb, "aaa")
            == knuth_bendix::reduce_no_run(kb, "a"));
    REQUIRE(knuth_bendix::reduce_no_run(kb, "bbbbb")
            == knuth_bendix::reduce_no_run(kb, "b"));
    REQUIRE(knuth_bendix::reduce_no_run(kb, "abbbabb")
            == knuth_bendix::reduce_no_run(kb, "bba"));

    // consequential relations (Chap. 11, Lem. 1.1 in NR)
    REQUIRE(knuth_bendix::reduce_no_run(kb, "babbbb")
            == knuth_bendix::reduce_no_run(kb, "ba"));
    REQUIRE(knuth_bendix::reduce_no_run(kb, "baabbbb")
            == knuth_bendix::reduce_no_run(kb, "baa"));
    REQUIRE(knuth_bendix::reduce_no_run(kb, "aabbbbbbbbbba")
            == knuth_bendix::reduce_no_run(kb, "bbbbbbbbbba"));
    REQUIRE(knuth_bendix::reduce_no_run(kb, "babbbbbbbbaa")
            == knuth_bendix::reduce_no_run(kb, "babbbbbbbb"));
    REQUIRE(knuth_bendix::reduce_no_run(kb, "baabbbbbbaa")
            == knuth_bendix::reduce_no_run(kb, "baabbbbbb"));
    REQUIRE(knuth_bendix::reduce_no_run(kb, "bbbbaabbbbaa")
            == knuth_bendix::reduce_no_run(kb, "bbbbaa"));
    REQUIRE(knuth_bendix::reduce_no_run(kb, "bbbaa")
            == knuth_bendix::reduce_no_run(kb, "baabb"));
    REQUIRE(knuth_bendix::reduce_no_run(kb, "abbbaabbba")
            == knuth_bendix::reduce_no_run(kb, "bbbbaa"));

    REQUIRE(kb.number_of_classes() == 86);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(POSITIVE_INFINITY).count()
            == 86);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "087",
                                   "Chap. 11, Sec. 1 (q = 8, r = 5) in NR",
                                   "[no-valgrind][knuth-bendix][quick]",
                                   KNUTH_BENDIX_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("ab");

    presentation::add_rule(p, "aaa", "a");
    presentation::add_rule(p, "bbbbbbbbb", "b");
    presentation::add_rule(p, "abbbbbabb", "bba");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    // kb.clear_stack_interval(0);

    REQUIRE(!kb.confluent());
    knuth_bendix::by_overlap_length(kb);
    REQUIRE(kb.number_of_active_rules() == 105);
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_classes() == 746);

    // defining relations
    REQUIRE(knuth_bendix::reduce_no_run(kb, "aaa")
            == knuth_bendix::reduce_no_run(kb, "a"));
    REQUIRE(knuth_bendix::reduce_no_run(kb, "bbbbbbbbb")
            == knuth_bendix::reduce_no_run(kb, "b"));
    REQUIRE(knuth_bendix::reduce_no_run(kb, "abbbbbabb")
            == knuth_bendix::reduce_no_run(kb, "bba"));

    // consequential relations (Chap. 11, Lem. 1.1 in NR)
    REQUIRE(knuth_bendix::reduce_no_run(kb, "babbbbbbbb")
            == knuth_bendix::reduce_no_run(kb, "ba"));
    REQUIRE(knuth_bendix::reduce_no_run(kb, "baabbbbbbbb")
            == knuth_bendix::reduce_no_run(kb, "baa"));
    REQUIRE(knuth_bendix::reduce_no_run(kb, "aabbbbbbbbbbbba")
            == knuth_bendix::reduce_no_run(kb, "bbbbbbbbbbbba"));
    REQUIRE(knuth_bendix::reduce_no_run(kb, "babbbbbbbbbbaa")
            == knuth_bendix::reduce_no_run(kb, "babbbbbbbbbb"));
    REQUIRE(knuth_bendix::reduce_no_run(kb, "baabbbbbbbbaa")
            == knuth_bendix::reduce_no_run(kb, "baabbbbbbbb"));
    REQUIRE(knuth_bendix::reduce_no_run(kb, "bbbbbbbbaabbbbbbbbaa")
            == knuth_bendix::reduce_no_run(kb, "bbbbbbbbaa"));
    REQUIRE(knuth_bendix::reduce_no_run(kb, "bbbaa")
            == knuth_bendix::reduce_no_run(kb, "baabb"));
    REQUIRE(knuth_bendix::reduce_no_run(kb, "abbbbbaabbbbba")
            == knuth_bendix::reduce_no_run(kb, "bbbbbbbbaa"));

    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(POSITIVE_INFINITY).count()
            == 746);
  }

  // See KBFP 07 also.
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "088",
                                   "Chap. 7, Thm. 3.9 in NR",
                                   "[no-valgrind][knuth-bendix][quick]",
                                   KNUTH_BENDIX_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("ab");

    presentation::add_rule(p, "aaa", "a");
    presentation::add_rule(p, "bbbb", "b");
    presentation::add_rule(p, "abbba", "aa");
    presentation::add_rule(p, "baab", "bb");
    presentation::add_rule(p, "aabababababa", "aa");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.confluent());
    kb.run();
    REQUIRE(kb.number_of_active_rules() == 24);
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_classes() == 240);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(POSITIVE_INFINITY).count()
            == 240);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "089",
                                   "F(2, 5) - Chap. 9, Sec. 1 in NR",
                                   "[knuth-bendix][quick]",
                                   KNUTH_BENDIX_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abcde");

    presentation::add_rule(p, "ab", "c");
    presentation::add_rule(p, "bc", "d");
    presentation::add_rule(p, "cd", "e");
    presentation::add_rule(p, "de", "a");
    presentation::add_rule(p, "ea", "b");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.confluent());
    kb.run();
    REQUIRE(kb.number_of_active_rules() == 24);
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_classes() == 11);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE(nf.min(1).max(POSITIVE_INFINITY).count() == 11);
    REQUIRE(
        (nf | to_vector())
        == std::vector<std::string>(
            {"a", "b", "c", "d", "e", "aa", "ac", "ad", "bb", "be", "aad"}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "090",
                                   "F(2, 6) - Chap. 9, Sec. 1 in NR",
                                   "[knuth-bendix][quick]",
                                   KNUTH_BENDIX_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abcdef");
    p.contains_empty_word(true);

    presentation::add_rule(p, "ab", "");
    presentation::add_rule(p, "bc", "d");
    presentation::add_rule(p, "cd", "e");
    presentation::add_rule(p, "de", "f");
    presentation::add_rule(p, "ef", "a");
    presentation::add_rule(p, "fa", "b");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.confluent());
    kb.run();
    REQUIRE(kb.number_of_active_rules() == 35);
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_classes() == 12);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE(nf.min(0).max(POSITIVE_INFINITY).count() == 12);
    REQUIRE(
        (nf | to_vector())
        == std::vector<std::string>(
            {"", "a", "b", "c", "d", "e", "f", "aa", "ac", "ae", "bd", "df"}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "091",
                                   "Chap. 10, Sec. 4 in NR",
                                   "[knuth-bendix][quick]",
                                   KNUTH_BENDIX_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abc");

    presentation::add_rule(p, "aaaa", "a");
    presentation::add_rule(p, "bbbb", "b");
    presentation::add_rule(p, "cccc", "c");
    presentation::add_rule(p, "abab", "aaa");
    presentation::add_rule(p, "bcbc", "bbb");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.confluent());
    kb.run();
    REQUIRE(kb.number_of_active_rules() == 31);
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(POSITIVE_INFINITY).count()
            == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(10).count() == 8'823);
  }

  // Note: the fourth relator in NR's thesis incorrectly has exponent 3, it
  // should be 2. With exponent 3, the presentation defines the trivial group,
  // with exponent of 2, it defines the symmetric group as desired.
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "092",
                                   "Sym(5) - Chap. 3, Prop. 1.1 in NR",
                                   "[no-valgrind][knuth-bendix][quick]",
                                   KNUTH_BENDIX_TYPES) {
    auto rg = ReportGuard(false);

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

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.confluent());

    kb.run();
    REQUIRE(kb.number_of_active_rules() == 36);
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_classes() == 120);
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(POSITIVE_INFINITY).count()
            == 120);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(0).max(4) | to_vector())
            == std::vector<std::string>({"",    "A",   "B",   "b",   "AB",
                                         "Ab",  "BA",  "BB",  "bA",  "bb",
                                         "ABA", "ABB", "AbA", "Abb", "BAB",
                                         "BAb", "BBA", "bAB", "bAb", "bbA"}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "093",
                                   "SL(2, 7) - Chap. 3, Prop. 1.5 in NR",
                                   "[no-valgrind][quick][knuth-bendix]",
                                   KNUTH_BENDIX_TYPES) {
    auto rg = ReportGuard(false);

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

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.confluent());

    kb.run();
    REQUIRE(kb.number_of_active_rules() == 152);
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_classes() == 336);
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(POSITIVE_INFINITY).count()
            == 336);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE(
        (nf.min(0).max(4) | to_vector())
        == std::vector<std::string>(
            {"",    "a",   "b",   "A",   "B",   "aa",  "ab",  "aB",  "ba",
             "bb",  "bA",  "Ab",  "AA",  "AB",  "Ba",  "BA",  "aaa", "aab",
             "aaB", "aba", "abb", "abA", "aBa", "aBA", "baa", "bab", "baB",
             "bbA", "bAA", "Aba", "AAb", "AAA", "AAB", "ABa", "Baa", "BAA"}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "094",
                                   "bicyclic monoid",
                                   "[knuth-bendix][quick]",
                                   KNUTH_BENDIX_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("ab");
    p.contains_empty_word(true);

    presentation::add_rule(p, "ab", "");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    // kb.process_pending_rules();
    REQUIRE(kb.confluent());
    kb.run();
    REQUIRE(kb.number_of_active_rules() == 1);
    REQUIRE(kb.confluent());
    REQUIRE(is_obviously_infinite(kb));
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(10).count() == 55);

    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(0).max(4) | to_vector())
            == std::vector<std::string>(
                {"", "a", "b", "aa", "ba", "bb", "aaa", "baa", "bba", "bbb"}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "095",
                                   "plactic monoid of degree 2",
                                   "[knuth-bendix][quick]",
                                   KNUTH_BENDIX_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abc");
    p.contains_empty_word(true);

    presentation::add_rule(p, "aba", "baa");
    presentation::add_rule(p, "bba", "bab");
    presentation::add_rule(p, "ac", "");
    presentation::add_rule(p, "ca", "");
    presentation::add_rule(p, "bc", "");
    presentation::add_rule(p, "cb", "");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.confluent());

    kb.run();
    REQUIRE(kb.number_of_active_rules() == 3);
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(10).count() == 19);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE(
        (nf.min(0).max(4) | to_vector())
        == std::vector<std::string>({"", "a", "c", "aa", "cc", "aaa", "ccc"}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "096",
                                   "before Chap. 7, Prop. 1.1 in NR",
                                   "[knuth-bendix][quick]",
                                   KNUTH_BENDIX_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("ab");

    presentation::add_rule(p, "aa", "a");
    presentation::add_rule(p, "bb", "b");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    // kb.process_pending_rules();
    REQUIRE(kb.confluent());
    kb.run();
    REQUIRE(kb.number_of_active_rules() == 2);
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(10).count() == 18);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(1).max(4) | to_vector())
            == std::vector<std::string>({"a", "b", "ab", "ba", "aba", "bab"}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "097",
                                   "Chap. 7, Thm. 3.6 in NR",
                                   "[knuth-bendix][quick]",
                                   KNUTH_BENDIX_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("ab");

    presentation::add_rule(p, "aaa", "a");
    presentation::add_rule(p, "bbbb", "b");
    presentation::add_rule(p, "ababababab", "aa");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.confluent());

    kb.run();
    REQUIRE(kb.number_of_active_rules() == 12);
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_classes() == 243);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(POSITIVE_INFINITY).count()
            == 243);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(1).max(4) | to_vector())
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

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "098",
                                   "finite semigroup",
                                   "[knuth-bendix][quick]",
                                   KNUTH_BENDIX_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("ab");

    presentation::add_rule(p, "aaa", "a");
    presentation::add_rule(p, "bbbb", "b");
    presentation::add_rule(p, "abababab", "aa");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.confluent());

    kb.run();
    REQUIRE(kb.number_of_active_rules() == 9);
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_classes() == 99);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(POSITIVE_INFINITY).count()
            == 99);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE((nf.min(1).max(4) | to_vector())
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

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE(
      "KnuthBendix",
      "099",
      "Giles Gardam in \"A counterexample to the unit conjecture for group "
      "rings\" (https://arxiv.org/abs/2102.11818)",
      "[fail]",
      KNUTH_BENDIX_TYPES) {
    Presentation<std::string> p;
    p.alphabet("bABa");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "BabA");
    presentation::add_rule(p, "Abba", "BB");
    presentation::add_rule(p, "Baab", "AA");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    // knuth_bendix::by_overlap_length(kb);

    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

}  // namespace libsemigroups
