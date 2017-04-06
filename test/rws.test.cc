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

#include "../cong.h"
#include "../rws.h"

#define RWS_REPORT false

using namespace libsemigroups;

template <typename T> static inline void really_delete_cont(T cont) {
  for (Element* x : cont) {
    x->really_delete();
    delete x;
  }
}

TEST_CASE("RWS 01: for a transformation semigroup of size 4",
          "[quick][rws][fpsemigroup]") {
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
          "[quick][rws][finite]") {
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
          "[quick][rws][finite]") {
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
          "[quick][rws][fpsemigroup]") {
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
          "[quick][rws][fpsemigroup]") {
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
          "[quick][rws][fpsemigroup]") {
  std::vector<rws_rule_t> rules = {rws_rule_t("01", "10"),
                                   rws_rule_t("02", "20"),
                                   rws_rule_t("00", "0"),
                                   rws_rule_t("02", "0"),
                                   rws_rule_t("20", "0"),
                                   rws_rule_t("11", "11"),
                                   rws_rule_t("12", "21"),
                                   rws_rule_t("111", "1"),
                                   rws_rule_t("12", "1"),
                                   rws_rule_t("21", "1"),
                                   rws_rule_t("0", "1")};
  RWS rws(rules);
  rws.set_report(RWS_REPORT);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 07: for a finite non-confluent fp semigroup from wikipedia",
          "[quick][rws][fpsemigroup]") {
  std::vector<rws_rule_t> rules = {
      rws_rule_t("000", ""), rws_rule_t("111", ""), rws_rule_t("010101", ""),
  };
  RWS rws(rules);
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 4);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 08: Example 5.1 in Sims", "[quick][rws][fpsemigroup]") {
  std::vector<rws_rule_t> rules = {rws_rule_t("ab", ""),
                                   rws_rule_t("ba", ""),
                                   rws_rule_t("cd", ""),
                                   rws_rule_t("dc", ""),
                                   rws_rule_t("ca", "ac")};
  RWS rws(rules);
  rws.set_report(RWS_REPORT);
  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 8);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 09: Example 5.1 in Sims", "[quick][rws][fpsemigroup]") {
  std::vector<rws_rule_t> rules = {rws_rule_t("aA", ""),
                                   rws_rule_t("Aa", ""),
                                   rws_rule_t("bB", ""),
                                   rws_rule_t("Bb", ""),
                                   rws_rule_t("ba", "ab")};
  RWS rws(new SHORTLEX([](rws_letter_t x, rws_letter_t y) {
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
          }),
          rules);
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 8);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 10: Example 5.3 in Sims", "[quick][rws][fpsemigroup]") {
  std::vector<rws_rule_t> rules
      = {rws_rule_t("aa", ""), rws_rule_t("bbb", ""), rws_rule_t("ababab", "")};
  RWS rws(rules);
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 6);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 11: Example 5.4 in Sims", "[quick][rws][fpsemigroup]") {
  std::vector<rws_rule_t> rules = {rws_rule_t("aa", ""),
                                   rws_rule_t("bB", ""),
                                   rws_rule_t("bbb", ""),
                                   rws_rule_t("ababab", "")};
  RWS rws(rules);
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 11);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 12: Example 6.4 in Sims", "[standard][rws][fpsemigroup]") {
  std::vector<rws_rule_t> rules = {
      rws_rule_t("aa", ""),
      rws_rule_t("bc", ""),
      rws_rule_t("bbb", ""),
      rws_rule_t("ababababababab", ""),
      rws_rule_t("abacabacabacabac", ""),
  };
  RWS rws(rules);
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 40);
  REQUIRE(rws.is_confluent());
}

