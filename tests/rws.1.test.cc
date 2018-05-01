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
size_t UNBOUNDED1 = RWS::UNBOUNDED;

TEST_CASE("RWS 01: for a transformation semigroup of size 4",
          "[quick][rws][fpsemigroup][01]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({1, 0}),
         new Transformation<u_int16_t>(std::vector<u_int16_t>({0, 0}))};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(RWS_REPORT);
  really_delete_cont(gens);
  REQUIRE(S.size() == 4);
  REQUIRE(S.degree() == 2);
  REQUIRE(S.nrrules() == 4);
  std::vector<relation_t> extra({});
  Congruence            cong("twosided", &S, extra);

  RWS rws;
  rws.add_rules(cong.relations());
  rws.add_rules(cong.extra());
  rws.set_report(RWS_REPORT);
  REQUIRE(rws.confluent());
}

TEST_CASE("RWS 02: for a transformation semigroup of size 9",
          "[quick][rws][finite][02]") {
  std::vector<Element*> gens = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
                                new Transformation<u_int16_t>({0, 0, 0, 0, 0})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(RWS_REPORT);
  really_delete_cont(gens);
  REQUIRE(S.size() == 9);
  REQUIRE(S.degree() == 5);
  REQUIRE(S.nrrules() == 3);
  std::vector<relation_t> extra({});
  Congruence            cong("twosided", &S, extra);

  RWS rws;
  rws.add_rules(cong.relations());
  rws.add_rules(cong.extra());

  rws.set_report(RWS_REPORT);
  REQUIRE(rws.confluent());
}

TEST_CASE("RWS 03: for a transformation semigroup of size 88",
          "[quick][rws][finite][03]") {
  std::vector<Element*> gens = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
                                new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(RWS_REPORT);
  really_delete_cont(gens);
  REQUIRE(S.size() == 88);
  REQUIRE(S.degree() == 5);
  REQUIRE(S.nrrules() == 18);
  std::vector<relation_t> extra({});
  Congruence            cong("twosided", &S, extra);

  RWS rws;
  rws.add_rules(cong.relations());
  rws.add_rules(cong.extra());

  rws.set_report(RWS_REPORT);
  REQUIRE(rws.confluent());
}

TEST_CASE("RWS 04: for an infinite confluent fp semigroup 1",
          "[quick][rws][fpsemigroup][04]") {
  std::vector<relation_t> rels = {relation_t({0, 1}, {1, 0}),
                                  relation_t({0, 2}, {2, 0}),
                                  relation_t({0, 0}, {0}),
                                  relation_t({0, 2}, {0}),
                                  relation_t({2, 0}, {0}),
                                  relation_t({1, 1}, {1, 1}),
                                  relation_t({1, 2}, {2, 1}),
                                  relation_t({1, 1, 1}, {1}),
                                  relation_t({1, 2}, {1}),
                                  relation_t({2, 1}, {1})};
  std::vector<relation_t> extra = {{{0}, {1}}};
  Congruence            cong("twosided", 3, rels, extra);

  RWS rws;
  rws.add_rules(cong.relations());
  rws.add_rules(cong.extra());

  rws.set_report(RWS_REPORT);
  REQUIRE(rws.confluent());
}

TEST_CASE("RWS 05: for an infinite confluent fp semigroup 2",
          "[quick][rws][fpsemigroup][05]") {
  std::vector<relation_t> rels = {relation_t({0, 1}, {1, 0}),
                                  relation_t({0, 2}, {2, 0}),
                                  relation_t({0, 0}, {0}),
                                  relation_t({0, 2}, {0}),
                                  relation_t({2, 0}, {0}),
                                  relation_t({1, 1}, {1, 1}),
                                  relation_t({1, 2}, {2, 1}),
                                  relation_t({1, 1, 1}, {1}),
                                  relation_t({1, 2}, {1}),
                                  relation_t({2, 1}, {1})};
  std::vector<relation_t> extra = {{{0}, {1}}};

  RWS rws;
  rws.add_rules(rels);
  rws.add_rules(extra);
  rws.set_report(RWS_REPORT);
  REQUIRE(rws.confluent());
}

TEST_CASE("RWS 06: for an infinite confluent fp semigroup 3",
          "[quick][rws][fpsemigroup][06]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("01", "10");
  rws.add_rule("02", "20");
  rws.add_rule("00", "0");
  rws.add_rule("02", "0");
  rws.add_rule("20", "0");
  rws.add_rule("11", "11");
  rws.add_rule("12", "21");
  rws.add_rule("111", "1");
  rws.add_rule("12", "1");
  rws.add_rule("21", "1");
  rws.add_rule("0", "1");

  REQUIRE(rws.confluent());
}

