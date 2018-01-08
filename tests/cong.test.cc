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

#include <utility>

#include "../src/cong.h"
#include "catch.hpp"

#define CONG_REPORT false

using namespace libsemigroups;
TEST_CASE("Congruence 00: 5-parameter constructor",
          "[quick][congruence][fpsemigroup][multithread][00]") {
  std::vector<relation_t> rels;
  rels.push_back(relation_t({0, 0, 0}, {0}));  // (a^3, a)
  rels.push_back(relation_t({0}, {1, 1}));     // (a, b^2)
  std::vector<relation_t> extra;

  Congruence cong("left", 2, rels, extra);
}

TEST_CASE("Congruence 01: Small fp semigroup",
          "[quick][congruence][fpsemigroup][multithread][01]") {
  std::vector<relation_t> rels;
  rels.push_back(relation_t({0, 0, 0}, {0}));  // (a^3, a)
  rels.push_back(relation_t({0}, {1, 1}));     // (a, b^2)
  std::vector<relation_t> extra;

  Congruence cong("twosided", 2, rels, extra);
  cong.set_report(CONG_REPORT);

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
  std::vector<relation_t> rels;
  rels.push_back(relation_t({0, 0, 0}, {0}));  // (a^3, a)
  rels.push_back(relation_t({0}, {1, 1}));     // (a, b^2)
  std::vector<relation_t> extra;

  Congruence cong("left", 2, rels, extra);
  cong.set_report(CONG_REPORT);
  REQUIRE(cong.nr_classes() == 5);
}

TEST_CASE("Congruence 03: Small right congruence on free semigroup",
          "[quick][congruence][fpsemigroup][multithread][03]") {
  std::vector<relation_t> rels;
  rels.push_back(relation_t({0, 0, 0}, {0}));  // (a^3, a)
  rels.push_back(relation_t({0}, {1, 1}));     // (a, b^2)
  std::vector<relation_t> extra;

  Congruence cong("right", 2, rels, extra);
  cong.set_report(CONG_REPORT);
  REQUIRE(cong.nr_classes() == 5);
  REQUIRE(cong.is_done());
}

TEST_CASE(
    "Congruence 04: word_to_class_index for left congruence on free semigroup",
    "[quick][congruence][fpsemigroup][multithread][04]") {
  std::vector<relation_t> rels;
  rels.push_back(relation_t({0, 0, 0}, {0}));  // (a^3, a)
  rels.push_back(relation_t({0}, {1, 1}));     // (a, b^2)
  std::vector<relation_t> extra;

  Congruence cong("left", 2, rels, extra);
  cong.set_report(CONG_REPORT);

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
  std::vector<relation_t> rels;
  rels.push_back(relation_t({0, 0, 0}, {0}));  // (a^3, a)
  rels.push_back(relation_t({0}, {1, 1}));     // (a, b^2)
  std::vector<relation_t> extra;

  Congruence cong1("twosided", 2, rels, extra);
  cong1.set_report(CONG_REPORT);

  REQUIRE(cong1.word_to_class_index({0, 0, 1})
          == cong1.word_to_class_index({0, 0, 0, 0, 1}));
  REQUIRE(cong1.word_to_class_index({0, 1, 1, 0, 0, 1})
          == cong1.word_to_class_index({0, 0, 0, 0, 1}));
  REQUIRE(cong1.word_to_class_index({0, 0, 0})
          == cong1.word_to_class_index({1, 1}));
  REQUIRE(cong1.word_to_class_index({1}) != cong1.word_to_class_index({0}));

  Congruence cong2("twosided", 2, rels, extra);
  cong2.set_report(CONG_REPORT);

  REQUIRE(cong2.word_to_class_index({0, 0, 0, 0})
          == cong2.word_to_class_index({0, 0}));
  REQUIRE(cong2.test_equals({0, 0, 0, 0}, {0, 1, 1, 0, 1, 1}));
}

