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

// TODO The other examples from Sims' book (Chapters 5 and 6) which use
// reduction orderings different from shortlex

#include "catch.hpp"
#include "element-helper.h"
#include "kbe.h"
#include "knuth-bendix.h"
#include "semigroup.h"

namespace libsemigroups {
  namespace knuth_bendix_fpsemigroup {
    using KnuthBendix = fpsemigroup::KnuthBendix;

    template <class TElementType>
    void delete_gens(std::vector<TElementType>& gens) {
      for (auto x : gens) {
        delete x;
      }
    }

    constexpr bool REPORT = false;

    TEST_CASE("Knuth-Bendix 01: transformation semigroup (size 4)",
              "[quick][knuth-bendix][finite][01]") {
      REPORTER.set_report(REPORT);
      using Transf = Transf<2>::type;

      Semigroup<Transf> S({Transf({1, 0}), Transf({0, 0})});
      REQUIRE(S.size() == 4);
      REQUIRE(S.nrrules() == 4);

      KnuthBendix kb(S);
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_rules() == 4);
      REQUIRE(kb.size() == 4);
    }

    TEST_CASE("Knuth-Bendix 02: transformation semigroup (size 9)",
              "[quick][knuth-bendix][finite][02]") {
      REPORTER.set_report(REPORT);
      std::vector<Element*> gens
          = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
             new Transformation<u_int16_t>({0, 0, 0, 0, 0})};
      Semigroup<> S = Semigroup<>(gens);
      REQUIRE(S.size() == 9);
      REQUIRE(S.degree() == 5);
      REQUIRE(S.nrrules() == 3);

      KnuthBendix kb(S);
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_rules() == 3);
      REQUIRE(kb.size() == 9);
      delete_gens(gens);
    }

    TEST_CASE("Knuth-Bendix 03: transformation semigroup (size 88)",
              "[quick][knuth-bendix][finite][03]") {
      REPORTER.set_report(REPORT);
      std::vector<Element*> gens
          = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
             new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
      Semigroup<> S = Semigroup<>(gens);
      REQUIRE(S.size() == 88);
      REQUIRE(S.degree() == 5);
      REQUIRE(S.nrrules() == 18);

      KnuthBendix kb(S);
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_rules() == 18);
      REQUIRE(kb.size() == 88);
      delete_gens(gens);
    }

    TEST_CASE("Knuth-Bendix 04: infinite confluent fp semigroup 1",
              "[quick][knuth-bendix][fpsemigroup][04]") {
      REPORTER.set_report(true);

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
      REQUIRE(kb.nr_rules() == 4);
      REQUIRE(kb.size() == 3);
    }

    TEST_CASE("Knuth-Bendix 05: infinite confluent fp semigroup 2",
              "[quick][knuth-bendix][fpsemigroup][05]") {
      REPORTER.set_report(REPORT);

      KnuthBendix kb;
      REQUIRE_THROWS_AS(kb.add_rule({0, 1}, {1, 0}), LibsemigroupsException);

      kb.set_alphabet(3);
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
      REQUIRE(kb.nr_rules() == 4);
      REQUIRE(kb.size() == 3);
    }

    TEST_CASE("Knuth-Bendix 06: infinite confluent fp semigroup 3",
        "[quick][knuth-bendix][fpsemigroup][06]") {
      REPORTER.set_report(true);

      std::cout << "What??" << &REPORTER << std::endl;

      KnuthBendix kb("012");
      kb.add_rule("01", "10");
      kb.add_rule("02", "20");
      kb.add_rule("00", "0");
      kb.add_rule("02", "0");
      kb.add_rule("20", "0");
      kb.add_rule("11", "11");
      kb.add_rule("12", "21");
      kb.add_rule("111", "1");
      kb.add_rule("12", "1");
      kb.add_rule("21", "1");
      kb.add_rule("0", "1");

      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_rules() == 4);
      REQUIRE(kb.nr_rules() == 4);
      //REQUIRE(kb.size() == 3);
      auto S = static_cast<Semigroup<KBE>*>(kb.isomorphic_non_fp_semigroup());
      REQUIRE(S->size() == 3);
      std::vector<std::string> elts(S->cbegin(), S->cend());
      REQUIRE(elts == std::vector<std::string>({"a", "b", "c"}));
    }
  }  // namespace knuth_bendix_fpsemigroup
}  // namespace libsemigroups


