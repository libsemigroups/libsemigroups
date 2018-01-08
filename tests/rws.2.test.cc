// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 James D. Mitchell
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

// TODO The other examples from Sims book (Chapters 5 and 6) which use
// reduction orderings different from shortlex

#include <utility>

#include "catch.hpp"

#include "../src/cong.h"
#include "../src/rws.h"

#define RWS_REPORT false

using namespace libsemigroups;
size_t UNBOUNDED2 = RWS::UNBOUNDED;

// Fibonacci group F(2,5) - monoid presentation - has order 12 (group elements
// + empty word)
TEST_CASE("RWS 51: (from kbmag/standalone/kb_data/f25monoid)",
          "[quick][rws][kbmag][shortlex][51]") {
  RWS rws("abcde");
  rws.add_rule("ab", "c");
  rws.add_rule("bc", "d");
  rws.add_rule("cd", "e");
  rws.add_rule("de", "a");
  rws.add_rule("ea", "b");
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.confluent());

  rws.knuth_bendix();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 24);

  REQUIRE(rws.rule("ab", "c"));
  REQUIRE(rws.rule("bc", "d"));
  REQUIRE(rws.rule("cd", "e"));
  REQUIRE(rws.rule("de", "a"));
  REQUIRE(rws.rule("ea", "b"));
  REQUIRE(rws.rule("cc", "ad"));
  REQUIRE(rws.rule("dd", "be"));
  REQUIRE(rws.rule("ee", "ca"));
  REQUIRE(rws.rule("ec", "bb"));
  REQUIRE(rws.rule("db", "aa"));
  REQUIRE(rws.rule("aac", "be"));
  REQUIRE(rws.rule("bd", "aa"));
  REQUIRE(rws.rule("bbe", "aad"));
  // Here we get different rules than KBMAG.
  REQUIRE(rws.test_equals("aaa", "e"));
  REQUIRE(rws.rule("eb", "be"));
  REQUIRE(rws.rule("ba", "c"));
  REQUIRE(rws.rule("da", "ad"));
  REQUIRE(rws.rule("ca", "ac"));
  REQUIRE(rws.rule("ce", "bb"));
  REQUIRE(rws.rule("cb", "d"));
  REQUIRE(rws.rule("ed", "a"));
  REQUIRE(rws.rule("dc", "e"));
  REQUIRE(rws.rule("ae", "b"));
  REQUIRE(rws.test_equals("bbb", "a"));
}

// trivial group - BHN presentation
TEST_CASE("RWS 52: (from kbmag/standalone/kb_data/degen4a)",
          "[quick][rws][kbmag][shortlex][52]") {
  RWS rws("aAbBcC");
  rws.add_rule("Aba", "bb");
  rws.add_rule("Bcb", "cc");
  rws.add_rule("Cac", "aa");
  rws.set_report(RWS_REPORT);

  REQUIRE(rws.confluent());

  rws.knuth_bendix();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 3);

  REQUIRE(rws.rule("Aba", "bb"));
  REQUIRE(rws.rule("Bcb", "cc"));
  REQUIRE(rws.rule("Cac", "aa"));
}

// Torus group
TEST_CASE("RWS 53: (from kbmag/standalone/kb_data/torus)",
          "[quick][rws][kbmag][shortlex][53]") {
  RWS rws("aAcCbBdD");
  rws.add_rule("ABab", "DCdc");
  rws.set_report(RWS_REPORT);

  REQUIRE(rws.confluent());

  rws.knuth_bendix();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 1);

  REQUIRE(rws.rule("DCdc", "ABab"));
}

// monoid presentation of F(2,7) - should produce a monoid of length 30 which is
// the same as the group, together with the empty word. This is a very difficult
// calculation indeed, however.
//
// KBMAG does not terminate when SHORTLEX order is used.
/*TEST_CASE("RWS 54: (from kbmag/standalone/kb_data/f27monoid)",
          "[fails][rws][kbmag][recursive][54]") {
  RWS rws(new RECURSIVE(), "abcdefg");
  rws.add_rule("ab", "c");
  rws.add_rule("bc", "d");
  rws.add_rule("cd", "e");
  rws.add_rule("de", "f");
  rws.add_rule("ef", "g");
  rws.add_rule("fg", "a");
  rws.add_rule("ga", "b");
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.confluent());

  rws.knuth_bendix();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 32767);
}*/