TEST_CASE("Congruence 06: 6-argument constructor (trivial cong)",
          "[quick][congruence][multithread][06]") {
  std::vector<Element*> gens = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
                                new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(CONG_REPORT);
  really_delete_cont(gens);

  REQUIRE(S.size() == 88);
  REQUIRE(S.degree() == 5);

  std::vector<relation_t> extra;
  Congruence              cong("twosided", &S, extra);
  cong.set_report(CONG_REPORT);
  REQUIRE(!cong.is_done());

  Partition<word_t>* ntc = cong.nontrivial_classes();
  REQUIRE(ntc->size() == 0);
  delete ntc;
}

TEST_CASE("Congruence 07: 6-argument constructor (nontrivial cong)",
          "[quick][congruence][multithread][07]") {
  std::vector<Element*> gens = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
                                new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(CONG_REPORT);
  really_delete_cont(gens);
  REQUIRE(S.size() == 88);
  REQUIRE(S.degree() == 5);

  Element* t1 = new Transformation<u_int16_t>({3, 4, 4, 4, 4});
  Element* t2 = new Transformation<u_int16_t>({3, 1, 3, 3, 3});
  word_t   w1, w2;
  S.factorisation(w1, S.position(t1));
  S.factorisation(w2, S.position(t2));
  std::vector<relation_t> extra({relation_t(w1, w2)});
  Congruence              cong("twosided", &S, extra);
  cong.set_report(CONG_REPORT);

  t1->really_delete();
  t2->really_delete();
  delete t1;
  delete t2;
}

TEST_CASE("Congruence 8T: transformation semigroup size 88",
          "[quick][congruence][multithread]") {
  std::vector<Element*> gens = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
                                new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(CONG_REPORT);
  really_delete_cont(gens);
  REQUIRE(S.size() == 88);
  REQUIRE(S.nrrules() == 18);
  REQUIRE(S.degree() == 5);
  Element* t1 = new Transformation<u_int16_t>({3, 4, 4, 4, 4});
  Element* t2 = new Transformation<u_int16_t>({3, 1, 3, 3, 3});
  word_t   w1, w2;
  S.factorisation(w1, S.position(t1));
  S.factorisation(w2, S.position(t2));
  std::vector<relation_t> extra({std::make_pair(w1, w2)});
  Congruence              cong("twosided", &S, extra);
  cong.set_report(CONG_REPORT);

  REQUIRE(cong.nr_classes() == 21);
  REQUIRE(cong.nr_classes() == 21);
  Element* t3 = new Transformation<u_int16_t>({1, 3, 1, 3, 3});
  Element* t4 = new Transformation<u_int16_t>({4, 2, 4, 4, 2});
  word_t   w3, w4;
  S.factorisation(w3, S.position(t3));
  S.factorisation(w4, S.position(t4));
  REQUIRE(cong.word_to_class_index(w3) == cong.word_to_class_index(w4));
  REQUIRE(cong.test_equals(w3, w4));

  t1->really_delete();
  t2->really_delete();
  t3->really_delete();
  t4->really_delete();
  delete t1;
  delete t2;
  delete t3;
  delete t4;
}

TEST_CASE("Congruence 8L: left congruence on transformation semigroup size 88",
          "[quick][congruence][multithread]") {
  std::vector<Element*> gens = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
                                new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(CONG_REPORT);
  really_delete_cont(gens);

  // REQUIRE(S.size() == 88);
  // REQUIRE(S.degree() == 5);
  std::vector<relation_t> extra(
      {relation_t({0, 1, 0, 0, 0, 1, 1, 0, 0}, {1, 0, 0, 0, 1})});
  Congruence cong("left", &S, extra);
  cong.set_report(CONG_REPORT);

  REQUIRE(cong.nr_classes() == 69);
  REQUIRE(cong.nr_classes() == 69);

  Element* t3 = new Transformation<u_int16_t>({1, 3, 1, 3, 3});
  Element* t4 = new Transformation<u_int16_t>({4, 2, 4, 4, 2});
  word_t   w3, w4;
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

  t3->really_delete();
  t4->really_delete();
  delete t3;
  delete t4;
}