/*



TEST_CASE("Knuth-Bendix 07: finite non-confluent fp semigroup from wikipedia",
          "[quick][knuth-bendix][fpsemigroup][07]") {
  Knuth-Bendix kb;
  REPORTER.set_report(REPORT);
  kb.add_rule("000", "");
  kb.add_rule("111", "");
  kb.add_rule("010101", "");

  REQUIRE(!kb.confluent());
  kb.knuth_bendix();
  REQUIRE(kb.nr_rules() == 4);
  REQUIRE(kb.confluent());
}

TEST_CASE("Knuth-Bendix 08: Example 5.1 in Sims", "[quick][knuth-bendix][fpsemigroup][08]") {
  Knuth-Bendix kb;
  REPORTER.set_report(REPORT);
  kb.add_rule("ab", "");
  kb.add_rule("ba", "");
  kb.add_rule("cd", "");
  kb.add_rule("dc", "");
  kb.add_rule("ca", "ac");

  REQUIRE(!kb.confluent());
  kb.knuth_bendix();
  REQUIRE(kb.nr_rules() == 8);
  REQUIRE(kb.confluent());
}

TEST_CASE("Knuth-Bendix 09: Example 5.1 in Sims", "[quick][knuth-bendix][fpsemigroup][09]") {
  Knuth-Bendix kb("aAbB");
  REPORTER.set_report(REPORT);

  kb.add_rule("aA", "");
  kb.add_rule("Aa", "");
  kb.add_rule("bB", "");
  kb.add_rule("Bb", "");
  kb.add_rule("ba", "ab");

  REQUIRE(!kb.confluent());

  kb.knuth_bendix();
  REQUIRE(kb.nr_rules() == 8);
  REQUIRE(kb.confluent());
}

TEST_CASE("Knuth-Bendix 10: Example 5.3 in Sims", "[quick][knuth-bendix][fpsemigroup][10]") {
  Knuth-Bendix kb;
  REPORTER.set_report(REPORT);
  kb.add_rule("aa", "");
  kb.add_rule("bbb", "");
  kb.add_rule("ababab", "");

  REQUIRE(!kb.confluent());

  kb.knuth_bendix();
  REQUIRE(kb.nr_rules() == 6);
  REQUIRE(kb.confluent());
}

TEST_CASE("Knuth-Bendix 11: Example 5.4 in Sims", "[quick][knuth-bendix][fpsemigroup][11]") {
  Knuth-Bendix kb;
  REPORTER.set_report(REPORT);
  kb.add_rule("aa", "");
  kb.add_rule("bB", "");
  kb.add_rule("bbb", "");
  kb.add_rule("ababab", "");

  REQUIRE(!kb.confluent());

  kb.knuth_bendix();
  REQUIRE(kb.nr_rules() == 11);
  REQUIRE(kb.confluent());
}

TEST_CASE("Knuth-Bendix 12: Example 6.4 in Sims", "[quick][knuth-bendix][fpsemigroup][12]") {
  Knuth-Bendix kb;
  REPORTER.set_report(REPORT);
  kb.add_rule("aa", "");
  kb.add_rule("bc", "");
  kb.add_rule("bbb", "");
  kb.add_rule("ababababababab", "");
  kb.add_rule("abacabacabacabac", "");

  REQUIRE(!kb.confluent());
  kb.knuth_bendix();
  REQUIRE(kb.nr_rules() == 40);
  REQUIRE(kb.confluent());
}

TEST_CASE("Knuth-Bendix 13: Example 6.6 in Sims", "[extreme][knuth-bendix][fpsemigroup][13]")
{ Knuth-Bendix kb; REPORTER.set_report(true);

  kb.add_rule("aa", "");
  kb.add_rule("bc", "");
  kb.add_rule("bbb", "");
  kb.add_rule("ababababababab", "");
  kb.add_rule("abacabacabacabacabacabacabacabac", "");

  REQUIRE(!kb.confluent());

  kb.knuth_bendix_by_overlap_length();
  REQUIRE(kb.nr_rules() == 1026);
  REQUIRE(kb.confluent());
}

TEST_CASE("Knuth-Bendix 14: Chapter 10, Section 4 in NR",
          "[knuth-bendix][quick][fpsemigroup][14]") {
  Knuth-Bendix kb;
  REPORTER.set_report(REPORT);

  kb.add_rule("aaaa", "a");
  kb.add_rule("bbbb", "b");
  kb.add_rule("cccc", "c");
  kb.add_rule("abab", "aaa");
  kb.add_rule("bcbc", "bbb");

  REQUIRE(!kb.confluent());

  kb.knuth_bendix();
  REQUIRE(kb.nr_rules() == 31);
  REQUIRE(kb.confluent());
}

TEST_CASE("Knuth-Bendix 15: Sym(5) from Chapter 3, Proposition 1.1 in NR",
          "[knuth-bendix][quick][fpsemigroup][15]") {
  Knuth-Bendix kb;
  REPORTER.set_report(REPORT);
  kb.add_rule("aa", "");
  kb.add_rule("bbbbb", "");
  kb.add_rule("babababa", "");
  kb.add_rule("bB", "");
  kb.add_rule("Bb", "");
  kb.add_rule("BabBabBab", "");
  kb.add_rule("aBBabbaBBabb", "");
  kb.add_rule("aBBBabbbaBBBabbb", "");
  kb.add_rule("aA", "");
  kb.add_rule("Aa", "");

  REQUIRE(!kb.confluent());

  kb.knuth_bendix();
  REQUIRE(kb.nr_rules() == 4);
  REQUIRE(kb.confluent());
}

TEST_CASE("Knuth-Bendix 16: SL(2, 7) from Chapter 3, Proposition 1.5 in NR",
          "[quick][knuth-bendix][fpsemigroup][16]") {
  Knuth-Bendix kb;
  REPORTER.set_report(REPORT);
  kb.add_rule("aaaaaaa", "");
  kb.add_rule("bb", "ababab");
  kb.add_rule("bb", "aaaabaaaabaaaabaaaab");
  kb.add_rule("aA", "");
  kb.add_rule("Aa", "");
  kb.add_rule("bB", "");
  kb.add_rule("Bb", "");

  // kb.set_clear_stack_interval(10);

  REQUIRE(!kb.confluent());

  kb.knuth_bendix();
  REQUIRE(kb.nr_rules() == 152);
  REQUIRE(kb.confluent());
}

TEST_CASE("Knuth-Bendix 17: Bicyclic monoid", "[knuth-bendix][quick][fpsemigroup][17]") {
  Knuth-Bendix kb;
  REPORTER.set_report(REPORT);
  kb.add_rule("ab", "");

  REQUIRE(kb.confluent());
  kb.knuth_bendix();
  REQUIRE(kb.nr_rules() == 1);
  REQUIRE(kb.confluent());
}

TEST_CASE("Knuth-Bendix 18: Plactic monoid of degree 2 from Wikipedia",
          "[knuth-bendix][quick][fpsemigroup][18]") {
  Knuth-Bendix kb;
  REPORTER.set_report(REPORT);
  kb.add_rule("aba", "baa");
  kb.add_rule("bba", "bab");
  kb.add_rule("ac", "");
  kb.add_rule("ca", "");
  kb.add_rule("bc", "");
  kb.add_rule("cb", "");

  REQUIRE(!kb.confluent());

  kb.knuth_bendix();
  REQUIRE(kb.nr_rules() == 3);
  REQUIRE(kb.confluent());
}

TEST_CASE("Knuth-Bendix 19: Example before Chapter 7, Proposition 1.1 in NR",
          "[knuth-bendix][quick][fpsemigroup][19]") {
  Knuth-Bendix kb;
  REPORTER.set_report(REPORT);
  kb.add_rule("aa", "a");
  kb.add_rule("bb", "b");

  REQUIRE(kb.confluent());
  kb.knuth_bendix();
  REQUIRE(kb.nr_rules() == 2);
  REQUIRE(kb.confluent());
}

TEST_CASE("Knuth-Bendix 20: size 243, Chapter 7, Theorem 3.6 in NR",
          "[knuth-bendix][quick][fpsemigroup][20]") {
  Knuth-Bendix kb;
  REPORTER.set_report(REPORT);
  kb.add_rule("aaa", "a");
  kb.add_rule("bbbb", "b");
  kb.add_rule("abababab", "aa");

  REQUIRE(!kb.confluent());

  kb.knuth_bendix();
  REQUIRE(kb.nr_rules() == 9);
  REQUIRE(kb.confluent());
}

// See KBFP 07 also.

TEST_CASE("Knuth-Bendix 21: size 240, Chapter 7, Theorem 3.9 in NR",
          "[knuth-bendix][quick][fpsemigroup][21]") {
  Knuth-Bendix kb;
  REPORTER.set_report(REPORT);
  kb.add_rule("aaa", "a");
  kb.add_rule("bbbb", "b");
  kb.add_rule("abbba", "aa");
  kb.add_rule("baab", "bb");
  kb.add_rule("aabababababa", "aa");

  REQUIRE(!kb.confluent());
  kb.knuth_bendix();
  REQUIRE(kb.nr_rules() == 24);
  REQUIRE(kb.confluent());
}

TEST_CASE("Knuth-Bendix 22: F(2, 5); size 11, from Chapter 9, Section 1 in NR",
          "[knuth-bendix][quick][fpsemigroup][22]") {
  Knuth-Bendix kb;
  REPORTER.set_report(REPORT);
  kb.add_rule("ab", "c");
  kb.add_rule("bc", "d");
  kb.add_rule("cd", "e");
  kb.add_rule("de", "a");
  kb.add_rule("ea", "b");

  REQUIRE(!kb.confluent());
  kb.knuth_bendix();
  REQUIRE(kb.nr_rules() == 24);
  REQUIRE(kb.confluent());
}

TEST_CASE("Knuth-Bendix 23: F(2, 6); infinite, from Chapter 9, Section 1 in NR",
          "[knuth-bendix][quick][fpsemigroup][23]") {
  Knuth-Bendix kb;
  REPORTER.set_report(REPORT);
  kb.add_rule("ab", "");
  kb.add_rule("bc", "d");
  kb.add_rule("cd", "e");
  kb.add_rule("de", "f");
  kb.add_rule("ef", "a");
  kb.add_rule("fa", "b");

  REQUIRE(!kb.confluent());
  kb.knuth_bendix();
  REQUIRE(kb.nr_rules() == 35);
  REQUIRE(kb.confluent());
}

TEST_CASE("Knuth-Bendix 24: add_rule", "[quick][knuth-bendix][fpsemigroup][24]") {
kb.add_rule({0, 1}, {1, 0});
kb.add_rule({0, 2}, {2, 0});
kb.add_rule({0, 0}, {0});
kb.add_rule({0, 2}, {0});
kb.add_rule({2, 0}, {0});
kb.add_rule({1, 1}, {1, 1});
kb.add_rule({1, 2}, {2, 1});
kb.add_rule({1, 1, 1}, {1});
kb.add_rule({1, 2}, {1});
kb.add_rule({2, 1}, {1})};
  std::vector<relation_type> extra = {{{0}, {1}}};

  Knuth-Bendix kb;
  kb.add_rules(rels);
  kb.add_rules(extra);
  REPORTER.set_report(REPORT);
  REQUIRE(kb.confluent());
  // We could rewrite here and check equality by this is simpler since all
  // allocation and deletion is handled in test_equals
  REQUIRE(kb.test_equals(rels[3].first, rels[3].second));
  REQUIRE(kb.test_equals(rels[6].first, rels[6].second));
  REQUIRE(kb.test_equals(rels[7].first, rels[7].second));
  REQUIRE(kb.test_equals(word_type({1, 0}), word_type({2, 2, 0, 1, 2})));
  REQUIRE(kb.test_equals(word_type({2, 1}), word_type({1, 1, 1, 2})));
  REQUIRE(!kb.test_equals(word_type({1, 0}), word_type({2})));
}

TEST_CASE("Knuth-Bendix 25: Chapter 11, Section 1 (q = 4, r = 3) in NR",
          "[knuth-bendix][quick][fpsemigroup][25]") {
  Knuth-Bendix kb;
  REPORTER.set_report(REPORT);
  kb.add_rule("aaa", "a");
  kb.add_rule("bbbbb", "b");
  kb.add_rule("abbbabb", "bba");

  REQUIRE(!kb.confluent());
  kb.knuth_bendix_by_overlap_length();
  REQUIRE(kb.nr_rules() == 20);
  REQUIRE(kb.confluent());

  // Check that rewrite to a non-pointer argument does not rewrite its
argument std::string w = "aaa"; REQUIRE(kb.rewrite(w) == "a"); REQUIRE(w ==
"aaa");

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

  REQUIRE(!kb.test_less_than("abbbaabbba", "bbbbaa"));
  REQUIRE(!kb.test_less_than("abba", "abba"));

  // Call test_less_than without knuth_bendix first
  Knuth-Bendix kb2;
  REPORTER.set_report(REPORT);
  kb2.add_rule("aaa", "a");
  kb2.add_rule("bbbbb", "b");
  kb2.add_rule("abbbabb", "bba");
  REQUIRE(!kb2.test_less_than("abbbaabbba", "bbbbaa"));
}

TEST_CASE("Knuth-Bendix 26: Chapter 11, Section 1 (q = 8, r = 5) in NR",
          "[knuth-bendix][fpsemigroup][quick][26]") {
  Knuth-Bendix kb;
  REPORTER.set_report(REPORT);
  kb.add_rule("aaa", "a");
  kb.add_rule("bbbbbbbbb", "b");
  kb.add_rule("abbbbbabb", "bba");

  // kb.set_clear_stack_interval(0);

  REQUIRE(!kb.confluent());
  kb.knuth_bendix_by_overlap_length();
  REQUIRE(kb.nr_rules() == 105);
  REQUIRE(kb.confluent());

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

  REQUIRE(kb.test_less_than("aaa", "bbbbbbbbb"));
}

TEST_CASE("Knuth-Bendix 27: Chapter 11, Lemma 1.8 (q = 6, r = 5) in NR",
          "[knuth-bendix][quick][fpsemigroup][27]") {
  Knuth-Bendix kb;
  REPORTER.set_report(REPORT);
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
  kb.knuth_bendix();
  REQUIRE(kb.nr_rules() == 16);
  REQUIRE(kb.confluent());
}

TEST_CASE("Knuth-Bendix 28: Chapter 11, Section 2 (q = 6, r = 2, alpha = abaabba) in
NR",
          "[knuth-bendix][quick][fpsemigroup][28]") {
  Knuth-Bendix kb;
  REPORTER.set_report(REPORT);
  kb.add_rule("aaa", "a");
  kb.add_rule("bbbbbbb", "b");
  kb.add_rule("abaabba", "bb");

  REQUIRE(!kb.confluent());
  kb.knuth_bendix();
  REQUIRE(kb.nr_rules() == 4);
  REQUIRE(kb.confluent());
}

TEST_CASE("Knuth-Bendix 29: Chapter 8, Theorem 4.2 in NR",
          "[knuth-bendix][quick][fpsemigroup][29]") {
  Knuth-Bendix kb;
  REPORTER.set_report(REPORT);
  kb.add_rule("aaa", "a");
  kb.add_rule("bbbb", "b");
  kb.add_rule("bababababab", "b");
  kb.add_rule("baab", "babbbab");

  REQUIRE(!kb.confluent());
  kb.knuth_bendix();
  REQUIRE(kb.nr_rules() == 8);
  REQUIRE(kb.confluent());

  REQUIRE(!kb.test_less_than("bababababab", "aaaaa"));
  REQUIRE(kb.test_less_than("aaaaa", "bababababab"));
}

TEST_CASE("Knuth-Bendix 30: test_equals", "[quick][knuth-bendix][fpsemigroup][30]") {
  Knuth-Bendix kb;
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

  REQUIRE(kb.test_equals("aa", "a"));
  REQUIRE(kb.test_equals("bb", "bb"));
  REQUIRE(kb.test_equals("bc", "cb"));
  REQUIRE(kb.test_equals("ba", "ccabc"));
  REQUIRE(kb.test_equals("cb", "bbbc"));
  REQUIRE(!kb.test_equals("ba", "c"));
}

TEST_CASE("Knuth-Bendix 31: free semigroup", "[quick][knuth-bendix][smalloverlap][31]")
{ Congruence cong("twosided", 2, std::vector<relation_type>(),
                  std::vector<relation_type>());
  Knuth-Bendix        kb;
  kb.add_rules(cong.relations());
  kb.add_rules(cong.extra());

  REQUIRE(!kb.test_equals({0}, {1}));
  REQUIRE(kb.test_equals({0}, {0}));
  REQUIRE(kb.test_equals({0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}));
}

TEST_CASE("Knuth-Bendix 32: from GAP smalloverlap gap/test.gi:32",
          "[quick][knuth-bendix][smalloverlap][32]") {
  Knuth-Bendix kb;
  kb.add_rule("abcd", "ce");
  kb.add_rule("df", "dg");

  REQUIRE(!kb.confluent());

  REQUIRE(kb.test_equals("dfabcdf", "dfabcdg"));
  REQUIRE(kb.test_equals("abcdf", "ceg"));
  REQUIRE(kb.test_equals("abcdf", "cef"));

  kb.knuth_bendix();
  REQUIRE(kb.nr_rules() == 3);
  REQUIRE(kb.confluent());
  REQUIRE(kb.test_equals("dfabcdf", "dfabcdg"));
  REQUIRE(kb.test_equals("abcdf", "ceg"));
  REQUIRE(kb.test_equals("abcdf", "cef"));
}

TEST_CASE("Knuth-Bendix 33: from GAP smalloverlap gap/test.gi:49",
          "[quick][knuth-bendix][smalloverlap][33]") {
  Knuth-Bendix kb;
  kb.add_rule("abcd", "ce");
  kb.add_rule("df", "hd");

  REQUIRE(kb.confluent());

  REQUIRE(kb.test_equals("abchd", "abcdf"));
  REQUIRE(!kb.test_equals("abchf", "abcdf"));
  REQUIRE(kb.test_equals("abchd", "abchd"));
  REQUIRE(kb.test_equals("abchdf", "abchhd"));
  // Test cases (4) and (5)
  REQUIRE(kb.test_equals("abchd", "cef"));
  REQUIRE(kb.test_equals("cef", "abchd"));
}

TEST_CASE("Knuth-Bendix 34: from GAP smalloverlap gap/test.gi:63",
          "[quick][knuth-bendix][smalloverlap][34]") {
  Knuth-Bendix kb;
  kb.add_rule("afh", "bgh");
  kb.add_rule("hc", "d");

  REQUIRE(!kb.confluent());

  // Test case (6)
  REQUIRE(kb.test_equals("afd", "bgd"));

  kb.knuth_bendix();
  REQUIRE(kb.nr_rules() == 3);
}

TEST_CASE("Knuth-Bendix 35: from GAP smalloverlap gap/test.gi:70",
          "[quick][knuth-bendix][smalloverlap][35]") {
  // The following permits a more complex test of case (6), which also
  // involves using the case (2) code to change the prefix being looked for:
  Knuth-Bendix kb;
  kb.add_rule("afh", "bgh");
  kb.add_rule("hc", "de");
  kb.add_rule("ei", "j");

  REQUIRE(!kb.confluent());

  REQUIRE(kb.test_equals("afdj", "bgdj"));
  REQUIRE(!kb.test_equals("xxxxxxxxxxxxxxxxxxxxxxx", "b"));

  kb.knuth_bendix();
  REQUIRE(kb.nr_rules() == 5);
}

TEST_CASE("Knuth-Bendix 36: from GAP smalloverlap gap/test.gi:77",
          "[quick][knuth-bendix][smalloverlap][36]") {
  // A slightly more complicated presentation for testing case (6), in which
  // the max piece suffixes of the first two relation words no longer agree
  // (since fh and gh are now pieces).
  Knuth-Bendix kb;
  kb.add_rule("afh", "bgh");
  kb.add_rule("hc", "de");
  kb.add_rule("ei", "j");
  kb.add_rule("fhk", "ghl");

  REQUIRE(!kb.confluent());

  REQUIRE(kb.test_equals("afdj", "bgdj"));

  kb.knuth_bendix();
  REQUIRE(kb.nr_rules() == 7);
}

TEST_CASE("Knuth-Bendix 37: from GAP smalloverlap gap/test.gi:85 (knuth_bendix
fails)",
          "[broken][knuth-bendix][smalloverlap][37]") {
  Knuth-Bendix kb;
  kb.add_rule("aabc", "acba");

  REQUIRE(!kb.confluent());

  // TODO REPORTER.set_report(); in all the new examples

  REQUIRE(!kb.test_equals("a", "b"));
  REQUIRE(kb.test_equals("aabcabc", "aabccba"));

  // kb.knuth_bendix();
}

TEST_CASE("Knuth-Bendix 38: Von Dyck (2,3,7) group - infinite",
          "[quick][knuth-bendix][smalloverlap][kbmag][38]") {
  Knuth-Bendix kb;
  kb.add_rule("aaaa", "AAA");
  kb.add_rule("bb", "B");
  kb.add_rule("BA", "c");

  REQUIRE(!kb.confluent());
  REPORTER.set_report(REPORT);
  kb.knuth_bendix();

  REQUIRE(kb.nr_rules() == 6);
  REQUIRE(kb.confluent());
  REQUIRE(!kb.test_equals("a", "b"));
  REQUIRE(!kb.test_equals("aabcabc", "aabccba"));
}

// Does not finish knuth_bendix
TEST_CASE("Knuth-Bendix 39: Von Dyck (2,3,7) group - infinite - different
presentation",
          "[extreme][knuth-bendix][smalloverlap][kbmag][39]") {
  Knuth-Bendix kb;
  kb.add_rule("aaaa", "AAA");
  kb.add_rule("bb", "B");
  kb.add_rule("abababa", "BABABAB");
  kb.add_rule("BA", "c");

  REQUIRE(!kb.confluent());
  REPORTER.set_report(true);
  kb.set_overlap_measure(Knuth-Bendix::overlap_measure::max_AB_BC);
  kb.set_max_rules(100);
  kb.knuth_bendix();
  REQUIRE(kb.nr_rules() == 109);
  kb.knuth_bendix();
  REQUIRE(kb.nr_rules() == 109);
  kb.set_max_rules(250);
  kb.knuth_bendix();
  REQUIRE(kb.nr_rules() == 262);
}

TEST_CASE("Knuth-Bendix 40: rewriting system from KBP 08",
          "[quick][knuth-bendix][smalloverlap][kbmag][40]") {
  Knuth-Bendix kb;
  kb.add_rule("bbbbbbb", "b");
  kb.add_rule("ccccc", "c");
  kb.add_rule("bccba", "bccb");
  kb.add_rule("bccbc", "bccb");
  kb.add_rule("bbcbca", "bbcbc");
  kb.add_rule("bbcbcb", "bbcbc");

  REQUIRE(!kb.confluent());
  REQUIRE(kb.nr_rules() == 6);
  REPORTER.set_report(REPORT);
  kb.knuth_bendix();
  REQUIRE(kb.confluent());
  REQUIRE(kb.nr_rules() == 8);

  REQUIRE(kb.rule("bbbbbbb", "b"));
  REQUIRE(kb.rule("ccccc", "c"));
  REQUIRE(kb.rule("bccba", "bccb"));
  REQUIRE(kb.rule("bccbc", "bccb"));
  REQUIRE(kb.rule("bcbca", "bcbc"));
  REQUIRE(kb.rule("bcbcb", "bcbc"));
  REQUIRE(kb.rule("bcbcc", "bcbc"));
  REQUIRE(kb.rule("bccbb", "bccb"));
  // Wrong way around rule
  REQUIRE(kb.rule("bccb", "bccbb"));
  // Not a rule
  REQUIRE(!kb.rule("aaaa", "bccbb"));

  std::vector<std::pair<std::string, std::string>> rules = kb.rules();
  REQUIRE(rules[0] == std::pair<std::string, std::string>("bcbca", "bcbc"));
  REQUIRE(rules[1] == std::pair<std::string, std::string>("bcbcb", "bcbc"));
  REQUIRE(rules[2] == std::pair<std::string, std::string>("bcbcc", "bcbc"));
  REQUIRE(rules[3] == std::pair<std::string, std::string>("bccba", "bccb"));
  REQUIRE(rules[4] == std::pair<std::string, std::string>("bccbb", "bccb"));
  REQUIRE(rules[5] == std::pair<std::string, std::string>("bccbc", "bccb"));
  REQUIRE(rules[6] == std::pair<std::string, std::string>("ccccc", "c"));
  REQUIRE(rules[7] == std::pair<std::string, std::string>("bbbbbbb", "b"));
}

TEST_CASE("Knuth-Bendix 41: rewriting system from Congruence 20", "[quick][knuth-bendix][41]")
{ Knuth-Bendix kb; kb.add_rule("aaa", "a"); kb.add_rule("ab", "ba");
  kb.add_rule("aa", "a");
  kb.knuth_bendix();

  REQUIRE(kb.test_equals("abbbbbbbbbbbbbb", "aabbbbbbbbbbbbbb"));
}

// The next test meets the definition of a standard test but causes valgrind
on
// travis to timeout.
TEST_CASE("Knuth-Bendix 42: Example 6.6 in Sims (with limited overlap lengths)",
          "[extreme][knuth-bendix][fpsemigroup][42]") {
  Knuth-Bendix kb;
  REPORTER.set_report(REPORT);

  kb.add_rule("aa", "");
  kb.add_rule("bc", "");
  kb.add_rule("bbb", "");
  kb.add_rule("ababababababab", "");
  kb.add_rule("abacabacabacabacabacabacabacabac", "");

  REQUIRE(!kb.confluent());

  // In Sims it says to use 44 here, but that doesn't seem to work.
  kb.set_max_overlap(45);
  // Avoid checking confluence since this is very slow, essentially takes
the
  // same amount of time as running Knuth-Bendix 13.
  kb.set_check_confluence_interval(LIMIT_MAX);

  kb.knuth_bendix();
  REQUIRE(kb.nr_rules() == 1026);
}*/