//  3-fold cover of A_6
TEST_CASE("RWS 55: (from kbmag/standalone/kb_data/3a6)",
          "[quick][rws][kbmag][shortlex][55]") {
  RWS rws("abAB");
  rws.add_rule("aaa", "");
  rws.add_rule("bbb", "");
  rws.add_rule("abababab", "");
  rws.add_rule("aBaBaBaBaB", "");
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.confluent());

  rws.knuth_bendix();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 11);

  REQUIRE(rws.rule("aaa", ""));
  REQUIRE(rws.rule("bbb", ""));
  REQUIRE(rws.rule("BaBaBaBaB", "aa"));
  REQUIRE(rws.rule("bababa", "aabb"));
  REQUIRE(rws.rule("ababab", "bbaa"));
  REQUIRE(rws.rule("aabbaa", "babab"));
  REQUIRE(rws.rule("bbaabb", "ababa"));
  REQUIRE(rws.rule("bababbabab", "aabbabbaa"));
  REQUIRE(rws.rule("ababaababa", "bbaabaabb"));
  REQUIRE(rws.rule("bababbabaababa", "aabbabbaabaabb"));
  REQUIRE(rws.rule("bbaabaabbabbaa", "ababaababbabab"));
}

//  Free group on 2 generators
TEST_CASE("RWS 56: (from kbmag/standalone/kb_data/f2)",
          "[quick][rws][kbmag][shortlex][56]") {
  RWS rws("aAbB");
  rws.set_report(RWS_REPORT);

  REQUIRE(rws.confluent());

  rws.knuth_bendix();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 0);
}

//  A nonhopfian group
/*TEST_CASE("RWS 57: (from kbmag/standalone/kb_data/nonhopf)",
          "[quick][rws][kbmag][recursive][57]") {
  RWS rws(new RECURSIVE(), "aAbB");
  rws.add_rule("Baab", "aaa");
  rws.set_report(RWS_REPORT);

  REQUIRE(rws.confluent());

  rws.knuth_bendix();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 1);

  REQUIRE(rws.rule("Baab", "aaa"));
}*/