TEST_CASE("RWS 07: for a finite non-confluent fp semigroup from wikipedia",
          "[quick][rws][fpsemigroup][07]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("000", "");
  rws.add_rule("111", "");
  rws.add_rule("010101", "");

  REQUIRE(!rws.confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 4);
  REQUIRE(rws.confluent());
}

TEST_CASE("RWS 08: Example 5.1 in Sims", "[quick][rws][fpsemigroup][08]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("ab", "");
  rws.add_rule("ba", "");
  rws.add_rule("cd", "");
  rws.add_rule("dc", "");
  rws.add_rule("ca", "ac");

  REQUIRE(!rws.confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 8);
  REQUIRE(rws.confluent());
}

TEST_CASE("RWS 09: Example 5.1 in Sims", "[quick][rws][fpsemigroup][09]") {
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

TEST_CASE("RWS 10: Example 5.3 in Sims", "[quick][rws][fpsemigroup][10]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("aa", "");
  rws.add_rule("bbb", "");
  rws.add_rule("ababab", "");

  REQUIRE(!rws.confluent());

  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 6);
  REQUIRE(rws.confluent());
}

TEST_CASE("RWS 11: Example 5.4 in Sims", "[quick][rws][fpsemigroup][11]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("aa", "");
  rws.add_rule("bB", "");
  rws.add_rule("bbb", "");
  rws.add_rule("ababab", "");

  REQUIRE(!rws.confluent());

  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 11);
  REQUIRE(rws.confluent());
}

TEST_CASE("RWS 12: Example 6.4 in Sims", "[quick][rws][fpsemigroup][12]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("aa", "");
  rws.add_rule("bc", "");
  rws.add_rule("bbb", "");
  rws.add_rule("ababababababab", "");
  rws.add_rule("abacabacabacabac", "");

  REQUIRE(!rws.confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 40);
  REQUIRE(rws.confluent());
}

TEST_CASE("RWS 13: Example 6.6 in Sims", "[extreme][rws][fpsemigroup][13]") {
  RWS rws;
  rws.set_report(true);

  rws.add_rule("aa", "");
  rws.add_rule("bc", "");
  rws.add_rule("bbb", "");
  rws.add_rule("ababababababab", "");
  rws.add_rule("abacabacabacabacabacabacabacabac", "");

  REQUIRE(!rws.confluent());

  rws.knuth_bendix_by_overlap_length();
  REQUIRE(rws.nr_rules() == 1026);
  REQUIRE(rws.confluent());
}

TEST_CASE("RWS 14: Chapter 10, Section 4 in NR",
          "[rws][quick][fpsemigroup][14]") {
  RWS rws;
  rws.set_report(RWS_REPORT);

  rws.add_rule("aaaa", "a");
  rws.add_rule("bbbb", "b");
  rws.add_rule("cccc", "c");
  rws.add_rule("abab", "aaa");
  rws.add_rule("bcbc", "bbb");

  REQUIRE(!rws.confluent());

  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 31);
  REQUIRE(rws.confluent());
}

TEST_CASE("RWS 15: Sym(5) from Chapter 3, Proposition 1.1 in NR",
          "[rws][quick][fpsemigroup][15]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("aa", "");
  rws.add_rule("bbbbb", "");
  rws.add_rule("babababa", "");
  rws.add_rule("bB", "");
  rws.add_rule("Bb", "");
  rws.add_rule("BabBabBab", "");
  rws.add_rule("aBBabbaBBabb", "");
  rws.add_rule("aBBBabbbaBBBabbb", "");
  rws.add_rule("aA", "");
  rws.add_rule("Aa", "");

  REQUIRE(!rws.confluent());

  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 4);
  REQUIRE(rws.confluent());
}

TEST_CASE("RWS 16: SL(2, 7) from Chapter 3, Proposition 1.5 in NR",
          "[quick][rws][fpsemigroup][16]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("aaaaaaa", "");
  rws.add_rule("bb", "ababab");
  rws.add_rule("bb", "aaaabaaaabaaaabaaaab");
  rws.add_rule("aA", "");
  rws.add_rule("Aa", "");
  rws.add_rule("bB", "");
  rws.add_rule("Bb", "");

  // rws.set_clear_stack_interval(10);

  REQUIRE(!rws.confluent());

  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 152);
  REQUIRE(rws.confluent());
}

