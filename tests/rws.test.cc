//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2017 James D. Mitchell
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

TEST_CASE("RWS 01: for a transformation semigroup of size 4",
          "[quick][rws][fpsemigroup][01]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({1, 0}),
         new Transformation<u_int16_t>(std::vector<u_int16_t>({0, 0}))};
  Semigroup S = Semigroup(gens);
  S.set_report(RWS_REPORT);
  really_delete_cont(gens);
  REQUIRE(S.size() == 4);
  REQUIRE(S.degree() == 2);
  REQUIRE(S.nrrules() == 4);
  std::vector<relation_t> extra({});
  Congruence              cong("twosided", &S, extra);

  RWS rws(cong);
  rws.set_report(RWS_REPORT);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 02: for a transformation semigroup of size 9",
          "[quick][rws][finite][02]") {
  std::vector<Element*> gens = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
                                new Transformation<u_int16_t>({0, 0, 0, 0, 0})};
  Semigroup S = Semigroup(gens);
  S.set_report(RWS_REPORT);
  really_delete_cont(gens);
  REQUIRE(S.size() == 9);
  REQUIRE(S.degree() == 5);
  REQUIRE(S.nrrules() == 3);
  std::vector<relation_t> extra({});
  Congruence              cong("twosided", &S, extra);

  RWS rws(cong);
  rws.set_report(RWS_REPORT);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 03: for a transformation semigroup of size 88",
          "[quick][rws][finite][03]") {
  std::vector<Element*> gens = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
                                new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
  Semigroup S = Semigroup(gens);
  S.set_report(RWS_REPORT);
  really_delete_cont(gens);
  REQUIRE(S.size() == 88);
  REQUIRE(S.degree() == 5);
  REQUIRE(S.nrrules() == 18);
  std::vector<relation_t> extra({});
  Congruence              cong("twosided", &S, extra);

  RWS rws(cong);
  rws.set_report(RWS_REPORT);
  REQUIRE(rws.is_confluent());
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
  Congruence              cong("twosided", 3, rels, extra);

  RWS rws(cong);
  rws.set_report(RWS_REPORT);
  REQUIRE(rws.is_confluent());
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
  REQUIRE(rws.is_confluent());
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

  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 07: for a finite non-confluent fp semigroup from wikipedia",
          "[quick][rws][fpsemigroup][07]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("000", "");
  rws.add_rule("111", "");
  rws.add_rule("010101", "");

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 4);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 08: Example 5.1 in Sims", "[quick][rws][fpsemigroup][08]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("ab", "");
  rws.add_rule("ba", "");
  rws.add_rule("cd", "");
  rws.add_rule("dc", "");
  rws.add_rule("ca", "ac");

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 8);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 09: Example 5.1 in Sims", "[quick][rws][fpsemigroup][09]") {
  SHORTLEX* ro = new SHORTLEX([](rws_letter_t x, rws_letter_t y) {
    if (y == x) {
      return false;
    } else if (y == 'a') {
      return false;
    } else if (y == 'A' && x != 'a') {
      return false;
    } else if (y == 'b' && x == 'B') {
      return false;
    } else {
      return true;
    }
  });

  RWS rws(ro);
  rws.set_report(RWS_REPORT);

  rws.add_rule("aA", "");
  rws.add_rule("Aa", "");
  rws.add_rule("bB", "");
  rws.add_rule("Bb", "");
  rws.add_rule("ba", "ab");

  REQUIRE(!(*ro)("aaaaab", "aaaaaa"));

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 8);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 10: Example 5.3 in Sims", "[quick][rws][fpsemigroup][10]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("aa", "");
  rws.add_rule("bbb", "");
  rws.add_rule("ababab", "");

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 6);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 11: Example 5.4 in Sims", "[quick][rws][fpsemigroup][11]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("aa", "");
  rws.add_rule("bB", "");
  rws.add_rule("bbb", "");
  rws.add_rule("ababab", "");

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 11);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 12: Example 6.4 in Sims", "[standard][rws][fpsemigroup][12]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("aa", "");
  rws.add_rule("bc", "");
  rws.add_rule("bbb", "");
  rws.add_rule("ababababababab", "");
  rws.add_rule("abacabacabacabac", "");

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 40);
  REQUIRE(rws.is_confluent());
}