TEST_CASE("Congruence 8R: right congruence on transformation semigroup size 88",
          "[quick][congruence][multithread]") {
  std::vector<Element*> gens = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
                                new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(CONG_REPORT);
  S.set_report(CONG_REPORT);
  really_delete_cont(gens);

  REQUIRE(S.size() == 88);
  REQUIRE(S.nrrules() == 18);
  REQUIRE(S.degree() == 5);
  Element* t1 = new Transformation<u_int16_t>({3, 4, 4, 4, 4});
  Element* t2 = new Transformation<u_int16_t>({3, 1, 3, 3, 3});
  word_t   w1, w2;
  S.factorisation(w1, S.position(t1));
  S.factorisation(w2, S.position(t2));
  std::vector<relation_t> extra({std::make_pair(w1, w2)});
  Congruence              cong("right", &S, extra);
  cong.set_report(CONG_REPORT);

  REQUIRE(cong.nr_classes() == 72);
  REQUIRE(cong.nr_classes() == 72);
  Element* t3 = new Transformation<u_int16_t>({1, 3, 3, 3, 3});
  Element* t4 = new Transformation<u_int16_t>({4, 2, 4, 4, 2});
  Element* t5 = new Transformation<u_int16_t>({2, 3, 2, 2, 2});
  Element* t6 = new Transformation<u_int16_t>({2, 3, 3, 3, 3});
  word_t   w3, w4, w5, w6;
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

  t1->really_delete();
  t2->really_delete();
  t3->really_delete();
  t4->really_delete();
  t5->really_delete();
  t6->really_delete();
  delete t1;
  delete t2;
  delete t3;
  delete t4;
  delete t5;
  delete t6;
}

TEST_CASE("Congruence 09: for an infinite fp semigroup",
          "[quick][congruence][fpsemigroup][multithread][09]") {
  std::vector<relation_t> relations = {relation_t({0, 1}, {1, 0}),
                                       relation_t({0, 2}, {2, 2}),
                                       relation_t({0, 2}, {0}),
                                       relation_t({0, 2}, {0}),
                                       relation_t({2, 2}, {0}),
                                       relation_t({1, 2}, {1, 2}),
                                       relation_t({1, 2}, {2, 2}),
                                       relation_t({1, 2, 2}, {1}),
                                       relation_t({1, 2}, {1}),
                                       relation_t({2, 2}, {1})};
  std::vector<relation_t> extra = {relation_t({0}, {1})};
  Congruence              cong("twosided", 3, relations, extra);
  cong.set_report(CONG_REPORT);

  REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({1}));
  REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({1, 0}));
  REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({1, 1}));
  REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({1, 0, 1}));

  REQUIRE(cong.test_equals({1}, {1, 1}));
  REQUIRE(cong.test_equals({1, 0, 1}, {1, 0}));
}