TEST_CASE("RWS 17: Bicyclic monoid", "[rws][quick][fpsemigroup][17]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("ab", "");

  REQUIRE(rws.confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 1);
  REQUIRE(rws.confluent());
}

TEST_CASE("RWS 18: Plactic monoid of degree 2 from Wikipedia",
          "[rws][quick][fpsemigroup][18]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("aba", "baa");
  rws.add_rule("bba", "bab");
  rws.add_rule("ac", "");
  rws.add_rule("ca", "");
  rws.add_rule("bc", "");
  rws.add_rule("cb", "");

  REQUIRE(!rws.confluent());

  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 3);
  REQUIRE(rws.confluent());
}

TEST_CASE("RWS 19: Example before Chapter 7, Proposition 1.1 in NR",
          "[rws][quick][fpsemigroup][19]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("aa", "a");
  rws.add_rule("bb", "b");

  REQUIRE(rws.confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 2);
  REQUIRE(rws.confluent());
}

TEST_CASE("RWS 20: size 243, Chapter 7, Theorem 3.6 in NR",
          "[rws][quick][fpsemigroup][20]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("aaa", "a");
  rws.add_rule("bbbb", "b");
  rws.add_rule("abababab", "aa");

  REQUIRE(!rws.confluent());

  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 9);
  REQUIRE(rws.confluent());
}

// See KBFP 07 also.

TEST_CASE("RWS 21: size 240, Chapter 7, Theorem 3.9 in NR",
          "[rws][quick][fpsemigroup][21]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("aaa", "a");
  rws.add_rule("bbbb", "b");
  rws.add_rule("abbba", "aa");
  rws.add_rule("baab", "bb");
  rws.add_rule("aabababababa", "aa");

  REQUIRE(!rws.confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 24);
  REQUIRE(rws.confluent());
}

TEST_CASE("RWS 22: F(2, 5); size 11, from Chapter 9, Section 1 in NR",
          "[rws][quick][fpsemigroup][22]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("ab", "c");
  rws.add_rule("bc", "d");
  rws.add_rule("cd", "e");
  rws.add_rule("de", "a");
  rws.add_rule("ea", "b");

  REQUIRE(!rws.confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 24);
  REQUIRE(rws.confluent());
}

TEST_CASE("RWS 23: F(2, 6); infinite, from Chapter 9, Section 1 in NR",
          "[rws][quick][fpsemigroup][23]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("ab", "");
  rws.add_rule("bc", "d");
  rws.add_rule("cd", "e");
  rws.add_rule("de", "f");
  rws.add_rule("ef", "a");
  rws.add_rule("fa", "b");

  REQUIRE(!rws.confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 35);
  REQUIRE(rws.confluent());
}

TEST_CASE("RWS 24: add_rule", "[quick][rws][fpsemigroup][24]") {
  std::vector<relation_t> rels = {relation_t({0, 1}, {1, 0}),
                                  relation_t({0, 2}, {2, 0}),
                                  relation_t({0, 0}, {0}),
                                  relation_t({0, 2}, {0}),
                                  relation_t({2, 0}, {0}),
                                  relation_t({1, 1}, {1, 1}),
                                  relation_t({1, 2}, {2, 1}),
                                  relation_t({1, 1, 1}, {1}),
                                  relation_t({1, 2}, {1}),
                                  relation_t({2, 1}, {1})};
  std::vector<relation_t> extra = {{{0}, {1}}};

  RWS rws;
  rws.add_rules(rels);
  rws.add_rules(extra);
  rws.set_report(RWS_REPORT);
  REQUIRE(rws.confluent());
  // We could rewrite here and check equality by this is simpler since all
  // allocation and deletion is handled in test_equals
  REQUIRE(rws.test_equals(rels[3].first, rels[3].second));
  REQUIRE(rws.test_equals(rels[6].first, rels[6].second));
  REQUIRE(rws.test_equals(rels[7].first, rels[7].second));
  REQUIRE(rws.test_equals(word_t({1, 0}), word_t({2, 2, 0, 1, 2})));
  REQUIRE(rws.test_equals(word_t({2, 1}), word_t({1, 1, 1, 2})));
  REQUIRE(!rws.test_equals(word_t({1, 0}), word_t({2})));
}

