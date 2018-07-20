//
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

// The purpose of this file is to provide unit tests for the FpSemigroup class.

#include <utility>

#include "../src/elements.h"
#include "../src/fpsemi.h"
#include "../src/semigroups.h"

#include "catch.hpp"

#define FPSEMI_REPORT false

namespace libsemigroups_tests_fpsemi {

  using namespace libsemigroups;

  TEST_CASE("FpSemigroup 01: normal_form", "[quick][fpsemigroup][01]") {
    glob_reporter.set_report(FPSEMI_REPORT);
    std::vector<relation_t> rels;
    rels.push_back(relation_t({0, 0, 0}, {0}));  // (a^3, a)
    rels.push_back(relation_t({0}, {1, 1}));     // (a, b^2)

    FpSemigroup S(2, rels);

    REQUIRE(S.size() == 5);

    REQUIRE(S.normal_form({0, 0, 1}) == word_t({0, 0, 1}));
    REQUIRE(S.normal_form({0, 0, 0, 0, 1}) == word_t({0, 0, 1}));
    REQUIRE(S.normal_form({0, 1, 1, 0, 0, 1}) == word_t({0, 0, 1}));
    REQUIRE(S.normal_form({0, 0, 0}) == word_t({0}));
    REQUIRE(S.normal_form({1}) == word_t({1}));
  }

  // Not sure I see the point of the next test
  /*TEST_CASE("FpSemigroup 02: word_to_class_index for fp semigroup",
            "[quick][fpsemigroup][02]") {
    std::vector<relation_t> rels;
    rels.push_back(relation_t({0, 0, 0}, {0}));  // (a^3, a)
    rels.push_back(relation_t({0}, {1, 1}));     // (a, b^2)
    std::vector<relation_t> extra;

    Congruence<> cong1("twosided", 2, rels, extra);
    cong1.set_report(FPSEMI_REPORT);

    REQUIRE(cong1.word_to_class_index({0, 0, 1}) == 4);
    REQUIRE(cong1.word_to_class_index({0, 0, 0, 0, 1}) == 4);
    REQUIRE(cong1.word_to_class_index({0, 1, 1, 0, 0, 1}) == 4);
    REQUIRE(cong1.word_to_class_index({0, 0, 0}) == 0);
    REQUIRE(cong1.word_to_class_index({1}) == 1);

    Congruence<> cong2("twosided", 2, rels, extra);
    cong2.set_report(FPSEMI_REPORT);

    REQUIRE(cong2.word_to_class_index({0, 0, 0, 0}) == 2);
  }*/

  TEST_CASE("FpSemigroup 03: for a finite semigroup",
            "[quick][fpsemigroup][03]") {
    glob_reporter.set_report(FPSEMI_REPORT);

    std::vector<Element*> gens
        = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
           new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
    Semigroup<> S = Semigroup<>(gens);
    really_delete_cont(gens);

    REQUIRE(S.size() == 88);
    REQUIRE(S.nrrules() == 18);
    REQUIRE(S.degree() == 5);

    Element* t1 = new Transformation<u_int16_t>({3, 4, 4, 4, 4});
    Element* t2 = new Transformation<u_int16_t>({3, 1, 3, 3, 3});
    Element* t3 = new Transformation<u_int16_t>({1, 3, 1, 3, 3});
    Element* t4 = new Transformation<u_int16_t>({4, 2, 4, 4, 2});
    word_t   w1, w2, w3, w4;
    S.factorisation(w1, S.position(t1));
    S.factorisation(w2, S.position(t2));
    S.factorisation(w3, S.position(t3));
    S.factorisation(w4, S.position(t4));
    t1->really_delete();
    t2->really_delete();
    t3->really_delete();
    t4->really_delete();
    delete t1;
    delete t2;
    delete t3;
    delete t4;

    FpSemigroup T(&S);
    T.add_relation(w1, w2);

    REQUIRE(T.size() == 21);

    REQUIRE(T.equal_to(w3, w4));
    REQUIRE(T.normal_form(w3) == T.normal_form(w4));
  }

