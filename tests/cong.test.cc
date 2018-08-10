//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2016 Michael Torpey
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

#include "catch.hpp"
#include "cong.h"
#include "element.h"

#define CONG_REPORT false

using namespace libsemigroups;

template <class TElementType>
void delete_gens(std::vector<TElementType>& gens) {
  for (auto& x : gens) {
    delete x;
  }
}

TEST_CASE("Congruence 00: 5-parameter constructor",
          "[quick][congruence][fpsemigroup][multithread][00]") {
  std::vector<relation_type> rels;
  rels.push_back(relation_type({0, 0, 0}, {0}));  // (a^3, a)
  rels.push_back(relation_type({0}, {1, 1}));     // (a, b^2)
  std::vector<relation_type> extra;

  Congruence cong("left", 2, rels, extra);
}

TEST_CASE("Congruence 01: Small fp semigroup",
          "[quick][congruence][fpsemigroup][multithread][01]") {
  std::vector<relation_type> rels;
  rels.push_back(relation_type({0, 0, 0}, {0}));  // (a^3, a)
  rels.push_back(relation_type({0}, {1, 1}));     // (a, b^2)
  std::vector<relation_type> extra;

  Congruence cong("twosided", 2, rels, extra);
  REPORTER.set_report(CONG_REPORT);

  REQUIRE(!cong.is_done());
  REQUIRE(cong.nr_classes() == 5);
  REQUIRE(cong.is_done());

  REQUIRE(cong.word_to_class_index({0, 0, 1})
          == cong.word_to_class_index({0, 0, 0, 0, 1}));
  REQUIRE(cong.test_equals({0, 0, 1}, {0, 0, 1}));
  REQUIRE(cong.test_equals({0, 0, 1}, {0, 0, 0, 0, 1}));
  REQUIRE(cong.word_to_class_index({0, 0, 0, 0, 1})
          == cong.word_to_class_index({0, 1, 1, 0, 0, 1}));
  REQUIRE(cong.test_equals({0, 0, 0, 0, 1}, {0, 1, 1, 0, 0, 1}));
  REQUIRE(cong.word_to_class_index({0, 0, 0})
          != cong.word_to_class_index({0, 0, 1}));
  REQUIRE(!cong.test_equals({0, 0, 0}, {0, 0, 1}));
  REQUIRE(cong.word_to_class_index({1}) != cong.word_to_class_index({0, 0, 0}));
  REQUIRE(!cong.test_equals({1}, {0, 0, 0}));
}

TEST_CASE("Congruence 02: Small left congruence on free semigroup",
          "[quick][congruence][fpsemigroup][multithread][02]") {
  std::vector<relation_type> rels;
  rels.push_back(relation_type({0, 0, 0}, {0}));  // (a^3, a)
  rels.push_back(relation_type({0}, {1, 1}));     // (a, b^2)
  std::vector<relation_type> extra;

  Congruence cong("left", 2, rels, extra);
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(cong.nr_classes() == 5);
}

TEST_CASE("Congruence 03: Small right congruence on free semigroup",
          "[quick][congruence][fpsemigroup][multithread][03]") {
  std::vector<relation_type> rels;
  rels.push_back(relation_type({0, 0, 0}, {0}));  // (a^3, a)
  rels.push_back(relation_type({0}, {1, 1}));     // (a, b^2)
  std::vector<relation_type> extra;

  Congruence cong("right", 2, rels, extra);
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(cong.nr_classes() == 5);
  REQUIRE(cong.is_done());
}

TEST_CASE(
    "Congruence 04: word_to_class_index for left congruence on free semigroup",
    "[quick][congruence][fpsemigroup][multithread][04]") {
  std::vector<relation_type> rels;
  rels.push_back(relation_type({0, 0, 0}, {0}));  // (a^3, a)
  rels.push_back(relation_type({0}, {1, 1}));     // (a, b^2)
  std::vector<relation_type> extra;

  Congruence cong("left", 2, rels, extra);
  REPORTER.set_report(CONG_REPORT);

  REQUIRE(cong.word_to_class_index({0, 0, 1})
          == cong.word_to_class_index({0, 0, 0, 0, 1}));
  REQUIRE(cong.test_equals({0, 1, 1, 0, 0, 1}, {0, 0, 1}));
  REQUIRE(cong.word_to_class_index({0, 0, 0})
          != cong.word_to_class_index({0, 0, 1}));
  REQUIRE(cong.word_to_class_index({1})
          != cong.word_to_class_index({0, 0, 0, 0}));
  REQUIRE(!cong.test_equals({0, 0, 0, 0}, {0, 0, 1}));
}

TEST_CASE("Congruence 05: word_to_class_index for small fp semigroup",
          "[quick][congruence][fpsemigroup][multithread][05]") {
  std::vector<relation_type> rels;
  rels.push_back(relation_type({0, 0, 0}, {0}));  // (a^3, a)
  rels.push_back(relation_type({0}, {1, 1}));     // (a, b^2)
  std::vector<relation_type> extra;

  Congruence cong1("twosided", 2, rels, extra);
  REPORTER.set_report(CONG_REPORT);

  REQUIRE(cong1.word_to_class_index({0, 0, 1})
          == cong1.word_to_class_index({0, 0, 0, 0, 1}));
  REQUIRE(cong1.word_to_class_index({0, 1, 1, 0, 0, 1})
          == cong1.word_to_class_index({0, 0, 0, 0, 1}));
  REQUIRE(cong1.word_to_class_index({0, 0, 0})
          == cong1.word_to_class_index({1, 1}));
  REQUIRE(cong1.word_to_class_index({1}) != cong1.word_to_class_index({0}));

  Congruence cong2("twosided", 2, rels, extra);
  REPORTER.set_report(CONG_REPORT);

  REQUIRE(cong2.word_to_class_index({0, 0, 0, 0})
          == cong2.word_to_class_index({0, 0}));
  REQUIRE(cong2.test_equals({0, 0, 0, 0}, {0, 1, 1, 0, 1, 1}));
}

TEST_CASE("Congruence 06: 6-argument constructor (trivial cong)",
          "[quick][congruence][multithread][06]") {
  std::vector<Element*> gens = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
                                new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
  Semigroup<>           S    = Semigroup<>(gens);
  REPORTER.set_report(CONG_REPORT);
  delete_gens(gens);

  REQUIRE(S.size() == 88);
  REQUIRE(S.degree() == 5);

  std::vector<relation_type> extra;
  Congruence                 cong("twosided", &S, extra);
  REQUIRE(!cong.is_done());

  Partition<word_type>* ntc = cong.nontrivial_classes();
  REQUIRE(ntc->size() == 0);
  delete ntc;
}

TEST_CASE("Congruence 07: 6-argument constructor (nontrivial cong)",
          "[quick][congruence][multithread][07]") {
  std::vector<Element*> gens = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
                                new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
  Semigroup<>           S    = Semigroup<>(gens);
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(S.size() == 88);
  REQUIRE(S.degree() == 5);
  delete_gens(gens);

  Element*  t1 = new Transformation<u_int16_t>({3, 4, 4, 4, 4});
  Element*  t2 = new Transformation<u_int16_t>({3, 1, 3, 3, 3});
  word_type w1, w2;
  S.factorisation(w1, S.position(t1));
  S.factorisation(w2, S.position(t2));
  std::vector<relation_type> extra({relation_type(w1, w2)});
  Congruence                 cong("twosided", &S, extra);
  REPORTER.set_report(CONG_REPORT);

  delete t1;
  delete t2;
}

TEST_CASE("Congruence 8T: transformation semigroup size 88",
          "[quick][congruence][multithread]") {
  std::vector<Element*> gens = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
                                new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
  Semigroup<>           S    = Semigroup<>(gens);
  REPORTER.set_report(CONG_REPORT);
  delete_gens(gens);
  REQUIRE(S.size() == 88);
  REQUIRE(S.nrrules() == 18);
  REQUIRE(S.degree() == 5);
  Element*  t1 = new Transformation<u_int16_t>({3, 4, 4, 4, 4});
  Element*  t2 = new Transformation<u_int16_t>({3, 1, 3, 3, 3});
  word_type w1, w2;
  S.factorisation(w1, S.position(t1));
  S.factorisation(w2, S.position(t2));
  std::vector<relation_type> extra({std::make_pair(w1, w2)});
  Congruence                 cong("twosided", &S, extra);
  REPORTER.set_report(CONG_REPORT);

  REQUIRE(cong.nr_classes() == 21);
  REQUIRE(cong.nr_classes() == 21);
  Element*  t3 = new Transformation<u_int16_t>({1, 3, 1, 3, 3});
  Element*  t4 = new Transformation<u_int16_t>({4, 2, 4, 4, 2});
  word_type w3, w4;
  S.factorisation(w3, S.position(t3));
  S.factorisation(w4, S.position(t4));
  REQUIRE(cong.word_to_class_index(w3) == cong.word_to_class_index(w4));
  REQUIRE(cong.test_equals(w3, w4));

  delete t1;
  delete t2;
  delete t3;
  delete t4;
}