TEST_CASE("Congruence 10: for an infinite fp semigroup",
          "[quick][congruence][fpsemigroup][multithread][10]") {
  std::vector<relation_t> rels = {relation_t({0, 1}, {1, 0}),
                                  relation_t({0, 2}, {2, 0}),
                                  relation_t({0, 0}, {0}),
                                  relation_t({0, 2}, {0}),
                                  relation_t({2, 0}, {0}),
                                  relation_t({1, 2}, {2, 1}),
                                  relation_t({1, 1, 1}, {1}),
                                  relation_t({1, 2}, {1}),
                                  relation_t({2, 1}, {1})};
  std::vector<relation_t> extra = {{{0}, {1}}};
  Congruence              cong("twosided", 3, rels, extra);
  cong.set_report(CONG_REPORT);
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
  S.set_report(CONG_REPORT);
  really_delete_cont(gens);

  // The following lines are intentionally commented out so that we can check
  // that P does not enumerate the semigroup, they remain to remind us of the
  // size and number of rules of the semigroups.
  // REQUIRE(S.size(false) == 11804);
  // REQUIRE(S.nrrules(false) == 2460);

  std::vector<relation_t> extra(
      {relation_t({0, 3, 2, 1, 3, 2, 2}, {3, 2, 2, 1, 3, 3})});
  Congruence cong("twosided", &S, extra);
  cong.set_report(CONG_REPORT);

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
  std::vector<Element*> gens = {
      new PBR(new std::vector<std::vector<u_int32_t>>({{2}, {3}, {0}, {1}})),
      new PBR(new std::vector<std::vector<u_int32_t>>({{}, {2}, {1}, {0, 3}})),
      new PBR(new std::vector<std::vector<u_int32_t>>({{0, 3}, {2}, {1}, {}})),
      new PBR(new std::vector<std::vector<u_int32_t>>({{1, 2}, {3}, {0}, {1}})),
      new PBR(new std::vector<std::vector<u_int32_t>>({{2}, {3}, {0}, {1, 3}})),
      new PBR(new std::vector<std::vector<u_int32_t>>({{3}, {1}, {0}, {1}})),
      new PBR(new std::vector<std::vector<u_int32_t>>({{3}, {2}, {0}, {0, 1}})),
      new PBR(new std::vector<std::vector<u_int32_t>>({{3}, {2}, {0}, {1}})),
      new PBR(new std::vector<std::vector<u_int32_t>>({{3}, {2}, {0}, {3}})),
      new PBR(new std::vector<std::vector<u_int32_t>>({{3}, {2}, {1}, {0}})),
      new PBR(
          new std::vector<std::vector<u_int32_t>>({{3}, {2, 3}, {0}, {1}}))};
  REQUIRE(gens[0]->degree() == 2);

  Semigroup<> S = Semigroup<>(gens);
  S.set_report(CONG_REPORT);
  really_delete_cont(gens);

  // REQUIRE(S.size() == 65536);
  // REQUIRE(S.nrrules() == 45416);

  std::vector<relation_t> extra(
      {relation_t({7, 10, 9, 3, 6, 9, 4, 7, 9, 10}, {9, 3, 6, 6, 10, 9, 4, 7}),
       relation_t({8, 7, 5, 8, 9, 8}, {6, 3, 8, 6, 1, 2, 4})});
  Congruence cong("twosided", &S, extra);
  cong.set_report(CONG_REPORT);

  REQUIRE(cong.nr_classes() == 19009);

  Partition<word_t>* ntc = cong.nontrivial_classes();
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
  S.set_report(CONG_REPORT);
  really_delete_cont(gens);

  // REQUIRE(S.size() == 712);
  // REQUIRE(S.nrrules() == 1121);

  std::vector<relation_t> extra
      = {relation_t({5, 3, 1}, {3, 3}), relation_t({2, 7}, {1, 6, 6, 1})};
  Congruence cong("twosided", &S, extra);
  cong.set_report(CONG_REPORT);

  REQUIRE(cong.nr_classes() == 32);
}

TEST_CASE("Congruence 14: Bicyclic monoid",
          "[quick][congruence][fpsemigroup][multithread][14]") {
  std::vector<relation_t> rels = {relation_t({0, 1}, {1}),
                                  relation_t({1, 0}, {1}),
                                  relation_t({0, 0}, {0}),
                                  relation_t({0, 2}, {2}),
                                  relation_t({2, 0}, {2}),
                                  relation_t({1, 2}, {0})};
  std::vector<relation_t> extra;
  Congruence              cong("twosided", 3, rels, extra);
  cong.set_report(CONG_REPORT);
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
  std::vector<relation_t> rels = {relation_t({0, 1}, {1}),
                                  relation_t({1, 0}, {1}),
                                  relation_t({0, 0}, {0}),
                                  relation_t({0, 2}, {2}),
                                  relation_t({2, 0}, {2}),
                                  relation_t({1, 2}, {0})};
  std::vector<relation_t> extra({relation_t({1, 1, 1}, {0})});
  Congruence              cong("twosided", 3, rels, extra);

  cong.set_report(CONG_REPORT);

  REQUIRE(cong.nr_classes() == 3);
}