TEST_CASE("RWS 25: Chapter 11, Section 1 (q = 4, r = 3) in NR",
          "[rws][quick][fpsemigroup][25]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("aaa", "a");
  rws.add_rule("bbbbb", "b");
  rws.add_rule("abbbabb", "bba");

  REQUIRE(!rws.confluent());
  rws.knuth_bendix_by_overlap_length();
  REQUIRE(rws.nr_rules() == 20);
  REQUIRE(rws.confluent());

  // Check that rewrite to a non-pointer argument does not rewrite its argument
  std::string w = "aaa";
  REQUIRE(rws.rewrite(w) == "a");
  REQUIRE(w == "aaa");

  // defining relations
  REQUIRE(rws.rewrite("aaa") == rws.rewrite("a"));
  REQUIRE(rws.rewrite("bbbbb") == rws.rewrite("b"));
  REQUIRE(rws.rewrite("abbbabb") == rws.rewrite("bba"));

  // consequential relations (Chapter 11, Lemma 1.1 in NR)
  REQUIRE(rws.rewrite("babbbb") == rws.rewrite("ba"));
  REQUIRE(rws.rewrite("baabbbb") == rws.rewrite("baa"));
  REQUIRE(rws.rewrite("aabbbbbbbbbba") == rws.rewrite("bbbbbbbbbba"));
  REQUIRE(rws.rewrite("babbbbbbbbaa") == rws.rewrite("babbbbbbbb"));
  REQUIRE(rws.rewrite("baabbbbbbaa") == rws.rewrite("baabbbbbb"));
  REQUIRE(rws.rewrite("bbbbaabbbbaa") == rws.rewrite("bbbbaa"));
  REQUIRE(rws.rewrite("bbbaa") == rws.rewrite("baabb"));
  REQUIRE(rws.rewrite("abbbaabbba") == rws.rewrite("bbbbaa"));

  REQUIRE(!rws.test_less_than("abbbaabbba", "bbbbaa"));
  REQUIRE(!rws.test_less_than("abba", "abba"));

  // Call test_less_than without knuth_bendix first
  RWS rws2;
  rws2.set_report(RWS_REPORT);
  rws2.add_rule("aaa", "a");
  rws2.add_rule("bbbbb", "b");
  rws2.add_rule("abbbabb", "bba");
  REQUIRE(!rws2.test_less_than("abbbaabbba", "bbbbaa"));
}

TEST_CASE("RWS 26: Chapter 11, Section 1 (q = 8, r = 5) in NR",
          "[rws][fpsemigroup][quick][26]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("aaa", "a");
  rws.add_rule("bbbbbbbbb", "b");
  rws.add_rule("abbbbbabb", "bba");

  // rws.set_clear_stack_interval(0);

  REQUIRE(!rws.confluent());
  rws.knuth_bendix_by_overlap_length();
  REQUIRE(rws.nr_rules() == 105);
  REQUIRE(rws.confluent());

  // defining relations
  REQUIRE(rws.rewrite("aaa") == rws.rewrite("a"));
  REQUIRE(rws.rewrite("bbbbbbbbb") == rws.rewrite("b"));
  REQUIRE(rws.rewrite("abbbbbabb") == rws.rewrite("bba"));

  // consequential relations (Chapter 11, Lemma 1.1 in NR)
  REQUIRE(rws.rewrite("babbbbbbbb") == rws.rewrite("ba"));
  REQUIRE(rws.rewrite("baabbbbbbbb") == rws.rewrite("baa"));
  REQUIRE(rws.rewrite("aabbbbbbbbbbbba") == rws.rewrite("bbbbbbbbbbbba"));
  REQUIRE(rws.rewrite("babbbbbbbbbbaa") == rws.rewrite("babbbbbbbbbb"));
  REQUIRE(rws.rewrite("baabbbbbbbbaa") == rws.rewrite("baabbbbbbbb"));
  REQUIRE(rws.rewrite("bbbbbbbbaabbbbbbbbaa") == rws.rewrite("bbbbbbbbaa"));
  REQUIRE(rws.rewrite("bbbaa") == rws.rewrite("baabb"));
  REQUIRE(rws.rewrite("abbbbbaabbbbba") == rws.rewrite("bbbbbbbbaa"));

  REQUIRE(rws.test_less_than("aaa", "bbbbbbbbb"));
}

TEST_CASE("RWS 27: Chapter 11, Lemma 1.8 (q = 6, r = 5) in NR",
          "[rws][quick][fpsemigroup][27]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("aA", "");
  rws.add_rule("Aa", "");
  rws.add_rule("bB", "");
  rws.add_rule("Bb", "");
  rws.add_rule("cC", "");
  rws.add_rule("Cc", "");
  rws.add_rule("aa", "");
  rws.add_rule("bbb", "");
  rws.add_rule("abaBaBabaBab", "");

  REQUIRE(!rws.confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 16);
  REQUIRE(rws.confluent());
}