TEST_CASE("Congruence 8L: left congruence on transformation semigroup size 88",
          "[quick][congruence][multithread]") {
  std::vector<Element*> gens = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
                                new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
  Semigroup<>           S    = Semigroup<>(gens);
  REPORTER.set_report(CONG_REPORT);
  delete_gens(gens);

  // REQUIRE(S.size() == 88);
  // REQUIRE(S.degree() == 5);
  std::vector<relation_type> extra(
      {relation_type({0, 1, 0, 0, 0, 1, 1, 0, 0}, {1, 0, 0, 0, 1})});
  Congruence cong("left", &S, extra);
  REPORTER.set_report(CONG_REPORT);

  REQUIRE(cong.nr_classes() == 69);
  REQUIRE(cong.nr_classes() == 69);

  Element*  t3 = new Transformation<u_int16_t>({1, 3, 1, 3, 3});
  Element*  t4 = new Transformation<u_int16_t>({4, 2, 4, 4, 2});
  word_type w3, w4;
  S.factorisation(w3, S.position(t3));
  S.factorisation(w4, S.position(t4));
  REQUIRE(cong.word_to_class_index(w3) != cong.word_to_class_index(w4));
  REQUIRE(cong.word_to_class_index(w3)
          == cong.word_to_class_index({0, 0, 1, 0, 1}));
  REQUIRE(cong.word_to_class_index({1, 0, 0, 1, 0, 1})
          == cong.word_to_class_index({0, 0, 1, 0, 0, 0, 1}));
  REQUIRE(cong.word_to_class_index({0, 1, 1, 0, 0, 0})
          != cong.word_to_class_index({1, 1}));
  REQUIRE(cong.word_to_class_index({1, 0, 0, 0, 1, 0, 0, 0})
          != cong.word_to_class_index({1, 0, 0, 1}));

  REQUIRE(cong.test_equals({1, 0, 0, 1, 0, 1}, {0, 0, 1, 0, 0, 0, 1}));
  REQUIRE(!cong.test_equals({1, 0, 0, 0, 1, 0, 0, 0}, {1, 0, 0, 1}));

  REQUIRE(!cong.test_less_than({1, 0, 0, 0, 1, 0, 0, 0}, {1, 0, 0, 1}));
  REQUIRE(cong.test_less_than({1, 0, 0, 1}, {1, 0, 0, 0, 1, 0, 0, 0}));

  delete t3;
  delete t4;
}

TEST_CASE("Congruence 8R: right congruence on transformation semigroup size 88",
          "[quick][congruence][multithread]") {
  std::vector<Element*> gens = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
                                new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
  Semigroup<>           S    = Semigroup<>(gens);
  REPORTER.set_report(CONG_REPORT);
  delete_gens(gens);

  REQUIRE(S.size() == 88);
  REQUIRE(S.nrrules() == 18);
  REQUIRE(S.degree() == 5);
  Element*  t1 = new Transformation<u_int16_t>({3, 4, 4, 4, 4});
  Element*  t2 = new Transformation<u_int16_t>({3, 1, 3, 3, 3});
  word_type w1, w2;
  S.factorisation(w1, S.position(t1));
  S.factorisation(w2, S.position(t2));
  std::vector<relation_type> extra({std::make_pair(w1, w2)});
  Congruence                 cong("right", &S, extra);
  REPORTER.set_report(CONG_REPORT);

  REQUIRE(cong.nr_classes() == 72);
  REQUIRE(cong.nr_classes() == 72);
  Element*  t3 = new Transformation<u_int16_t>({1, 3, 3, 3, 3});
  Element*  t4 = new Transformation<u_int16_t>({4, 2, 4, 4, 2});
  Element*  t5 = new Transformation<u_int16_t>({2, 3, 2, 2, 2});
  Element*  t6 = new Transformation<u_int16_t>({2, 3, 3, 3, 3});
  word_type w3, w4, w5, w6;
  S.factorisation(w3, S.position(t3));
  S.factorisation(w4, S.position(t4));
  S.factorisation(w5, S.position(t5));
  S.factorisation(w6, S.position(t6));
  REQUIRE(cong.word_to_class_index(w3) != cong.word_to_class_index(w4));
  REQUIRE(cong.word_to_class_index(w5) == cong.word_to_class_index(w6));
  REQUIRE(cong.word_to_class_index(w3) != cong.word_to_class_index(w6));

  REQUIRE(cong.test_equals(w1, w2));
  REQUIRE(cong.test_equals(w5, w6));
  REQUIRE(!cong.test_equals(w3, w5));

  delete t1;
  delete t2;
  delete t3;
  delete t4;
  delete t5;
  delete t6;
}

TEST_CASE("Congruence 09: for an infinite fp semigroup",
          "[quick][congruence][fpsemigroup][multithread][09]") {
  std::vector<relation_type> relations = {relation_type({0, 1}, {1, 0}),
                                          relation_type({0, 2}, {2, 2}),
                                          relation_type({0, 2}, {0}),
                                          relation_type({0, 2}, {0}),
                                          relation_type({2, 2}, {0}),
                                          relation_type({1, 2}, {1, 2}),
                                          relation_type({1, 2}, {2, 2}),
                                          relation_type({1, 2, 2}, {1}),
                                          relation_type({1, 2}, {1}),
                                          relation_type({2, 2}, {1})};
  std::vector<relation_type> extra     = {relation_type({0}, {1})};
  Congruence                 cong("twosided", 3, relations, extra);
  REPORTER.set_report(CONG_REPORT);
  cong.force_kbfp();

  REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({1}));
  REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({1, 0}));
  REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({1, 1}));
  REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({1, 0, 1}));

  REQUIRE(cong.test_equals({1}, {1, 1}));
  REQUIRE(cong.test_equals({1, 0, 1}, {1, 0}));
}

TEST_CASE("Congruence 10: for an infinite fp semigroup",
          "[quick][congruence][fpsemigroup][multithread][10]") {
  std::vector<relation_type> rels  = {relation_type({0, 1}, {1, 0}),
                                     relation_type({0, 2}, {2, 0}),
                                     relation_type({0, 0}, {0}),
                                     relation_type({0, 2}, {0}),
                                     relation_type({2, 0}, {0}),
                                     relation_type({1, 2}, {2, 1}),
                                     relation_type({1, 1, 1}, {1}),
                                     relation_type({1, 2}, {1}),
                                     relation_type({2, 1}, {1})};
  std::vector<relation_type> extra = {{{0}, {1}}};
  Congruence                 cong("twosided", 3, rels, extra);
  REPORTER.set_report(CONG_REPORT);
  // This line is here to make sure that the max_threads is ignored here, since
  // if we are limited to one thread here then this example doesn't run!
  cong.set_max_threads(1);
  REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({1}));
  REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({1, 0}));
  REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({1, 1}));
  REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({1, 0, 1}));

  REQUIRE(cong.test_equals({1}, {1, 1}));
  REQUIRE(cong.test_equals({1, 0, 1}, {1, 0}));

  REQUIRE(!cong.test_less_than({1, 0, 1}, {1, 0}));
}