// The next test takes too long to run
TEST_CASE("RWS 13: Example 6.6 in Sims", "[hide][extreme][rws][fpsemigroup]") {
  std::vector<rws_rule_t> rules = {
      rws_rule_t("aa", ""),
      rws_rule_t("bc", ""),
      rws_rule_t("bbb", ""),
      rws_rule_t("ababababababab", ""),
      rws_rule_t("abacabacabacabacabacabacabacabac", ""),
  };
  RWS rws(rules);
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 1026);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 14: Chapter 10, Section 4 in NR", "[rws][quick][fpsemigroup]") {
  std::vector<rws_rule_t> rules = {
      rws_rule_t("aaaa", "a"),
      rws_rule_t("bbbb", "b"),
      rws_rule_t("cccc", "c"),
      rws_rule_t("abab", "aaa"),
      rws_rule_t("bcbc", "bbb"),
  };
  RWS rws(rules);
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 31);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 15: Sym(5) from Chapter 3, Proposition 1.1 in NR",
          "[rws][quick][fpsemigroup]") {
  std::vector<rws_rule_t> rules = {
      rws_rule_t("aa", ""),
      rws_rule_t("bbbbb", ""),
      rws_rule_t("babababa", ""),
      rws_rule_t("bB", ""),
      rws_rule_t("Bb", ""),
      rws_rule_t("BabBabBab", ""),
      rws_rule_t("aBBabbaBBabb", ""),
      rws_rule_t("aBBBabbbaBBBabbb", ""),
      rws_rule_t("aA", ""),
      rws_rule_t("Aa", ""),
  };
  RWS rws(rules);
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 4);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 16: SL(2, 7) from Chapter 3, Proposition 1.5 in NR",
          "[extreme][rws][fpsemigroup]") {
  std::vector<rws_rule_t> rules = {
      rws_rule_t("aaaaaaa", ""),
      rws_rule_t("bb", "ababab"),
      rws_rule_t("bb", "aaaabaaaabaaaabaaaab"),
      rws_rule_t("aA", ""),
      rws_rule_t("Aa", ""),
      rws_rule_t("bB", ""),
      rws_rule_t("Bb", ""),
  };
  RWS rws(rules);
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 152);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 17: Bicyclic monoid", "[rws][quick][fpsemigroup]") {
  std::vector<rws_rule_t> rules = {
      rws_rule_t("ab", ""),
  };
  RWS rws(rules);
  rws.set_report(RWS_REPORT);

  REQUIRE(rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 1);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 18: Plactic monoid of degree 2 from Wikipedia",
          "[rws][quick][fpsemigroup]") {
  std::vector<rws_rule_t> rules = {
      rws_rule_t("aba", "baa"),
      rws_rule_t("bba", "bab"),
      rws_rule_t("ac", ""),
      rws_rule_t("ca", ""),
      rws_rule_t("bc", ""),
      rws_rule_t("cb", ""),
  };
  RWS rws(rules);
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 3);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 19: Example before Chapter 7, Proposition 1.1 in NR",
          "[rws][quick][fpsemigroup]") {
  std::vector<rws_rule_t> rules = {
      rws_rule_t("aa", "a"), rws_rule_t("bb", "b"),
  };
  RWS rws(rules);
  rws.set_report(RWS_REPORT);

  REQUIRE(rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 2);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 20: size 243, Chapter 7, Theorem 3.6 in NR",
          "[rws][quick][fpsemigroup]") {
  std::vector<rws_rule_t> rules = {
      rws_rule_t("aaa", "a"),
      rws_rule_t("bbbb", "b"),
      rws_rule_t("abababab", "aa"),
  };
  RWS rws(rules);
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 9);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 21: size 240, Chapter 7, Theorem 3.9 in NR",
          "[rws][quick][fpsemigroup]") {
  std::vector<rws_rule_t> rules = {
      rws_rule_t("aaa", "a"),
      rws_rule_t("bbbb", "b"),
      rws_rule_t("abbba", "aa"),
      rws_rule_t("baab", "bb"),
      rws_rule_t("aabababababa", "aa"),
  };
  RWS rws(rules);
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 24);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 22: F(2, 5), size 11, from Chapter 9, Section 1 in NR",
          "[rws][quick][fpsemigroup]") {
  std::vector<rws_rule_t> rules = {
      rws_rule_t("ab", "c"),
      rws_rule_t("bc", "d"),
      rws_rule_t("cd", "e"),
      rws_rule_t("de", "a"),
      rws_rule_t("ea", "b"),
  };
  RWS rws(rules);
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 24);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 23: F(2, 6), infinite, from Chapter 9, Section 1 in NR",
          "[rws][quick][fpsemigroup]") {
  std::vector<rws_rule_t> rules = {
      rws_rule_t("ab", ""),
      rws_rule_t("bc", "d"),
      rws_rule_t("cd", "e"),
      rws_rule_t("de", "f"),
      rws_rule_t("ef", "a"),
      rws_rule_t("fa", "b"),
  };
  RWS rws(rules);
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 35);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 24: add_rule", "[quick][rws][fpsemigroup]") {
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
    rws.add_rule(rws_rule_t(rws.word_to_rws_word(rel.first),
                            rws.word_to_rws_word(rel.second)));
  }
  rws.add_rules(extra);
  rws.set_report(RWS_REPORT);
  REQUIRE(rws.is_confluent());

  REQUIRE(rws.rewrite(rws.word_to_rws_word(rels[3].first))
          == rws.rewrite(rws.word_to_rws_word(rels[3].second)));
  REQUIRE(rws.rewrite(rws.word_to_rws_word(rels[6].first))
          == rws.rewrite(rws.word_to_rws_word(rels[6].second)));
  REQUIRE(rws.rewrite(rws.word_to_rws_word(rels[7].first))
          == rws.rewrite(rws.word_to_rws_word(rels[7].second)));
  REQUIRE(rws.rewrite(rws.word_to_rws_word(word_t({1, 0})))
          == rws.rewrite(rws.word_to_rws_word(word_t({2, 2, 0, 1, 2}))));
  REQUIRE(rws.rewrite(rws.word_to_rws_word(word_t({2, 1})))
          == rws.rewrite(rws.word_to_rws_word(word_t({1, 1, 1, 2}))));
  REQUIRE(rws.rewrite(rws.word_to_rws_word(word_t({1, 0})))
          != rws.rewrite(rws.word_to_rws_word(word_t({2}))));
}