// Symmetric group S_16
// knuth_bendix/2 fail to terminate
TEST_CASE("RWS 58: (from kbmag/standalone/kb_data/s16)",
          "[extreme][rws][kbmag][shortlex][58]") {
  RWS rws("abcdefghijklmno");
  rws.add_rule("bab", "aba");
  rws.add_rule("ca", "ac");
  rws.add_rule("da", "ad");
  rws.add_rule("ea", "ae");
  rws.add_rule("fa", "af");
  rws.add_rule("ga", "ag");
  rws.add_rule("ha", "ah");
  rws.add_rule("ia", "ai");
  rws.add_rule("ja", "aj");
  rws.add_rule("ka", "ak");
  rws.add_rule("la", "al");
  rws.add_rule("ma", "am");
  rws.add_rule("na", "an");
  rws.add_rule("oa", "ao");
  rws.add_rule("cbc", "bcb");
  rws.add_rule("db", "bd");
  rws.add_rule("eb", "be");
  rws.add_rule("fb", "bf");
  rws.add_rule("gb", "bg");
  rws.add_rule("hb", "bh");
  rws.add_rule("ib", "bi");
  rws.add_rule("jb", "bj");
  rws.add_rule("kb", "bk");
  rws.add_rule("lb", "bl");
  rws.add_rule("mb", "bm");
  rws.add_rule("nb", "bn");
  rws.add_rule("ob", "bo");
  rws.add_rule("dcd", "cdc");
  rws.add_rule("ec", "ce");
  rws.add_rule("fc", "cf");
  rws.add_rule("gc", "cg");
  rws.add_rule("hc", "ch");
  rws.add_rule("ic", "ci");
  rws.add_rule("jc", "cj");
  rws.add_rule("kc", "ck");
  rws.add_rule("lc", "cl");
  rws.add_rule("mc", "cm");
  rws.add_rule("nc", "cn");
  rws.add_rule("oc", "co");
  rws.add_rule("ede", "ded");
  rws.add_rule("fd", "df");
  rws.add_rule("gd", "dg");
  rws.add_rule("hd", "dh");
  rws.add_rule("id", "di");
  rws.add_rule("jd", "dj");
  rws.add_rule("kd", "dk");
  rws.add_rule("ld", "dl");
  rws.add_rule("md", "dm");
  rws.add_rule("nd", "dn");
  rws.add_rule("od", "do");
  rws.add_rule("fef", "efe");
  rws.add_rule("ge", "eg");
  rws.add_rule("he", "eh");
  rws.add_rule("ie", "ei");
  rws.add_rule("je", "ej");
  rws.add_rule("ke", "ek");
  rws.add_rule("le", "el");
  rws.add_rule("me", "em");
  rws.add_rule("ne", "en");
  rws.add_rule("oe", "eo");
  rws.add_rule("gfg", "fgf");
  rws.add_rule("hf", "fh");
  rws.add_rule("if", "fi");
  rws.add_rule("jf", "fj");
  rws.add_rule("kf", "fk");
  rws.add_rule("lf", "fl");
  rws.add_rule("mf", "fm");
  rws.add_rule("nf", "fn");
  rws.add_rule("of", "fo");
  rws.add_rule("hgh", "ghg");
  rws.add_rule("ig", "gi");
  rws.add_rule("jg", "gj");
  rws.add_rule("kg", "gk");
  rws.add_rule("lg", "gl");
  rws.add_rule("mg", "gm");
  rws.add_rule("ng", "gn");
  rws.add_rule("og", "go");
  rws.add_rule("ihi", "hih");
  rws.add_rule("jh", "hj");
  rws.add_rule("kh", "hk");
  rws.add_rule("lh", "hl");
  rws.add_rule("mh", "hm");
  rws.add_rule("nh", "hn");
  rws.add_rule("oh", "ho");
  rws.add_rule("jij", "iji");
  rws.add_rule("ki", "ik");
  rws.add_rule("li", "il");
  rws.add_rule("mi", "im");
  rws.add_rule("ni", "in");
  rws.add_rule("oi", "io");
  rws.add_rule("kjk", "jkj");
  rws.add_rule("lj", "jl");
  rws.add_rule("mj", "jm");
  rws.add_rule("nj", "jn");
  rws.add_rule("oj", "jo");
  rws.add_rule("lkl", "klk");
  rws.add_rule("mk", "km");
  rws.add_rule("nk", "kn");
  rws.add_rule("ok", "ko");
  rws.add_rule("mlm", "lml");
  rws.add_rule("nl", "ln");
  rws.add_rule("ol", "lo");
  rws.add_rule("nmn", "mnm");
  rws.add_rule("om", "mo");
  rws.add_rule("ono", "non");
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.confluent());

  rws.knuth_bendix_by_overlap_length();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 32767);
}

// Presentation of group A_4 regarded as monoid presentation - gives infinite
// monoid.
TEST_CASE("RWS 59: (from kbmag/standalone/kb_data/a4monoid)",
          "[quick][rws][kbmag][shortlex][59]") {
  RWS rws("abB");
  rws.add_rule("bb", "B");
  rws.add_rule("BaB", "aba");
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.confluent());

  rws.knuth_bendix();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 6);

  REQUIRE(rws.rule("bb", "B"));
  REQUIRE(rws.rule("BaB", "aba"));
  REQUIRE(rws.rule("Bb", "bB"));
  REQUIRE(rws.rule("Baaba", "abaaB"));
  REQUIRE(rws.rule("BabB", "abab"));
  REQUIRE(rws.rule("Bababa", "ababaB"));
}

// fairly clearly the trivial group
TEST_CASE("RWS 60: (from kbmag/standalone/kb_data/degen3)",
          "[quick][rws][kbmag][shortlex][60]") {
  RWS rws("aAbB");
  rws.add_rule("ab", "");
  rws.add_rule("abb", "");
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.confluent());

  rws.knuth_bendix();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 2);

  REQUIRE(rws.rule("b", ""));
  REQUIRE(rws.rule("a", ""));
}