TEST_CASE("Congruence 11: congruence on big finite semigroup",
          "[quick][congruence][multithread][finite][11]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({7, 3, 5, 3, 4, 2, 7, 7}),
         new Transformation<u_int16_t>({1, 2, 4, 4, 7, 3, 0, 7}),
         new Transformation<u_int16_t>({0, 6, 4, 2, 2, 6, 6, 4}),
         new Transformation<u_int16_t>({3, 6, 3, 4, 0, 6, 0, 7})};
  Semigroup<> S = Semigroup<>(gens);
  REPORTER.set_report(CONG_REPORT);
  delete_gens(gens);

  // The following lines are intentionally commented out so that we can check
  // that P does not enumerate the semigroup, they remain to remind us of the
  // size and number of rules of the semigroups.
  // REQUIRE(S.size(false) == 11804);
  // REQUIRE(S.nrrules(false) == 2460);

  std::vector<relation_type> extra(
      {relation_type({0, 3, 2, 1, 3, 2, 2}, {3, 2, 2, 1, 3, 3})});
  Congruence cong("twosided", &S, extra);
  REPORTER.set_report(CONG_REPORT);

  REQUIRE(cong.word_to_class_index({0, 0, 0, 1})
          == cong.word_to_class_index({0, 0, 1, 0, 0}));
  REQUIRE(cong.word_to_class_index({0, 0, 1, 0, 1})
          == cong.word_to_class_index({1, 1, 0, 1}));
  REQUIRE(cong.word_to_class_index({1, 1, 0, 0})
          != cong.word_to_class_index({0, 0, 0, 1}));
  REQUIRE(cong.word_to_class_index({0, 0, 3})
          != cong.word_to_class_index({0, 0, 0, 1}));
  REQUIRE(cong.word_to_class_index({1, 1, 0, 0})
          != cong.word_to_class_index({0, 0, 3}));
  REQUIRE(cong.word_to_class_index({1, 2, 1, 3, 3, 2, 1, 2})
          == cong.word_to_class_index({2, 1, 3, 3, 2, 1, 0}));
  REQUIRE(cong.word_to_class_index({0, 3, 1, 1, 1, 3, 2, 2, 1, 0})
          == cong.word_to_class_index({0, 3, 2, 2, 1}));
  REQUIRE(cong.word_to_class_index({0, 3, 2, 1, 3, 3, 3})
          != cong.word_to_class_index({0, 0, 3}));
  REQUIRE(cong.word_to_class_index({1, 1, 0})
          != cong.word_to_class_index({1, 3, 3, 2, 2, 1, 0}));

  REQUIRE(cong.test_equals({1, 2, 1, 3, 3, 2, 1, 2}, {2, 1, 3, 3, 2, 1, 0}));
  REQUIRE(!cong.test_equals({1, 1, 0}, {1, 3, 3, 2, 2, 1, 0}));

  REQUIRE(cong.test_less_than({1, 3, 3, 2, 2, 1, 0}, {1, 1, 0}));
  REQUIRE(!cong.test_less_than({1, 1, 0, 0}, {0, 0, 3}));

  REQUIRE(cong.nr_classes() == 525);
  REQUIRE(cong.nr_classes() == 525);
}

TEST_CASE("Congruence 12: Congruence on full PBR monoid on 2 points",
          "[extreme][congruence][multithread][finite][pbr][12]") {
  std::vector<Element*> gens = {new PBR({{2}, {3}, {0}, {1}}),
                                new PBR({{}, {2}, {1}, {0, 3}}),
                                new PBR({{0, 3}, {2}, {1}, {}}),
                                new PBR({{1, 2}, {3}, {0}, {1}}),
                                new PBR({{2}, {3}, {0}, {1, 3}}),
                                new PBR({{3}, {1}, {0}, {1}}),
                                new PBR({{3}, {2}, {0}, {0, 1}}),
                                new PBR({{3}, {2}, {0}, {1}}),
                                new PBR({{3}, {2}, {0}, {3}}),
                                new PBR({{3}, {2}, {1}, {0}}),
                                new PBR({{3}, {2, 3}, {0}, {1}})};
  REQUIRE(gens[0]->degree() == 2);

  Semigroup<> S = Semigroup<>(gens);
  REPORTER.set_report(CONG_REPORT);
  delete_gens(gens);

  // REQUIRE(S.size() == 65536);
  // REQUIRE(S.nrrules() == 45416);

  std::vector<relation_type> extra(
      {relation_type({7, 10, 9, 3, 6, 9, 4, 7, 9, 10},
                     {9, 3, 6, 6, 10, 9, 4, 7}),
       relation_type({8, 7, 5, 8, 9, 8}, {6, 3, 8, 6, 1, 2, 4})});
  Congruence cong("twosided", &S, extra);
  REPORTER.set_report(CONG_REPORT);

  REQUIRE(cong.nr_classes() == 19009);

  Partition<word_type>* ntc = cong.nontrivial_classes();
  REQUIRE(ntc->size() == 577);
  std::vector<size_t> sizes({0, 0, 0, 0});
  for (size_t i = 0; i < ntc->size(); i++) {
    switch (ntc->at(i)->size()) {
      case 4:
        sizes[0]++;
        break;
      case 16:
        sizes[1]++;
        break;
      case 96:
        sizes[2]++;
        break;
      case 41216:
        sizes[3]++;
        break;
      default:
        REQUIRE(false);
    }
  }
  REQUIRE(sizes == std::vector<size_t>({384, 176, 16, 1}));
  delete ntc;
}

TEST_CASE("Congruence 13: partial perm example",
          "[quick][congruence][multithread][finite][13]") {
  std::vector<Element*> gens
      = {new PartialPerm<u_int16_t>({0, 1, 2}, {4, 0, 1}, 6),
         new PartialPerm<u_int16_t>({0, 1, 2, 3, 5}, {2, 5, 3, 0, 4}, 6),
         new PartialPerm<u_int16_t>({0, 1, 2, 3}, {5, 0, 3, 1}, 6),
         new PartialPerm<u_int16_t>({0, 2, 5}, {3, 4, 1}, 6),
         new PartialPerm<u_int16_t>({0, 2, 5}, {0, 2, 5}, 6),
         new PartialPerm<u_int16_t>({0, 1, 4}, {1, 2, 0}, 6),
         new PartialPerm<u_int16_t>({0, 2, 3, 4, 5}, {3, 0, 2, 5, 1}, 6),
         new PartialPerm<u_int16_t>({0, 1, 3, 5}, {1, 3, 2, 0}, 6),
         new PartialPerm<u_int16_t>({1, 3, 4}, {5, 0, 2}, 6)};

  Semigroup<> S = Semigroup<>(gens);
  REPORTER.set_report(CONG_REPORT);
  delete_gens(gens);

  // REQUIRE(S.size() == 712);
  // REQUIRE(S.nrrules() == 1121);

  std::vector<relation_type> extra
      = {relation_type({5, 3, 1}, {3, 3}), relation_type({2, 7}, {1, 6, 6, 1})};
  Congruence cong("twosided", &S, extra);
  REPORTER.set_report(CONG_REPORT);

  REQUIRE(cong.nr_classes() == 32);
}

TEST_CASE("Congruence 14: Bicyclic monoid",
          "[quick][congruence][fpsemigroup][multithread][14]") {
  std::vector<relation_type> rels = {relation_type({0, 1}, {1}),
                                     relation_type({1, 0}, {1}),
                                     relation_type({0, 0}, {0}),
                                     relation_type({0, 2}, {2}),
                                     relation_type({2, 0}, {2}),
                                     relation_type({1, 2}, {0})};
  std::vector<relation_type> extra;
  Congruence                 cong("twosided", 3, rels, extra);
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(cong.word_to_class_index({0})
          == cong.word_to_class_index({1, 2, 1, 1, 2, 2}));
  REQUIRE(cong.word_to_class_index({0})
          == cong.word_to_class_index({1, 0, 2, 0, 1, 2}));
  REQUIRE(cong.word_to_class_index({2, 1})
          == cong.word_to_class_index({1, 2, 0, 2, 1, 1, 2}));
  REQUIRE(cong.test_equals({2, 1}, {1, 2, 0, 2, 1, 1, 2}));
}

TEST_CASE("Congruence 15: Congruence on bicyclic monoid",
          "[fixme][quick][congruence][fpsemigroup][multithread][15]") {
  std::vector<relation_type> rels = {relation_type({0, 1}, {1}),
                                     relation_type({1, 0}, {1}),
                                     relation_type({0, 0}, {0}),
                                     relation_type({0, 2}, {2}),
                                     relation_type({2, 0}, {2}),
                                     relation_type({1, 2}, {0})};
  std::vector<relation_type> extra({relation_type({1, 1, 1}, {0})});
  Congruence                 cong("twosided", 3, rels, extra);

  REPORTER.set_report(CONG_REPORT);

  REQUIRE(cong.nr_classes() == 3);
}

TEST_CASE("Congruence 16: Congruence on free abelian monoid with 15 classes",
          "[quick][congruence][fpsemigroup][multithread][16]") {
  std::vector<relation_type> rels = {relation_type({0, 1}, {1}),
                                     relation_type({1, 0}, {1}),
                                     relation_type({0, 0}, {0}),
                                     relation_type({0, 2}, {2}),
                                     relation_type({2, 0}, {2}),
                                     relation_type({1, 2}, {2, 1})};
  std::vector<relation_type> extra(
      {relation_type({1, 1, 1, 1, 1}, {1}), relation_type({2, 2, 2}, {2})});
  Congruence cong("twosided", 3, rels, extra);

  REPORTER.set_report(CONG_REPORT);

  REQUIRE(cong.nr_classes() == 15);
}

