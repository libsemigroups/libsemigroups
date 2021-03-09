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

// This file is the second of six that contains tests for the KnuthBendix
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
#include <string>    // for string
#include <vector>    // for vector

#include "catch.hpp"      // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/constants.hpp"     // for POSITIVE_INFINITY
#include "libsemigroups/knuth-bendix.hpp"  // for KnuthBendix, operator<<
#include "libsemigroups/report.hpp"        // for ReportGuard

namespace libsemigroups {
  struct LibsemigroupsException;

  constexpr bool REPORT = false;
  using rule_type       = fpsemigroup::KnuthBendix::rule_type;

  namespace fpsemigroup {
    // Fibonacci group F(2,5) - monoid presentation - has order 12 (group
    // elements + empty word)
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "021",
                            "(from kbmag/standalone/kb_data/f25monoid)",
                            "[quick][knuth-bendix][kbmag][shortlex]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("abcde");

      kb.add_rule("ab", "c");
      kb.add_rule("bc", "d");
      kb.add_rule("cd", "e");
      kb.add_rule("de", "a");
      kb.add_rule("ea", "b");

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
          kb.active_rules()
          == std::vector<rule_type>(
              {{"ab", "c"},  {"ae", "b"},   {"ba", "c"},   {"bc", "d"},
               {"bd", "aa"}, {"ca", "ac"},  {"cb", "d"},   {"cc", "ad"},
               {"cd", "e"},  {"ce", "bb"},  {"da", "ad"},  {"db", "aa"},
               {"dc", "e"},  {"dd", "be"},  {"de", "a"},   {"ea", "b"},
               {"eb", "be"}, {"ec", "bb"},  {"ed", "a"},   {"ee", "ca"},
               {"aaa", "e"}, {"aac", "be"}, {"bbb", "ed"}, {"bbe", "aad"}}));
      REQUIRE(
          std::vector<std::string>(
              {"a", "b", "c", "d", "e", "aa", "ac", "ad", "bb", "be", "aad"})
          == std::vector<std::string>(kb.cbegin_normal_forms(0, 5),
                                      kb.cend_normal_forms()));
      REQUIRE(kb.size() == 11);
      REQUIRE(std::distance(kb.cbegin_normal_forms(0, POSITIVE_INFINITY),
                            kb.cend_normal_forms())
              == 11);
    }

    // trivial group - BHN presentation
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "022",
                            "(from kbmag/standalone/kb_data/degen4a)",
                            "[quick][knuth-bendix][kbmag][shortlex]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("aAbBcC");
      kb.set_identity("");
      kb.set_inverses("AaBbCc");

      kb.add_rule("Aba", "bb");
      kb.add_rule("Bcb", "cc");
      kb.add_rule("Cac", "aa");

      REQUIRE(!kb.confluent());

      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.number_of_active_rules() == 6);

      REQUIRE(kb.equal_to("Aba", "bb"));
      REQUIRE(kb.equal_to("Bcb", "cc"));
      REQUIRE(kb.equal_to("Cac", "aa"));
      REQUIRE(kb.active_rules()
              == std::vector<rule_type>({{"A", ""},
                                         {"B", ""},
                                         {"C", ""},
                                         {"a", ""},
                                         {"b", ""},
                                         {"c", ""}}));
      REQUIRE(kb.size() == 1);
      REQUIRE(std::vector<std::string>({""})
              == std::vector<std::string>(
                  kb.cbegin_normal_forms(0, POSITIVE_INFINITY),
                  kb.cend_normal_forms()));
    }

    // Torus group
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "023",
                            "(from kbmag/standalone/kb_data/torus)",
                            "[quick][knuth-bendix][kbmag][shortlex]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("aAcCbBdD");
      kb.set_identity("");
      kb.set_inverses("AaCcBbDd");

      kb.add_rule("ABab", "DCdc");

      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.number_of_active_rules() == 16);

      REQUIRE(kb.equal_to("DCdc", "ABab"));
      REQUIRE(kb.active_rules()
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
      REQUIRE(
          std::distance(kb.cbegin_normal_forms(0, 7), kb.cend_normal_forms())
          == 155577);
      REQUIRE(
          std::vector<std::string>(
              {"",   "a",  "A",  "c",  "C",  "b",  "B",  "d",  "D",  "aa", "ac",
               "aC", "ab", "aB", "ad", "aD", "AA", "Ac", "AC", "Ab", "AB", "Ad",
               "AD", "ca", "cA", "cc", "cb", "cB", "cd", "cD", "Ca", "CA", "CC",
               "Cb", "CB", "Cd", "CD", "ba", "bA", "bc", "bC", "bb", "bd", "bD",
               "Ba", "BA", "Bc", "BC", "BB", "Bd", "BD", "da", "dA", "dc", "dC",
               "db", "dB", "dd", "Da", "DA", "Dc", "DC", "Db", "DB", "DD"})
          == std::vector<std::string>(kb.cbegin_normal_forms(0, 3),
                                      kb.cend_normal_forms()));
    }

    //  3-fold cover of A_6
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "024",
                            "(from kbmag/standalone/kb_data/3a6)",
                            "[quick][knuth-bendix][kbmag][shortlex]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("abAB");
      kb.set_identity("");
      kb.set_inverses("ABab");

      kb.add_rule("aaa", "");
      kb.add_rule("bbb", "");
      kb.add_rule("abababab", "");
      kb.add_rule("aBaBaBaBaB", "");

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
      REQUIRE(std::distance(kb.cbegin_normal_forms(0, POSITIVE_INFINITY),
                            kb.cend_normal_forms())
              == 1080);
      REQUIRE(std::vector<std::string>({"",
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
                                        "BA"})
              == std::vector<std::string>(kb.cbegin_normal_forms(0, 3),
                                          kb.cend_normal_forms()));
    }

    //  Free group on 2 generators
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "025",
                            "(from kbmag/standalone/kb_data/f2)",
                            "[quick][knuth-bendix][kbmag][shortlex]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("aAbB");
      kb.set_identity("");
      kb.set_inverses("AaBb");

      REQUIRE(kb.confluent());
      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.number_of_active_rules() == 4);
      REQUIRE(kb.size() == POSITIVE_INFINITY);
      REQUIRE(std::vector<std::string>({"",
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
                                        "BB"})
              == std::vector<std::string>(kb.cbegin_normal_forms(0, 3),
                                          kb.cend_normal_forms()));
      REQUIRE(
          std::distance(kb.cbegin_normal_forms(0, 5), kb.cend_normal_forms())
          == 161);
    }

    // Symmetric group S_16
    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "026",
        "(from kbmag/standalone/kb_data/s16)",
        "[quick][knuth-bendix][kbmag][shortlex][no-valgrind]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("abcdefghijklmno");
      kb.set_identity("");
      kb.set_inverses("abcdefghijklmno");

      kb.add_rule("bab", "aba");
      kb.add_rule("ca", "ac");
      kb.add_rule("da", "ad");
      kb.add_rule("ea", "ae");
      kb.add_rule("fa", "af");
      kb.add_rule("ga", "ag");
      kb.add_rule("ha", "ah");
      kb.add_rule("ia", "ai");
      kb.add_rule("ja", "aj");
      kb.add_rule("ka", "ak");
      kb.add_rule("la", "al");
      kb.add_rule("ma", "am");
      kb.add_rule("na", "an");
      kb.add_rule("oa", "ao");
      kb.add_rule("cbc", "bcb");
      kb.add_rule("db", "bd");
      kb.add_rule("eb", "be");
      kb.add_rule("fb", "bf");
      kb.add_rule("gb", "bg");
      kb.add_rule("hb", "bh");
      kb.add_rule("ib", "bi");
      kb.add_rule("jb", "bj");
      kb.add_rule("kb", "bk");
      kb.add_rule("lb", "bl");
      kb.add_rule("mb", "bm");
      kb.add_rule("nb", "bn");
      kb.add_rule("ob", "bo");
      kb.add_rule("dcd", "cdc");
      kb.add_rule("ec", "ce");
      kb.add_rule("fc", "cf");
      kb.add_rule("gc", "cg");
      kb.add_rule("hc", "ch");
      kb.add_rule("ic", "ci");
      kb.add_rule("jc", "cj");
      kb.add_rule("kc", "ck");
      kb.add_rule("lc", "cl");
      kb.add_rule("mc", "cm");
      kb.add_rule("nc", "cn");
      kb.add_rule("oc", "co");
      kb.add_rule("ede", "ded");
      kb.add_rule("fd", "df");
      kb.add_rule("gd", "dg");
      kb.add_rule("hd", "dh");
      kb.add_rule("id", "di");
      kb.add_rule("jd", "dj");
      kb.add_rule("kd", "dk");
      kb.add_rule("ld", "dl");
      kb.add_rule("md", "dm");
      kb.add_rule("nd", "dn");
      kb.add_rule("od", "do");
      kb.add_rule("fef", "efe");
      kb.add_rule("ge", "eg");
      kb.add_rule("he", "eh");
      kb.add_rule("ie", "ei");
      kb.add_rule("je", "ej");
      kb.add_rule("ke", "ek");
      kb.add_rule("le", "el");
      kb.add_rule("me", "em");
      kb.add_rule("ne", "en");
      kb.add_rule("oe", "eo");
      kb.add_rule("gfg", "fgf");
      kb.add_rule("hf", "fh");
      kb.add_rule("if", "fi");
      kb.add_rule("jf", "fj");
      kb.add_rule("kf", "fk");
      kb.add_rule("lf", "fl");
      kb.add_rule("mf", "fm");
      kb.add_rule("nf", "fn");
      kb.add_rule("of", "fo");
      kb.add_rule("hgh", "ghg");
      kb.add_rule("ig", "gi");
      kb.add_rule("jg", "gj");
      kb.add_rule("kg", "gk");
      kb.add_rule("lg", "gl");
      kb.add_rule("mg", "gm");
      kb.add_rule("ng", "gn");
      kb.add_rule("og", "go");
      kb.add_rule("ihi", "hih");
      kb.add_rule("jh", "hj");
      kb.add_rule("kh", "hk");
      kb.add_rule("lh", "hl");
      kb.add_rule("mh", "hm");
      kb.add_rule("nh", "hn");
      kb.add_rule("oh", "ho");
      kb.add_rule("jij", "iji");
      kb.add_rule("ki", "ik");
      kb.add_rule("li", "il");
      kb.add_rule("mi", "im");
      kb.add_rule("ni", "in");
      kb.add_rule("oi", "io");
      kb.add_rule("kjk", "jkj");
      kb.add_rule("lj", "jl");
      kb.add_rule("mj", "jm");
      kb.add_rule("nj", "jn");
      kb.add_rule("oj", "jo");
      kb.add_rule("lkl", "klk");
      kb.add_rule("mk", "km");
      kb.add_rule("nk", "kn");
      kb.add_rule("ok", "ko");
      kb.add_rule("mlm", "lml");
      kb.add_rule("nl", "ln");
      kb.add_rule("ol", "lo");
      kb.add_rule("nmn", "mnm");
      kb.add_rule("om", "mo");
      kb.add_rule("ono", "non");

      REQUIRE(!kb.confluent());

      // kb.knuth_bendix_by_overlap_length();
      kb.run();  // faster
      REQUIRE(kb.confluent());
      REQUIRE(kb.number_of_active_rules() == 211);  // verified with KBMAG
      REQUIRE(kb.gilman_digraph().number_of_nodes() == 121);
      REQUIRE(kb.gilman_digraph().number_of_edges() == 680);
      // verified with KBMAG
      REQUIRE(
          std::distance(kb.cbegin_normal_forms(0, 7), kb.cend_normal_forms())
          == 49436);
      REQUIRE(kb.number_of_normal_forms(0, 7) == 49436);

      // verified with KBMAG
      REQUIRE(kb.number_of_normal_forms(0, 11) == 2554607);
      REQUIRE(kb.size() == 20922789888000);
    }

    // Presentation of group A_4 regarded as monoid presentation - gives
    // infinite monoid.
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "027",
                            "(from kbmag/standalone/kb_data/a4monoid)",
                            "[quick][knuth-bendix][kbmag][shortlex]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("abB");

      kb.add_rule("bb", "B");
      kb.add_rule("BaB", "aba");

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
      REQUIRE(kb.active_rules()
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
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("aAbB");
      kb.add_rule("ab", "");
      kb.add_rule("abb", "");

      REQUIRE(kb.active_rules()
              == std::vector<rule_type>({{"a", ""}, {"b", ""}}));
      REQUIRE(kb.number_of_active_rules() == 2);
      REQUIRE(kb.confluent());

      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.number_of_active_rules() == 2);

      REQUIRE(kb.equal_to("b", ""));
      REQUIRE(kb.equal_to("a", ""));
      REQUIRE(kb.active_rules()
              == std::vector<rule_type>({{"a", ""}, {"b", ""}}));
    }

    // infinite cyclic group
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "029",
                            "(from kbmag/standalone/kb_data/ab1)",
                            "[quick][knuth-bendix][kbmag][shortlex]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("aA");
      kb.set_identity("");
      kb.set_inverses("Aa");

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
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("aA");
      kb.add_rule("a", "");

      REQUIRE(kb.confluent());

      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.number_of_active_rules() == 1);

      REQUIRE(kb.equal_to("a", ""));
      REQUIRE(kb.active_rules() == std::vector<rule_type>({{"a", ""}}));
    }

    // Fibonacci group F(2,5)
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "031",
                            "(from kbmag/standalone/kb_data/f25)",
                            "[quick][knuth-bendix][kbmag][shortlex]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("aAbBcCdDyY");

      kb.add_rule("ab", "c");
      kb.add_rule("bc", "d");
      kb.add_rule("cd", "y");
      kb.add_rule("dy", "a");
      kb.add_rule("ya", "b");

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
      REQUIRE(
          kb.active_rules()
          == std::vector<rule_type>(
              {{"ab", "c"},  {"ay", "b"},   {"ba", "c"},   {"bc", "d"},
               {"bd", "aa"}, {"ca", "ac"},  {"cb", "d"},   {"cc", "ad"},
               {"cd", "y"},  {"cy", "bb"},  {"da", "ad"},  {"db", "aa"},
               {"dc", "y"},  {"dd", "by"},  {"dy", "a"},   {"ya", "b"},
               {"yb", "by"}, {"yc", "bb"},  {"yd", "a"},   {"yy", "ca"},
               {"aaa", "y"}, {"aac", "by"}, {"bbb", "yd"}, {"bby", "aad"}}));
    }

    // Von Dyck (2,3,7) group - infinite hyperbolic - small tidyint works better
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "032",
                            "(from kbmag/standalone/kb_data/237)",
                            "[quick][knuth-bendix][kbmag][shortlex]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("aAbBc");
      kb.set_identity("");
      kb.set_inverses("AaBbc");

      kb.add_rule("aaaa", "AAA");
      kb.add_rule("bb", "B");
      kb.add_rule("BA", "c");

      REQUIRE(!kb.confluent());

      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.number_of_active_rules() == 32);
      REQUIRE(kb.active_rules()
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
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("a");
      kb.add_rule("aa", "");

      REQUIRE(kb.confluent());

      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.number_of_active_rules() == 1);

      REQUIRE(kb.active_rules() == std::vector<rule_type>({{"aa", ""}}));
    }

    // The group is S_4, and the subgroup H of order 4. There are 30 reduced
    // words - 24 for the group elements, and 6 for the 6 cosets Hg.
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "034",
                            "(from kbmag/standalone/kb_data/cosets)",
                            "[quick][knuth-bendix][kbmag][shortlex]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("HaAbB");

      kb.add_rule("aaa", "");
      kb.add_rule("bbbb", "");
      kb.add_rule("abab", "");
      kb.add_rule("Hb", "H");
      kb.add_rule("HH", "H");
      kb.add_rule("aH", "H");
      kb.add_rule("bH", "H");

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
      REQUIRE(kb.active_rules()
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
                            "[quick][knuth-bendix][fpsemigroup]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("aAbB");

      kb.add_rule("aA", "");
      kb.add_rule("Aa", "");
      kb.add_rule("bB", "");
      kb.add_rule("Bb", "");
      kb.add_rule("ba", "ab");

      REQUIRE(!kb.confluent());

      kb.run();
      REQUIRE(kb.number_of_active_rules() == 8);
      REQUIRE(kb.confluent());
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "036",
                            "(from kbmag/standalone/kb_data/nilp2)",
                            "[quick][knuth-bendix][kbmag][shortlex]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("cCbBaA");
      kb.set_identity("");
      kb.set_inverses("CcBbAa");

      kb.add_rule("ba", "abc");
      kb.add_rule("ca", "ac");
      kb.add_rule("cb", "bc");

      REQUIRE(!kb.confluent());

      // The following never terminates (requires recursive order?)
      // kb.knuth_bendix_by_overlap_length();
      // REQUIRE(kb.confluent());
      // REQUIRE(kb.number_of_active_rules() == 32758);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "037",
                            "Example 6.4 in Sims",
                            "[quick][knuth-bendix][fpsemigroup][no-valgrind]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("abc");
      kb.add_rule("aa", "");
      kb.add_rule("bc", "");
      kb.add_rule("bbb", "");
      kb.add_rule("ababababababab", "");
      kb.add_rule("abacabacabacabac", "");

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
                            "[no-valgrind][quick][knuth-bendix][shortlex]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("aAbBc");
      kb.set_identity("");
      kb.set_inverses("AaBbc");

      kb.add_rule("BA", "c");
      kb.add_rule("Bb", "bB");
      kb.add_rule("bb", "B");
      kb.add_rule("AAAa", "aAAA");
      kb.add_rule("aaaa", "AAA");
      kb.add_rule("BaAAA", "cAAa");
      kb.add_rule("BaaAAA", "cAAaa");
      kb.add_rule("BaAaAAA", "cAAaAa");
      kb.add_rule("BaaaAAA", "cAAaaa");
      kb.add_rule("BaAAaAAA", "cAAaAAa");
      kb.add_rule("BaAaaAAA", "cAAaAaa");
      kb.add_rule("BaaAaAAA", "cAAaaAa");
      kb.add_rule("BaAAaaAAA", "cAAaAAaa");
      kb.add_rule("BaAaAaAAA", "cAAaAaAa");
      kb.add_rule("BaAaaaAAA", "cAAaAaaa");
      kb.add_rule("BaaAAaAAA", "cAAaaAAa");
      kb.add_rule("BaaAaaAAA", "cAAaaAaa");
      kb.add_rule("BaAAaAaAAA", "cAAaAAaAa");
      kb.add_rule("BaAAaaaAAA", "cAAaAAaaa");
      kb.add_rule("BaAaAAaAAA", "cAAaAaAAa");
      kb.add_rule("BaAaAaaAAA", "cAAaAaAaa");
      kb.add_rule("BaAaaAaAAA", "cAAaAaaAa");
      kb.add_rule("BaaAAaaAAA", "cAAaaAAaa");
      kb.add_rule("BaaAaAaAAA", "cAAaaAaAa");
      kb.add_rule("BaAAaAAaAAA", "cAAaAAaAAa");
      kb.add_rule("BaAAaAaaAAA", "cAAaAAaAaa");
      kb.add_rule("BaAAaaAaAAA", "cAAaAAaaAa");
      kb.add_rule("BaAaAAaaAAA", "cAAaAaAAaa");
      kb.add_rule("BaAaAaAaAAA", "cAAaAaAaAa");
      kb.add_rule("BaAaaAAaAAA", "cAAaAaaAAa");
      kb.add_rule("BaaAAaAaAAA", "cAAaaAAaAa");
      kb.add_rule("BaaAaAAaAAA", "cAAaaAaAAa");
      kb.add_rule("BaAAaAAaaAAA", "cAAaAAaAAaa");
      kb.add_rule("BaAAaAaAaAAA", "cAAaAAaAaAa");
      kb.add_rule("BaAAaaAAaAAA", "cAAaAAaaAAa");
      kb.add_rule("BaAaAAaAaAAA", "cAAaAaAAaAa");
      kb.add_rule("BaAaAaAAaAAA", "cAAaAaAaAAa");
      kb.add_rule("BaaAAaAAaAAA", "cAAaaAAaAAa");
      kb.add_rule("BaAAaAAaAaAAA", "cAAaAAaAAaAa");
      kb.add_rule("BaAAaAaAAaAAA", "cAAaAAaAaAAa");
      kb.add_rule("BaAaAAaAAaAAA", "cAAaAaAAaAAa");
      kb.add_rule("BaAAaAAaAAaAAA", "cAAaAAaAAaAAa");

      REQUIRE(kb.number_of_active_rules() == 9);
      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.number_of_active_rules() == 32);
      REQUIRE(kb.size() == POSITIVE_INFINITY);
      REQUIRE(kb.number_of_normal_forms(4, 5) == 24);
      REQUIRE(std::vector<std::string>(kb.cbegin_normal_forms(4, 5),
                                       kb.cend_normal_forms())
              == std::vector<std::string>(
                  {"aaaB", "aaac", "aaBa", "aacA", "aBaa", "aBac",
                   "acAA", "acAb", "AAAB", "AAbA", "AABa", "AbAA",
                   "AbAb", "ABaa", "ABac", "bAAA", "bAAb", "bAAB",
                   "Baaa", "BaaB", "Baac", "BacA", "cAAb", "cAAB"}));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "039",
                            "Example 5.4 in Sims (KnuthBendix 11 again) "
                            "(different overlap policy)",
                            "[quick][knuth-bendix][fpsemigroup]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("Bab");
      kb.add_rule("aa", "");
      kb.add_rule("bB", "");
      kb.add_rule("bbb", "");
      kb.add_rule("ababab", "");
      kb.overlap_policy(KnuthBendix::options::overlap::AB_BC);

      REQUIRE(!kb.confluent());

      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.number_of_active_rules() == 11);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 12);
      REQUIRE(kb.number_of_normal_forms(4, 5) == 0);
      REQUIRE(
          std::vector<std::string>(kb.cbegin_normal_forms(0, POSITIVE_INFINITY),
                                   kb.cend_normal_forms())
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
                            "Example 5.4 in Sims (KnuthBendix 11 again) "
                            "(different overlap policy)",
                            "[quick][knuth-bendix][fpsemigroup]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("Bab");
      kb.add_rule("aa", "");
      kb.add_rule("bB", "");
      kb.add_rule("bbb", "");
      kb.add_rule("ababab", "");
      kb.overlap_policy(KnuthBendix::options::overlap::MAX_AB_BC);
      // The next line tests that we don't delete the old OverlapMeasure.
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
      KnuthBendix        kb1;
      kb1.set_alphabet("Bab");
      kb1.add_rule("aa", "");
      kb1.add_rule("bB", "");
      kb1.add_rule("bbb", "");
      kb1.add_rule("ababab", "");
      os << kb1;  // Does not do anything visible
      KnuthBendix kb2;
      kb2.set_alphabet("cbaB");
      kb2.add_rule("aa", "");
      kb2.add_rule("bB", "");
      kb2.add_rule("bbb", "");
      kb2.add_rule("ababab", "");
      os << kb2;  // Does not do anything visible
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "042",
                            "confluence_interval",
                            "[quick][knuth-bendix]") {
      KnuthBendix kb;
      kb.set_alphabet("Bab");
      kb.add_rule("aa", "");
      kb.add_rule("bB", "");
      kb.add_rule("bbb", "");
      kb.add_rule("ababab", "");
      kb.check_confluence_interval(LIMIT_MAX);
      kb.check_confluence_interval(10);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "043",
                            "max_overlap",
                            "[quick][knuth-bendix]") {
      KnuthBendix kb;
      kb.set_alphabet("Bab");

      kb.add_rule("aa", "");
      kb.add_rule("bB", "");
      kb.add_rule("bbb", "");
      kb.add_rule("ababab", "");
      kb.max_overlap(10);
      kb.max_overlap(-11);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "044",
        "(fpsemi) (from kbmag/standalone/kb_data/d22) (2 / 3) (finite)",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][kbmag][shortlex]") {
      auto rg = ReportGuard(REPORT);

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
      REQUIRE(kb.number_of_rules() == 18);
      REQUIRE(kb.size() == 22);
      REQUIRE(
          std::vector<std::string>(kb.cbegin_normal_forms(0, POSITIVE_INFINITY),
                                   kb.cend_normal_forms())
          == std::vector<std::string>({"",    "A",   "B",   "C",  "D",  "Y",
                                       "F",   "AB",  "AC",  "AD", "AY", "AF",
                                       "BA",  "BD",  "BY",  "CY", "DB", "ABA",
                                       "ABD", "ABY", "ACY", "ADB"}));
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "045",
        "(fpsemi) (from kbmag/standalone/kb_data/d22) (3 / 3) (finite)",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][kbmag][shortlex]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("aAbBcCdDyYfF");
      kb.set_identity("");
      kb.set_inverses("AaBbCcDdYyFf");

      kb.add_rule("aCAd", "");
      kb.add_rule("bfBY", "");
      kb.add_rule("cyCD", "");
      kb.add_rule("dFDa", "");
      kb.add_rule("ybYA", "");
      kb.add_rule("fCFB", "");
      REQUIRE(!kb.confluent());

      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.confluent());
      REQUIRE(kb.number_of_rules() == 18);
      REQUIRE(kb.size() == 22);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "046",
        "(fpsemi) small example",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][shortlex]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("ab");
      kb.add_rule("aaa", "a");
      kb.add_rule("bbbb", "b");
      kb.add_rule("ababababab", "aa");
      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 243);
      REQUIRE(std::vector<std::string>(kb.cbegin_normal_forms(0, 3),
                                       kb.cend_normal_forms())
              == std::vector<std::string>({"a", "b", "aa", "ab", "ba", "bb"}));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix", "047", "code coverage", "[quick]") {
      KnuthBendix kb1;
      KnuthBendix kb2(kb1);
      REQUIRE(kb1.size() == 0);

      kb1.set_alphabet("ab");
      kb1.add_rule("aaa", "a");
      KnuthBendix kb3(kb1);
      REQUIRE(kb3.number_of_rules() == 1);
      REQUIRE_THROWS_AS(kb3.set_identity("ab"), LibsemigroupsException);
      REQUIRE_NOTHROW(kb3.set_identity("a"));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "048",
                            "small overlap 1",
                            "[quick]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("BCA");
      kb.add_rule("AABC", "ACBA");
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

      // REQUIRE(std::vector<std::string>(cbegin_silo("BCA", 5, 6),
      //                                cend_silo("BCA", 5, 6))
      //         == std::vector<std::string>());
      // REQUIRE(number_of_words(3, 20, 21) == size_t(18446744071562067968ULL));

      // auto lex_normal_form = [&kb](std::string const& w) {
      //   auto ww = kb.normal_form(w);
      //   auto it = std::find_if(cbegin_silo("BCA", ww, 4 * w.size()),
      //                          cend_silo("BCA", ww.size(), 4 * w.size()),
      //                          [&kb, &ww](std::string const& u) {
      //                            return kb.normal_form(u) == ww;
      //                          });
      //   return *it;
      // };
      // kb.run();
      // REQUIRE(kb.finished());
      // REQUIRE(lex_normal_form("BBBBB") == "BBBBB");
      // REQUIRE(kb.normal_form("AABCB") == "ACBAB");
      // REQUIRE(lex_normal_form("AABCB") == "");
      //      std::vector<std::string> result(number_of_words(3, 5, 20));
      //      std::transform(cbegin_
      // TODO(later) The following code spends the majority of its time in
      // FpSemigroupInterface::validate_letter
      //  auto it =
      //  REQUIRE(it != cend_silo("BCA", 0, 80));
      //  REQUIRE(*it ==  "CBACBAABCAABCACBACBA");
    }

    // Symmetric group S_9
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "049",
                            "(from kbmag/standalone/kb_data/s9)",
                            "[quick][knuth-bendix][kbmag][shortlex]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("abcdefgh");
      kb.set_identity("");
      kb.set_inverses("abcdefgh");

      kb.add_rule("bab", "aba");
      kb.add_rule("ca", "ac");
      kb.add_rule("da", "ad");
      kb.add_rule("ea", "ae");
      kb.add_rule("fa", "af");
      kb.add_rule("ga", "ag");
      kb.add_rule("ha", "ah");
      kb.add_rule("cbc", "bcb");
      kb.add_rule("db", "bd");
      kb.add_rule("eb", "be");
      kb.add_rule("fb", "bf");
      kb.add_rule("gb", "bg");
      kb.add_rule("hb", "bh");
      kb.add_rule("dcd", "cdc");
      kb.add_rule("ec", "ce");
      kb.add_rule("fc", "cf");
      kb.add_rule("gc", "cg");
      kb.add_rule("hc", "ch");
      kb.add_rule("ede", "ded");
      kb.add_rule("fd", "df");
      kb.add_rule("gd", "dg");
      kb.add_rule("hd", "dh");
      kb.add_rule("fef", "efe");
      kb.add_rule("ge", "eg");
      kb.add_rule("he", "eh");
      kb.add_rule("gfg", "fgf");
      kb.add_rule("hf", "fh");
      kb.add_rule("hgh", "ghg");

      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.number_of_active_rules() == 57);
      REQUIRE(kb.size() == 362880);
    }
  }  // namespace fpsemigroup
}  // namespace libsemigroups