// Symmetric group S_9
// knuth_bendix/2 fail to temrinate
TEST_CASE("RWS 61: (from kbmag/standalone/kb_data/s9)",
          "[extreme][rws][kbmag][shortlex][61]") {
  RWS rws("abcdefgh");
  rws.add_rule("bab", "aba");
  rws.add_rule("ca", "ac");
  rws.add_rule("da", "ad");
  rws.add_rule("ea", "ae");
  rws.add_rule("fa", "af");
  rws.add_rule("ga", "ag");
  rws.add_rule("ha", "ah");
  rws.add_rule("cbc", "bcb");
  rws.add_rule("db", "bd");
  rws.add_rule("eb", "be");
  rws.add_rule("fb", "bf");
  rws.add_rule("gb", "bg");
  rws.add_rule("hb", "bh");
  rws.add_rule("dcd", "cdc");
  rws.add_rule("ec", "ce");
  rws.add_rule("fc", "cf");
  rws.add_rule("gc", "cg");
  rws.add_rule("hc", "ch");
  rws.add_rule("ede", "ded");
  rws.add_rule("fd", "df");
  rws.add_rule("gd", "dg");
  rws.add_rule("hd", "dh");
  rws.add_rule("fef", "efe");
  rws.add_rule("ge", "eg");
  rws.add_rule("he", "eh");
  rws.add_rule("gfg", "fgf");
  rws.add_rule("hf", "fh");
  rws.add_rule("hgh", "ghg");
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.confluent());

  rws.knuth_bendix_by_overlap_length();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 32767);
}

// infinite cyclic group
TEST_CASE("RWS 62: (from kbmag/standalone/kb_data/ab1)",
          "[quick][rws][kbmag][shortlex][62]") {
  RWS rws("aA");
  rws.set_report(RWS_REPORT);

  REQUIRE(rws.confluent());

  rws.knuth_bendix();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 0);
}

// A generator, but trivial.
TEST_CASE("RWS 63: (from kbmag/standalone/kb_data/degen2)",
          "[quick][rws][kbmag][shortlex][63]") {
  RWS rws("aA");
  rws.add_rule("a", "");
  rws.set_report(RWS_REPORT);

  REQUIRE(rws.confluent());

  rws.knuth_bendix();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 1);

  REQUIRE(rws.rule("a", ""));
}

// Fibonacci group F(2,5)
TEST_CASE("RWS 64: (from kbmag/standalone/kb_data/f25)",
          "[quick][rws][kbmag][shortlex][64]") {
  RWS rws("aAbBcCdDyY");
  rws.add_rule("ab", "c");
  rws.add_rule("bc", "d");
  rws.add_rule("cd", "y");
  rws.add_rule("dy", "a");
  rws.add_rule("ya", "b");
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.confluent());

  rws.knuth_bendix();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 24);

  REQUIRE(rws.rule("ab", "c"));
  REQUIRE(rws.rule("bc", "d"));
  REQUIRE(rws.rule("cd", "y"));
  REQUIRE(rws.rule("dy", "a"));
  REQUIRE(rws.rule("ya", "b"));
  REQUIRE(rws.rule("cc", "ad"));
  REQUIRE(rws.rule("dd", "by"));
  REQUIRE(rws.test_equals("yy", "ac"));
  REQUIRE(rws.rule("yc", "bb"));
  REQUIRE(rws.rule("db", "aa"));
  REQUIRE(rws.rule("aac", "by"));
  REQUIRE(rws.rule("bd", "aa"));
  REQUIRE(rws.rule("bby", "aad"));
  REQUIRE(rws.test_equals("aaa", "y"));
  REQUIRE(rws.rule("yb", "by"));
  REQUIRE(rws.rule("ba", "c"));
  REQUIRE(rws.rule("da", "ad"));
  REQUIRE(rws.rule("ca", "ac"));
  REQUIRE(rws.rule("cy", "bb"));
  REQUIRE(rws.rule("cb", "d"));
  REQUIRE(rws.rule("yd", "a"));
  REQUIRE(rws.rule("dc", "y"));
  REQUIRE(rws.rule("ay", "b"));
  REQUIRE(rws.test_equals("bbb", "a"));
}

// Second of BHN's series of increasingly complicated presentations of 1. Works
// quickest with large value of tidyint
// knuth_bendix/2 fail to temrinate
TEST_CASE("RWS 65: (from kbmag/standalone/kb_data/degen4b)",
          "[extreme][rws][kbmag][shortlex][65]") {
  RWS rws("aAbBcC");
  rws.add_rule("bbABaBcbCCAbaBBccBCbccBCb", "");
  rws.add_rule("ccBCbCacAABcbCCaaCAcaaCAc", "");
  rws.add_rule("aaCAcAbaBBCacAAbbABabbABa", "");
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.confluent());

  rws.knuth_bendix_by_overlap_length();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 32766);
}