TEST_CASE("Congruence 17: Congruence on full PBR monoid on 2 points (max 2)",
          "[extreme][congruence][multithread][finite][pbr][17]") {
  std::vector<Element*> gens = {new PBR({{2}, {3}, {0}, {1}}),
                                new PBR({{}, {2}, {1}, {0, 3}}),
                                new PBR({{0, 3}, {2}, {1}, {}}),
                                new PBR({{1, 2}, {3}, {0}, {1}}),
                                new PBR({{2}, {3}, {0}, {1, 3}}),
                                new PBR({{3}, {1}, {0}, {1}}),
                                new PBR({{3}, {2}, {0}, {0, 1}}),
                                new PBR({{3}, {2}, {0}, {1}}),
                                new PBR({{3}, {2}, {0}, {3}}),
                                new PBR({{3}, {2}, {1}, {0}}),
                                new PBR({{3}, {2, 3}, {0}, {1}})};
  REQUIRE(gens[0]->degree() == 2);

  Semigroup<> S = Semigroup<>(gens);
  REPORTER.set_report(CONG_REPORT);
  delete_gens(gens);

  // REQUIRE(S.size() == 65536);
  // REQUIRE(S.nrrules() == 45416);

  std::vector<relation_type> extra(
      {relation_type({7, 10, 9, 3, 6, 9, 4, 7, 9, 10},
                     {9, 3, 6, 6, 10, 9, 4, 7}),
       relation_type({8, 7, 5, 8, 9, 8}, {6, 3, 8, 6, 1, 2, 4})});
  Congruence cong("twosided", &S, extra);
  REPORTER.set_report(CONG_REPORT);
  cong.set_max_threads(2);

  REQUIRE(cong.nr_classes() == 19009);

  Partition<word_type>* ntc = cong.nontrivial_classes();
  REQUIRE(ntc->size() == 577);
  std::vector<size_t> sizes({0, 0, 0, 0});
  for (size_t i = 0; i < ntc->size(); i++) {
    switch (ntc->at(i)->size()) {
      case 4:
        sizes[0]++;
        break;
      case 16:
        sizes[1]++;
        break;
      case 96:
        sizes[2]++;
        break;
      case 41216:
        sizes[3]++;
        break;
      default:
        REQUIRE(false);
    }
  }
  REQUIRE(sizes == std::vector<size_t>({384, 176, 16, 1}));
  delete ntc;
}

TEST_CASE("Congruence 18: Congruence on full PBR monoid on 2 points (max 1)",
          "[extreme][congruence][multithread][finite][pbr][18]") {
  std::vector<Element*> gens = {new PBR({{2}, {3}, {0}, {1}}),
                                new PBR({{}, {2}, {1}, {0, 3}}),
                                new PBR({{0, 3}, {2}, {1}, {}}),
                                new PBR({{1, 2}, {3}, {0}, {1}}),
                                new PBR({{2}, {3}, {0}, {1, 3}}),
                                new PBR({{3}, {1}, {0}, {1}}),
                                new PBR({{3}, {2}, {0}, {0, 1}}),
                                new PBR({{3}, {2}, {0}, {1}}),
                                new PBR({{3}, {2}, {0}, {3}}),
                                new PBR({{3}, {2}, {1}, {0}}),
                                new PBR({{3}, {2, 3}, {0}, {1}})};
  REQUIRE(gens[0]->degree() == 2);

  Semigroup<> S = Semigroup<>(gens);
  REPORTER.set_report(CONG_REPORT);
  delete_gens(gens);

  // REQUIRE(S.size() == 65536);
  // REQUIRE(S.nrrules() == 45416);

  std::vector<relation_type> extra(
      {relation_type({7, 10, 9, 3, 6, 9, 4, 7, 9, 10},
                     {9, 3, 6, 6, 10, 9, 4, 7}),
       relation_type({8, 7, 5, 8, 9, 8}, {6, 3, 8, 6, 1, 2, 4})});
  Congruence cong("twosided", &S, extra);
  REPORTER.set_report(CONG_REPORT);
  cong.set_max_threads(1);

  REQUIRE(cong.nr_classes() == 19009);

  Partition<word_type>* ntc = cong.nontrivial_classes();
  REQUIRE(ntc->size() == 577);
  std::vector<size_t> sizes({0, 0, 0, 0});
  for (size_t i = 0; i < ntc->size(); i++) {
    switch (ntc->at(i)->size()) {
      case 4:
        sizes[0]++;
        break;
      case 16:
        sizes[1]++;
        break;
      case 96:
        sizes[2]++;
        break;
      case 41216:
        sizes[3]++;
        break;
      default:
        REQUIRE(false);
    }
  }
  REQUIRE(sizes == std::vector<size_t>({384, 176, 16, 1}));
  delete ntc;
}

TEST_CASE("Congruence 19: Infinite fp semigroup from GAP library",
          "[quick][congruence][fpsemigroup][multithread][19]") {
  std::vector<relation_type> rels  = {relation_type({0, 0}, {0, 0}),
                                     relation_type({0, 1}, {1, 0}),
                                     relation_type({0, 2}, {2, 0}),
                                     relation_type({0, 0}, {0}),
                                     relation_type({0, 2}, {0}),
                                     relation_type({2, 0}, {0}),
                                     relation_type({1, 0}, {0, 1}),
                                     relation_type({1, 1}, {1, 1}),
                                     relation_type({1, 2}, {2, 1}),
                                     relation_type({1, 1, 1}, {1}),
                                     relation_type({1, 2}, {1}),
                                     relation_type({2, 1}, {1})};
  std::vector<relation_type> extra = {relation_type({0}, {1})};

  Congruence cong("twosided", 3, rels, extra);
  REPORTER.set_report(CONG_REPORT);

  REQUIRE(!cong.is_done());

  Partition<word_type>* ntc = cong.nontrivial_classes();
  REQUIRE(ntc->size() == 1);
  REQUIRE((*ntc)[0]->size() == 5);
  delete ntc;

  REQUIRE(cong.is_done());
}

TEST_CASE("Congruence 20: Infinite fp semigroup with infinite classes",
          "[quick][congruence][fpsemigroup][multithread][20]") {
  std::vector<relation_type> rels
      = {relation_type({0, 0, 0}, {0}), relation_type({0, 1}, {1, 0})};
  std::vector<relation_type> extra = {relation_type({0}, {0, 0})};
  Congruence                 cong("twosided", 2, rels, extra);
  REPORTER.set_report(CONG_REPORT);

  word_type x = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  word_type y
      = {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

  REQUIRE(cong.test_equals(x, y));

  REQUIRE(cong.test_less_than({0, 0, 0}, {1}));
  REQUIRE(!cong.test_less_than({1}, {0, 0, 0}));
  REQUIRE(!cong.test_less_than(x, y));
  REQUIRE(!cong.test_less_than(y, x));

  REQUIRE(!cong.is_done());

  cong.force_kbfp();  // clear data
  REQUIRE(cong.test_equals(x, y));
}

TEST_CASE("Congruence 21: trivial cong on an fp semigroup",
          "[quick][congruence][fpsemigroup][multithread][21]") {
  std::vector<relation_type> rels;
  rels.push_back(relation_type({0, 0, 0}, {0}));  // (a^3, a)
  rels.push_back(relation_type({0}, {1}));        // (a, b)
  std::vector<relation_type> extra;

  Congruence cong("left", 2, rels, extra);

  Partition<word_type>* ntc = cong.nontrivial_classes();
  REQUIRE(ntc->size() == 0);
  delete ntc;
}

TEST_CASE("Congruence 22: duplicate generators on a finite semigroup",
          "[quick][congruence][finite][multithread][22]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({7, 3, 5, 3, 4, 2, 7, 7}),
         new Transformation<u_int16_t>({7, 3, 5, 3, 4, 2, 7, 7}),
         new Transformation<u_int16_t>({7, 3, 5, 3, 4, 2, 7, 7}),
         new Transformation<u_int16_t>({3, 6, 3, 4, 0, 6, 0, 7})};
  Semigroup<> S = Semigroup<>(gens);
  REPORTER.set_report(CONG_REPORT);
  delete_gens(gens);
  Congruence cong("twosided", &S, std::vector<relation_type>());

  REQUIRE(cong.nr_classes() == S.size());
}

