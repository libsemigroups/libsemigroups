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
#include "element.hpp"
#include "kbe.hpp"
#include "knuth-bendix.hpp"
#include "semigroup.hpp"

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

    TEST_CASE("Knuth-Bendix (fpsemi) 01: transformation semigroup (size 4)",
              "[quick][knuth-bendix][finite][01]") {
      REPORTER.set_report(REPORT);
      using Transf = Transf<2>::type;

      Semigroup<Transf> S({Transf({1, 0}), Transf({0, 0})});
      REQUIRE(S.size() == 4);
      REQUIRE(S.nr_rules() == 4);

      KnuthBendix kb(S);
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_rules() == 4);
      REQUIRE(kb.size() == 4);
    }

    TEST_CASE("Knuth-Bendix (fpsemi) 02: transformation semigroup (size 9)",
              "[quick][knuth-bendix][finite][02]") {
      REPORTER.set_report(REPORT);
      std::vector<Element*> gens
          = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
             new Transformation<u_int16_t>({0, 0, 0, 0, 0})};
      Semigroup<> S = Semigroup<>(gens);
      REQUIRE(S.size() == 9);
      REQUIRE(S.degree() == 5);
      REQUIRE(S.nr_rules() == 3);

      KnuthBendix kb(S);
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_rules() == 3);
      REQUIRE(kb.size() == 9);
      delete_gens(gens);
    }

    TEST_CASE("Knuth-Bendix (fpsemi) 03: transformation semigroup (size 88)",
              "[quick][knuth-bendix][finite][03]") {
      REPORTER.set_report(REPORT);
      std::vector<Element*> gens
          = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
             new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
      Semigroup<> S = Semigroup<>(gens);
      REQUIRE(S.size() == 88);
      REQUIRE(S.degree() == 5);
      REQUIRE(S.nr_rules() == 18);

      KnuthBendix kb(S);
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_rules() == 18);
      REQUIRE(kb.size() == 88);
      delete_gens(gens);
    }

    TEST_CASE("Knuth-Bendix (fpsemi) 04: confluent fp semigroup 1 (infinite)",
              "[quick][knuth-bendix][fpsemigroup][04]") {
      REPORTER.set_report(REPORT);

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
#ifdef LIBSEMIGROUPS_DEBUG
      REQUIRE(kb.alphabet() == "abc");
#endif
      REQUIRE(kb.nr_rules() == 4);
      REQUIRE(kb.normal_form({2, 0}) == word_type({0}));
      REQUIRE(kb.normal_form({0, 2}) == word_type({0}));
      REQUIRE(kb.equal_to(word_type({2, 0}), word_type({0})));
      REQUIRE(kb.equal_to(word_type({0, 2}), word_type({0})));
      REQUIRE(kb.normal_form("ac") == "a");
      // REQUIRE(kb.is_obviously_infinite());
    }

    TEST_CASE("Knuth-Bendix (fpsemi) 05: confluent fp semigroup 2 (infinite)",
              "[quick][knuth-bendix][05]") {
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
    }

    TEST_CASE("Knuth-Bendix (fpsemi) 06: confluent fp semigroup 3 (infinite)",
              "[quick][knuth-bendix][06]") {
      REPORTER.set_report(REPORT);

      KnuthBendix kb("012");
      REQUIRE(kb.alphabet() == "012");
      REQUIRE(kb.nr_rules() == 0);

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

      REQUIRE(kb.nr_rules() == 4);
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_rules() == 4);
      auto S = static_cast<Semigroup<KBE>*>(kb.isomorphic_non_fp_semigroup());

      // At this point only the generators are known
      REQUIRE(S->current_size() == 2);
      std::vector<std::string> v(S->cbegin(), S->cend());
      REQUIRE(v == std::vector<std::string>({"0", "2"}));

      S->set_batch_size(10);
      S->enumerate(10);
      REQUIRE(S->current_size() == 12);
      v.clear();
      v.insert(v.begin(), S->cbegin(), S->cend());
      REQUIRE(v.size() == S->current_size());
      REQUIRE(v
              == std::vector<std::string>({"0",
                                           "2",
                                           "22",
                                           "222",
                                           "2222",
                                           "22222",
                                           "222222",
                                           "2222222",
                                           "22222222",
                                           "222222222",
                                           "2222222222",
                                           "22222222222"}));
    }

    TEST_CASE("Knuth-Bendix (fpsemi) 07: non-confluent fp semigroup from "
              "wikipedia (infinite)",
              "[quick][knuth-bendix][07]") {
      REPORTER.set_report(REPORT);

      KnuthBendix kb("01");
      kb.add_rule("000", "");
      kb.add_rule("111", "");
      kb.add_rule("010101", "");

      REQUIRE(kb.alphabet() == "01");
      REQUIRE(!kb.confluent());
      kb.knuth_bendix();
      REQUIRE(kb.nr_rules() == 4);
      REQUIRE(kb.confluent());
    }

    TEST_CASE("Knuth-Bendix (fpsemi) 08: Example 5.1 in Sims (infinite)",
              "[quick][knuth-bendix][08]") {
      REPORTER.set_report(REPORT);

      KnuthBendix kb("abcd");
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

    TEST_CASE("Knuth-Bendix (fpsemi) 09: Example 5.1 in Sims (infinite)",
              "[quick][knuth-bendix][09]") {
      KnuthBendix kb("aAbB");
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

    TEST_CASE("Knuth-Bendix (fpsemi) 10: Example 5.3 in Sims (infinite)",
              "[quick][knuth-bendix][10]") {
      KnuthBendix kb("ab");
      REPORTER.set_report(REPORT);
      kb.add_rule("aa", "");
      kb.add_rule("bbb", "");
      kb.add_rule("ababab", "");

      REQUIRE(!kb.confluent());
      kb.knuth_bendix();
      REQUIRE(kb.nr_rules() == 6);
      REQUIRE(kb.confluent());
    }

    TEST_CASE("Knuth-Bendix (fpsemi) 11: Example 5.4 in Sims (infinite)",
              "[quick][knuth-bendix][11]") {
      KnuthBendix kb("Bab");
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

    TEST_CASE("Knuth-Bendix (fpsemi) 12: Example 6.4 in Sims (size 168)",
              "[quick][knuth-bendix][12]") {
      KnuthBendix kb("abc");
      REPORTER.set_report(REPORT);
      kb.add_rule("aa", "");
      kb.add_rule("bc", "");
      kb.add_rule("bbb", "");
      kb.add_rule("ababababababab", "");
      kb.add_rule("abacabacabacabac", "");

      REQUIRE(kb.alphabet() == "abc");
      REQUIRE(!kb.confluent());
      REQUIRE(!kb.is_obviously_infinite());
      REQUIRE(!kb.is_obviously_finite());
      kb.knuth_bendix();
      REQUIRE(kb.nr_rules() == 40);
      REQUIRE(kb.confluent());
      REQUIRE(kb.normal_form("cc") == "b");
      REQUIRE(kb.normal_form("ccc") == "");

      REQUIRE(kb.size() == 168);
      auto S = static_cast<Semigroup<KBE>*>(kb.isomorphic_non_fp_semigroup());
      REQUIRE(S->size() == 168);
      auto T = Semigroup<KBE>({S->generator(2)});
      REQUIRE(T.size() == 3);
    }

    TEST_CASE("Knuth-Bendix (fpsemi) 13: Example 6.6 in Sims",
              "[extreme][knuth-bendix][13]") {
      REPORTER.set_report(true);
      KnuthBendix kb("abc");

      kb.add_rule("aa", "");
      kb.add_rule("bc", "");
      kb.add_rule("bbb", "");
      kb.add_rule("ababababababab", "");
      kb.add_rule("abacabacabacabacabacabacabacabac", "");

      REQUIRE(!kb.confluent());
      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.nr_rules() == 1026);
      REQUIRE(kb.confluent());
      // TODO find size when compiled without debug mode
    }

    TEST_CASE(
        "Knuth-Bendix (fpsemi) 14: Chapter 10, Section 4 in NR (infinite)",
        "[knuth-bendix][quick][14]") {
      KnuthBendix kb("abc");
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

    // Note: the fourth relator in NR's thesis incorrectly has exponent 3, it
    // should be 2. With exponent 3, the presentation defines the trivial
    // group, with exponent of 2, it defines the symmetric group as desired.
    TEST_CASE("Knuth-Bendix (fpsemi) 15: Sym(5) from Chapter 3, Proposition "
              "1.1 in NR "
              "(size 120)",
              "[knuth-bendix][quick][15]") {
      KnuthBendix kb("ABab");
      REPORTER.set_report(REPORT);
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

      kb.knuth_bendix();
      REQUIRE(kb.nr_rules() == 36);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 120);
    }

    TEST_CASE("Knuth-Bendix (fpsemi) 16: SL(2, 7) from Chapter 3, Proposition "
              "1.5 in NR "
              "(size 336)",
              "[quick][knuth-bendix][16]") {
      REPORTER.set_report(REPORT);

      KnuthBendix kb("abAB");
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
      REQUIRE(kb.size() == 336);
    }

    TEST_CASE("Knuth-Bendix (fpsemi) 17: bicyclic monoid (infinite)",
              "[knuth-bendix][quick][17]") {
      REPORTER.set_report(REPORT);
      KnuthBendix kb("ab");
      kb.add_rule("ab", "");

      REQUIRE(kb.confluent());
      kb.knuth_bendix();
      REQUIRE(kb.nr_rules() == 1);
      REQUIRE(kb.confluent());
      REQUIRE(kb.is_obviously_infinite());
    }

    TEST_CASE("Knuth-Bendix (fpsemi) 18: plactic monoid of degree 2 (infinite)",
              "[knuth-bendix][quick][18]") {
      REPORTER.set_report(REPORT);
      KnuthBendix kb("abc");
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

    TEST_CASE("Knuth-Bendix (fpsemi) 19: example before Chapter 7, Proposition "
              "1.1 in "
              "NR (infinite)",
              "[knuth-bendix][quick][19]") {
      KnuthBendix kb("ab");
      REPORTER.set_report(REPORT);
      kb.add_rule("aa", "a");
      kb.add_rule("bb", "b");

      REQUIRE(kb.confluent());
      kb.knuth_bendix();
      REQUIRE(kb.nr_rules() == 2);
      REQUIRE(kb.confluent());
    }

    TEST_CASE(
        "Knuth-Bendix (fpsemi) 20: Chapter 7, Theorem 3.6 in NR (size 243)",
        "[knuth-bendix][quick][20]") {
      REPORTER.set_report(REPORT);
      KnuthBendix kb("ab");
      kb.add_rule("aaa", "a");
      kb.add_rule("bbbb", "b");
      kb.add_rule("ababababab", "aa");

      REQUIRE(!kb.confluent());

      kb.knuth_bendix();
      REQUIRE(kb.nr_rules() == 12);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 243);
    }

    TEST_CASE("Knuth-Bendix (fpsemi) 86: finite semigroup (size 99)",
              "[knuth-bendix][quick][86]") {
      REPORTER.set_report(REPORT);
      KnuthBendix kb("ab");
      kb.add_rule("aaa", "a");
      kb.add_rule("bbbb", "b");
      kb.add_rule("abababab", "aa");

      REQUIRE(!kb.confluent());

      kb.knuth_bendix();
      REQUIRE(kb.nr_rules() == 9);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 99);
    }

    // See KBFP 07 also. TODO remove this comment
    TEST_CASE(
        "Knuth-Bendix (fpsemi) 21: Chapter 7, Theorem 3.9 in NR (size 240)",
        "[knuth-bendix][quick][21]") {
      REPORTER.set_report(REPORT);
      KnuthBendix kb("ab");
      kb.add_rule("aaa", "a");
      kb.add_rule("bbbb", "b");
      kb.add_rule("abbba", "aa");
      kb.add_rule("baab", "bb");
      kb.add_rule("aabababababa", "aa");

      REQUIRE(!kb.confluent());
      kb.knuth_bendix();
      REQUIRE(kb.nr_rules() == 24);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 240);
    }

    TEST_CASE("Knuth-Bendix (fpsemi) 22: F(2, 5) - Chapter 9, Section 1 in NR "
              "(size 11)",
              "[knuth-bendix][quick][22]") {
      REPORTER.set_report(REPORT);
      KnuthBendix kb("abcde");
      kb.add_rule("ab", "c");
      kb.add_rule("bc", "d");
      kb.add_rule("cd", "e");
      kb.add_rule("de", "a");
      kb.add_rule("ea", "b");

      REQUIRE(!kb.confluent());
      kb.knuth_bendix();
      REQUIRE(kb.nr_rules() == 24);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 11);
    }

    TEST_CASE("Knuth-Bendix (fpsemi) 23: F(2, 6) - Chapter 9, Section 1 in NR "
              "(infinite)",
              "[knuth-bendix][quick][23]") {
      REPORTER.set_report(REPORT);
      KnuthBendix kb("abcdef");
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

    TEST_CASE("Knuth-Bendix (fpsemi) 24: fp semigroup (infinite)",
              "[quick][knuth-bendix][24]") {
      REPORTER.set_report(REPORT);
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
      // We could rewrite here and check equality by this is simpler since all
      // allocation and deletion is handled in equal_to
      REQUIRE(kb.equal_to(word_type({0, 0}), word_type({0})));
      REQUIRE(kb.equal_to(word_type({1, 1}), word_type({1, 1})));
      REQUIRE(kb.equal_to(word_type({1, 2}), word_type({2, 1})));
      REQUIRE(kb.equal_to(word_type({1, 0}), word_type({2, 2, 0, 1, 2})));
      REQUIRE(kb.equal_to(word_type({2, 1}), word_type({1, 1, 1, 2})));
      REQUIRE(!kb.equal_to(word_type({1, 0}), word_type({2})));
    }

    TEST_CASE("Knuth-Bendix (fpsemi) 25: Chapter 11, Section 1 (q = 4, r = 3) "
              "in NR (size 86)",
              "[knuth-bendix][quick][25]") {
      REPORTER.set_report(REPORT);

      KnuthBendix kb("ab");
      kb.add_rule("aaa", "a");
      kb.add_rule("bbbbb", "b");
      kb.add_rule("abbbabb", "bba");

      REQUIRE(!kb.confluent());
      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.nr_rules() == 20);
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

      // TODO add to the congruence tests for KnuthBendix
      //      // REQUIRE(!kb.test_less_than("abbbaabbba", "bbbbaa"));
      //      // REQUIRE(!kb.test_less_than("abba", "abba"));

      //      // Call test_less_than without knuth_bendix first
      // KnuthBendix kb2;
      // REPORTER.set_report(REPORT);
      // kb2.add_rule("aaa", "a");
      // kb2.add_rule("bbbbb", "b");
      // kb2.add_rule("abbbabb", "bba");
      //      // REQUIRE(!kb2.test_less_than("abbbaabbba", "bbbbaa"));
    }

    TEST_CASE(
        "Knuth-Bendix (fpsemi) 26: Chapter 11, Section 1 (q = 8, r = 5) in NR "
        "(size 746)",
        "[knuth-bendix][quick][26]") {
      REPORTER.set_report(REPORT);
      KnuthBendix kb("ab");
      kb.add_rule("aaa", "a");
      kb.add_rule("bbbbbbbbb", "b");
      kb.add_rule("abbbbbabb", "bba");

      // kb.set_clear_stack_interval(0);

      REQUIRE(!kb.confluent());
      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.nr_rules() == 105);
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
      // TODO add to congruence test
      //      // REQUIRE(kb.test_less_than("aaa", "bbbbbbbbb"));
    }

    TEST_CASE(
        "Knuth-Bendix (fpsemi) 27: Chapter 11, Lemma 1.8 (q = 6, r = 5) in NR "
        "(infinite)",
        "[knuth-bendix][quick][27]") {
      REPORTER.set_report(REPORT);
      KnuthBendix kb("ABCabc");
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

    TEST_CASE("Knuth-Bendix (fpsemi) 28: Chapter 11, Section 2 (q = 6, r = 2, "
              "alpha = "
              "abaabba) in NR (size 4)",
              "[knuth-bendix][quick][28]") {
      REPORTER.set_report(REPORT);
      KnuthBendix kb("ab");
      kb.add_rule("aaa", "a");
      kb.add_rule("bbbbbbb", "b");
      kb.add_rule("abaabba", "bb");

      REQUIRE(!kb.confluent());
      kb.knuth_bendix();
      REQUIRE(kb.nr_rules() == 4);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 4);
    }

    TEST_CASE(
        "Knuth-Bendix (fpsemi) 29: Chapter 8, Theorem 4.2 in NR (infinite)",
        "[knuth-bendix][quick][29]") {
      REPORTER.set_report(REPORT);
      KnuthBendix kb("ab");
      kb.add_rule("aaa", "a");
      kb.add_rule("bbbb", "b");
      kb.add_rule("bababababab", "b");
      kb.add_rule("baab", "babbbab");

      REQUIRE(!kb.confluent());
      kb.knuth_bendix();
      REQUIRE(kb.nr_rules() == 8);
      REQUIRE(kb.confluent());

      //  REQUIRE(!kb.test_less_than("bababababab", "aaaaa"));
      //  REQUIRE(kb.test_less_than("aaaaa", "bababababab"));
    }

    TEST_CASE("Knuth-Bendix (fpsemi) 30: equal_to fp semigroup",
              "[quick][knuth-bendix][30]") {
      REPORTER.set_report(REPORT);
      KnuthBendix kb("abc");
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
    }

    TEST_CASE("Knuth-Bendix (fpsemi) 31: equal_to free semigroup",
              "[quick][knuth-bendix][smalloverlap][31]") {
      REPORTER.set_report(REPORT);
      KnuthBendix kb(2);
      REQUIRE(!kb.equal_to(word_type({0}), word_type({1})));
      REQUIRE(kb.equal_to(word_type({0}), word_type({0})));
      REQUIRE(kb.equal_to(word_type({0, 0, 0, 0, 0, 0, 0}),
                          word_type({0, 0, 0, 0, 0, 0, 0})));
    }

    TEST_CASE("Knuth-Bendix (fpsemi) 32: from GAP smalloverlap gap/test.gi:32 "
              "(infinite)",
              "[quick][knuth-bendix][smalloverlap][32]") {
      REPORTER.set_report(REPORT);
      KnuthBendix kb("abcdefg");
      kb.add_rule("abcd", "ce");
      kb.add_rule("df", "dg");

      REQUIRE(kb.is_obviously_infinite());
      REQUIRE(!kb.confluent());

      REQUIRE(kb.equal_to("dfabcdf", "dfabcdg"));
      REQUIRE(kb.equal_to("abcdf", "ceg"));
      REQUIRE(kb.equal_to("abcdf", "cef"));

      kb.knuth_bendix();
      REQUIRE(kb.nr_rules() == 3);
      REQUIRE(kb.confluent());
      REQUIRE(kb.equal_to("dfabcdf", "dfabcdg"));
      REQUIRE(kb.equal_to("abcdf", "ceg"));
      REQUIRE(kb.equal_to("abcdf", "cef"));
    }

    TEST_CASE("Knuth-Bendix (fpsemi) 33: from GAP smalloverlap gap/test.gi:49 "
              "(infinite)",
              "[quick][knuth-bendix][smalloverlap][33]") {
      REPORTER.set_report(REPORT);
      KnuthBendix kb("abcdefgh");
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
    }

    TEST_CASE("Knuth-Bendix (fpsemi) 34: from GAP smalloverlap gap/test.gi:63 "
              "(infinite)",
              "[quick][knuth-bendix][smalloverlap][34]") {
      REPORTER.set_report(REPORT);
      KnuthBendix kb("abcdefgh");
      kb.add_rule("afh", "bgh");
      kb.add_rule("hc", "d");

      REQUIRE(kb.is_obviously_infinite());
      REQUIRE(!kb.confluent());

      // Test case (6)
      REQUIRE(kb.equal_to("afd", "bgd"));

      kb.knuth_bendix();
      REQUIRE(kb.nr_rules() == 3);
    }

    TEST_CASE("Knuth-Bendix (fpsemi) 35: from GAP smalloverlap gap/test.gi:70 "
              "(infinite)",
              "[quick][knuth-bendix][smalloverlap][35]") {
      REPORTER.set_report(REPORT);
      // The following permits a more complex test of case (6), which also
      // involves using the case (2) code to change the prefix being looked for:
      KnuthBendix kb("abcdefghij");
      kb.add_rule("afh", "bgh");
      kb.add_rule("hc", "de");
      kb.add_rule("ei", "j");

      REQUIRE(kb.is_obviously_infinite());
      REQUIRE(!kb.confluent());

      REQUIRE(kb.equal_to("afdj", "bgdj"));
      REQUIRE_THROWS_AS(!kb.equal_to("xxxxxxxxxxxxxxxxxxxxxxx", "b"),
                        LibsemigroupsException);

      kb.knuth_bendix();
      REQUIRE(kb.nr_rules() == 5);
    }

    TEST_CASE("Knuth-Bendix (fpsemi) 36: from GAP smalloverlap gap/test.gi:77 "
              "(infinite)",
              "[quick][knuth-bendix][smalloverlap][36]") {
      REPORTER.set_report(REPORT);
      // A slightly more complicated presentation for testing case (6), in which
      // the max piece suffixes of the first two relation words no longer agree
      // (since fh and gh are now pieces).
      KnuthBendix kb("abcdefghijkl");
      kb.add_rule("afh", "bgh");
      kb.add_rule("hc", "de");
      kb.add_rule("ei", "j");
      kb.add_rule("fhk", "ghl");

      REQUIRE(kb.is_obviously_infinite());
      REQUIRE(!kb.confluent());

      REQUIRE(kb.equal_to("afdj", "bgdj"));

      kb.knuth_bendix();
      REQUIRE(kb.nr_rules() == 7);
    }

    // TODO check that this is really confluent, with different alphabet
    // ordering knuth_bendix runs forever.
    TEST_CASE("Knuth-Bendix (fpsemi) 37: from GAP smalloverlap gap/test.gi:85 "
              "(infinite)",
              "[quick][knuth-bendix][smalloverlap][37]") {
      REPORTER.set_report(REPORT);
      KnuthBendix kb("cab");
      kb.add_rule("aabc", "acba");

      REQUIRE(kb.is_obviously_infinite());
      REQUIRE(kb.confluent());

      REQUIRE(!kb.equal_to("a", "b"));
      REQUIRE(kb.equal_to("aabcabc", "aabccba"));

      kb.knuth_bendix();
      REQUIRE(kb.nr_rules() == 1);
      REQUIRE(kb.size() == POSITIVE_INFINITY);
    }

    TEST_CASE("Knuth-Bendix (fpsemi) 38: Von Dyck (2,3,7) group (infinite)",
              "[quick][knuth-bendix][smalloverlap][kbmag][38]") {
      KnuthBendix kb("ABabc");
      kb.add_rule("aaaa", "AAA");
      kb.add_rule("bb", "B");
      kb.add_rule("BA", "c");

      REQUIRE(!kb.confluent());
      REPORTER.set_report(REPORT);
      kb.knuth_bendix();

      REQUIRE(kb.nr_rules() == 6);
      REQUIRE(kb.confluent());
      REQUIRE(!kb.equal_to("a", "b"));
      REQUIRE(!kb.equal_to("aabcabc", "aabccba"));
    }

    // Does not finish knuth_bendix
    TEST_CASE("Knuth-Bendix (fpsemi) 39: Von Dyck (2,3,7) group - different "
              "presentation (infinite)",
              "[quick][knuth-bendix][smalloverlap][kbmag][39]") {
      REPORTER.set_report(REPORT);
      KnuthBendix kb("ABabc");
      kb.add_rule("aaaa", "AAA");
      kb.add_rule("bb", "B");
      kb.add_rule("abababa", "BABABAB");
      kb.add_rule("BA", "c");

      REQUIRE(!kb.confluent());
      kb.set_overlap_policy(KnuthBendix::overlap_policy::max_AB_BC);
      kb.set_max_rules(100);
      kb.knuth_bendix();
      REQUIRE(kb.nr_rules() == 101);
      kb.knuth_bendix();
      REQUIRE(kb.nr_rules() == 101);
      kb.set_max_rules(250);
      kb.knuth_bendix();
      REQUIRE(kb.nr_rules() == 255);
      // kb.set_max_rules(POSITIVE_INFINITY);
      // kb.knuth_bendix();
    }

    TEST_CASE("Knuth-Bendix (fpsemi) 40: rewriting system from KBP 08",
              "[quick][knuth-bendix][smalloverlap][kbmag][40]") {
      REPORTER.set_report(REPORT);

      KnuthBendix kb("abc");
      kb.add_rule("bbbbbbb", "b");
      kb.add_rule("ccccc", "c");
      kb.add_rule("bccba", "bccb");
      kb.add_rule("bccbc", "bccb");
      kb.add_rule("bbcbca", "bbcbc");
      kb.add_rule("bbcbcb", "bbcbc");

      REQUIRE(!kb.confluent());
      REQUIRE(kb.nr_rules() == 6);
      kb.knuth_bendix();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_rules() == 8);

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

      using rule_type = std::pair<std::string, std::string>;

      std::vector<rule_type> rules = kb.rules();
      REQUIRE(rules[0] == rule_type("bcbca", "bcbc"));
      REQUIRE(rules[1] == rule_type("bcbcb", "bcbc"));
      REQUIRE(rules[2] == rule_type("bcbcc", "bcbc"));
      REQUIRE(rules[3] == rule_type("bccba", "bccb"));
      REQUIRE(rules[4] == rule_type("bccbb", "bccb"));
      REQUIRE(rules[5] == rule_type("bccbc", "bccb"));
      REQUIRE(rules[6] == rule_type("ccccc", "c"));
      REQUIRE(rules[7] == rule_type("bbbbbbb", "b"));
    }
    TEST_CASE("Knuth-Bendix (fpsemi) 41: rewriting system from Congruence 20",
              "[quick][knuth-bendix][41]") {
      KnuthBendix kb("ab");
      kb.add_rule("aaa", "a");
      kb.add_rule("ab", "ba");
      kb.add_rule("aa", "a");
      kb.knuth_bendix();

      REQUIRE(kb.equal_to("abbbbbbbbbbbbbb", "aabbbbbbbbbbbbbb"));
    }

    // The next test meets the definition of a standard test but causes valgrind
    // on travis to timeout.
    TEST_CASE("Knuth-Bendix (fpsemi) 42: Example 6.6 in Sims (with limited "
              "overlap lengths)",
              "[extreme][knuth-bendix][42]") {
      REPORTER.set_report(REPORT);
      KnuthBendix kb("abc");

      kb.add_rule("aa", "");
      kb.add_rule("bc", "");
      kb.add_rule("bbb", "");
      kb.add_rule("ababababababab", "");
      kb.add_rule("abacabacabacabacabacabacabacabac", "");

      REQUIRE(!kb.confluent());

      // In Sims it says to use 44 here, but that doesn't seem to work.
      kb.set_max_overlap(45);
      // Avoid checking confluence since this is very slow, essentially takes
      // the same amount of time as running Knuth-Bendix (fpsemi) 13.
      kb.set_check_confluence_interval(LIMIT_MAX);

      kb.knuth_bendix();
      REQUIRE(kb.nr_rules() == 1026);
    }

    // This example verifies the nilpotence of the group using the Sims
    // algorithm. The original presentation was <a,b| [b,a,a,a], [b^-1,a,a,a],
    // [a,b,b,b], [a^-1,b,b,b], [a,a*b,a*b,a*b], [a^-1,a*b,a*b,a*b] >. (where []
    // mean left-normed commutators. The presentation here was derived by first
    // applying the NQA to find the maximal nilpotent quotient, and then
    // introducing new generators for the PCP generators.
    // TEST_CASE("Knuth-Bendix (fpsemi) 43: (from
    // kbmag/standalone/kb_data/heinnilp)",
    //           "[fails][knuth-bendix][kbmag][recursive][43]") {
    //   // TODO fails because internal_rewrite expect rules to be length
    //   reducing KnuthBendix kb(new RECURSIVE(), "fFyYdDcCbBaA");
    //   kb.add_rule("BAba", "c");
    //   kb.add_rule("CAca", "d");
    //   kb.add_rule("CBcb", "y");
    //   kb.add_rule("DBdb", "f");
    //   kb.add_rule("cBCb", "bcBC");
    //   kb.add_rule("babABaBA", "abABaBAb");
    //   kb.add_rule("cBACab", "abcBAC");
    //   kb.add_rule("BabABBAbab", "aabABBAb");
    //   REPORTER.set_report(REPORT);

    //   REQUIRE(!kb.confluent());

    //   kb.knuth_bendix();
    //   REQUIRE(kb.confluent());
    //   REQUIRE(kb.nr_rules() == 32767);
    // }

    // Fibonacci group F(2,7) - order 29 - works better with largish tidyint
    // knuth_bendix does not terminate
    TEST_CASE("Knuth-Bendix (fpsemi) 44: (from kbmag/standalone/kb_data/f27) "
              "(infinite)",
              "[extreme][knuth-bendix][kbmag][shortlex][44]") {
      REPORTER.set_report(true);
      KnuthBendix kb("aAbBcCdDyYfFgG");
      kb.add_rule("ab", "c");
      kb.add_rule("bc", "d");
      kb.add_rule("cd", "y");
      kb.add_rule("dy", "f");
      kb.add_rule("yf", "g");
      kb.add_rule("fg", "a");
      kb.add_rule("ga", "b");

      REQUIRE(!kb.confluent());

      kb.knuth_bendix_by_overlap_length();
      // Fails to terminate, or is very slow, with knuth_bendix
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_rules() == 47);
      // KBMAG does not terminate with this example :-)
      // TODO Add inverses here, maybe this is why we terminate but KBMAG does
      // not, i.e. the two presentations are not the same.
    }

    // An extension of 2^6 be L32
    // knuth_bendix/2 does not terminate
    TEST_CASE(
        "Knuth-Bendix (fpsemi) 45: (from kbmag/standalone/kb_data/l32ext)",
        "[fails][knuth-bendix][kbmag][shortlex][45]") {
      REPORTER.set_report(true);
      KnuthBendix kb("abB");
      kb.add_rule("aa", "");
      kb.add_rule("BB", "b");
      kb.add_rule("BaBaBaB", "abababa");
      kb.add_rule("aBabaBabaBabaBab", "BabaBabaBabaBaba");

      REQUIRE(!kb.confluent());

      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_rules() == 32750);
    }

    // 2-generator free abelian group (with this ordering KB terminates - but no
    // all)
    TEST_CASE("Knuth-Bendix (fpsemi) 46: (from kbmag/standalone/kb_data/ab2)",
              "[quick][knuth-bendix][kbmag][shortlex][46]") {
      REPORTER.set_report(REPORT);
      KnuthBendix kb("aAbB");
      kb.add_rule("Bab", "a");

      REQUIRE(kb.confluent());

      kb.knuth_bendix();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_rules() == 1);

      REQUIRE(kb.equal_to("Bab", "a"));
    }

    // This group is actually D_22 (although it wasn't meant to be). All
    // generators are unexpectedly involutory.

    // knuth_bendix/2 does not terminate with the given ordering, terminates
    // almost immediately with the standard order.
    // TODO double check that something isn't going wrong in the nonstandard
    // alphabet case.
    // TODO Change this to a group presentation.
    TEST_CASE("Knuth-Bendix (fpsemi) 47: (from kbmag/standalone/kb_data/d22) "
              "(infinite)",
              "[quick][knuth-bendix][kbmag][shortlex][47]") {
      // KnuthBendix kb("aAbBcCdDyYfF");
      KnuthBendix kb("ABCDYFabcdyf");
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

      REQUIRE(kb.equal_to("bfBY", ""));
      REQUIRE(kb.equal_to("cyCD", ""));
      REQUIRE(kb.equal_to("ybYA", ""));
      REQUIRE(kb.equal_to("fCFB", ""));
      REQUIRE(kb.equal_to("CAd", "dFD"));
      REQUIRE(kb.equal_to("FDa", "aCA"));
      REQUIRE(kb.equal_to("adFD", ""));
      REQUIRE(kb.equal_to("daCA", ""));

      REQUIRE(kb.size() == POSITIVE_INFINITY);
    }

    // No generators - no anything!
    // JDM: arguably this should throw an exception
    TEST_CASE(
        "Knuth-Bendix (fpsemi) 48: (from kbmag/standalone/kb_data/degen1)",
        "[quick][knuth-bendix][kbmag][shortlex][48]") {
      KnuthBendix kb;
      REPORTER.set_report(REPORT);

      REQUIRE(kb.confluent());

      kb.knuth_bendix();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_rules() == 0);
    }

    // knuth_bendix/2 fails to temrinate
    // Symmetric group S_4
    TEST_CASE("Knuth-Bendix (fpsemi) 49: (from kbmag/standalone/kb_data/s4)",
              "[fails][knuth-bendix][kbmag][shortlex][49]") {
      KnuthBendix kb("abB");
      kb.add_rule("bb", "B");
      kb.add_rule("BaBa", "abab");
      REPORTER.set_report(REPORT);

      REQUIRE(!kb.confluent());

      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_rules() == 32767);
    }

    // This example verifies the nilpotence of the group using the Sims
    // algorithm. The original presentation was <a,b| [b,a,b], [b,a,a,a,a],
    // [b,a,a,a,b,a,a] >. (where [] mean left-normed commutators). The
    // presentation here was derived by first applying the NQA to find the
    // maximal nilpotent quotient, and then introducing new generators for the
    // PCP generators. It is essential for success that reasonably low values of
    // the maxstoredlen parameter are given.
    // TEST_CASE("Knuth-Bendix (fpsemi) 50: (from
    // kbmag/standalone/kb_data/verifynilp)",
    //           "[quick][knuth-bendix][kbmag][recursive][50]") {
    //   KnuthBendix kb(new RECURSIVE(), "hHgGfFyYdDcCbBaA");
    //   kb.add_rule("BAba", "c");
    //   kb.add_rule("CAca", "d");
    //   kb.add_rule("DAda", "y");
    //   kb.add_rule("YByb", "f");
    //   kb.add_rule("FAfa", "g");
    //   kb.add_rule("ga", "ag");
    //   kb.add_rule("GBgb", "h");
    //   kb.add_rule("cb", "bc");
    //   kb.add_rule("ya", "ay");
    //   REPORTER.set_report(REPORT);
    //
    //   REQUIRE(kb.confluent());
    //
    //   kb.knuth_bendix();
    //   REQUIRE(kb.confluent());
    //   REQUIRE(kb.nr_rules() == 9);
    //
    //   REQUIRE(kb.rule("BAba", "c"));
    //   REQUIRE(kb.rule("CAca", "d"));
    //   REQUIRE(kb.rule("DAda", "y"));
    //   REQUIRE(kb.rule("YByb", "f"));
    //   REQUIRE(kb.rule("FAfa", "g"));
    //   REQUIRE(kb.rule("ga", "ag"));
    //   REQUIRE(kb.rule("GBgb", "h"));
    //   REQUIRE(kb.rule("cb", "bc"));
    //   REQUIRE(kb.rule("ya", "ay"));
    // }
  }  // namespace knuth_bendix_fpsemigroup

  namespace knuth_bendix_congruence {
    using KnuthBendix = congruence::KnuthBendix;

    constexpr bool REPORT = false;

    TEST_CASE("Knuth-Bendix (cong) 01: free semigroup congruence (5 classes)",
              "[quick][congruence][knuth-bendix][congruence][01]") {
      REPORTER.set_report(REPORT);

      KnuthBendix kb;
      kb.set_nr_generators(2);
      kb.add_pair({0, 0, 0}, {0});
      kb.add_pair({0}, {1, 1});

      REQUIRE(!kb.finished());
      REQUIRE(kb.nr_classes() == 5);
      REQUIRE(kb.finished());

      REQUIRE(kb.word_to_class_index({0, 0, 1}) == 4);
      REQUIRE(kb.word_to_class_index({0, 0, 0, 0, 1}) == 4);
      REQUIRE(kb.word_to_class_index({0, 1, 1, 0, 0, 1}) == 4);
      REQUIRE(kb.word_to_class_index({0, 0, 0}) == 0);
      REQUIRE(kb.word_to_class_index({1}) == 1);
      REQUIRE(kb.word_to_class_index({0, 0, 0, 0}) == 2);
    }

    // Knuth-Bendix (cong) 02 was identical to Knuth-Bendix (cong)
    // 01, so removed it

    TEST_CASE("Knuth-Bendix (cong) 03: finite transformation semigroup "
              "congruence (21 classes)",
              "[quick][congruence][knuth-bendix][03]") {
      REPORTER.set_report(REPORT);
      using Transf = Transf<5>::type;
      Semigroup<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});

      REQUIRE(S.size() == 88);
      REQUIRE(S.nr_rules() == 18);

      KnuthBendix kb(S);
      auto        P = kb.quotient_semigroup();  // Parent
      REQUIRE(P == &S);                         // Pointers the same
      kb.add_pair(S.factorisation(Transf({3, 4, 4, 4, 4})),
                  S.factorisation(Transf({3, 1, 3, 3, 3})));

      REQUIRE(kb.nr_classes() == 21);
      REQUIRE(kb.nr_classes() == 21);
      auto Q = kb.quotient_semigroup();  // quotient

      REQUIRE(P != Q);
      REQUIRE(Q->size() == 21);
      REQUIRE(Q->nr_idempotents() == 3);

      std::vector<std::string> v(static_cast<Semigroup<KBE>*>(Q)->cbegin(),
                                 static_cast<Semigroup<KBE>*>(Q)->cend());
      REQUIRE(v
              == std::vector<std::string>(
                     {"a",     "b",     "aa",    "ab",   "ba",   "bb",
                      "aaa",   "aab",   "aba",   "abb",  "baa",  "bba",
                      "aaaa",  "abaa",  "abba",  "baaa", "bbaa", "abaaa",
                      "abbaa", "bbaaa", "abbaaa"}));

      REQUIRE(
          kb.word_to_class_index(S.factorisation(Transf({1, 3, 1, 3, 3})))
          == kb.word_to_class_index(S.factorisation(Transf({4, 2, 4, 4, 2}))));

      REQUIRE(kb.nr_non_trivial_classes() == 1);
      REQUIRE(kb.nr_generators() == 2);
      REQUIRE(kb.cbegin_ntc()->size() == 68);
    }

    TEST_CASE("KnuthBendix (cong) 04: free semigroup congruence (6 classes)"
              "[quick][congruence][knuth-bendix][04]") {
      REPORTER.set_report(REPORT);
      KnuthBendix kb;
      kb.set_nr_generators(5);
      kb.add_pair({0, 0}, {0});
      kb.add_pair({0, 1}, {1});
      kb.add_pair({1, 0}, {1});
      kb.add_pair({0, 2}, {2});
      kb.add_pair({2, 0}, {2});
      kb.add_pair({0, 3}, {3});
      kb.add_pair({3, 0}, {3});
      kb.add_pair({0, 4}, {4});
      kb.add_pair({4, 0}, {4});
      kb.add_pair({1, 2}, {0});
      kb.add_pair({2, 1}, {0});
      kb.add_pair({3, 4}, {0});
      kb.add_pair({4, 3}, {0});
      kb.add_pair({2, 2}, {0});
      kb.add_pair({1, 4, 2, 3, 3}, {0});
      kb.add_pair({4, 4, 4}, {0});

      REQUIRE(kb.nr_classes() == 6);
      // Throws because there's no parent semigroup
      REQUIRE_THROWS_AS(kb.nr_non_trivial_classes(), LibsemigroupsException);
      REQUIRE(kb.word_to_class_index({1}) == kb.word_to_class_index({2}));
    }

    TEST_CASE("Knuth-Bendix (cong) 05: free semigroup congruence (16 classes)",
              "[quick][congruence][knuth-bendix][05]") {
      REPORTER.set_report(REPORT);
      KnuthBendix kb;
      kb.set_nr_generators(4);
      kb.add_pair({3}, {2});
      kb.add_pair({0, 3}, {0, 2});
      kb.add_pair({1, 1}, {1});
      kb.add_pair({1, 3}, {1, 2});
      kb.add_pair({2, 1}, {2});
      kb.add_pair({2, 2}, {2});
      kb.add_pair({2, 3}, {2});
      kb.add_pair({0, 0, 0}, {0});
      kb.add_pair({0, 0, 1}, {1});
      kb.add_pair({0, 0, 2}, {2});
      kb.add_pair({0, 1, 2}, {1, 2});
      kb.add_pair({1, 0, 0}, {1});
      kb.add_pair({1, 0, 2}, {0, 2});
      kb.add_pair({2, 0, 0}, {2});
      kb.add_pair({0, 1, 0, 1}, {1, 0, 1});
      kb.add_pair({0, 2, 0, 2}, {2, 0, 2});
      kb.add_pair({1, 0, 1, 0}, {1, 0, 1});
      kb.add_pair({1, 2, 0, 1}, {1, 0, 1});
      kb.add_pair({1, 2, 0, 2}, {2, 0, 2});
      kb.add_pair({2, 0, 1, 0}, {2, 0, 1});
      kb.add_pair({2, 0, 2, 0}, {2, 0, 2});

      REQUIRE(kb.nr_classes() == 16);
      REQUIRE(kb.word_to_class_index({2}) == kb.word_to_class_index({3}));
    }

    TEST_CASE("Knuth-Bendix (cong) 06: free semigroup congruence (6 classes)",
              "[quick][congruence][knuth-bendix][06]") {
      REPORTER.set_report(REPORT);
      KnuthBendix kb;
      kb.set_nr_generators(11);
      kb.add_pair({2}, {1});
      kb.add_pair({4}, {3});
      kb.add_pair({5}, {0});
      kb.add_pair({6}, {3});
      kb.add_pair({7}, {1});
      kb.add_pair({8}, {3});
      kb.add_pair({9}, {3});
      kb.add_pair({10}, {0});
      kb.add_pair({0, 2}, {0, 1});
      kb.add_pair({0, 4}, {0, 3});
      kb.add_pair({0, 5}, {0, 0});
      kb.add_pair({0, 6}, {0, 3});
      kb.add_pair({0, 7}, {0, 1});
      kb.add_pair({0, 8}, {0, 3});
      kb.add_pair({0, 9}, {0, 3});
      kb.add_pair({0, 10}, {0, 0});
      kb.add_pair({1, 1}, {1});
      kb.add_pair({1, 2}, {1});
      kb.add_pair({1, 4}, {1, 3});
      kb.add_pair({1, 5}, {1, 0});
      kb.add_pair({1, 6}, {1, 3});
      kb.add_pair({1, 7}, {1});
      kb.add_pair({1, 8}, {1, 3});
      kb.add_pair({1, 9}, {1, 3});
      kb.add_pair({1, 10}, {1, 0});
      kb.add_pair({3, 1}, {3});
      kb.add_pair({3, 2}, {3});
      kb.add_pair({3, 3}, {3});
      kb.add_pair({3, 4}, {3});
      kb.add_pair({3, 5}, {3, 0});
      kb.add_pair({3, 6}, {3});
      kb.add_pair({3, 7}, {3});
      kb.add_pair({3, 8}, {3});
      kb.add_pair({3, 9}, {3});
      kb.add_pair({3, 10}, {3, 0});
      kb.add_pair({0, 0, 0}, {0});
      kb.add_pair({0, 0, 1}, {1});
      kb.add_pair({0, 0, 3}, {3});
      kb.add_pair({0, 1, 3}, {1, 3});
      kb.add_pair({1, 0, 0}, {1});
      kb.add_pair({1, 0, 3}, {0, 3});
      kb.add_pair({3, 0, 0}, {3});
      kb.add_pair({0, 1, 0, 1}, {1, 0, 1});
      kb.add_pair({0, 3, 0, 3}, {3, 0, 3});
      kb.add_pair({1, 0, 1, 0}, {1, 0, 1});
      kb.add_pair({1, 3, 0, 1}, {1, 0, 1});
      kb.add_pair({1, 3, 0, 3}, {3, 0, 3});
      kb.add_pair({3, 0, 1, 0}, {3, 0, 1});
      kb.add_pair({3, 0, 3, 0}, {3, 0, 3});

      REQUIRE(kb.nr_classes() == 16);
      REQUIRE(kb.word_to_class_index({0}) == kb.word_to_class_index({5}));
      REQUIRE(kb.word_to_class_index({0}) == kb.word_to_class_index({10}));
      REQUIRE(kb.word_to_class_index({1}) == kb.word_to_class_index({2}));
      REQUIRE(kb.word_to_class_index({1}) == kb.word_to_class_index({7}));
      REQUIRE(kb.word_to_class_index({3}) == kb.word_to_class_index({4}));
      REQUIRE(kb.word_to_class_index({3}) == kb.word_to_class_index({6}));
      REQUIRE(kb.word_to_class_index({3}) == kb.word_to_class_index({8}));
      REQUIRE(kb.word_to_class_index({3}) == kb.word_to_class_index({9}));
    }

    TEST_CASE("Knuth-Bendix (cong) 07: free semigroup congruence (240 classes)",
              "[quick][congruence][knuth-bendix][07]") {
      REPORTER.set_report(REPORT);
      KnuthBendix kb;
      kb.set_nr_generators(2);
      kb.add_pair({0, 0, 0}, {0});
      kb.add_pair({1, 1, 1, 1}, {1});
      kb.add_pair({0, 1, 1, 1, 0}, {0, 0});
      kb.add_pair({1, 0, 0, 1}, {1, 1});
      kb.add_pair({0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}, {0, 0});

      REQUIRE(kb.nr_classes() == 240);
    }

    // TODO add tests from rws.2.test.cc
  }  // namespace knuth_bendix_congruence
}  // namespace libsemigroups