// Free nilpotent group of rank 2 and class 2
/*TEST_CASE("RWS 66: (from kbmag/standalone/kb_data/nilp2)",
          "[quick][rws][kbmag][recursive][66]") {
  RWS rws(new RECURSIVE(), "cCbBaA");
  rws.add_rule("ba", "abc");
  rws.add_rule("ca", "ac");
  rws.add_rule("cb", "bc");
  rws.set_report(RWS_REPORT);

  REQUIRE(rws.confluent());

  rws.knuth_bendix();
  REQUIRE(rws.confluent());

  REQUIRE(rws.nr_rules() == 3);
  // FIXME KBMAG says this terminates with 32758 rules, maybe that was with
  // shortlex order?
}*/

// knuth_bendix/2 don't finish
TEST_CASE("RWS 67: (from kbmag/standalone/kb_data/funny3)",
          "[extreme][rws][kbmag][shortlex][67]") {
  RWS rws("aAbBcC");
  rws.add_rule("aaa", "");
  rws.add_rule("bbb", "");
  rws.add_rule("ccc", "");
  rws.add_rule("ABa", "BaB");
  rws.add_rule("bcB", "cBc");
  rws.add_rule("caC", "aCa");
  rws.add_rule("abcABCabcABCabcABC", "");
  rws.add_rule("BcabCABcabCABcabCA", "");
  rws.add_rule("cbACBacbACBacbACBa", "");
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.confluent());

  rws.knuth_bendix_by_overlap_length();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 32767);
}

// Two generator presentation of Fibonacci group F(2,7) - order 29. Large
// value of tidyint works better.
// knuth_bendix/2 don't finish
TEST_CASE("RWS 68: (from kbmag/standalone/kb_data/f27_2gen)",
          "[extreme][rws][kbmag][shortlex][68]") {
  RWS rws("aAbB");
  rws.add_rule("bababbababbabbababbab", "a");
  rws.add_rule("abbabbababbaba", "b");
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.confluent());

  rws.knuth_bendix_by_overlap_length();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 32763);
}

// Mathieu group M_11
// knuth_bendix/2 don't finish
TEST_CASE("RWS 69: (from kbmag/standalone/kb_data/m11)",
          "[extreme][rws][kbmag][shortlex][69]") {
  RWS rws("abB");
  rws.add_rule("BB", "bb");
  rws.add_rule("BaBaBaBaBaB", "abababababa");
  rws.add_rule("bbabbabba", "abbabbabb");
  rws.add_rule("aBaBababaBabaBBaBab", "");
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.confluent());

  rws.knuth_bendix();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 32761);
}

// Weyl group E8 (all gens involutory).
// knuth_bendix/2 don't finish
TEST_CASE("RWS 70: (from kbmag/standalone/kb_data/e8)",
          "[extreme][rws][kbmag][shortlex][70]") {
  RWS rws("abcdefgh");
  rws.add_rule("bab", "aba");
  rws.add_rule("ca", "ac");
  rws.add_rule("da", "ad");
  rws.add_rule("ea", "ae");
  rws.add_rule("fa", "af");
  rws.add_rule("ga", "ag");
  rws.add_rule("ha", "ah");
  rws.add_rule("cbc", "bcb");
  rws.add_rule("db", "bd");
  rws.add_rule("eb", "be");
  rws.add_rule("fb", "bf");
  rws.add_rule("gb", "bg");
  rws.add_rule("hb", "bh");
  rws.add_rule("dcd", "cdc");
  rws.add_rule("ece", "cec");
  rws.add_rule("fc", "cf");
  rws.add_rule("gc", "cg");
  rws.add_rule("hc", "ch");
  rws.add_rule("ed", "de");
  rws.add_rule("fd", "df");
  rws.add_rule("gd", "dg");
  rws.add_rule("hd", "dh");
  rws.add_rule("fef", "efe");
  rws.add_rule("ge", "eg");
  rws.add_rule("he", "eh");
  rws.add_rule("gfg", "fgf");
  rws.add_rule("hf", "fh");
  rws.add_rule("hgh", "ghg");
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.confluent());

  rws.knuth_bendix_by_overlap_length();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 32767);
}