TEST_CASE("Congruence 23: test nontrivial_classes for a fp semigroup cong",
          "[quick][congruence][finite][fpsemigroup][23]") {
  std::vector<relation_type> rels
      = {relation_type({0, 0, 0}, {0}),
         relation_type({1, 0, 0}, {1, 0}),
         relation_type({1, 0, 1, 1, 1}, {1, 0}),
         relation_type({1, 1, 1, 1, 1}, {1, 1}),
         relation_type({1, 1, 0, 1, 1, 0}, {1, 0, 1, 0, 1, 1}),
         relation_type({0, 0, 1, 0, 1, 1, 0}, {0, 1, 0, 1, 1, 0}),
         relation_type({0, 0, 1, 1, 0, 1, 0}, {0, 1, 1, 0, 1, 0}),
         relation_type({0, 1, 0, 1, 0, 1, 0}, {1, 0, 1, 0, 1, 0}),
         relation_type({1, 0, 1, 0, 1, 0, 1}, {1, 0, 1, 0, 1, 0}),
         relation_type({1, 0, 1, 0, 1, 1, 0}, {1, 0, 1, 0, 1, 1}),
         relation_type({1, 0, 1, 1, 0, 1, 0}, {1, 0, 1, 1, 0, 1}),
         relation_type({1, 1, 0, 1, 0, 1, 0}, {1, 0, 1, 0, 1, 0}),
         relation_type({1, 1, 1, 1, 0, 1, 0}, {1, 0, 1, 0}),
         relation_type({0, 0, 1, 1, 1, 0, 1, 0}, {1, 1, 1, 0, 1, 0})};

  Congruence cong("twosided",
                  2,
                  rels,
                  std::vector<relation_type>({relation_type({0}, {1})}));
  REPORTER.set_report(CONG_REPORT);

  Partition<word_type>* ntc = cong.nontrivial_classes();
  REQUIRE(ntc->size() == 1);
  delete ntc;
}

TEST_CASE("Congruence 24: example from GAP which once messed up prefill",
          "[quick][congruence][multithread][24]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5, 6, 7}),
         new Transformation<u_int16_t>({1, 2, 3, 4, 5, 0, 6, 7}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5, 6, 7}),
         new Transformation<u_int16_t>({0, 1, 2, 3, 4, 0, 6, 7}),
         new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5, 7, 6})};
  Semigroup<> S = Semigroup<>(gens);
  REPORTER.set_report(CONG_REPORT);
  delete_gens(gens);

  std::vector<Element*> elms
      = {new Transformation<u_int16_t>({0, 0, 0, 0, 0, 0, 7, 6}),
         new Transformation<u_int16_t>({0, 0, 0, 0, 0, 0, 6, 7}),
         new Transformation<u_int16_t>({0, 0, 0, 0, 0, 0, 6, 7}),
         new Transformation<u_int16_t>({1, 1, 1, 1, 1, 1, 6, 7}),
         new Transformation<u_int16_t>({0, 0, 0, 0, 0, 0, 6, 7}),
         new Transformation<u_int16_t>({2, 2, 2, 2, 2, 2, 6, 7}),
         new Transformation<u_int16_t>({0, 0, 0, 0, 0, 0, 6, 7}),
         new Transformation<u_int16_t>({3, 3, 3, 3, 3, 3, 6, 7}),
         new Transformation<u_int16_t>({0, 0, 0, 0, 0, 0, 6, 7}),
         new Transformation<u_int16_t>({4, 4, 4, 4, 4, 4, 6, 7}),
         new Transformation<u_int16_t>({0, 0, 0, 0, 0, 0, 6, 7}),
         new Transformation<u_int16_t>({5, 5, 5, 5, 5, 5, 6, 7}),
         new Transformation<u_int16_t>({0, 0, 0, 0, 0, 0, 7, 6}),
         new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5, 7, 6})};

  std::vector<relation_type> extra;
  word_type                  w1, w2;
  for (size_t i = 0; i < elms.size(); i += 2) {
    S.factorisation(w1, S.position(elms[i]));
    S.factorisation(w2, S.position(elms[i + 1]));
    extra.push_back(std::make_pair(w1, w2));
    delete elms[i];
    delete elms[i + 1];
  }

  Congruence cong("right", &S, extra);
  REPORTER.set_report(CONG_REPORT);

  REQUIRE(cong.nr_classes() == 1);
}

TEST_CASE("Congruence 25: free semigroup with redundant relations",
          "[quick][congruence][multithread][fpsemigroup][25]") {
  std::vector<relation_type> extra = {relation_type({0, 0}, {0, 0})};
  Congruence                 cong("twosided", 1, {}, extra);
  REQUIRE(cong.test_equals({0, 0}, {0, 0}));
}

TEST_CASE("Congruence 26: free semigroup with redundant relations",
          "[quick][congruence][multithread][fpsemigroup][26]") {
  Congruence cong("twosided", 1, {}, {});
  REQUIRE(cong.test_equals({0, 0}, {0, 0}));
  REQUIRE(!cong.test_equals({0, 0}, {0}));
}

TEST_CASE("Congruence 27: is_obviously_infinite",
          "[quick][congruence][fpsemigroup][27]") {
  Congruence cong1("twosided",
                   3,
                   {relation_type({0, 1}, {0})},
                   {relation_type({2, 2}, {2})});
  REQUIRE(cong1.is_obviously_infinite());
  Congruence cong2("twosided",
                   3,
                   {relation_type({0, 1}, {0}), relation_type({0, 0}, {0})},
                   {relation_type({1, 1}, {1})});
  REQUIRE(cong2.is_obviously_infinite());
  Congruence cong3("twosided",
                   3,
                   {relation_type({0, 1}, {0}), relation_type({0, 0}, {0})},
                   {relation_type({1, 2}, {1})});
  REQUIRE(!cong3.is_obviously_infinite());

  std::vector<Element*> gens = {new Transformation<u_int16_t>({0, 1, 0}),
                                new Transformation<u_int16_t>({0, 1, 2})};

  Semigroup<> S = Semigroup<>(gens);
  REPORTER.set_report(CONG_REPORT);
  delete_gens(gens);

  Congruence cong4("twosided", &S, {relation_type({1}, {0})});
  REQUIRE(!cong4.is_obviously_infinite());
}

TEST_CASE("Congruence 28: test_less_than",
          "[quick][congruence][fpsemigroup][28]") {
  Congruence cong("twosided", 2, {relation_type({0, 0}, {0})}, {});
  REQUIRE(!cong.test_less_than({0, 0}, {0}));
}

/*TEST_CASE("Congruence 29: test BMat8", "[quick][congruence][fpsemigroup][29]")
{ std::vector<BMat8> gens = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0},
{0, 0, 0, 1}}), BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};

  Semigroup<BMat8> S(gens);
  REPORTER.set_report(CONG_REPORT);

  Congruence<BMat8> cong1("twosided", &S, {relation_type({1}, {0})});
  REQUIRE(cong1.nr_classes() == 3);
  REQUIRE(cong1.word_to_class_index({1}) == 0);

  Congruence<BMat8> cong2("twosided", &S, {relation_type({1}, {0})});
  cong2.force_kbfp();
  REQUIRE(cong2.nr_classes() == 3);
  REQUIRE(cong2.word_to_class_index({1}) == 0);

  Semigroup<BMat8>  T({gens[0], gens[2], gens[3]});
  Congruence<BMat8> cong3("twosided", &T, {relation_type({1}, {0})});
  cong3.force_p();
  REQUIRE(cong3.nr_classes() == 2);
  REQUIRE(cong3.word_to_class_index({1}) == 0);

  Congruence<BMat8> cong4("twosided", &S, {relation_type({1}, {0})});
  cong2.force_tc();
  REQUIRE(cong4.nr_classes() == 3);
  REQUIRE(cong4.word_to_class_index({1}) == 0);
}*/

