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

// This file is the second of six that contains tests for the KnuthBendix
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
// 6: contains tests for KnuthBendix created from word_type presentations

#include <cstddef>      // for size_t
#include <cstdint>      // for uint64_t
#include <iostream>     // for string, ostringstream
#include <string>       // for allocator, basic_string
#include <type_traits>  // for is_same_v
#include <utility>      // for move
#include <vector>       // for vector, operator==

#include "catch.hpp"      // for AssertionHandler, oper...
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/constants.hpp"         // for operator==, operator!=
#include "libsemigroups/digraph.hpp"           // for ActionDigraph
#include "libsemigroups/exception.hpp"         // for LibsemigroupsException
#include "libsemigroups/knuth-bendix-new.hpp"  // for KnuthBendix, normal_forms
#include "libsemigroups/order.hpp"             // for shortlex_compare
#include "libsemigroups/paths.hpp"             // for Paths
#include "libsemigroups/present.hpp"           // for add_rule, Presentation
#include "libsemigroups/report.hpp"            // for ReportGuard
#include "libsemigroups/words.hpp"             // for Inner, to_strings

#include "rx/ranges.hpp"  // for operator|, Inner, to_v...

namespace libsemigroups {
  using namespace rx;

  struct LibsemigroupsException;

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

  // Fibonacci group F(2,5) - monoid presentation - has order 12 (group
  // elements + empty word)
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "021",
                          "(from kbmag/standalone/kb_data/f25monoid)",
                          "[quick][knuth-bendix][kbmag][shortlex]") {
    auto rg = ReportGuard(false);

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
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 24);

    REQUIRE(kb.equal_to("ab", "c"));
    REQUIRE(kb.equal_to("bc", "d"));
    REQUIRE(kb.equal_to("cd", "e"));
    REQUIRE(kb.equal_to("de", "a"));
    REQUIRE(kb.equal_to("ea", "b"));
    REQUIRE(kb.equal_to("cc", "ad"));
    REQUIRE(kb.equal_to("dd", "be"));
    REQUIRE(kb.equal_to("ee", "ca"));
    REQUIRE(kb.equal_to("ec", "bb"));
    REQUIRE(kb.equal_to("db", "aa"));
    REQUIRE(kb.equal_to("aac", "be"));
    REQUIRE(kb.equal_to("bd", "aa"));
    REQUIRE(kb.equal_to("bbe", "aad"));
    REQUIRE(kb.equal_to("aaa", "e"));
    REQUIRE(kb.equal_to("eb", "be"));
    REQUIRE(kb.equal_to("ba", "c"));
    REQUIRE(kb.equal_to("da", "ad"));
    REQUIRE(kb.equal_to("ca", "ac"));
    REQUIRE(kb.equal_to("ce", "bb"));
    REQUIRE(kb.equal_to("cb", "d"));
    REQUIRE(kb.equal_to("ed", "a"));
    REQUIRE(kb.equal_to("dc", "e"));
    REQUIRE(kb.equal_to("ae", "b"));
    REQUIRE(kb.equal_to("bbb", "a"));
    REQUIRE(
        ((kb.active_rules() | to_vector()) | sort(weird_cmp()) | to_vector())
        == std::vector<rule_type>(
            {{"ab", "c"},  {"ae", "b"},   {"ba", "c"},   {"bc", "d"},
             {"bd", "aa"}, {"ca", "ac"},  {"cb", "d"},   {"cc", "ad"},
             {"cd", "e"},  {"ce", "bb"},  {"da", "ad"},  {"db", "aa"},
             {"dc", "e"},  {"dd", "be"},  {"de", "a"},   {"ea", "b"},
             {"eb", "be"}, {"ec", "bb"},  {"ed", "a"},   {"ee", "ca"},
             {"aaa", "e"}, {"aac", "be"}, {"bbb", "ed"}, {"bbe", "aad"}}));

    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE(
        (nf.min(1).max(5) | to_strings(p.alphabet()) | to_vector())
        == std::vector<std::string>(
            {"a", "b", "c", "d", "e", "aa", "ac", "ad", "bb", "be", "aad"}));
    REQUIRE(kb.size() == 11);
    REQUIRE(nf.min(1).max(POSITIVE_INFINITY).count() == 11);
  }

  // trivial group - BHN presentation
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "022",
                          "(from kbmag/standalone/kb_data/degen4a)",
                          "[quick][knuth-bendix][kbmag][shortlex]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbBcC");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "AaBbCc");

    presentation::add_rule(p, "Aba", "bb");
    presentation::add_rule(p, "Bcb", "cc");
    presentation::add_rule(p, "Cac", "aa");

    KnuthBendix kb(p);

    REQUIRE(!kb.confluent());

    kb.run();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 6);

    REQUIRE(kb.equal_to("Aba", "bb"));
    REQUIRE(kb.equal_to("Bcb", "cc"));
    REQUIRE(kb.equal_to("Cac", "aa"));
    REQUIRE((kb.active_rules() | sort(weird_cmp()) | to_vector())
            == std::vector<rule_type>({{"A", ""},
                                       {"B", ""},
                                       {"C", ""},
                                       {"a", ""},
                                       {"b", ""},
                                       {"c", ""}}));
    REQUIRE(kb.size() == 1);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE(nf.count() == 1);
  }

  // Torus group
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "023",
                          "(from kbmag/standalone/kb_data/torus)",
                          "[quick][knuth-bendix][kbmag][shortlex]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAcCbBdD");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "AaCcBbDd");
    presentation::add_rule(p, "ABab", "DCdc");

    KnuthBendix kb(p);
    REQUIRE(!kb.confluent());
    kb.run();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 16);

    REQUIRE(kb.equal_to("DCdc", "ABab"));
    REQUIRE((kb.active_rules() | sort(weird_cmp()) | to_vector())
            == std::vector<rule_type>({{"Aa", ""},
                                       {"Bb", ""},
                                       {"Cc", ""},
                                       {"Dd", ""},
                                       {"aA", ""},
                                       {"bB", ""},
                                       {"cC", ""},
                                       {"dD", ""},
                                       {"BAba", "CDcd"},
                                       {"BabC", "aDCd"},
                                       {"DCdc", "ABab"},
                                       {"DcdA", "cBAb"},
                                       {"bCDc", "AbaD"},
                                       {"baDC", "abCD"},
                                       {"dABa", "CdcB"},
                                       {"dcBA", "cdAB"}}));
    REQUIRE(kb.size() == POSITIVE_INFINITY);
    auto nf = knuth_bendix::normal_forms(kb).min(0).max(7);
    REQUIRE(nf.count() == 155'577);
    REQUIRE(
        (nf.min(0).max(3) | to_strings(p.alphabet()) | to_vector())
        == std::vector<std::string>(
            {"",   "a",  "A",  "c",  "C",  "b",  "B",  "d",  "D",  "aa", "ac",
             "aC", "ab", "aB", "ad", "aD", "AA", "Ac", "AC", "Ab", "AB", "Ad",
             "AD", "ca", "cA", "cc", "cb", "cB", "cd", "cD", "Ca", "CA", "CC",
             "Cb", "CB", "Cd", "CD", "ba", "bA", "bc", "bC", "bb", "bd", "bD",
             "Ba", "BA", "Bc", "BC", "BB", "Bd", "BD", "da", "dA", "dc", "dC",
             "db", "dB", "dd", "Da", "DA", "Dc", "DC", "Db", "DB", "DD"}));
  }

  //  3-fold cover of A_6
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "024",
                          "(from kbmag/standalone/kb_data/3a6)",
                          "[quick][knuth-bendix][kbmag][shortlex]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("abAB");

    presentation::add_inverse_rules(p, "ABab");

    presentation::add_rule(p, "aaa", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "abababab", "");
    presentation::add_rule(p, "aBaBaBaBaB", "");

    KnuthBendix kb(p);
    REQUIRE(!kb.confluent());

    kb.run();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 183);

    REQUIRE(kb.equal_to("aaa", ""));
    REQUIRE(kb.equal_to("bbb", ""));
    REQUIRE(kb.equal_to("BaBaBaBaB", "aa"));
    REQUIRE(kb.equal_to("bababa", "aabb"));
    REQUIRE(kb.equal_to("ababab", "bbaa"));
    REQUIRE(kb.equal_to("aabbaa", "babab"));
    REQUIRE(kb.equal_to("bbaabb", "ababa"));
    REQUIRE(kb.equal_to("bababbabab", "aabbabbaa"));
    REQUIRE(kb.equal_to("ababaababa", "bbaabaabb"));
    REQUIRE(kb.equal_to("bababbabaababa", "aabbabbaabaabb"));
    REQUIRE(kb.equal_to("bbaabaabbabbaa", "ababaababbabab"));

    REQUIRE(kb.size() == 1080);

    auto nf = knuth_bendix::normal_forms(kb);

    REQUIRE(nf.count() == 1080);
    REQUIRE((nf.min(0).max(3) | to_strings(p.alphabet()) | to_vector())
            == std::vector<std::string>({"",
                                         "a",
                                         "b",
                                         "A",
                                         "B",
                                         "ab",
                                         "aB",
                                         "ba",
                                         "bA",
                                         "Ab",
                                         "AB",
                                         "Ba",
                                         "BA"}));
  }

  //  Free group on 2 generators
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "025",
                          "(from kbmag/standalone/kb_data/f2)",
                          "[quick][knuth-bendix][kbmag][shortlex]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbB");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "AaBb");

    KnuthBendix kb(p);
    REQUIRE(kb.confluent());
    kb.run();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 4);
    REQUIRE(kb.size() == POSITIVE_INFINITY);

    auto nf = knuth_bendix::normal_forms(kb).min(0).max(3);

    REQUIRE((nf | to_strings(p.alphabet()) | to_vector())
            == std::vector<std::string>({"",
                                         "a",
                                         "A",
                                         "b",
                                         "B",
                                         "aa",
                                         "ab",
                                         "aB",
                                         "AA",
                                         "Ab",
                                         "AB",
                                         "ba",
                                         "bA",
                                         "bb",
                                         "Ba",
                                         "BA",
                                         "BB"}));
    REQUIRE(nf.min(0).max(5).count() == 161);
  }

  // Symmetric group S_16
  LIBSEMIGROUPS_TEST_CASE(
      "KnuthBendix",
      "026",
      "(from kbmag/standalone/kb_data/s16)",
      "[quick][knuth-bendix][kbmag][shortlex][no-valgrind]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abcdefghijklmno");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "abcdefghijklmno");

    presentation::add_rule(p, "bab", "aba");
    presentation::add_rule(p, "ca", "ac");
    presentation::add_rule(p, "da", "ad");
    presentation::add_rule(p, "ea", "ae");
    presentation::add_rule(p, "fa", "af");
    presentation::add_rule(p, "ga", "ag");
    presentation::add_rule(p, "ha", "ah");
    presentation::add_rule(p, "ia", "ai");
    presentation::add_rule(p, "ja", "aj");
    presentation::add_rule(p, "ka", "ak");
    presentation::add_rule(p, "la", "al");
    presentation::add_rule(p, "ma", "am");
    presentation::add_rule(p, "na", "an");
    presentation::add_rule(p, "oa", "ao");
    presentation::add_rule(p, "cbc", "bcb");
    presentation::add_rule(p, "db", "bd");
    presentation::add_rule(p, "eb", "be");
    presentation::add_rule(p, "fb", "bf");
    presentation::add_rule(p, "gb", "bg");
    presentation::add_rule(p, "hb", "bh");
    presentation::add_rule(p, "ib", "bi");
    presentation::add_rule(p, "jb", "bj");
    presentation::add_rule(p, "kb", "bk");
    presentation::add_rule(p, "lb", "bl");
    presentation::add_rule(p, "mb", "bm");
    presentation::add_rule(p, "nb", "bn");
    presentation::add_rule(p, "ob", "bo");
    presentation::add_rule(p, "dcd", "cdc");
    presentation::add_rule(p, "ec", "ce");
    presentation::add_rule(p, "fc", "cf");
    presentation::add_rule(p, "gc", "cg");
    presentation::add_rule(p, "hc", "ch");
    presentation::add_rule(p, "ic", "ci");
    presentation::add_rule(p, "jc", "cj");
    presentation::add_rule(p, "kc", "ck");
    presentation::add_rule(p, "lc", "cl");
    presentation::add_rule(p, "mc", "cm");
    presentation::add_rule(p, "nc", "cn");
    presentation::add_rule(p, "oc", "co");
    presentation::add_rule(p, "ede", "ded");
    presentation::add_rule(p, "fd", "df");
    presentation::add_rule(p, "gd", "dg");
    presentation::add_rule(p, "hd", "dh");
    presentation::add_rule(p, "id", "di");
    presentation::add_rule(p, "jd", "dj");
    presentation::add_rule(p, "kd", "dk");
    presentation::add_rule(p, "ld", "dl");
    presentation::add_rule(p, "md", "dm");
    presentation::add_rule(p, "nd", "dn");
    presentation::add_rule(p, "od", "do");
    presentation::add_rule(p, "fef", "efe");
    presentation::add_rule(p, "ge", "eg");
    presentation::add_rule(p, "he", "eh");
    presentation::add_rule(p, "ie", "ei");
    presentation::add_rule(p, "je", "ej");
    presentation::add_rule(p, "ke", "ek");
    presentation::add_rule(p, "le", "el");
    presentation::add_rule(p, "me", "em");
    presentation::add_rule(p, "ne", "en");
    presentation::add_rule(p, "oe", "eo");
    presentation::add_rule(p, "gfg", "fgf");
    presentation::add_rule(p, "hf", "fh");
    presentation::add_rule(p, "if", "fi");
    presentation::add_rule(p, "jf", "fj");
    presentation::add_rule(p, "kf", "fk");
    presentation::add_rule(p, "lf", "fl");
    presentation::add_rule(p, "mf", "fm");
    presentation::add_rule(p, "nf", "fn");
    presentation::add_rule(p, "of", "fo");
    presentation::add_rule(p, "hgh", "ghg");
    presentation::add_rule(p, "ig", "gi");
    presentation::add_rule(p, "jg", "gj");
    presentation::add_rule(p, "kg", "gk");
    presentation::add_rule(p, "lg", "gl");
    presentation::add_rule(p, "mg", "gm");
    presentation::add_rule(p, "ng", "gn");
    presentation::add_rule(p, "og", "go");
    presentation::add_rule(p, "ihi", "hih");
    presentation::add_rule(p, "jh", "hj");
    presentation::add_rule(p, "kh", "hk");
    presentation::add_rule(p, "lh", "hl");
    presentation::add_rule(p, "mh", "hm");
    presentation::add_rule(p, "nh", "hn");
    presentation::add_rule(p, "oh", "ho");
    presentation::add_rule(p, "jij", "iji");
    presentation::add_rule(p, "ki", "ik");
    presentation::add_rule(p, "li", "il");
    presentation::add_rule(p, "mi", "im");
    presentation::add_rule(p, "ni", "in");
    presentation::add_rule(p, "oi", "io");
    presentation::add_rule(p, "kjk", "jkj");
    presentation::add_rule(p, "lj", "jl");
    presentation::add_rule(p, "mj", "jm");
    presentation::add_rule(p, "nj", "jn");
    presentation::add_rule(p, "oj", "jo");
    presentation::add_rule(p, "lkl", "klk");
    presentation::add_rule(p, "mk", "km");
    presentation::add_rule(p, "nk", "kn");
    presentation::add_rule(p, "ok", "ko");
    presentation::add_rule(p, "mlm", "lml");
    presentation::add_rule(p, "nl", "ln");
    presentation::add_rule(p, "ol", "lo");
    presentation::add_rule(p, "nmn", "mnm");
    presentation::add_rule(p, "om", "mo");
    presentation::add_rule(p, "ono", "non");
    KnuthBendix kb(p);

    REQUIRE(!kb.confluent());

    kb.run();  // faster
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 211);  // verified with KBMAG
    REQUIRE(kb.gilman_digraph().number_of_nodes() == 121);
    REQUIRE(kb.gilman_digraph().number_of_edges() == 680);
    // verified with KBMAG
    auto nf = knuth_bendix::normal_forms(kb).min(0).max(7);
    REQUIRE(nf.count() == 49'436);

    // verified with KBMAG
    REQUIRE(nf.min(0).max(11).count() == 2'554'607);
    REQUIRE(std::is_same_v<decltype(nf.size_hint()), uint64_t>);
    REQUIRE(nf.max(POSITIVE_INFINITY).size_hint() == 20'922'789'888'000);
    REQUIRE(kb.size() == 20'922'789'888'000);
  }

  // Presentation of group A_4 regarded as monoid presentation - gives
  // infinite monoid.
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "027",
                          "(from kbmag/standalone/kb_data/a4monoid)",
                          "[quick][knuth-bendix][kbmag][shortlex]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abB");

    presentation::add_rule(p, "bb", "B");
    presentation::add_rule(p, "BaB", "aba");

    KnuthBendix kb(p);
    REQUIRE(!kb.confluent());

    kb.run();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 6);

    REQUIRE(kb.equal_to("bb", "B"));
    REQUIRE(kb.equal_to("BaB", "aba"));
    REQUIRE(kb.equal_to("Bb", "bB"));
    REQUIRE(kb.equal_to("Baaba", "abaaB"));
    REQUIRE(kb.equal_to("BabB", "abab"));
    REQUIRE(kb.equal_to("Bababa", "ababaB"));
    REQUIRE((kb.active_rules() | sort(weird_cmp()) | to_vector())
            == std::vector<rule_type>({{{"Bb", "bB"},
                                        {"bb", "B"},
                                        {"BaB", "aba"},
                                        {"BabB", "abab"},
                                        {"Baaba", "abaaB"},
                                        {"Bababa", "ababaB"}}}));
  }

  // fairly clearly the trivial group
  LIBSEMIGROUPS_TEST_CASE(
      "KnuthBendix",
      "028",
      "(from kbmag/standalone/kb_data/degen3)",
      "[quick][knuth-bendix][kbmag][shortlex][no-valgrind]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbB");
    p.contains_empty_word(true);
    presentation::add_rule(p, "ab", "");
    presentation::add_rule(p, "abb", "");

    KnuthBendix kb(p);
    REQUIRE((kb.active_rules() | sort(weird_cmp()) | to_vector())
            == std::vector<rule_type>({{"a", ""}, {"b", ""}}));
    REQUIRE(kb.number_of_active_rules() == 2);
    REQUIRE(kb.confluent());

    kb.run();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 2);

    REQUIRE(kb.equal_to("b", ""));
    REQUIRE(kb.equal_to("a", ""));
    REQUIRE((kb.active_rules() | sort(weird_cmp()) | to_vector())
            == std::vector<rule_type>({{"a", ""}, {"b", ""}}));
  }

  // infinite cyclic group
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "029",
                          "(from kbmag/standalone/kb_data/ab1)",
                          "[quick][knuth-bendix][kbmag][shortlex]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("aA");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "Aa");

    KnuthBendix kb(p);
    REQUIRE(kb.confluent());

    kb.run();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 2);
    REQUIRE(kb.size() == POSITIVE_INFINITY);
  }

  // A generator, but trivial.
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "030",
                          "(from kbmag/standalone/kb_data/degen2)",
                          "[quick][knuth-bendix][kbmag][shortlex]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aA");
    p.contains_empty_word(true);
    presentation::add_rule(p, "a", "");

    KnuthBendix kb(p);
    REQUIRE(kb.confluent());

    kb.run();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 1);

    REQUIRE(kb.equal_to("a", ""));
    REQUIRE((kb.active_rules() | to_vector())
            == std::vector<rule_type>({{"a", ""}}));
  }

  // Fibonacci group F(2,5)
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "031",
                          "(from kbmag/standalone/kb_data/f25)",
                          "[quick][knuth-bendix][kbmag][shortlex]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbBcCdDyY");

    presentation::add_rule(p, "ab", "c");
    presentation::add_rule(p, "bc", "d");
    presentation::add_rule(p, "cd", "y");
    presentation::add_rule(p, "dy", "a");
    presentation::add_rule(p, "ya", "b");

    KnuthBendix kb(p);
    REQUIRE(!kb.confluent());

    kb.run();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 24);

    REQUIRE(kb.equal_to("ab", "c"));
    REQUIRE(kb.equal_to("bc", "d"));
    REQUIRE(kb.equal_to("cd", "y"));
    REQUIRE(kb.equal_to("dy", "a"));
    REQUIRE(kb.equal_to("ya", "b"));
    REQUIRE(kb.equal_to("cc", "ad"));
    REQUIRE(kb.equal_to("dd", "by"));
    REQUIRE(kb.equal_to("yy", "ac"));
    REQUIRE(kb.equal_to("yc", "bb"));
    REQUIRE(kb.equal_to("db", "aa"));
    REQUIRE(kb.equal_to("aac", "by"));
    REQUIRE(kb.equal_to("bd", "aa"));
    REQUIRE(kb.equal_to("bby", "aad"));
    REQUIRE(kb.equal_to("aaa", "y"));
    REQUIRE(kb.equal_to("yb", "by"));
    REQUIRE(kb.equal_to("ba", "c"));
    REQUIRE(kb.equal_to("da", "ad"));
    REQUIRE(kb.equal_to("ca", "ac"));
    REQUIRE(kb.equal_to("cy", "bb"));
    REQUIRE(kb.equal_to("cb", "d"));
    REQUIRE(kb.equal_to("yd", "a"));
    REQUIRE(kb.equal_to("dc", "y"));
    REQUIRE(kb.equal_to("ay", "b"));
    REQUIRE(kb.equal_to("bbb", "a"));
    REQUIRE((kb.active_rules() | sort(weird_cmp()) | to_vector())
            == std::vector<rule_type>(
                {{"ab", "c"},  {"ay", "b"},   {"ba", "c"},   {"bc", "d"},
                 {"bd", "aa"}, {"ca", "ac"},  {"cb", "d"},   {"cc", "ad"},
                 {"cd", "y"},  {"cy", "bb"},  {"da", "ad"},  {"db", "aa"},
                 {"dc", "y"},  {"dd", "by"},  {"dy", "a"},   {"ya", "b"},
                 {"yb", "by"}, {"yc", "bb"},  {"yd", "a"},   {"yy", "ca"},
                 {"aaa", "y"}, {"aac", "by"}, {"bbb", "yd"}, {"bby", "aad"}}));
  }

  // Von Dyck (2,3,7) group - infinite hyperbolic
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "032",
                          "(from kbmag/standalone/kb_data/237)",
                          "[quick][knuth-bendix][kbmag][shortlex]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("aAbBc");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "AaBbc");

    presentation::add_rule(p, "aaaa", "AAA");
    presentation::add_rule(p, "bb", "B");
    presentation::add_rule(p, "BA", "c");

    KnuthBendix kb(p);
    REQUIRE(!kb.confluent());

    kb.run();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 32);
    REQUIRE((kb.active_rules() | sort(weird_cmp()) | to_vector())
            == std::vector<rule_type>({{"Aa", ""},
                                       {"Ac", "b"},
                                       {"BA", "c"},
                                       {"BB", "b"},
                                       {"Bb", ""},
                                       {"Bc", "bA"},
                                       {"aA", ""},
                                       {"ab", "c"},
                                       {"bB", ""},
                                       {"ba", "AB"},
                                       {"bb", "B"},
                                       {"bc", "A"},
                                       {"cB", "a"},
                                       {"ca", "B"},
                                       {"cb", "aB"},
                                       {"cc", ""},
                                       {"BaB", "bAb"},
                                       {"bAB", "Ba"},
                                       {"cAB", "aBa"},
                                       {"AAAA", "aaa"},
                                       {"AAAb", "aaac"},
                                       {"aaaa", "AAA"},
                                       {"bAbA", "Bac"},
                                       {"cAAA", "Baaa"},
                                       {"cAbA", "aBac"},
                                       {"ABaaa", "bAAA"},
                                       {"Baaac", "cAAb"},
                                       {"bAABaac", "BacAAb"},
                                       {"cAABaac", "aBacAAb"},
                                       {"BaaaBaaa", "cAAbAAA"},
                                       {"bAABaaBaaa", "BacAAbAAA"},
                                       {"cAABaaBaaa", "aBacAAbAAA"}}));
    REQUIRE(kb.size() == POSITIVE_INFINITY);
  }

  // Cyclic group of order 2.
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "033",
                          "(from kbmag/standalone/kb_data/c2)",
                          "[quick][knuth-bendix][kbmag][shortlex]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("a");
    p.contains_empty_word(true);
    presentation::add_rule(p, "aa", "");

    KnuthBendix kb(p);

    REQUIRE(kb.confluent());

    kb.run();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 1);

    REQUIRE((kb.active_rules() | sort(weird_cmp()) | to_vector())
            == std::vector<rule_type>({{"aa", ""}}));
  }

  // The group is S_4, and the subgroup H of order 4. There are 30 reduced
  // words - 24 for the group elements, and 6 for the 6 cosets Hg.
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "034",
                          "(from kbmag/standalone/kb_data/cosets)",
                          "[quick][knuth-bendix][kbmag][shortlex]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("HaAbB");

    presentation::add_rule(p, "aaa", "");
    presentation::add_rule(p, "bbbb", "");
    presentation::add_rule(p, "abab", "");
    presentation::add_rule(p, "Hb", "H");
    presentation::add_rule(p, "HH", "H");
    presentation::add_rule(p, "aH", "H");
    presentation::add_rule(p, "bH", "H");

    KnuthBendix kb(p);
    REQUIRE(!kb.confluent());

    kb.run();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 14);

    REQUIRE(kb.equal_to("aaa", ""));
    REQUIRE(kb.equal_to("Hb", "H"));
    REQUIRE(kb.equal_to("HH", "H"));
    REQUIRE(kb.equal_to("aH", "H"));
    REQUIRE(kb.equal_to("bH", "H"));
    REQUIRE(kb.equal_to("bab", "aa"));
    REQUIRE(kb.equal_to("bbb", "aba"));
    REQUIRE(kb.equal_to("Hab", "Haa"));
    REQUIRE(kb.equal_to("abaab", "bbaa"));
    REQUIRE(kb.equal_to("baaba", "aabb"));
    REQUIRE(kb.equal_to("Haabb", "Haaba"));
    REQUIRE(kb.equal_to("bbaabb", "abba"));
    REQUIRE(kb.equal_to("aabbaa", "baab"));
    REQUIRE(kb.equal_to("baabba", "abbaab"));
    REQUIRE((kb.active_rules() | sort(weird_cmp()) | to_vector())
            == std::vector<rule_type>({{{"HH", "H"},
                                        {"Hb", "H"},
                                        {"aH", "H"},
                                        {"bH", "H"},
                                        {"Hab", "Haa"},
                                        {"aaa", ""},
                                        {"bab", "aa"},
                                        {"bbb", "aba"},
                                        {"Haabb", "Haaba"},
                                        {"abaab", "bbaa"},
                                        {"baaba", "aabb"},
                                        {"aabbaa", "baab"},
                                        {"baabba", "abbaab"},
                                        {"bbaabb", "abba"}}}));
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "035",
                          "Example 5.1 in Sims (KnuthBendix 09 again)",
                          "[quick][knuth-bendix]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbB");
    p.contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "ba", "ab");

    KnuthBendix kb(p);
    REQUIRE(!kb.confluent());

    kb.run();
    REQUIRE(kb.number_of_active_rules() == 8);
    REQUIRE(kb.confluent());
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "036",
                          "(from kbmag/standalone/kb_data/nilp2)",
                          "[quick][knuth-bendix][kbmag][shortlex]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("cCbBaA");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "CcBbAa");

    presentation::add_rule(p, "ba", "abc");
    presentation::add_rule(p, "ca", "ac");
    presentation::add_rule(p, "cb", "bc");

    KnuthBendix kb(p);
    REQUIRE(!kb.confluent());
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "037",
                          "Example 6.4 in Sims",
                          "[quick][knuth-bendix][no-valgrind]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("abc");
    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bc", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "ababababababab", "");
    presentation::add_rule(p, "abacabacabacabac", "");

    KnuthBendix kb(p);
    REQUIRE(kb.number_of_active_rules() == 5);
    REQUIRE(!kb.confluent());

    kb.max_rules(10);
    kb.run();
    REQUIRE(kb.number_of_active_rules() == 10);
    REQUIRE(!kb.confluent());

    kb.run();
    REQUIRE(kb.number_of_active_rules() == 10);
    REQUIRE(!kb.confluent());

    kb.max_rules(20);
    kb.run();
    REQUIRE(kb.number_of_active_rules() == 21);
    REQUIRE(!kb.confluent());

    kb.max_rules(LIMIT_MAX);
    kb.run();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 40);
  }

  // Von Dyck (2,3,7) group - infinite hyperbolic
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "038",
                          "KnuthBendix 071 again",
                          "[no-valgrind][quick][knuth-bendix]["
                          "shortlex]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("aAbBc");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "AaBbc");

    presentation::add_rule(p, "BA", "c");
    presentation::add_rule(p, "Bb", "bB");
    presentation::add_rule(p, "bb", "B");
    presentation::add_rule(p, "AAAa", "aAAA");
    presentation::add_rule(p, "aaaa", "AAA");
    presentation::add_rule(p, "BaAAA", "cAAa");
    presentation::add_rule(p, "BaaAAA", "cAAaa");
    presentation::add_rule(p, "BaAaAAA", "cAAaAa");
    presentation::add_rule(p, "BaaaAAA", "cAAaaa");
    presentation::add_rule(p, "BaAAaAAA", "cAAaAAa");
    presentation::add_rule(p, "BaAaaAAA", "cAAaAaa");
    presentation::add_rule(p, "BaaAaAAA", "cAAaaAa");
    presentation::add_rule(p, "BaAAaaAAA", "cAAaAAaa");
    presentation::add_rule(p, "BaAaAaAAA", "cAAaAaAa");
    presentation::add_rule(p, "BaAaaaAAA", "cAAaAaaa");
    presentation::add_rule(p, "BaaAAaAAA", "cAAaaAAa");
    presentation::add_rule(p, "BaaAaaAAA", "cAAaaAaa");
    presentation::add_rule(p, "BaAAaAaAAA", "cAAaAAaAa");
    presentation::add_rule(p, "BaAAaaaAAA", "cAAaAAaaa");
    presentation::add_rule(p, "BaAaAAaAAA", "cAAaAaAAa");
    presentation::add_rule(p, "BaAaAaaAAA", "cAAaAaAaa");
    presentation::add_rule(p, "BaAaaAaAAA", "cAAaAaaAa");
    presentation::add_rule(p, "BaaAAaaAAA", "cAAaaAAaa");
    presentation::add_rule(p, "BaaAaAaAAA", "cAAaaAaAa");
    presentation::add_rule(p, "BaAAaAAaAAA", "cAAaAAaAAa");
    presentation::add_rule(p, "BaAAaAaaAAA", "cAAaAAaAaa");
    presentation::add_rule(p, "BaAAaaAaAAA", "cAAaAAaaAa");
    presentation::add_rule(p, "BaAaAAaaAAA", "cAAaAaAAaa");
    presentation::add_rule(p, "BaAaAaAaAAA", "cAAaAaAaAa");
    presentation::add_rule(p, "BaAaaAAaAAA", "cAAaAaaAAa");
    presentation::add_rule(p, "BaaAAaAaAAA", "cAAaaAAaAa");
    presentation::add_rule(p, "BaaAaAAaAAA", "cAAaaAaAAa");
    presentation::add_rule(p, "BaAAaAAaaAAA", "cAAaAAaAAaa");
    presentation::add_rule(p, "BaAAaAaAaAAA", "cAAaAAaAaAa");
    presentation::add_rule(p, "BaAAaaAAaAAA", "cAAaAAaaAAa");
    presentation::add_rule(p, "BaAaAAaAaAAA", "cAAaAaAAaAa");
    presentation::add_rule(p, "BaAaAaAAaAAA", "cAAaAaAaAAa");
    presentation::add_rule(p, "BaaAAaAAaAAA", "cAAaaAAaAAa");
    presentation::add_rule(p, "BaAAaAAaAaAAA", "cAAaAAaAAaAa");
    presentation::add_rule(p, "BaAAaAaAAaAAA", "cAAaAAaAaAAa");
    presentation::add_rule(p, "BaAaAAaAAaAAA", "cAAaAaAAaAAa");
    presentation::add_rule(p, "BaAAaAAaAAaAAA", "cAAaAAaAAaAAa");

    KnuthBendix kb(p);
    REQUIRE(kb.number_of_active_rules() == 9);
    REQUIRE(!kb.confluent());
    kb.run();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 32);
    REQUIRE(kb.size() == POSITIVE_INFINITY);
    auto nf = knuth_bendix::normal_forms(kb);

    REQUIRE(nf.min(4).max(5).count() == 24);
    nf.min(4);
    REQUIRE((nf | to_strings(p.alphabet()) | to_vector())
            == std::vector<std::string>({"aaaB", "aaac", "aaBa", "aacA", "aBaa",
                                         "aBac", "acAA", "acAb", "AAAB", "AAbA",
                                         "AABa", "AbAA", "AbAb", "ABaa", "ABac",
                                         "bAAA", "bAAb", "bAAB", "Baaa", "BaaB",
                                         "Baac", "BacA", "cAAb", "cAAB"}));
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "039",
                          "Example 5.4 in Sims (KnuthBendix 11 "
                          "again) "
                          "(different overlap policy)",
                          "[quick][knuth-bendix]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("Bab");
    p.contains_empty_word(true);
    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "ababab", "");

    KnuthBendix kb(p);
    kb.overlap_policy(KnuthBendix::options::overlap::AB_BC);

    REQUIRE(!kb.confluent());

    kb.knuth_bendix_by_overlap_length();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 11);
    REQUIRE(kb.confluent());
    REQUIRE(kb.size() == 12);
    auto nf1 = knuth_bendix::normal_forms(kb).min(4).max(5);
    REQUIRE(nf1.count() == 0);

    auto nf = (knuth_bendix::normal_forms(kb) | to_strings(p.alphabet()));
    REQUIRE((nf | to_vector())
            == std::vector<std::string>({"",
                                         "B",
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
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "040",
                          "Example 5.4 in Sims (KnuthBendix 11 "
                          "again) "
                          "(different overlap policy)",
                          "[quick][knuth-bendix]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("Bab");
    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "ababab", "");

    KnuthBendix kb(p);
    kb.overlap_policy(KnuthBendix::options::overlap::MAX_AB_BC);
    // The next line tests that we don't delete
    // the old OverlapMeasure.
    kb.overlap_policy(KnuthBendix::options::overlap::MAX_AB_BC);

    REQUIRE(!kb.confluent());

    kb.knuth_bendix_by_overlap_length();
    REQUIRE(kb.number_of_active_rules() == 11);
    REQUIRE(kb.confluent());
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "041",
                          "operator<<",
                          "[quick][knuth-bendix]") {
    std::ostringstream os;

    Presentation<std::string> p;
    p.alphabet("Bab");
    p.contains_empty_word(true);
    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "ababab", "");

    KnuthBendix kb1(p);
    os << kb1;  // Does not do anything visible
    p.alphabet("cbaB");
    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "ababab", "");
    KnuthBendix kb2(p);
    os << kb2;  // Does not do anything visible
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "042",
                          "confluence_interval",
                          "[quick][knuth-bendix]") {
    KnuthBendix               kb;
    Presentation<std::string> p;
    p.alphabet("Bab");
    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "ababab", "");
    kb.check_confluence_interval(LIMIT_MAX);
    kb.check_confluence_interval(10);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "043",
                          "max_overlap",
                          "[quick][knuth-bendix]") {
    KnuthBendix               kb;
    Presentation<std::string> p;
    p.alphabet("Bab");

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "ababab", "");
    kb.max_overlap(10);
    kb.max_overlap(-11);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "044",
                          "(from "
                          "kbmag/standalone/kb_data/d22) (2 / 3) "
                          "(finite)",
                          "[quick][knuth-bendix]["
                          "fpsemi][kbmag][shortlex]") {
    auto rg = ReportGuard(false);

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
    REQUIRE(kb.presentation().rules.size() / 2 == 18);
    REQUIRE(kb.size() == 22);

    auto nf = (knuth_bendix::normal_forms(kb) | to_strings(p.alphabet()));
    REQUIRE((nf | to_vector())
            == std::vector<std::string>({"",    "A",   "B",   "C",  "D",  "Y",
                                         "F",   "AB",  "AC",  "AD", "AY", "AF",
                                         "BA",  "BD",  "BY",  "CY", "DB", "ABA",
                                         "ABD", "ABY", "ACY", "ADB"}));
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "045",
                          "(from "
                          "kbmag/standalone/kb_data/d22) (3 / 3) "
                          "(finite)",
                          "[quick][knuth-bendix]["
                          "fpsemi][kbmag][shortlex]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("aAbBcCdDyYfF");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "AaBbCcDdYyFf");

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
    REQUIRE(kb.presentation().rules.size() / 2 == 18);
    REQUIRE(kb.size() == 22);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "046",
                          "small example",
                          "[quick][knuth-bendix][shortlex]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "aaa", "a");
    presentation::add_rule(p, "bbbb", "b");
    presentation::add_rule(p, "ababababab", "aa");
    KnuthBendix kb(p);
    kb.run();
    REQUIRE(kb.confluent());
    REQUIRE(kb.size() == 243);
    auto nf = (knuth_bendix::normal_forms(kb).min(1).max(3)
               | to_strings(p.alphabet()));
    REQUIRE((nf | to_vector())
            == std::vector<std::string>({"a", "b", "aa", "ab", "ba", "bb"}));
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix", "047", "code coverage", "[quick]") {
    KnuthBendix kb1;
    KnuthBendix kb2(kb1);
    REQUIRE(kb1.size() == 0);

    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "aaa", "a");
    KnuthBendix kb3(p);
    REQUIRE(kb3.presentation().rules.size() / 2 == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix", "048", "small overlap 1", "[quick]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("BCA");
    presentation::add_rule(p, "AABC", "ACBA");

    KnuthBendix kb(p);
    REQUIRE(kb.confluent());
    REQUIRE(kb.normal_form("CBACBAABCAABCACBACBA") == "CBACBACBAACBAACBACBA");
    REQUIRE(kb.equal_to("CBAABCABCAABCAABCABC", "CBACBAABCAABCACBACBA"));
    REQUIRE(kb.equal_to("CBAABCABCAABCAABCABC", "CBACBAABCAABCACBACBA"));
    REQUIRE(kb.equal_to("AABCAABCCACAACBBCBCCACBBAABCBA",
                        "ACBAACBACACAACBBCBCCACBBACBABA"));
    REQUIRE(kb.equal_to("CACCBABACCBABACCAAAABCAABCBCAA",
                        "CACCBABACCBABACCAAACBAACBABCAA"));
    REQUIRE(kb.equal_to("CAAACAABCCBABCCBCCBCACABACBBAC",
                        "CAAACACBACBABCCBCCBCACABACBBAC"));
    REQUIRE(kb.equal_to("BABCACBACBCCCCCAACCAAABAABCBCC",
                        "BABCACBACBCCCCCAACCAAABACBABCC"));
    REQUIRE(kb.size() == POSITIVE_INFINITY);
  }

  // Symmetric group S_9
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "049",
                          "(from kbmag/standalone/kb_data/s9)",
                          "[quick][knuth-bendix][kbmag][shortlex]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abcdefgh");
    p.contains_empty_word(true);

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
    presentation::add_rule(p, "ec", "ce");
    presentation::add_rule(p, "fc", "cf");
    presentation::add_rule(p, "gc", "cg");
    presentation::add_rule(p, "hc", "ch");
    presentation::add_rule(p, "ede", "ded");
    presentation::add_rule(p, "fd", "df");
    presentation::add_rule(p, "gd", "dg");
    presentation::add_rule(p, "hd", "dh");
    presentation::add_rule(p, "fef", "efe");
    presentation::add_rule(p, "ge", "eg");
    presentation::add_rule(p, "he", "eh");
    presentation::add_rule(p, "gfg", "fgf");
    presentation::add_rule(p, "hf", "fh");
    presentation::add_rule(p, "hgh", "ghg");

    KnuthBendix kb(p);
    REQUIRE(!kb.confluent());
    kb.run();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 57);
    REQUIRE(kb.size() == 362880);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "019",
                          "C(4) monoid",
                          "[quick][knuthbendix]") {
    Presentation<std::string> p;
    p.alphabet("abcde");
    presentation::add_rule(p, "bceac", "aeebbc");
    presentation::add_rule(p, "aeebbc", "dabcd");

    KnuthBendix kb(p);
    kb.run();
    REQUIRE(kb.confluent());
  }
}  // namespace libsemigroups