// Von Dyck (2,3,7) group - infinite hyperbolic - small tidyint works better
// knuth_bendix/knuth_bendix_by_overlap_length do not terminate
TEST_CASE("RWS 71: (from kbmag/standalone/kb_data/237)",
          "[extreme][rws][kbmag][shortlex][71]") {
  RWS rws("aAbBc");
  rws.add_rule("aaaa", "AAA");
  rws.add_rule("bb", "B");
  rws.add_rule("BA", "c");
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.confluent());

  rws.knuth_bendix();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 42);
  // KBMAG stops with 32767 rules and is not confluent
  std::cout << rws;
  REQUIRE(rws.rules() == std::vector<std::pair<std::string, std::string>>());
}

// Cyclic group of order 2.
TEST_CASE("RWS 72: (from kbmag/standalone/kb_data/c2)",
          "[quick][rws][kbmag][shortlex][72]") {
  RWS rws("a");
  rws.add_rule("aa", "");
  rws.set_report(RWS_REPORT);

  REQUIRE(rws.confluent());

  rws.knuth_bendix();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 1);

  REQUIRE(rws.rule("aa", ""));
}

/*TEST_CASE("RWS 73: (from kbmag/standalone/kb_data/freenilpc3)",
          "[quick][rws][kbmag][recursive][73]") {
  RWS rws(new RECURSIVE(), "yYdDcCbBaA");
  rws.add_rule("BAba", "c");
  rws.add_rule("CAca", "d");
  rws.add_rule("CBcb", "y");
  rws.add_rule("da", "ad");
  rws.add_rule("ya", "ay");
  rws.add_rule("db", "bd");
  rws.add_rule("yb", "by");
  rws.set_report(RWS_REPORT);

  REQUIRE(rws.confluent());

  rws.knuth_bendix();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 7);

  REQUIRE(rws.rule("BAba", "c"));
  REQUIRE(rws.rule("CAca", "d"));
  REQUIRE(rws.rule("CBcb", "y"));
  REQUIRE(rws.rule("da", "ad"));
  REQUIRE(rws.rule("ya", "ay"));
  REQUIRE(rws.rule("db", "bd"));
  REQUIRE(rws.rule("yb", "by"));
}*/

// The group is S_4, and the subgroup H of order 4. There are 30 reduced words -
// 24 for the group elements, and 6 for the 6 cosets Hg.
TEST_CASE("RWS 74: (from kbmag/standalone/kb_data/cosets)",
          "[quick][rws][kbmag][shortlex][74]") {
  RWS rws("HaAbB");
  rws.add_rule("aaa", "");
  rws.add_rule("bbbb", "");
  rws.add_rule("abab", "");
  rws.add_rule("Hb", "H");
  rws.add_rule("HH", "H");
  rws.add_rule("aH", "H");
  rws.add_rule("bH", "H");
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.confluent());

  rws.knuth_bendix();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 14);

  REQUIRE(rws.rule("aaa", ""));
  REQUIRE(rws.rule("Hb", "H"));
  REQUIRE(rws.rule("HH", "H"));
  REQUIRE(rws.rule("aH", "H"));
  REQUIRE(rws.rule("bH", "H"));
  REQUIRE(rws.rule("bab", "aa"));
  REQUIRE(rws.rule("bbb", "aba"));
  REQUIRE(rws.rule("Hab", "Haa"));
  REQUIRE(rws.rule("abaab", "bbaa"));
  REQUIRE(rws.rule("baaba", "aabb"));
  REQUIRE(rws.rule("Haabb", "Haaba"));
  REQUIRE(rws.rule("bbaabb", "abba"));
  REQUIRE(rws.rule("aabbaa", "baab"));
  REQUIRE(rws.rule("baabba", "abbaab"));
}

TEST_CASE("RWS 75: Example 5.1 in Sims (RWS 09 again)",
          "[quick][rws][fpsemigroup][75]") {
  RWS rws("aAbB");
  rws.set_report(RWS_REPORT);

  rws.add_rule("aA", "");
  rws.add_rule("Aa", "");
  rws.add_rule("bB", "");
  rws.add_rule("Bb", "");
  rws.add_rule("ba", "ab");

  REQUIRE(!rws.confluent());

  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 8);
  REQUIRE(rws.confluent());
}