std::vector<relation_type> RennerCommonTypeBMonoid(size_t l, int q) {
  // q is supposed to be 0 or 1
  std::vector<size_t> s;
  std::vector<size_t> e;
  for (size_t i = 0; i < l; ++i) {
    s.push_back(i);
  }
  for (size_t i = l; i < 2 * l + 1; ++i) {
    e.push_back(i);
  }
  size_t id = 2 * l + 1;

  std::vector<relation_type> rels = {relation_type({id, id}, {id})};
  // identity relations
  for (size_t i = 0; i < l; ++i) {
    rels.push_back({{s[i], id}, {s[i]}});
    rels.push_back({{id, s[i]}, {s[i]}});
    rels.push_back({{id, e[i]}, {e[i]}});
    rels.push_back({{e[i], id}, {e[i]}});
  }
  rels.push_back({{id, e[l]}, {e[l]}});
  rels.push_back({{e[l], id}, {e[l]}});

  switch (q) {
    case 0:
      for (size_t i = 0; i < l; ++i)
        rels.push_back({{s[i], s[i]}, {s[i]}});
      break;
    case 1:
      for (size_t i = 0; i < l; ++i)
        rels.push_back({{s[i], s[i]}, {id}});
      break;
      // default: assert(FALSE)
  }
  for (int i = 0; i < static_cast<int>(l); ++i) {
    for (int j = 0; j < static_cast<int>(l); ++j) {
      if (std::abs(i - j) >= 2) {
        rels.push_back({{s[i], s[j]}, {s[j], s[i]}});
      }
    }
  }

  for (size_t i = 1; i < l - 1; ++i) {
    rels.push_back({{s[i], s[i + 1], s[i]}, {s[i + 1], s[i], s[i + 1]}});
  }

  rels.push_back({{s[1], s[0], s[1], s[0]}, {s[0], s[1], s[0], s[1]}});

  for (size_t i = 1; i < l; ++i) {
    for (size_t j = 0; j < i; ++j) {
      rels.push_back({{s[i], e[j]}, {e[j], s[i]}});
    }
  }

  for (size_t i = 0; i < l; ++i) {
    for (size_t j = i + 1; j < l + 1; ++j) {
      rels.push_back({{s[i], e[j]}, {e[j], s[i]}});
      rels.push_back({{s[i], e[j]}, {e[j]}});
    }
  }

  for (size_t i = 0; i < l + 1; ++i) {
    for (size_t j = 0; j < l + 1; ++j) {
      rels.push_back({{e[i], e[j]}, {e[j], e[i]}});
      rels.push_back({{e[i], e[j]}, {e[std::max(i, j)]}});
    }
  }

  for (size_t i = 0; i < l; ++i) {
    rels.push_back({{e[i], s[i], e[i]}, {e[i + 1]}});
  }

  // Extra check relations
  // rels.push_back({{e[3]},{e[2]}});
  // rels.push_back({{e[0], s[0], s[1], s[0], s[2], s[1], s[0], e[0]}, {s[2]}});

  return rels;
}

std::vector<relation_type> EGTypeBMonoid(size_t l, int q) {
  std::vector<size_t> s;
  std::vector<size_t> e;
  for (size_t i = 0; i < l; ++i) {
    s.push_back(i);
  }
  for (size_t i = l; i < 2 * l + 1; ++i) {
    e.push_back(i);
  }
  size_t id = 2 * l + 1;

  std::vector<relation_type> rels = RennerCommonTypeBMonoid(l, q);

  if (l >= 2)
    rels.push_back({{e[0], s[0], s[1], s[0], e[0]}, {e[2]}});

  return rels;
}

std::vector<size_t> max_elt_B(size_t i) {
  std::vector<size_t> t(0);
  for (int end = i; end >= 0; end--) {
    for (int k = 0; k <= end; k++) {
      t.push_back(k);
    }
  }
  return t;
}

std::vector<relation_type> RennerTypeBMonoid(size_t l, int q) {
  std::vector<size_t> s;
  std::vector<size_t> e;
  for (size_t i = 0; i < l; ++i) {
    s.push_back(i);
  }
  for (size_t i = l; i < 2 * l + 1; ++i) {
    e.push_back(i);
  }
  size_t id = 2 * l + 1;

  std::vector<relation_type> rels = RennerCommonTypeBMonoid(l, q);

  for (size_t i = 1; i < l; i++) {
    std::vector<size_t> new_rel = max_elt_B(i);
    new_rel.push_back(e[0]);
    new_rel.insert(new_rel.begin(), e[0]);
    rels.push_back({new_rel, {e[i + 1]}});
  }

  return rels;
}

std::vector<relation_type> RennerCommonTypeDMonoid(size_t l, int q) {
  // q is supposed to be 0 or 1
  std::vector<size_t> s;
  std::vector<size_t> e;
  for (size_t i = 0; i < l; ++i) {
    s.push_back(i);  // 0 est \pi_1^f et 1 est \pi_1^e
  }
  for (size_t i = l; i < 2 * l + 1; ++i) {
    e.push_back(i);
  }
  size_t f  = 2 * l + 1;
  size_t id = 2 * l + 2;

  std::vector<relation_type> rels = {relation_type({id, id}, {id})};
  // identity relations
  for (size_t i = 0; i < l; ++i) {
    rels.push_back({{s[i], id}, {s[i]}});
    rels.push_back({{id, s[i]}, {s[i]}});
    rels.push_back({{id, e[i]}, {e[i]}});
    rels.push_back({{e[i], id}, {e[i]}});
  }
  rels.push_back({{id, e[l]}, {e[l]}});
  rels.push_back({{e[l], id}, {e[l]}});
  rels.push_back({{id, f}, {f}});
  rels.push_back({{f, id}, {f}});

  switch (q) {
    case 0:
      for (size_t i = 0; i < l; ++i)
        rels.push_back({{s[i], s[i]}, {s[i]}});
      break;
    case 1:
      for (size_t i = 0; i < l; ++i)
        rels.push_back({{s[i], s[i]}, {id}});
      break;
      // default: assert(FALSE)
  }
  for (int i = 1; i < static_cast<int>(l); ++i) {  // tout sauf \pi_1^f
    for (int j = 1; j < static_cast<int>(l); ++j) {
      if (std::abs(i - j) >= 2) {
        rels.push_back({{s[i], s[j]}, {s[j], s[i]}});
      }
    }
  }
  for (int i = 3; i < static_cast<int>(l);
       ++i) {  // \pi_1^f avec les \pi_i pour i>2
    rels.push_back({{s[0], s[i]}, {s[i], s[0]}});
  }
  rels.push_back({{s[0], s[1]}, {s[1], s[0]}});  // \pi_1^f avec \pi_1^e

  for (size_t i = 1; i < l - 1; ++i) {  // tresses
    rels.push_back({{s[i], s[i + 1], s[i]}, {s[i + 1], s[i], s[i + 1]}});
  }
  rels.push_back(
      {{s[0], s[2], s[0]}, {s[2], s[0], s[2]}});  // tresse de \pi_1^f

  for (size_t i = 2; i < l; ++i) {  // commutation, attention début à 2
    for (size_t j = 0; j < i; ++j) {
      rels.push_back({{s[i], e[j]}, {e[j], s[i]}});
    }
    rels.push_back({{s[i], f}, {f, s[i]}});
  }
  rels.push_back({{s[0], f}, {f, s[0]}});        // commutation \pi_1^f et f
  rels.push_back({{s[1], e[0]}, {e[0], s[1]}});  // commutation \pi_1^e et e

  for (size_t i = 1; i < l; ++i) {  // absorption
    for (size_t j = i + 1; j < l + 1; ++j) {
      rels.push_back({{s[i], e[j]}, {e[j], s[i]}});
      rels.push_back({{s[i], e[j]}, {e[j]}});
      if (i == 1) {  // cas particulier \pi_1^f
        rels.push_back({{s[0], e[j]}, {e[j], s[0]}});
        rels.push_back({{s[0], e[j]}, {e[j]}});
      }
    }
  }

  for (size_t i = 0; i < l + 1; ++i) {  // produit e_i
    for (size_t j = 0; j < l + 1; ++j) {
      rels.push_back({{e[i], e[j]}, {e[j], e[i]}});
      rels.push_back({{e[i], e[j]}, {e[std::max(i, j)]}});
    }
    if (i > 1) {
      rels.push_back({{f, e[i]}, {e[i], f}});
      rels.push_back({{f, e[i]}, {e[i]}});
    }
  }
  rels.push_back({{f, f}, {f}});
  rels.push_back({{f, e[0]}, {e[1]}});
  rels.push_back({{e[0], f}, {e[1]}});

  for (size_t i = 2; i < l; ++i) {
    rels.push_back({{e[i], s[i], e[i]}, {e[i + 1]}});
  }
  rels.push_back({{e[0], s[0], e[0]}, {e[2]}});
  rels.push_back({{f, s[1], f}, {e[2]}});

  return rels;
}