TEST_CASE("RWS 28: Chapter 11, Section 2 (q = 6, r = 2, alpha = abaabba) in NR",
          "[rws][quick][fpsemigroup][28]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("aaa", "a");
  rws.add_rule("bbbbbbb", "b");
  rws.add_rule("abaabba", "bb");

  REQUIRE(!rws.confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 4);
  REQUIRE(rws.confluent());
}

TEST_CASE("RWS 29: Chapter 8, Theorem 4.2 in NR",
          "[rws][quick][fpsemigroup][29]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("aaa", "a");
  rws.add_rule("bbbb", "b");
  rws.add_rule("bababababab", "b");
  rws.add_rule("baab", "babbbab");

  REQUIRE(!rws.confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 8);
  REQUIRE(rws.confluent());

  REQUIRE(!rws.test_less_than("bababababab", "aaaaa"));
  REQUIRE(rws.test_less_than("aaaaa", "bababababab"));
}

TEST_CASE("RWS 30: test_equals", "[quick][rws][fpsemigroup][30]") {
  RWS rws;
  rws.add_rule("ab", "ba");
  rws.add_rule("ac", "ca");
  rws.add_rule("aa", "a");
  rws.add_rule("ac", "a");
  rws.add_rule("ca", "a");
  rws.add_rule("bb", "bb");
  rws.add_rule("bc", "cb");
  rws.add_rule("bbb", "b");
  rws.add_rule("bc", "b");
  rws.add_rule("cb", "b");
  rws.add_rule("a", "b");

  REQUIRE(rws.test_equals("aa", "a"));
  REQUIRE(rws.test_equals("bb", "bb"));
  REQUIRE(rws.test_equals("bc", "cb"));
  REQUIRE(rws.test_equals("ba", "ccabc"));
  REQUIRE(rws.test_equals("cb", "bbbc"));
  REQUIRE(!rws.test_equals("ba", "c"));
}

TEST_CASE("RWS 31: for a free semigroup", "[quick][rws][smalloverlap][31]") {
  Congruence cong(
      "twosided", 2, std::vector<relation_t>(), std::vector<relation_t>());
  RWS rws;
  rws.add_rules(cong.relations());
  rws.add_rules(cong.extra());

  REQUIRE(!rws.test_equals({0}, {1}));
  REQUIRE(rws.test_equals({0}, {0}));
  REQUIRE(rws.test_equals({0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}));
}

TEST_CASE("RWS 32: from GAP smalloverlap gap/test.gi:32",
          "[quick][rws][smalloverlap][32]") {
  RWS rws;
  rws.add_rule("abcd", "ce");
  rws.add_rule("df", "dg");

  REQUIRE(!rws.confluent());

  REQUIRE(rws.test_equals("dfabcdf", "dfabcdg"));
  REQUIRE(rws.test_equals("abcdf", "ceg"));
  REQUIRE(rws.test_equals("abcdf", "cef"));

  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 3);
  REQUIRE(rws.confluent());
  REQUIRE(rws.test_equals("dfabcdf", "dfabcdg"));
  REQUIRE(rws.test_equals("abcdf", "ceg"));
  REQUIRE(rws.test_equals("abcdf", "cef"));
}

TEST_CASE("RWS 33: from GAP smalloverlap gap/test.gi:49",
          "[quick][rws][smalloverlap][33]") {
  RWS rws;
  rws.add_rule("abcd", "ce");
  rws.add_rule("df", "hd");

  REQUIRE(rws.confluent());

  REQUIRE(rws.test_equals("abchd", "abcdf"));
  REQUIRE(!rws.test_equals("abchf", "abcdf"));
  REQUIRE(rws.test_equals("abchd", "abchd"));
  REQUIRE(rws.test_equals("abchdf", "abchhd"));
  // Test cases (4) and (5)
  REQUIRE(rws.test_equals("abchd", "cef"));
  REQUIRE(rws.test_equals("cef", "abchd"));
}

TEST_CASE("RWS 34: from GAP smalloverlap gap/test.gi:63",
          "[quick][rws][smalloverlap][34]") {
  RWS rws;
  rws.add_rule("afh", "bgh");
  rws.add_rule("hc", "d");

  REQUIRE(!rws.confluent());

  // Test case (6)
  REQUIRE(rws.test_equals("afd", "bgd"));

  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 3);
}