// This example verifies the nilpotence of the group using the Sims
// algorithm. The original presentation was <a,b| [b,a,a,a], [b^-1,a,a,a],
// [a,b,b,b], [a^-1,b,b,b], [a,a*b,a*b,a*b], [a^-1,a*b,a*b,a*b] >. (where []
// mean left-normed commutators. The presentation here was derived by first
// applying the NQA to find the maximal nilpotent quotient, and then
// introducing new generators for the PCP generators.
/*TEST_CASE("Knuth-Bendix 43: (from kbmag/standalone/kb_data/heinnilp)",
          "[fails][knuth-bendix][kbmag][recursive][43]") {
  // TODO fails because internal_rewrite expect rules to be length reducing
  Knuth-Bendix kb(new RECURSIVE(), "fFyYdDcCbBaA");
  kb.add_rule("BAba", "c");
  kb.add_rule("CAca", "d");
  kb.add_rule("CBcb", "y");
  kb.add_rule("DBdb", "f");
  kb.add_rule("cBCb", "bcBC");
  kb.add_rule("babABaBA", "abABaBAb");
  kb.add_rule("cBACab", "abcBAC");
  kb.add_rule("BabABBAbab", "aabABBAb");
  REPORTER.set_report(REPORT);

  REQUIRE(!kb.confluent());

  kb.knuth_bendix();
  REQUIRE(kb.confluent());
  REQUIRE(kb.nr_rules() == 32767);
}*/