TEST_CASE("Congruence 16: Congruence on free abelian monoid with 15 classes",
          "[quick][congruence][fpsemigroup][multithread][16]") {
  std::vector<relation_t> rels = {relation_t({0, 1}, {1}),
                                  relation_t({1, 0}, {1}),
                                  relation_t({0, 0}, {0}),
                                  relation_t({0, 2}, {2}),
                                  relation_t({2, 0}, {2}),
                                  relation_t({1, 2}, {2, 1})};
  std::vector<relation_t> extra(
      {relation_t({1, 1, 1, 1, 1}, {1}), relation_t({2, 2, 2}, {2})});
  Congruence cong("twosided", 3, rels, extra);

  cong.set_report(CONG_REPORT);

  REQUIRE(cong.nr_classes() == 15);
}

TEST_CASE("Congruence 17: Congruence on full PBR monoid on 2 points (max 2)",
          "[extreme][congruence][multithread][finite][pbr][17]") {
  std::vector<Element*> gens = {
      new PBR(new std::vector<std::vector<u_int32_t>>({{2}, {3}, {0}, {1}})),
      new PBR(new std::vector<std::vector<u_int32_t>>({{}, {2}, {1}, {0, 3}})),
      new PBR(new std::vector<std::vector<u_int32_t>>({{0, 3}, {2}, {1}, {}})),
      new PBR(new std::vector<std::vector<u_int32_t>>({{1, 2}, {3}, {0}, {1}})),
      new PBR(new std::vector<std::vector<u_int32_t>>({{2}, {3}, {0}, {1, 3}})),
      new PBR(new std::vector<std::vector<u_int32_t>>({{3}, {1}, {0}, {1}})),
      new PBR(new std::vector<std::vector<u_int32_t>>({{3}, {2}, {0}, {0, 1}})),
      new PBR(new std::vector<std::vector<u_int32_t>>({{3}, {2}, {0}, {1}})),
      new PBR(new std::vector<std::vector<u_int32_t>>({{3}, {2}, {0}, {3}})),
      new PBR(new std::vector<std::vector<u_int32_t>>({{3}, {2}, {1}, {0}})),
      new PBR(
          new std::vector<std::vector<u_int32_t>>({{3}, {2, 3}, {0}, {1}}))};
  REQUIRE(gens[0]->degree() == 2);

  Semigroup<> S = Semigroup<>(gens);
  S.set_report(CONG_REPORT);
  really_delete_cont(gens);

  // REQUIRE(S.size() == 65536);
  // REQUIRE(S.nrrules() == 45416);

  std::vector<relation_t> extra(
      {relation_t({7, 10, 9, 3, 6, 9, 4, 7, 9, 10}, {9, 3, 6, 6, 10, 9, 4, 7}),
       relation_t({8, 7, 5, 8, 9, 8}, {6, 3, 8, 6, 1, 2, 4})});
  Congruence cong("twosided", &S, extra);
  cong.set_report(CONG_REPORT);
  cong.set_max_threads(2);

  REQUIRE(cong.nr_classes() == 19009);

  Partition<word_t>* ntc = cong.nontrivial_classes();
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
  std::vector<Element*> gens = {
      new PBR(new std::vector<std::vector<u_int32_t>>({{2}, {3}, {0}, {1}})),
      new PBR(new std::vector<std::vector<u_int32_t>>({{}, {2}, {1}, {0, 3}})),
      new PBR(new std::vector<std::vector<u_int32_t>>({{0, 3}, {2}, {1}, {}})),
      new PBR(new std::vector<std::vector<u_int32_t>>({{1, 2}, {3}, {0}, {1}})),
      new PBR(new std::vector<std::vector<u_int32_t>>({{2}, {3}, {0}, {1, 3}})),
      new PBR(new std::vector<std::vector<u_int32_t>>({{3}, {1}, {0}, {1}})),
      new PBR(new std::vector<std::vector<u_int32_t>>({{3}, {2}, {0}, {0, 1}})),
      new PBR(new std::vector<std::vector<u_int32_t>>({{3}, {2}, {0}, {1}})),
      new PBR(new std::vector<std::vector<u_int32_t>>({{3}, {2}, {0}, {3}})),
      new PBR(new std::vector<std::vector<u_int32_t>>({{3}, {2}, {1}, {0}})),
      new PBR(
          new std::vector<std::vector<u_int32_t>>({{3}, {2, 3}, {0}, {1}}))};
  REQUIRE(gens[0]->degree() == 2);

  Semigroup<> S = Semigroup<>(gens);
  S.set_report(CONG_REPORT);
  really_delete_cont(gens);

  // REQUIRE(S.size() == 65536);
  // REQUIRE(S.nrrules() == 45416);

  std::vector<relation_t> extra(
      {relation_t({7, 10, 9, 3, 6, 9, 4, 7, 9, 10}, {9, 3, 6, 6, 10, 9, 4, 7}),
       relation_t({8, 7, 5, 8, 9, 8}, {6, 3, 8, 6, 1, 2, 4})});
  Congruence cong("twosided", &S, extra);
  cong.set_report(CONG_REPORT);
  cong.set_max_threads(1);

  REQUIRE(cong.nr_classes() == 19009);

  Partition<word_t>* ntc = cong.nontrivial_classes();
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
  std::vector<relation_t> rels = {relation_t({0, 0}, {0, 0}),
                                  relation_t({0, 1}, {1, 0}),
                                  relation_t({0, 2}, {2, 0}),
                                  relation_t({0, 0}, {0}),
                                  relation_t({0, 2}, {0}),
                                  relation_t({2, 0}, {0}),
                                  relation_t({1, 0}, {0, 1}),
                                  relation_t({1, 1}, {1, 1}),
                                  relation_t({1, 2}, {2, 1}),
                                  relation_t({1, 1, 1}, {1}),
                                  relation_t({1, 2}, {1}),
                                  relation_t({2, 1}, {1})};
  std::vector<relation_t> extra = {relation_t({0}, {1})};

  Congruence cong("twosided", 3, rels, extra);
  cong.set_report(CONG_REPORT);

  REQUIRE(!cong.is_done());

  Partition<word_t>* ntc = cong.nontrivial_classes();
  REQUIRE(ntc->size() == 1);
  REQUIRE((*ntc)[0]->size() == 5);
  delete ntc;

  REQUIRE(cong.is_done());
}