TEST_CASE("RWS 35: from GAP smalloverlap gap/test.gi:70",
          "[quick][rws][smalloverlap][35]") {
  // The following permits a more complex test of case (6), which also
  // involves using the case (2) code to change the prefix being looked for:
  RWS rws;
  rws.add_rule("afh", "bgh");
  rws.add_rule("hc", "de");
  rws.add_rule("ei", "j");

  REQUIRE(!rws.confluent());

  REQUIRE(rws.test_equals("afdj", "bgdj"));
  REQUIRE(!rws.test_equals("xxxxxxxxxxxxxxxxxxxxxxx", "b"));

  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 5);
}

TEST_CASE("RWS 36: from GAP smalloverlap gap/test.gi:77",
          "[quick][rws][smalloverlap][36]") {
  // A slightly more complicated presentation for testing case (6), in which
  // the max piece suffixes of the first two relation words no longer agree
  // (since fh and gh are now pieces).
  RWS rws;
  rws.add_rule("afh", "bgh");
  rws.add_rule("hc", "de");
  rws.add_rule("ei", "j");
  rws.add_rule("fhk", "ghl");

  REQUIRE(!rws.confluent());

  REQUIRE(rws.test_equals("afdj", "bgdj"));

  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 7);
}

TEST_CASE("RWS 37: from GAP smalloverlap gap/test.gi:85 (knuth_bendix fails)",
          "[broken][rws][smalloverlap][37]") {
  RWS rws;
  rws.add_rule("aabc", "acba");

  REQUIRE(!rws.confluent());

  // TODO rws.set_report(); in all the new examples

  REQUIRE(!rws.test_equals("a", "b"));
  REQUIRE(rws.test_equals("aabcabc", "aabccba"));

  // rws.knuth_bendix();
}

TEST_CASE("RWS 38: Von Dyck (2,3,7) group - infinite",
          "[quick][rws][smalloverlap][kbmag][38]") {
  RWS rws;
  rws.add_rule("aaaa", "AAA");
  rws.add_rule("bb", "B");
  rws.add_rule("BA", "c");

  REQUIRE(!rws.confluent());
  rws.set_report(RWS_REPORT);
  rws.knuth_bendix();

  REQUIRE(rws.nr_rules() == 6);
  REQUIRE(rws.confluent());
  REQUIRE(!rws.test_equals("a", "b"));
  REQUIRE(!rws.test_equals("aabcabc", "aabccba"));
}

// Does not finish knuth_bendix
TEST_CASE("RWS 39: Von Dyck (2,3,7) group - infinite - different presentation",
          "[extreme][rws][smalloverlap][kbmag][39]") {
  RWS rws;
  rws.add_rule("aaaa", "AAA");
  rws.add_rule("bb", "B");
  rws.add_rule("abababa", "BABABAB");
  rws.add_rule("BA", "c");

  REQUIRE(!rws.confluent());
  rws.set_report(true);
  rws.set_overlap_measure(RWS::overlap_measure::max_AB_BC);
  rws.set_max_rules(100);
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 109);
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 109);
  rws.set_max_rules(250);
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 262);
}

TEST_CASE("RWS 40: rewriting system from KBP 08",
          "[quick][rws][smalloverlap][kbmag][40]") {
  RWS rws;
  rws.add_rule("bbbbbbb", "b");
  rws.add_rule("ccccc", "c");
  rws.add_rule("bccba", "bccb");
  rws.add_rule("bccbc", "bccb");
  rws.add_rule("bbcbca", "bbcbc");
  rws.add_rule("bbcbcb", "bbcbc");

  REQUIRE(!rws.confluent());
  REQUIRE(rws.nr_rules() == 6);
  rws.set_report(RWS_REPORT);
  rws.knuth_bendix();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 8);

  REQUIRE(rws.rule("bbbbbbb", "b"));
  REQUIRE(rws.rule("ccccc", "c"));
  REQUIRE(rws.rule("bccba", "bccb"));
  REQUIRE(rws.rule("bccbc", "bccb"));
  REQUIRE(rws.rule("bcbca", "bcbc"));
  REQUIRE(rws.rule("bcbcb", "bcbc"));
  REQUIRE(rws.rule("bcbcc", "bcbc"));
  REQUIRE(rws.rule("bccbb", "bccb"));
  // Wrong way around rule
  REQUIRE(rws.rule("bccb", "bccbb"));
  // Not a rule
  REQUIRE(!rws.rule("aaaa", "bccbb"));

  std::vector<std::pair<std::string, std::string>> rules = rws.rules();
  REQUIRE(rules[0] == std::pair<std::string, std::string>("bcbca", "bcbc"));
  REQUIRE(rules[1] == std::pair<std::string, std::string>("bcbcb", "bcbc"));
  REQUIRE(rules[2] == std::pair<std::string, std::string>("bcbcc", "bcbc"));
  REQUIRE(rules[3] == std::pair<std::string, std::string>("bccba", "bccb"));
  REQUIRE(rules[4] == std::pair<std::string, std::string>("bccbb", "bccb"));
  REQUIRE(rules[5] == std::pair<std::string, std::string>("bccbc", "bccb"));
  REQUIRE(rules[6] == std::pair<std::string, std::string>("ccccc", "c"));
  REQUIRE(rules[7] == std::pair<std::string, std::string>("bbbbbbb", "b"));
}