TEST_CASE("RWS 25: Chapter 11, Section 1 (q = 4, r = 3) in NR",
          "[rws][quick][fpsemigroup]") {
  std::vector<rws_rule_t> rules = {
      rws_rule_t("aaa", "a"),
      rws_rule_t("bbbbb", "b"),
      rws_rule_t("abbbabb", "bba"),
  };
  RWS rws(rules);
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 20);
  REQUIRE(rws.is_confluent());

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
  RWS rws2(rules);
  rws2.set_report(RWS_REPORT);
  REQUIRE(!rws2.test_less_than("abbbaabbba", "bbbbaa"));
}

TEST_CASE("RWS 26: Chapter 11, Section 1 (q = 8, r = 5) in NR",
          "[rws][fpsemigroup][extreme]") {
  std::vector<rws_rule_t> rules = {
      rws_rule_t("aaa", "a"),
      rws_rule_t("bbbbbbbbb", "b"),
      rws_rule_t("abbbbbabb", "bba"),
  };
  RWS rws(rules);
  rws.set_report(RWS_REPORT);

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
          "[rws][quick][fpsemigroup]") {
  std::vector<rws_rule_t> rules = {
      rws_rule_t("aA", ""),
      rws_rule_t("Aa", ""),
      rws_rule_t("bB", ""),
      rws_rule_t("Bb", ""),
      rws_rule_t("cC", ""),
      rws_rule_t("Cc", ""),
      rws_rule_t("aa", ""),
      rws_rule_t("bbb", ""),
      rws_rule_t("abaBaBabaBab", ""),
  };
  RWS rws(rules);
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 16);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 27: Chapter 11, Section 2 (q = 6, r = 2, alpha = abaabba) in NR",
          "[rws][quick][fpsemigroup]") {
  std::vector<rws_rule_t> rules = {
      rws_rule_t("aaa", "a"),
      rws_rule_t("bbbbbbb", "b"),
      rws_rule_t("abaabba", "bb"),
  };
  RWS rws(rules);
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 4);
  REQUIRE(rws.is_confluent());
}

TEST_CASE("RWS 28: Chapter 8, Theorem 4.2 in NR", "[rws][quick][fpsemigroup]") {
  std::vector<rws_rule_t> rules = {
      rws_rule_t("aaa", "a"),
      rws_rule_t("bbbb", "b"),
      rws_rule_t("bababababab", "b"),
      rws_rule_t("baab", "babbbab"),
  };
  RWS rws(rules);
  rws.set_report(RWS_REPORT);

  REQUIRE(!rws.is_confluent());
  rws.knuth_bendix();
  REQUIRE(rws.nr_rules() == 8);
  REQUIRE(rws.is_confluent());

  REQUIRE(!rws.test_less_than("bababababab", "aaaaa"));
  REQUIRE(rws.test_less_than("aaaaa", "bababababab"));
}