// knuth_bendix/knuth_bendix_by_overlap_length fail to terminate
TEST_CASE("RWS 76: (RWS 50 again) (from kbmag/standalone/kb_data/verifynilp)",
          "[extreme][rws][kbmag][shortlex][76]") {
  RWS rws("hHgGfFyYdDcCbBaA");
  rws.add_rule("BAba", "c");
  rws.add_rule("CAca", "d");
  rws.add_rule("DAda", "y");
  rws.add_rule("YByb", "f");
  rws.add_rule("FAfa", "g");
  rws.add_rule("ga", "ag");
  rws.add_rule("GBgb", "h");
  rws.add_rule("cb", "bc");
  rws.add_rule("ya", "ay");
  rws.set_report(true);

  REQUIRE(!rws.confluent());

  rws.knuth_bendix_by_overlap_length();
  REQUIRE(rws.confluent());
}

TEST_CASE("RWS 77: (RWS 66 again) (from kbmag/standalone/kb_data/nilp2)",
          "[quick][rws][kbmag][shortlex][77]") {
  RWS rws("cCbBaA");
  rws.add_rule("ba", "abc");
  rws.add_rule("ca", "ac");
  rws.add_rule("cb", "bc");
  rws.set_report(true);

  REQUIRE(!rws.confluent());
  // This fails if clear_stack_interval is set to 50.

  // The following never terminates
  // rws.knuth_bendix_by_overlap_length();
  // REQUIRE(rws.confluent());
  // REQUIRE(rws.nr_rules() == 32758);
}

TEST_CASE("RWS 78: Example 6.4 in Sims", "[quick][rws][fpsemigroup][78]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("aa", "");
  rws.add_rule("bc", "");
  rws.add_rule("bbb", "");
  rws.add_rule("ababababababab", "");
  rws.add_rule("abacabacabacabac", "");

  REQUIRE(rws.nr_rules() == 5);
  REQUIRE(!rws.confluent());

  rws.set_max_rules(10);
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 10);
  REQUIRE(!rws.confluent());

  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 10);
  REQUIRE(!rws.confluent());

  rws.set_max_rules(20);
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 21);
  REQUIRE(!rws.confluent());

  rws.set_max_rules(UNBOUNDED2);
  rws.knuth_bendix();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 40);
}

// Von Dyck (2,3,7) group - infinite hyperbolic
TEST_CASE("RWS 79: RWS 71 again", "[extreme][rws][shortlex][79]") {
  RWS rws("aAbBc");
  rws.add_rule("BA", "c");
  rws.add_rule("Bb", "bB");
  rws.add_rule("bb", "B");
  rws.add_rule("AAAa", "aAAA");
  rws.add_rule("aaaa", "AAA");
  rws.add_rule("BaAAA", "cAAa");
  rws.add_rule("BaaAAA", "cAAaa");
  rws.add_rule("BaAaAAA", "cAAaAa");
  rws.add_rule("BaaaAAA", "cAAaaa");
  rws.add_rule("BaAAaAAA", "cAAaAAa");
  rws.add_rule("BaAaaAAA", "cAAaAaa");
  rws.add_rule("BaaAaAAA", "cAAaaAa");
  rws.add_rule("BaAAaaAAA", "cAAaAAaa");
  rws.add_rule("BaAaAaAAA", "cAAaAaAa");
  rws.add_rule("BaAaaaAAA", "cAAaAaaa");
  rws.add_rule("BaaAAaAAA", "cAAaaAAa");
  rws.add_rule("BaaAaaAAA", "cAAaaAaa");
  rws.add_rule("BaAAaAaAAA", "cAAaAAaAa");
  rws.add_rule("BaAAaaaAAA", "cAAaAAaaa");
  rws.add_rule("BaAaAAaAAA", "cAAaAaAAa");
  rws.add_rule("BaAaAaaAAA", "cAAaAaAaa");
  rws.add_rule("BaAaaAaAAA", "cAAaAaaAa");
  rws.add_rule("BaaAAaaAAA", "cAAaaAAaa");
  rws.add_rule("BaaAaAaAAA", "cAAaaAaAa");
  rws.add_rule("BaAAaAAaAAA", "cAAaAAaAAa");
  rws.add_rule("BaAAaAaaAAA", "cAAaAAaAaa");
  rws.add_rule("BaAAaaAaAAA", "cAAaAAaaAa");
  rws.add_rule("BaAaAAaaAAA", "cAAaAaAAaa");
  rws.add_rule("BaAaAaAaAAA", "cAAaAaAaAa");
  rws.add_rule("BaAaaAAaAAA", "cAAaAaaAAa");
  rws.add_rule("BaaAAaAaAAA", "cAAaaAAaAa");
  rws.add_rule("BaaAaAAaAAA", "cAAaaAaAAa");
  rws.add_rule("BaAAaAAaaAAA", "cAAaAAaAAaa");
  rws.add_rule("BaAAaAaAaAAA", "cAAaAAaAaAa");
  rws.add_rule("BaAAaaAAaAAA", "cAAaAAaaAAa");
  rws.add_rule("BaAaAAaAaAAA", "cAAaAaAAaAa");
  rws.add_rule("BaAaAaAAaAAA", "cAAaAaAaAAa");
  rws.add_rule("BaaAAaAAaAAA", "cAAaaAAaAAa");
  rws.add_rule("BaAAaAAaAaAAA", "cAAaAAaAAaAa");
  rws.add_rule("BaAAaAaAAaAAA", "cAAaAAaAaAAa");
  rws.add_rule("BaAaAAaAAaAAA", "cAAaAaAAaAAa");
  rws.add_rule("BaAAaAAaAAaAAA", "cAAaAAaAAaAAa");
  rws.set_report(true);

  REQUIRE(!rws.confluent());
  rws.set_max_rules(32768);
  rws.knuth_bendix();
  REQUIRE(!rws.confluent());
  REQUIRE(rws.nr_rules() == 42);
}