// Fibonacci group F(2,7) - order 29 - works better with largish tidyint
// knuth_bendix does not terminate
/* TEST_CASE("Knuth-Bendix 44: (from kbmag/standalone/kb_data/f27)",
          "[extreme][knuth-bendix][kbmag][shortlex][44]") {
  Knuth-Bendix kb("aAbBcCdDyYfFgG");
  kb.add_rule("ab", "c");
  kb.add_rule("bc", "d");
  kb.add_rule("cd", "y");
  kb.add_rule("dy", "f");
  kb.add_rule("yf", "g");
  kb.add_rule("fg", "a");
  kb.add_rule("ga", "b");
  REPORTER.set_report(REPORT);

  REQUIRE(!kb.confluent());

  kb.knuth_bendix_by_overlap_length();
  // Fails to terminate, or is very slow, with knuth_bendix
  REQUIRE(kb.confluent());
  REQUIRE(kb.nr_rules() == 47);
  // KBMAG does not terminate with this example :-)
}

// An extension of 2^6 be L32
// knuth_bendix/2 does not terminate
TEST_CASE("Knuth-Bendix 45: (from kbmag/standalone/kb_data/l32ext)",
          "[extreme][knuth-bendix][kbmag][shortlex][45]") {
  Knuth-Bendix kb("abB");
  kb.add_rule("aa", "");
  kb.add_rule("BB", "b");
  kb.add_rule("BaBaBaB", "abababa");
  kb.add_rule("aBabaBabaBabaBab", "BabaBabaBabaBaba");
  REPORTER.set_report(REPORT);

  REQUIRE(!kb.confluent());

  kb.knuth_bendix_by_overlap_length();
  REQUIRE(kb.confluent());
  REQUIRE(kb.nr_rules() == 32750);
}

// 2-generator free abelian group (with this ordering KB terminates - but no
// all)
TEST_CASE("Knuth-Bendix 46: (from kbmag/standalone/kb_data/ab2)",
          "[quick][knuth-bendix][kbmag][shortlex][46]") {
  Knuth-Bendix kb("aAbB");
  kb.add_rule("Bab", "a");
  REPORTER.set_report(REPORT);

  REQUIRE(kb.confluent());

  kb.knuth_bendix();
  REQUIRE(kb.confluent());
  REQUIRE(kb.nr_rules() == 1);

  REQUIRE(kb.rule("Bab", "a"));
}

// This group is actually D_22 (although it wasn't meant to be). All
generators
// are unexpectedly involutory.

// knuth_bendix/2 does not terminate with the given ordering, terminates
almost
// immediately with the standard order. TODO double check that something
isn't
// going wrong in the nonstandard alphabet case.
TEST_CASE("Knuth-Bendix 47: (from kbmag/standalone/kb_data/d22)",
          "[knuth-bendix][kbmag][shortlex][47]") {
  Knuth-Bendix kb("aAbBcCdDyYfF");
  kb.add_rule("aCAd", "");
  kb.add_rule("bfBY", "");
  kb.add_rule("cyCD", "");
  kb.add_rule("dFDa", "");
  kb.add_rule("ybYA", "");
  kb.add_rule("fCFB", "");
  REPORTER.set_report(REPORT);

  REQUIRE(!kb.confluent());

  kb.knuth_bendix_by_overlap_length();
  REQUIRE(kb.confluent());
  REQUIRE(kb.nr_rules() == 8);

  REQUIRE(kb.rule("bfBY", ""));
  REQUIRE(kb.rule("cyCD", ""));
  REQUIRE(kb.rule("ybYA", ""));
  REQUIRE(kb.rule("fCFB", ""));
  REQUIRE(kb.rule("CAd", "dFD"));
  REQUIRE(kb.rule("FDa", "aCA"));
  REQUIRE(kb.rule("adFD", ""));
  REQUIRE(kb.rule("daCA", ""));
}

// No generators - no anything!
TEST_CASE("Knuth-Bendix 48: (from kbmag/standalone/kb_data/degen1)",
          "[quick][knuth-bendix][kbmag][shortlex][48]") {
  Knuth-Bendix kb("");
  REPORTER.set_report(REPORT);

  REQUIRE(kb.confluent());

  kb.knuth_bendix();
  REQUIRE(kb.confluent());
  REQUIRE(kb.nr_rules() == 0);
}

// Symmetric group S_4

// knuth_bendix/2 fails to temrinate
TEST_CASE("Knuth-Bendix 49: (from kbmag/standalone/kb_data/s4)",
          "[extreme][knuth-bendix][kbmag][shortlex][49]") {
  Knuth-Bendix kb("abB");
  kb.add_rule("bb", "B");
  kb.add_rule("BaBa", "abab");
  REPORTER.set_report(REPORT);

  REQUIRE(!kb.confluent());

  kb.knuth_bendix_by_overlap_length();
  REQUIRE(kb.confluent());
  REQUIRE(kb.nr_rules() == 32767);
}*/