TEST_CASE("Congruence 20: Infinite fp semigroup with infinite classes",
          "[quick][congruence][fpsemigroup][multithread][20]") {
  std::vector<relation_t> rels
      = {relation_t({0, 0, 0}, {0}), relation_t({0, 1}, {1, 0})};
  std::vector<relation_t> extra = {relation_t({0}, {0, 0})};
  Congruence              cong("twosided", 2, rels, extra);
  cong.set_report(CONG_REPORT);

  word_t x = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  word_t y = {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

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
  std::vector<relation_t> rels;
  rels.push_back(relation_t({0, 0, 0}, {0}));  // (a^3, a)
  rels.push_back(relation_t({0}, {1}));        // (a, b)
  std::vector<relation_t> extra;

  Congruence cong("left", 2, rels, extra);

  Partition<word_t>* ntc = cong.nontrivial_classes();
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
  S.set_report(CONG_REPORT);
  really_delete_cont(gens);
  Congruence cong("twosided", &S, std::vector<relation_t>());

  REQUIRE(cong.nr_classes() == S.size());
}

TEST_CASE("Congruence 23: test nontrivial_classes for a fp semigroup cong",
          "[quick][congruence][finite][fpsemigroup][23]") {
  std::vector<relation_t> rels
      = {relation_t({0, 0, 0}, {0}),
         relation_t({1, 0, 0}, {1, 0}),
         relation_t({1, 0, 1, 1, 1}, {1, 0}),
         relation_t({1, 1, 1, 1, 1}, {1, 1}),
         relation_t({1, 1, 0, 1, 1, 0}, {1, 0, 1, 0, 1, 1}),
         relation_t({0, 0, 1, 0, 1, 1, 0}, {0, 1, 0, 1, 1, 0}),
         relation_t({0, 0, 1, 1, 0, 1, 0}, {0, 1, 1, 0, 1, 0}),
         relation_t({0, 1, 0, 1, 0, 1, 0}, {1, 0, 1, 0, 1, 0}),
         relation_t({1, 0, 1, 0, 1, 0, 1}, {1, 0, 1, 0, 1, 0}),
         relation_t({1, 0, 1, 0, 1, 1, 0}, {1, 0, 1, 0, 1, 1}),
         relation_t({1, 0, 1, 1, 0, 1, 0}, {1, 0, 1, 1, 0, 1}),
         relation_t({1, 1, 0, 1, 0, 1, 0}, {1, 0, 1, 0, 1, 0}),
         relation_t({1, 1, 1, 1, 0, 1, 0}, {1, 0, 1, 0}),
         relation_t({0, 0, 1, 1, 1, 0, 1, 0}, {1, 1, 1, 0, 1, 0})};

  Congruence cong(
      "twosided", 2, rels, std::vector<relation_t>({relation_t({0}, {1})}));
  cong.set_report(CONG_REPORT);

  Partition<word_t>* ntc = cong.nontrivial_classes();
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
  S.set_report(CONG_REPORT);
  really_delete_cont(gens);

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

  std::vector<relation_t> extra;
  word_t                  w1, w2;
  for (size_t i = 0; i < elms.size(); i += 2) {
    S.factorisation(w1, S.position(elms[i]));
    S.factorisation(w2, S.position(elms[i + 1]));
    extra.push_back(std::make_pair(w1, w2));
    elms[i]->really_delete();
    elms[i + 1]->really_delete();
    delete elms[i];
    delete elms[i + 1];
  }

  Congruence cong("right", &S, extra);
  cong.set_report(CONG_REPORT);

  REQUIRE(cong.nr_classes() == 1);
}

TEST_CASE("Congruence 25: free semigroup with redundant relations",
          "[quick][congruence][multithread][fpsemigroup][25]") {
  std::vector<relation_t> extra = {relation_t({0, 0}, {0, 0})};
  Congruence              cong("twosided", 1, {}, extra);
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
  Congruence cong1(
      "twosided", 3, {relation_t({0, 1}, {0})}, {relation_t({2, 2}, {2})});
  REQUIRE(cong1.is_obviously_infinite());
  Congruence cong2("twosided",
                   3,
                   {relation_t({0, 1}, {0}), relation_t({0, 0}, {0})},
                   {relation_t({1, 1}, {1})});
  REQUIRE(cong2.is_obviously_infinite());
  Congruence cong3("twosided",
                   3,
                   {relation_t({0, 1}, {0}), relation_t({0, 0}, {0})},
                   {relation_t({1, 2}, {1})});
  REQUIRE(!cong3.is_obviously_infinite());

  std::vector<Element*> gens = {new Transformation<u_int16_t>({0, 1, 0}),
                                new Transformation<u_int16_t>({0, 1, 2})};

  Semigroup<> S = Semigroup<>(gens);
  S.set_report(CONG_REPORT);
  really_delete_cont(gens);

  Congruence cong4("twosided", &S, {relation_t({1}, {0})});
  REQUIRE(!cong4.is_obviously_infinite());
}

TEST_CASE("Congruence 28: test_less_than",
          "[quick][congruence][fpsemigroup][28]") {
  Congruence cong("twosided", 2, {relation_t({0, 0}, {0})}, {});
  REQUIRE(!cong.test_less_than({0, 0}, {0}));
}