  /*TEST_CASE("FpSemigroup 04: finite fp-semigroup, dihedral group of order 6",
            "[quick][fpsemigroup][kbfp][fpsemigroup][04]") {
    std::vector<relation_t> rels = {relation_t({0, 0}, {0}),
                                    relation_t({0, 1}, {1}),
                                    relation_t({1, 0}, {1}),
                                    relation_t({0, 2}, {2}),
                                    relation_t({2, 0}, {2}),
                                    relation_t({0, 3}, {3}),
                                    relation_t({3, 0}, {3}),
                                    relation_t({0, 4}, {4}),
                                    relation_t({4, 0}, {4}),
                                    relation_t({1, 2}, {0}),
                                    relation_t({2, 1}, {0}),
                                    relation_t({3, 4}, {0}),
                                    relation_t({4, 3}, {0}),
                                    relation_t({2, 2}, {0}),
                                    relation_t({1, 4, 2, 3, 3}, {0}),
                                    relation_t({4, 4, 4}, {0})};

    std::vector<relation_t> extra = {};

    Congruence<> cong("twosided", 5, rels, extra);
    glob_reporter.set_report(FPSEMI_REPORT);

    REQUIRE(S.size() == 6);
    REQUIRE(cong.word_to_class_index({1}) == cong.word_to_class_index({2}));
  }

  TEST_CASE("FpSemigroup 05: finite fp-semigroup, size 16",
            "[quick][fpsemigroup][kbfp][fpsemigroup][05]") {
    std::vector<relation_t> rels = {relation_t({3}, {2}),
                                    relation_t({0, 3}, {0, 2}),
                                    relation_t({1, 1}, {1}),
                                    relation_t({1, 3}, {1, 2}),
                                    relation_t({2, 1}, {2}),
                                    relation_t({2, 2}, {2}),
                                    relation_t({2, 3}, {2}),
                                    relation_t({0, 0, 0}, {0}),
                                    relation_t({0, 0, 1}, {1}),
                                    relation_t({0, 0, 2}, {2}),
                                    relation_t({0, 1, 2}, {1, 2}),
                                    relation_t({1, 0, 0}, {1}),
                                    relation_t({1, 0, 2}, {0, 2}),
                                    relation_t({2, 0, 0}, {2}),
                                    relation_t({0, 1, 0, 1}, {1, 0, 1}),
                                    relation_t({0, 2, 0, 2}, {2, 0, 2}),
                                    relation_t({1, 0, 1, 0}, {1, 0, 1}),
                                    relation_t({1, 2, 0, 1}, {1, 0, 1}),
                                    relation_t({1, 2, 0, 2}, {2, 0, 2}),
                                    relation_t({2, 0, 1, 0}, {2, 0, 1}),
                                    relation_t({2, 0, 2, 0}, {2, 0, 2})};
    std::vector<relation_t> extra = {};

    Congruence<> cong("twosided", 4, rels, extra);
    glob_reporter.set_report(FPSEMI_REPORT);

    REQUIRE(S.size() == 16);
    REQUIRE(cong.word_to_class_index({2}) == cong.word_to_class_index({3}));
  }

  TEST_CASE("FpSemigroup 06: finite fp-semigroup, size 16",
            "[quick][fpsemigroup][kbfp][fpsemigroup][06]") {
    std::vector<relation_t> rels = {relation_t({2}, {1}),
                                    relation_t({4}, {3}),
                                    relation_t({5}, {0}),
                                    relation_t({6}, {3}),
                                    relation_t({7}, {1}),
                                    relation_t({8}, {3}),
                                    relation_t({9}, {3}),
                                    relation_t({10}, {0}),
                                    relation_t({0, 2}, {0, 1}),
                                    relation_t({0, 4}, {0, 3}),
                                    relation_t({0, 5}, {0, 0}),
                                    relation_t({0, 6}, {0, 3}),
                                    relation_t({0, 7}, {0, 1}),
                                    relation_t({0, 8}, {0, 3}),
                                    relation_t({0, 9}, {0, 3}),
                                    relation_t({0, 10}, {0, 0}),
                                    relation_t({1, 1}, {1}),
                                    relation_t({1, 2}, {1}),
                                    relation_t({1, 4}, {1, 3}),
                                    relation_t({1, 5}, {1, 0}),
                                    relation_t({1, 6}, {1, 3}),
                                    relation_t({1, 7}, {1}),
                                    relation_t({1, 8}, {1, 3}),
                                    relation_t({1, 9}, {1, 3}),
                                    relation_t({1, 10}, {1, 0}),
                                    relation_t({3, 1}, {3}),
                                    relation_t({3, 2}, {3}),
                                    relation_t({3, 3}, {3}),
                                    relation_t({3, 4}, {3}),
                                    relation_t({3, 5}, {3, 0}),
                                    relation_t({3, 6}, {3}),
                                    relation_t({3, 7}, {3}),
                                    relation_t({3, 8}, {3}),
                                    relation_t({3, 9}, {3}),
                                    relation_t({3, 10}, {3, 0}),
                                    relation_t({0, 0, 0}, {0}),
                                    relation_t({0, 0, 1}, {1}),
                                    relation_t({0, 0, 3}, {3}),
                                    relation_t({0, 1, 3}, {1, 3}),
                                    relation_t({1, 0, 0}, {1}),
                                    relation_t({1, 0, 3}, {0, 3}),
                                    relation_t({3, 0, 0}, {3}),
                                    relation_t({0, 1, 0, 1}, {1, 0, 1}),
                                    relation_t({0, 3, 0, 3}, {3, 0, 3}),
                                    relation_t({1, 0, 1, 0}, {1, 0, 1}),
                                    relation_t({1, 3, 0, 1}, {1, 0, 1}),
                                    relation_t({1, 3, 0, 3}, {3, 0, 3}),
                                    relation_t({3, 0, 1, 0}, {3, 0, 1}),
                                    relation_t({3, 0, 3, 0}, {3, 0, 3})};
    std::vector<relation_t> extra = {};

    Congruence<> cong("twosided", 11, rels, extra);
    glob_reporter.set_report(FPSEMI_REPORT);

    REQUIRE(S.size() == 16);
    REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({5}));
    REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({10}));
    REQUIRE(cong.word_to_class_index({1}) == cong.word_to_class_index({2}));
    REQUIRE(cong.word_to_class_index({1}) == cong.word_to_class_index({7}));
    REQUIRE(cong.word_to_class_index({3}) == cong.word_to_class_index({4}));
    REQUIRE(cong.word_to_class_index({3}) == cong.word_to_class_index({6}));
    REQUIRE(cong.word_to_class_index({3}) == cong.word_to_class_index({8}));
    REQUIRE(cong.word_to_class_index({3}) == cong.word_to_class_index({9}));
  }

  TEST_CASE("FpSemigroup 07: fp semigroup, size 240",
            "[quick][fpsemigroup][07]") {
    std::vector<relation_t> extra
        = {relation_t({0, 0, 0}, {0}),
           relation_t({1, 1, 1, 1}, {1}),
           relation_t({0, 1, 1, 1, 0}, {0, 0}),
           relation_t({1, 0, 0, 1}, {1, 1}),
           relation_t({0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}, {0, 0})};

    Congruence<> cong("twosided", 2, std::vector<relation_t>(), extra);
    glob_reporter.set_report(FPSEMI_REPORT);

    REQUIRE(S.size() == 240);
  }*/