// This example verifies the nilpotence of the group using the Sims
// algorithm. The original presentation was <a,b| [b,a,b], [b,a,a,a,a],
// [b,a,a,a,b,a,a] >. (where [] mean left-normed commutators). The
// presentation here was derived by first applying the NQA to find the
// maximal nilpotent quotient, and then introducing new generators for the
// PCP generators. It is essential for success that reasonably low values of
// the maxstoredlen parameter are given.
/*TEST_CASE("Knuth-Bendix 50: (from kbmag/standalone/kb_data/verifynilp)",
          "[quick][knuth-bendix][kbmag][recursive][50]") {
  Knuth-Bendix kb(new RECURSIVE(), "hHgGfFyYdDcCbBaA");
  kb.add_rule("BAba", "c");
  kb.add_rule("CAca", "d");
  kb.add_rule("DAda", "y");
  kb.add_rule("YByb", "f");
  kb.add_rule("FAfa", "g");
  kb.add_rule("ga", "ag");
  kb.add_rule("GBgb", "h");
  kb.add_rule("cb", "bc");
  kb.add_rule("ya", "ay");
  REPORTER.set_report(REPORT);

  REQUIRE(kb.confluent());

  kb.knuth_bendix();
  REQUIRE(kb.confluent());
  REQUIRE(kb.nr_rules() == 9);

  REQUIRE(kb.rule("BAba", "c"));
  REQUIRE(kb.rule("CAca", "d"));
  REQUIRE(kb.rule("DAda", "y"));
  REQUIRE(kb.rule("YByb", "f"));
  REQUIRE(kb.rule("FAfa", "g"));
  REQUIRE(kb.rule("ga", "ag"));
  REQUIRE(kb.rule("GBgb", "h"));
  REQUIRE(kb.rule("cb", "bc"));
  REQUIRE(kb.rule("ya", "ay"));
}*/