// The next test takes too long to run
/*TEST_CASE("RWS 13: Example 6.6 in Sims",
"[hide][extreme][rws][fpsemigroup][13]")
{
  RWS rws;
  rws.set_report(RWS_REPORT);

  rws.add_rule("aa", "");
  rws.add_rule("bc", "");
  rws.add_rule("bbb", "");
  rws.add_rule("ababababababab", "");
  rws.add_rule("abacabacabacabacabacabacabacabac", "");

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 1026);
  REQUIRE(rws.is_confluent());
}*/

TEST_CASE("RWS 14: Chapter 10, Section 4 in NR",
          "[rws][quick][fpsemigroup][14]") {
  RWS rws;
  rws.set_report(RWS_REPORT);

  rws.add_rule("aaaa", "a");
  rws.add_rule("bbbb", "b");
  rws.add_rule("cccc", "c");
  rws.add_rule("abab", "aaa");
  rws.add_rule("bcbc", "bbb");

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 31);
  REQUIRE(rws.is_confluent());
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

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 4);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 16: SL(2, 7) from Chapter 3, Proposition 1.5 in NR",
          "[extreme][rws][fpsemigroup][16]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("aaaaaaa", "");
  rws.add_rule("bb", "ababab");
  rws.add_rule("bb", "aaaabaaaabaaaabaaaab");
  rws.add_rule("aA", "");
  rws.add_rule("Aa", "");
  rws.add_rule("bB", "");
  rws.add_rule("Bb", "");

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 152);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 17: Bicyclic monoid", "[rws][quick][fpsemigroup][17]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("ab", "");

  REQUIRE(rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 1);
  REQUIRE(rws.is_confluent());
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

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 3);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 19: Example before Chapter 7, Proposition 1.1 in NR",
          "[rws][quick][fpsemigroup][19]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("aa", "a");
  rws.add_rule("bb", "b");

  REQUIRE(rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 2);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 20: size 243, Chapter 7, Theorem 3.6 in NR",
          "[rws][quick][fpsemigroup][20]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("aaa", "a");
  rws.add_rule("bbbb", "b");
  rws.add_rule("abababab", "aa");

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 9);
  REQUIRE(rws.is_confluent());
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

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 24);
  REQUIRE(rws.is_confluent());
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

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 24);
  REQUIRE(rws.is_confluent());
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

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 35);
  REQUIRE(rws.is_confluent());
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
  for (relation_t rel : rels) {
    rws.add_rule(rws.word_to_rws_word(rel.first),
                 rws.word_to_rws_word(rel.second));
  }
  rws.add_rules(extra);
  rws.set_report(RWS_REPORT);
  REQUIRE(rws.is_confluent());
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

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 20);
  REQUIRE(rws.is_confluent());

  // Check that rewrite to a non-pointer argument does not rewrite its argument
  rws_word_t w = "aaa";
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
          "[rws][fpsemigroup][extreme][26]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("aaa", "a");
  rws.add_rule("bbbbbbbbb", "b");
  rws.add_rule("abbbbbabb", "bba");

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 105);
  REQUIRE(rws.is_confluent());

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

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 16);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 28: Chapter 11, Section 2 (q = 6, r = 2, alpha = abaabba) in NR",
          "[rws][quick][fpsemigroup][28]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("aaa", "a");
  rws.add_rule("bbbbbbb", "b");
  rws.add_rule("abaabba", "bb");

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 4);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 29: Chapter 8, Theorem 4.2 in NR",
          "[rws][quick][fpsemigroup][29]") {
  RWS rws;
  rws.set_report(RWS_REPORT);
  rws.add_rule("aaa", "a");
  rws.add_rule("bbbb", "b");
  rws.add_rule("bababababab", "b");
  rws.add_rule("baab", "babbbab");

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 8);
  REQUIRE(rws.is_confluent());

  REQUIRE(!rws.test_less_than("bababababab", "aaaaa"));
  REQUIRE(rws.test_less_than("aaaaa", "bababababab"));
}

TEST_CASE("RWS 30: test_equals", "[quick][rws][fpsemigroup][30]") {
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