  TEST_CASE("FpSemigroup 08: add_relation", "[quick][fpsemigroup][08]") {
    glob_reporter.set_report(FPSEMI_REPORT);
    FpSemigroup S;
    S.set_alphabet("ab");
    REQUIRE(S.is_obviously_infinite());
    S.add_relation("aaa", "a");
    S.add_relation("a", "bb");
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(S.size() == 5);

    SemigroupBase* T = S.isomorphic_non_fp_semigroup();
    REQUIRE(T->size() == 5);
    REQUIRE(T->nridempotents() == 1);
  }

  TEST_CASE("FpSemigroup 09: add_relation", "[quick][fpsemigroup][09]") {
    glob_reporter.set_report(FPSEMI_REPORT);
    FpSemigroup S;
    S.set_alphabet("ab");
    REQUIRE(S.is_obviously_infinite());
    S.add_relation("aaa", "a");
    S.add_relation("a", "bb");
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(S.rws()->isomorphic_non_fp_semigroup()->size() == 5);
    REQUIRE(S.size() == 5);

    SemigroupBase* T = S.isomorphic_non_fp_semigroup();
    REQUIRE(T->size() == 5);
    REQUIRE(T->nridempotents() == 1);
  }

  TEST_CASE("FpSemigroup 10: equal_to", "[quick][fpsemigroup][10]") {
    glob_reporter.set_report(FPSEMI_REPORT);

    FpSemigroup S;
    S.set_alphabet("ab");
    S.add_relation("aa", "a");
    S.add_relation("ab", "a");
    S.add_relation("ba", "a");

    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(S.equal_to("ab", "a"));
    REQUIRE(S.equal_to("ba", "a"));
    REQUIRE(S.equal_to("aa", "a"));
  }

}  // namespace libsemigroups_tests_fpsemi