std::vector<relation_type> EGTypeDMonoid(size_t l, int q) {
  std::vector<size_t> s;
  std::vector<size_t> e;
  for (size_t i = 0; i < l; ++i) {
    s.push_back(i);
  }
  for (size_t i = l; i < 2 * l + 1; ++i) {
    e.push_back(i);
  }
  size_t f  = 2 * l + 1;
  size_t id = 2 * l + 2;

  std::vector<relation_type> rels = RennerCommonTypeDMonoid(l, q);

  if (l >= 3) {
    rels.push_back({{e[0], s[0], s[2], s[1], f}, {e[3]}});
    rels.push_back({{f, s[1], s[2], s[0], e[0]}, {e[3]}});
  }
  return rels;
}

std::vector<size_t> max_elt_D(size_t i, int g) {
  // g est 0 ou 1 : 0 pour f et 1 pour e
  std::vector<size_t> t(0);
  int                 parity = g;
  for (int end = i; end > 0; end--) {
    t.push_back(parity);
    for (int k = 2; k <= end; k++) {
      t.push_back(k);
    }
    parity = (parity + 1) % 2;
  }
  return t;
}

#include <iostream>

std::vector<relation_type> RennerTypeDMonoid(size_t l, int q) {
  std::vector<size_t> s;
  std::vector<size_t> e;
  for (size_t i = 0; i < l; ++i) {
    s.push_back(i);
  }
  for (size_t i = l; i < 2 * l + 1; ++i) {
    e.push_back(i);
  }
  size_t f  = 2 * l + 1;
  size_t id = 2 * l + 2;

  std::vector<relation_type> rels = RennerCommonTypeDMonoid(l, q);

  for (size_t i = 2; i < l; i++) {
    std::vector<size_t> new_rel_f = max_elt_D(i, 0);
    std::vector<size_t> new_rel_e = max_elt_D(i, 1);

    if (i % 2 == 0) {
      new_rel_e.insert(new_rel_e.begin(), f);
      new_rel_e.push_back(e[0]);
      rels.push_back({new_rel_e, {e[i + 1]}});

      new_rel_f.insert(new_rel_f.begin(), e[0]);
      new_rel_f.push_back(f);
      rels.push_back({new_rel_f, {e[i + 1]}});
    } else {
      new_rel_e.insert(new_rel_e.begin(), f);
      new_rel_e.push_back(f);
      rels.push_back({new_rel_e, {e[i + 1]}});

      new_rel_f.insert(new_rel_f.begin(), e[0]);
      new_rel_f.push_back(e[0]);
      rels.push_back({new_rel_f, {e[i + 1]}});
    }
  }
  return rels;
}

TEST_CASE("Congruence 29: Renner monoid type B2 (E. G. presentation), q = 1",
          "[congruence][fpsemigroup][quick][29]") {
  Congruence cong("twosided", 6, {}, EGTypeBMonoid(2, 1));
  std::cout << to_string(EGTypeBMonoid(2, 1)) << std::endl;
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 57);
}

TEST_CASE("Congruence 30: Renner monoid type B2 (E. G. presentation), q = 0",
          "[congruence][fpsemigroup][quick][30]") {
  Congruence cong("twosided", 6, {}, EGTypeBMonoid(2, 0));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 57);
}

// Loops for ever: Infinite monoid ???
TEST_CASE("Congruence 31: Renner monoid type B3 (E. G. presentation), q = 1",
          "[congruence][fpsemigroup][31]") {
  Congruence cong("twosided", 8, {}, EGTypeBMonoid(3, 1));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 757);
}

// Loops for ever: Infinite monoid ???
TEST_CASE("Congruence 32: Renner monoid type B3 (E. G. presentation), q = 0",
          "[congruence][fpsemigroup][32]") {
  Congruence cong("twosided", 8, {}, EGTypeBMonoid(3, 0));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 757);
}

TEST_CASE(
    "Congruence 33: Renner monoid type B2 (Gay-Hivert presentation), q = 1",
    "[congruence][fpsemigroup][quick][33]") {
  Congruence cong("twosided", 6, {}, RennerTypeBMonoid(2, 1));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 57);
}

TEST_CASE(
    "Congruence 34: Renner monoid type B2 (Gay-Hivert presentation), q = 0",
    "[congruence][fpsemigroup][quick][34]") {
  Congruence cong("twosided", 6, {}, RennerTypeBMonoid(2, 0));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 57);
}

TEST_CASE(
    "Congruence 35: Renner monoid type B3 (Gay-Hivert presentation), q = 1",
    "[congruence][fpsemigroup][quick][35]") {
  Congruence cong("twosided", 8, {}, RennerTypeBMonoid(3, 1));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 757);
}

TEST_CASE(
    "Congruence 36: Renner monoid type B3 (Gay-Hivert presentation), q = 0",
    "[congruence][fpsemigroup][quick][36]") {
  Congruence cong("twosided", 8, {}, RennerTypeBMonoid(3, 0));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 757);
}

TEST_CASE(
    "Congruence 37: Renner monoid type B4 (Gay-Hivert presentation), q = 1",
    "[congruence][fpsemigroup][standard][37]") {
  Congruence cong("twosided", 10, {}, RennerTypeBMonoid(4, 1));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 13889);
}

TEST_CASE(
    "Congruence 38: Renner monoid type B4 (Gay-Hivert presentation), q = 0",
    "[congruence][fpsemigroup][standard][38]") {
  Congruence cong("twosided", 10, {}, RennerTypeBMonoid(4, 0));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 13889);
}

TEST_CASE(
    "Congruence 39: Renner monoid type B5 (Gay-Hivert presentation), q = 1",
    "[congruence][fpsemigroup][extreme][39]") {
  Congruence cong("twosided", 12, {}, RennerTypeBMonoid(5, 1));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 322021);
}

TEST_CASE(
    "Congruence 40: Renner monoid type B5 (Gay-Hivert presentation), q = 0",
    "[congruence][fpsemigroup][extreme][40]") {
  Congruence cong("twosided", 12, {}, RennerTypeBMonoid(5, 0));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 322021);
}

// TODO: segfaults!
TEST_CASE("Congruence 41: Renner monoid type D2 (E. G. presentation), q = 1",
          "[congruence][fpsemigroup][41]") {
  Congruence cong("twosided", 7, {}, EGTypeDMonoid(2, 1));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 37);
}

// TODO: fails!
TEST_CASE("Congruence 42: Renner monoid type D2 (E. G. presentation), q = 0",
          "[congruence][fpsemigroup][42]") {
  Congruence cong("twosided", 7, {}, EGTypeDMonoid(2, 0));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 37);
}

TEST_CASE("Congruence 43: Renner monoid type D3 (E. G. presentation), q = 1",
          "[congruence][fpsemigroup][quick][43]") {
  Congruence cong("twosided", 9, {}, EGTypeDMonoid(3, 1));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 541);
}

TEST_CASE("Congruence 44: Renner monoid type D3 (E. G. presentation), q = 0",
          "[congruence][fpsemigroup][quick][44]") {
  Congruence cong("twosided", 9, {}, EGTypeDMonoid(3, 0));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 541);
}

// Loops for ever: Infinite monoid ???
TEST_CASE("Congruence 45: Renner monoid type D4 (E. G. presentation), q = 1",
          "[congruence][fpsemigroup][45]") {
  Congruence cong("twosided", 11, {}, EGTypeDMonoid(4, 1));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 10625);
}

// Loops for ever: Infinite monoid ???
TEST_CASE("Congruence 46: Renner monoid type D4 (E. G. presentation), q = 0",
          "[congruence][fpsemigroup][46]") {
  Congruence cong("twosided", 11, {}, EGTypeDMonoid(4, 0));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 10625);
}

// TODO: fails!
TEST_CASE(
    "Congruence 47: Renner monoid type D2 (Gay-Hivert presentation), q = 1",
    "[congruence][fpsemigroup][47]") {
  Congruence cong("twosided", 7, {}, RennerTypeDMonoid(2, 1));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 37);
}

// TODO: segfaults!
TEST_CASE(
    "Congruence 48: Renner monoid type D2 (Gay-Hivert presentation), q = 0",
    "[congruence][fpsemigroup][48]") {
  Congruence cong("twosided", 7, {}, RennerTypeDMonoid(2, 0));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 37);
}

TEST_CASE(
    "Congruence 49: Renner monoid type D3 (Gay-Hivert presentation), q = 1",
    "[congruence][fpsemigroup][quick][49]") {
  Congruence cong("twosided", 9, {}, RennerTypeDMonoid(3, 1));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 541);
}

TEST_CASE(
    "Congruence 50: Renner monoid type D3 (Gay-Hivert presentation), q = 0",
    "[congruence][fpsemigroup][quick][50]") {
  Congruence cong("twosided", 9, {}, RennerTypeDMonoid(3, 0));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 541);
}