TEST_CASE("RWS 41: rewriting system from Congruence 20", "[quick][rws][41]") {
  RWS rws;
  rws.add_rule("aaa", "a");
  rws.add_rule("ab", "ba");
  rws.add_rule("aa", "a");
  rws.knuth_bendix();

  REQUIRE(rws.test_equals("abbbbbbbbbbbbbb", "aabbbbbbbbbbbbbb"));
}

// The next test meets the definition of a standard test but causes valgrind on
// travis to timeout.
TEST_CASE("RWS 42: Example 6.6 in Sims (with limited overlap lengths)",
          "[extreme][rws][fpsemigroup][42]") {
  RWS rws;
  rws.set_report(RWS_REPORT);

  rws.add_rule("aa", "");
  rws.add_rule("bc", "");
  rws.add_rule("bbb", "");
  rws.add_rule("ababababababab", "");
  rws.add_rule("abacabacabacabacabacabacabacabac", "");

  REQUIRE(!rws.confluent());

  // In Sims it says to use 44 here, but that doesn't seem to work.
  rws.set_max_overlap(45);
  // Avoid checking confluence since this is very slow, essentially takes the
  // same amount of time as running RWS 13.
  rws.set_check_confluence_interval(UNBOUNDED1);

  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 1026);
}

// This example verifies the nilpotence of the group using the Sims algorithm.
// The original presentation was <a,b| [b,a,a,a], [b^-1,a,a,a], [a,b,b,b],
// [a^-1,b,b,b], [a,a*b,a*b,a*b], [a^-1,a*b,a*b,a*b] >. (where [] mean
// left-normed commutators. The presentation here was derived by first
// applying the NQA to find the maximal nilpotent quotient, and then
// introducing new generators for the PCP generators.
/*TEST_CASE("RWS 43: (from kbmag/standalone/kb_data/heinnilp)",
          "[fails][rws][kbmag][recursive][43]") {
  // FIXME fails because internal_rewrite expect rules to be length reducing
  RWS rws(new RECURSIVE(), "fFyYdDcCbBaA");
  rws.add_rule("BAba", "c");
  rws.add_rule("CAca", "d");
  rws.add_rule("CBcb", "y");
  rws.add_rule("DBdb", "f");
  rws.add_rule("cBCb", "bcBC");
  rws.add_rule("babABaBA", "abABaBAb");
  rws.add_rule("cBACab", "abcBAC");
  rws.add_rule("BabABBAbab", "aabABBAb");
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.confluent());

  rws.knuth_bendix();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 32767);
}*/

// Fibonacci group F(2,7) - order 29 - works better with largish tidyint
// knuth_bendix does not terminate
TEST_CASE("RWS 44: (from kbmag/standalone/kb_data/f27)",
          "[extreme][rws][kbmag][shortlex][44]") {
  RWS rws("aAbBcCdDyYfFgG");
  rws.add_rule("ab", "c");
  rws.add_rule("bc", "d");
  rws.add_rule("cd", "y");
  rws.add_rule("dy", "f");
  rws.add_rule("yf", "g");
  rws.add_rule("fg", "a");
  rws.add_rule("ga", "b");
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.confluent());

  rws.knuth_bendix_by_overlap_length();
  // Fails to terminate, or is very slow, with knuth_bendix
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 47);
  // KBMAG does not terminate with this example :-)
}

// An extension of 2^6 be L32
// knuth_bendix/2 does not terminate
TEST_CASE("RWS 45: (from kbmag/standalone/kb_data/l32ext)",
          "[extreme][rws][kbmag][shortlex][45]") {
  RWS rws("abB");
  rws.add_rule("aa", "");
  rws.add_rule("BB", "b");
  rws.add_rule("BaBaBaB", "abababa");
  rws.add_rule("aBabaBabaBabaBab", "BabaBabaBabaBaba");
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.confluent());

  rws.knuth_bendix_by_overlap_length();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 32750);
}