TEST_CASE(
    "RWS 80: Example 5.4 in Sims (RWS 11 again) (different overlap policy)",
    "[quick][rws][fpsemigroup][80]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("aa", "");
  rws.add_rule("bB", "");
  rws.add_rule("bbb", "");
  rws.add_rule("ababab", "");
  rws.set_overlap_measure(RWS::overlap_measure::AB_BC);

  REQUIRE(!rws.confluent());

  rws.knuth_bendix_by_overlap_length();
  REQUIRE(rws.nr_rules() == 11);
  REQUIRE(rws.confluent());
}

TEST_CASE(
    "RWS 81: Example 5.4 in Sims (RWS 11 again) (different overlap policy)",
    "[quick][rws][fpsemigroup][81]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("aa", "");
  rws.add_rule("bB", "");
  rws.add_rule("bbb", "");
  rws.add_rule("ababab", "");
  rws.set_overlap_measure(RWS::overlap_measure::max_AB_BC);

  REQUIRE(!rws.confluent());

  rws.knuth_bendix_by_overlap_length();
  REQUIRE(rws.nr_rules() == 11);
  REQUIRE(rws.confluent());
}

TEST_CASE("RWS 82: operator<<", "[quick][rws][82]") {
  std::ostringstream os;
  RWS                rws1;
  rws1.add_rule("aa", "");
  rws1.add_rule("bB", "");
  rws1.add_rule("bbb", "");
  rws1.add_rule("ababab", "");
  os << rws1;  // Does not do anything visible
  RWS rws2("cbaB");
  rws2.add_rule("aa", "");
  rws2.add_rule("bB", "");
  rws2.add_rule("bbb", "");
  rws2.add_rule("ababab", "");
  os << rws2;  // Does not do anything visible
}

TEST_CASE("RWS 83: set_confluence_interval", "[quick][rws][83]") {
  std::ostringstream os;
  RWS                rws;
  rws.add_rule("aa", "");
  rws.add_rule("bB", "");
  rws.add_rule("bbb", "");
  rws.add_rule("ababab", "");
  rws.set_check_confluence_interval(-1);
  rws.set_check_confluence_interval(10);
}

TEST_CASE("RWS 84: set_max_overlap", "[quick][rws][84]") {
  std::ostringstream os;
  RWS                rws;
  rws.add_rule("aa", "");
  rws.add_rule("bB", "");
  rws.add_rule("bbb", "");
  rws.add_rule("ababab", "");
  rws.set_max_overlap(10);
  rws.set_max_overlap(-11);
}