TEST_CASE(
    "Congruence 51: Renner monoid type D4 (Gay-Hivert presentation), q = 1",
    "[congruence][fpsemigroup][quick][51]") {
  Congruence cong("twosided", 11, {}, RennerTypeDMonoid(4, 1));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 10625);
}

TEST_CASE(
    "Congruence 52: Renner monoid type D4 (Gay-Hivert presentation), q = 0",
    "[congruence][fpsemigroup][standard][52]") {
  Congruence cong("twosided", 11, {}, RennerTypeDMonoid(4, 0));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 10625);
}
// HERE
TEST_CASE(
    "Congruence 53: Renner monoid type D5 (Gay-Hivert presentation), q = 1",
    "[congruence][fpsemigroup][extreme][53]") {
  Congruence cong("twosided", 13, {}, RennerTypeDMonoid(5, 1));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 258661);
}

TEST_CASE(
    "Congruence 54: Renner monoid type D5 (Gay-Hivert presentation), q = 0",
    "[congruence][fpsemigroup][extreme][54]") {
  Congruence cong("twosided", 13, {}, RennerTypeDMonoid(5, 0));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 258661);
}

// The two following are quite large, so I keep them commented out
// TEST_CASE("Congruence 55: Renner monoid type D6 (Gay-Hivert presentation), q
// = 1",
//           "[congruence][fpsemigroup][55]") {
//   Congruence cong("twosided", 15, {}, RennerTypeDMonoid(6, 1));
//   REPORTER.set_report(CONG_REPORT);
//   REQUIRE(!cong.is_obviously_infinite());
//   REQUIRE(cong.nr_classes() == 7464625);
// }

// TEST_CASE("Congruence 56: Renner monoid type D6 (Gay-Hivert presentation), q
// = 0",
//           "[congruence][fpsemigroup][56]") {
//   Congruence cong("twosided", 15, {}, RennerTypeDMonoid(6, 0));
//   REPORTER.set_report(CONG_REPORT);
//   REQUIRE(!cong.is_obviously_infinite());
//   REQUIRE(cong.nr_classes() == 7464625);
// }

std::vector<relation_type> RookMonoid(size_t l, int q) {
  // q is supposed to be 0 or 1

  std::vector<size_t> s;
  for (size_t i = 0; i < l; ++i) {
    s.push_back(i);  // 0 est \pi_0
  }

  // identity relations
  size_t                     id   = l;
  std::vector<relation_type> rels = {relation_type({id, id}, {id})};
  for (size_t i = 0; i < l; ++i) {
    rels.push_back({{s[i], id}, {s[i]}});
    rels.push_back({{id, s[i]}, {s[i]}});
  }

  switch (q) {
    case 0:
      for (size_t i = 0; i < l; ++i)
        rels.push_back({{s[i], s[i]}, {s[i]}});
      break;
    case 1:
      rels.push_back({{s[0], s[0]}, {s[0]}});
      for (size_t i = 1; i < l; ++i)
        rels.push_back({{s[i], s[i]}, {id}});
      break;
      // default: assert(FALSE)
  }
  for (int i = 0; i < static_cast<int>(l); ++i) {
    for (int j = 0; j < static_cast<int>(l); ++j) {
      if (std::abs(i - j) >= 2) {
        rels.push_back({{s[i], s[j]}, {s[j], s[i]}});
      }
    }
  }

  for (size_t i = 1; i < l - 1; ++i) {
    rels.push_back({{s[i], s[i + 1], s[i]}, {s[i + 1], s[i], s[i + 1]}});
  }

  rels.push_back({{s[1], s[0], s[1], s[0]}, {s[0], s[1], s[0], s[1]}});
  rels.push_back({{s[1], s[0], s[1], s[0]}, {s[0], s[1], s[0]}});

  return rels;
}

TEST_CASE("Congruence 57: Rook monoid R5, q = 0",
          "[congruence][fpsemigroup][quick][57]") {
  Congruence cong("twosided", 6, {}, RookMonoid(5, 0));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 1546);
}

TEST_CASE("Congruence 58: Rook monoid R5, q = 1",
          "[congruence][fpsemigroup][quick][58]") {
  Congruence cong("twosided", 6, {}, RookMonoid(5, 1));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 1546);
}

TEST_CASE("Congruence 59: Rook monoid R6, q = 0",
          "[congruence][fpsemigroup][quick][59]") {
  Congruence cong("twosided", 7, {}, RookMonoid(6, 0));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 13327);
}

TEST_CASE("Congruence 60: Rook monoid R6, q = 1",
          "[congruence][fpsemigroup][quick][60]") {
  Congruence cong("twosided", 7, {}, RookMonoid(6, 1));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 13327);
}

template <typename T>
std::vector<T> concat(std::vector<T> lhs, const std::vector<T>& rhs) {
  lhs.insert(lhs.end(), rhs.begin(), rhs.end());
  return lhs;
}

std::vector<relation_type> Stell(size_t l) {
  std::vector<size_t> pi;
  for (size_t i = 0; i < l; ++i) {
    pi.push_back(i);  // 0 est \pi_0
  }

  std::vector<relation_type> rels{};
  std::vector<size_t>        t{pi[0]};
  for (int i = 1; i < static_cast<int>(l); ++i) {
    t.insert(t.begin(), pi[i]);
    rels.push_back({concat(t, {pi[i]}), t});
  }
  return rels;
}

TEST_CASE("Congruence 61: Stellar S2", "[congruence][fpsemigroup][quick][61]") {
  Congruence cong("twosided", 3, RookMonoid(2, 0), Stell(2));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 5);
}

TEST_CASE("Congruence 62: Stellar S3", "[congruence][fpsemigroup][quick][62]") {
  Congruence cong("twosided", 4, RookMonoid(3, 0), Stell(3));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 16);
}

TEST_CASE("Congruence 63: Stellar S4", "[congruence][fpsemigroup][quick][63]") {
  Congruence cong("twosided", 5, RookMonoid(4, 0), Stell(4));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 65);
}

TEST_CASE("Congruence 64: Stellar S5", "[congruence][fpsemigroup][quick][64]") {
  Congruence cong("twosided", 6, RookMonoid(5, 0), Stell(5));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 326);
}

TEST_CASE("Congruence 65: Stellar S6", "[congruence][fpsemigroup][quick][65]") {
  Congruence cong("twosided", 7, RookMonoid(6, 0), Stell(6));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 1957);
}

TEST_CASE("Congruence 66: Stellar S7", "[congruence][fpsemigroup][quick][66]") {
  Congruence cong("twosided", 8, RookMonoid(7, 0), Stell(7));
  REPORTER.set_report(CONG_REPORT);
  REQUIRE(!cong.is_obviously_infinite());
  REQUIRE(cong.nr_classes() == 13700);
}

TEST_CASE(
    "Congruence 67: transformation semigroup size 88, duplicate generators",
    "[quick][congruence][multithread]") {
  std::vector<Element*> gens = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
                                new Transformation<u_int16_t>({3, 2, 1, 3, 3}),
                                new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
  Semigroup<>           S    = Semigroup<>(gens);
  REPORTER.set_report(CONG_REPORT);
  delete_gens(gens);
  REQUIRE(S.size() == 88);
  REQUIRE(S.nrrules() == 21);
  REQUIRE(S.degree() == 5);
  Element*  t1 = new Transformation<u_int16_t>({3, 4, 4, 4, 4});
  Element*  t2 = new Transformation<u_int16_t>({3, 1, 3, 3, 3});
  word_type w1, w2;
  S.factorisation(w1, S.position(t1));
  S.factorisation(w2, S.position(t2));
  std::vector<relation_type> extra({std::make_pair(w1, w2)});
  Congruence                 cong("twosided", &S, extra);
  REPORTER.set_report(CONG_REPORT);

  REQUIRE(cong.nr_classes() == 21);
  REQUIRE(cong.nr_classes() == 21);
  Element*  t3 = new Transformation<u_int16_t>({1, 3, 1, 3, 3});
  Element*  t4 = new Transformation<u_int16_t>({4, 2, 4, 4, 2});
  word_type w3, w4;
  S.factorisation(w3, S.position(t3));
  S.factorisation(w4, S.position(t4));
  REQUIRE(cong.word_to_class_index(w3) == cong.word_to_class_index(w4));
  REQUIRE(cong.test_equals(w3, w4));

  delete t1;
  delete t2;
  delete t3;
  delete t4;
}