// 2-generator free abelian group (with this ordering KB terminates - but no
// all)
TEST_CASE("RWS 46: (from kbmag/standalone/kb_data/ab2)",
          "[quick][rws][kbmag][shortlex][46]") {
  RWS rws("aAbB");
  rws.add_rule("Bab", "a");
  rws.set_report(RWS_REPORT);

  REQUIRE(rws.confluent());

  rws.knuth_bendix();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 1);

  REQUIRE(rws.rule("Bab", "a"));
}

// This group is actually D_22 (although it wasn't meant to be). All generators
// are unexpectedly involutory.

// knuth_bendix/2 does not terminate with the given ordering, terminates almost
// immediately with the standard order. TODO double check that something isn't
// going wrong in the nonstandard alphabet case.
TEST_CASE("RWS 47: (from kbmag/standalone/kb_data/d22)",
          "[rws][kbmag][shortlex][47]") {
  RWS rws("aAbBcCdDyYfF");
  rws.add_rule("aCAd", "");
  rws.add_rule("bfBY", "");
  rws.add_rule("cyCD", "");
  rws.add_rule("dFDa", "");
  rws.add_rule("ybYA", "");
  rws.add_rule("fCFB", "");
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.confluent());

  rws.knuth_bendix_by_overlap_length();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 8);

  REQUIRE(rws.rule("bfBY", ""));
  REQUIRE(rws.rule("cyCD", ""));
  REQUIRE(rws.rule("ybYA", ""));
  REQUIRE(rws.rule("fCFB", ""));
  REQUIRE(rws.rule("CAd", "dFD"));
  REQUIRE(rws.rule("FDa", "aCA"));
  REQUIRE(rws.rule("adFD", ""));
  REQUIRE(rws.rule("daCA", ""));
}

// No generators - no anything!
TEST_CASE("RWS 48: (from kbmag/standalone/kb_data/degen1)",
          "[quick][rws][kbmag][shortlex][48]") {
  RWS rws("");
  rws.set_report(RWS_REPORT);

  REQUIRE(rws.confluent());

  rws.knuth_bendix();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 0);
}

// Symmetric group S_4

// knuth_bendix/2 fails to temrinate
TEST_CASE("RWS 49: (from kbmag/standalone/kb_data/s4)",
          "[extreme][rws][kbmag][shortlex][49]") {
  RWS rws("abB");
  rws.add_rule("bb", "B");
  rws.add_rule("BaBa", "abab");
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.confluent());

  rws.knuth_bendix_by_overlap_length();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 32767);
}

// This example verifies the nilpotence of the group using the Sims algorithm.
// The original presentation was <a,b| [b,a,b], [b,a,a,a,a], [b,a,a,a,b,a,a] >.
// (where [] mean left-normed commutators). The presentation here was derived
// by first applying the NQA to find the maximal nilpotent quotient, and then
// introducing new generators for the PCP generators. It is essential for
// success that reasonably low values of the maxstoredlen parameter are given.
/*TEST_CASE("RWS 50: (from kbmag/standalone/kb_data/verifynilp)",
          "[quick][rws][kbmag][recursive][50]") {
  RWS rws(new RECURSIVE(), "hHgGfFyYdDcCbBaA");
  rws.add_rule("BAba", "c");
  rws.add_rule("CAca", "d");
  rws.add_rule("DAda", "y");
  rws.add_rule("YByb", "f");
  rws.add_rule("FAfa", "g");
  rws.add_rule("ga", "ag");
  rws.add_rule("GBgb", "h");
  rws.add_rule("cb", "bc");
  rws.add_rule("ya", "ay");
  rws.set_report(RWS_REPORT);

  REQUIRE(rws.confluent());

  rws.knuth_bendix();
  REQUIRE(rws.confluent());
  REQUIRE(rws.nr_rules() == 9);

  REQUIRE(rws.rule("BAba", "c"));
  REQUIRE(rws.rule("CAca", "d"));
  REQUIRE(rws.rule("DAda", "y"));
  REQUIRE(rws.rule("YByb", "f"));
  REQUIRE(rws.rule("FAfa", "g"));
  REQUIRE(rws.rule("ga", "ag"));
  REQUIRE(rws.rule("GBgb", "h"));
  REQUIRE(rws.rule("cb", "bc"));
  REQUIRE(rws.rule("ya", "ay"));
}*/
