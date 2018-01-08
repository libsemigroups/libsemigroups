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

// The purpose of this file is to test the Congruence::TC class, this is
// achieved by calling cong->tc() before calculating anything about the
// congruence.

#include <utility>

#include "../src/cong.h"
#include "catch.hpp"

#define TC_REPORT false

using namespace libsemigroups;

TEST_CASE("TC 01: Small fp semigroup",
          "[quick][congruence][tc][fpsemigroup][01]") {
  std::vector<relation_t> rels;
  rels.push_back(relation_t({0, 0, 0}, {0}));  // (a^3, a)
  rels.push_back(relation_t({0}, {1, 1}));     // (a, b^2)
  std::vector<relation_t> extra;

  Congruence* cong = new Congruence("twosided", 2, rels, extra);
  cong->force_tc();
  cong->set_report(TC_REPORT);

  REQUIRE(!cong->is_done());

  REQUIRE(cong->nr_classes() == 5);
  REQUIRE(cong->is_done());

  REQUIRE(cong->word_to_class_index({0, 0, 1})
          == cong->word_to_class_index({0, 0, 0, 0, 1}));
  REQUIRE(cong->word_to_class_index({0, 1, 1, 0, 0, 1})
          == cong->word_to_class_index({0, 0, 0, 0, 1}));
  REQUIRE(cong->word_to_class_index({0, 0, 0})
          != cong->word_to_class_index({1}));

  delete cong;
}

TEST_CASE("TC 02: Small left congruence on free semigroup",
          "[quick][congruence][tc][fpsemigroup][02]") {
  std::vector<relation_t> rels;
  rels.push_back(relation_t({0, 0, 0}, {0}));  // (a^3, a)
  rels.push_back(relation_t({0}, {1, 1}));     // (a, b^2)
  std::vector<relation_t> extra;

  Congruence* cong = new Congruence("left", 2, rels, extra);
  cong->force_tc();
  cong->set_report(TC_REPORT);
  REQUIRE(cong->nr_classes() == 5);
  delete cong;
}

TEST_CASE("TC 03: Small right congruence on free semigroup",
          "[quick][congruence][tc][fpsemigroup][03]") {
  std::vector<relation_t> rels;
  rels.push_back(relation_t({0, 0, 0}, {0}));  // (a^3, a)
  rels.push_back(relation_t({0}, {1, 1}));     // (a, b^2)
  std::vector<relation_t> extra;

  Congruence* cong = new Congruence("right", 2, rels, extra);
  cong->force_tc();
  cong->set_report(TC_REPORT);
  REQUIRE(cong->nr_classes() == 5);
  REQUIRE(cong->is_done());
  delete cong;
}

TEST_CASE("TC 04: word_to_class_index for left congruence on free "
          "semigroup",
          "[quick][congruence][tc][fpsemigroup][04]") {
  std::vector<relation_t> rels;
  rels.push_back(relation_t({0, 0, 0}, {0}));  // (a^3, a)
  rels.push_back(relation_t({0}, {1, 1}));     // (a, b^2)
  std::vector<relation_t> extra;

  Congruence* cong = new Congruence("left", 2, rels, extra);
  cong->force_tc();
  cong->set_report(TC_REPORT);
  REQUIRE(cong->word_to_class_index({0, 0, 1})
          == cong->word_to_class_index({0, 0, 0, 0, 1}));
  REQUIRE(cong->word_to_class_index({0, 1, 1, 0, 0, 1})
          == cong->word_to_class_index({0, 0, 0, 0, 1}));
  REQUIRE(cong->word_to_class_index({1})
          != cong->word_to_class_index({0, 0, 0, 0}));
  REQUIRE(cong->word_to_class_index({0, 0, 0})
          != cong->word_to_class_index({0, 0, 0, 0}));
  delete cong;
}

TEST_CASE("TC 05: word_to_class_index for small fp semigroup",
          "[quick][congruence][tc][fpsemigroup][05]") {
  std::vector<relation_t> rels;
  rels.push_back(relation_t({0, 0, 0}, {0}));  // (a^3, a)
  rels.push_back(relation_t({0}, {1, 1}));     // (a, b^2)
  std::vector<relation_t> extra;

  Congruence* cong = new Congruence("twosided", 2, rels, extra);
  cong->force_tc();
  cong->set_report(TC_REPORT);
  REQUIRE(cong->word_to_class_index({0, 0, 1})
          == cong->word_to_class_index({0, 0, 0, 0, 1}));
  REQUIRE(cong->word_to_class_index({0, 1, 1, 0, 0, 1})
          == cong->word_to_class_index({0, 0, 0, 0, 1}));
  REQUIRE(cong->word_to_class_index({0, 0, 0})
          != cong->word_to_class_index({1}));
  delete cong;

  cong = new Congruence("twosided", 2, rels, extra);
  cong->force_tc();
  cong->set_report(TC_REPORT);
  REQUIRE(cong->word_to_class_index({0, 0, 0, 0}) < cong->nr_classes());
  delete cong;
}

TEST_CASE("TC 06: transformation semigroup size 88",
          "[quick][congruence][tc][finite][06]") {
  std::vector<Element*> vec = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
                               new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
  Semigroup<> S = Semigroup<>(vec);
  S.set_report(TC_REPORT);

  REQUIRE(S.size() == 88);
  REQUIRE(S.nrrules() == 18);
  REQUIRE(S.degree() == 5);

  vec.push_back(new Transformation<u_int16_t>({3, 4, 4, 4, 4}));
  word_t w1;
  S.factorisation(w1, S.position(vec.back()));

  vec.push_back(new Transformation<u_int16_t>({3, 1, 3, 3, 3}));
  word_t w2;
  S.factorisation(w2, S.position(vec.back()));

  std::vector<relation_t> extra({relation_t(w1, w2)});
  Congruence              cong("twosided", &S, extra);
  cong.force_tc();
  cong.set_report(TC_REPORT);

  REQUIRE(cong.nr_classes() == 21);
  REQUIRE(cong.nr_classes() == 21);

  vec.push_back(new Transformation<u_int16_t>({1, 3, 1, 3, 3}));
  S.factorisation(w1, S.position(vec.back()));

  vec.push_back(new Transformation<u_int16_t>({4, 2, 4, 4, 2}));
  S.factorisation(w2, S.position(vec.back()));

  REQUIRE(cong.word_to_class_index(w1) == cong.word_to_class_index(w2));

  Partition<word_t>* ntc = cong.nontrivial_classes();
  REQUIRE(ntc->size() == 1);
  REQUIRE(ntc->at(0)->size() == 68);
  delete ntc;

  really_delete_cont(vec);
}

TEST_CASE("TC 07: left congruence on transformation semigroup size 88",
          "[quick][congruence][tc][finite][07]") {
  std::vector<Element*> vec = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
                               new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
  Semigroup<> S = Semigroup<>(vec);
  S.set_report(TC_REPORT);

  REQUIRE(S.size() == 88);
  REQUIRE(S.degree() == 5);

  vec.push_back(new Transformation<u_int16_t>({3, 4, 4, 4, 4}));
  word_t w1;
  S.factorisation(w1, S.position(vec.back()));
  vec.push_back(new Transformation<u_int16_t>({3, 1, 3, 3, 3}));
  word_t w2;
  S.factorisation(w2, S.position(vec.at(3)));
  std::vector<relation_t> extra({relation_t(w1, w2)});
  Congruence              cong("left", &S, extra);
  cong.force_tc();
  cong.set_report(TC_REPORT);

  REQUIRE(cong.nr_classes() == 69);
  REQUIRE(cong.nr_classes() == 69);

  Partition<word_t>* ntc = cong.nontrivial_classes();
  REQUIRE(ntc->size() == 1);
  REQUIRE(ntc->at(0)->size() == 20);
  delete ntc;

  really_delete_cont(vec);
}

TEST_CASE("TC 08: right congruence on transformation semigroup size 88",
          "[quick][congruence][tc][finite][08]") {
  std::vector<Element*> vec = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
                               new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
  Semigroup<> S = Semigroup<>(vec);
  S.set_report(TC_REPORT);

  REQUIRE(S.size() == 88);
  REQUIRE(S.nrrules() == 18);
  REQUIRE(S.degree() == 5);

  word_t w1, w2;
  vec.push_back(new Transformation<u_int16_t>({3, 4, 4, 4, 4}));
  S.factorisation(w1, S.position(vec.back()));
  vec.push_back(new Transformation<u_int16_t>({3, 1, 3, 3, 3}));
  S.factorisation(w2, S.position(vec.back()));

  Congruence cong("right", &S, {relation_t(w1, w2)});
  cong.force_tc();
  cong.set_report(TC_REPORT);

  REQUIRE(cong.nr_classes() == 72);
  REQUIRE(cong.nr_classes() == 72);

  word_t w3, w4, w5, w6;
  vec.push_back(new Transformation<u_int16_t>({1, 3, 3, 3, 3}));
  S.factorisation(w3, S.position(vec.back()));
  vec.push_back(new Transformation<u_int16_t>({4, 2, 4, 4, 2}));
  S.factorisation(w4, S.position(vec.back()));
  vec.push_back(new Transformation<u_int16_t>({2, 4, 2, 2, 2}));
  S.factorisation(w5, S.position(vec.back()));
  vec.push_back(new Transformation<u_int16_t>({2, 3, 3, 3, 3}));
  S.factorisation(w6, S.position(vec.back()));

  REQUIRE(cong.word_to_class_index(w3) != cong.word_to_class_index(w4));
  REQUIRE(cong.word_to_class_index(w5) == cong.word_to_class_index(w6));
  REQUIRE(cong.word_to_class_index(w3) != cong.word_to_class_index(w6));

  Partition<word_t>* ntc = cong.nontrivial_classes();
  REQUIRE(ntc->size() == 4);
  std::vector<size_t> sizes({0, 0, 0});
  for (size_t i = 0; i < ntc->size(); i++) {
    switch (ntc->at(i)->size()) {
      case 3:
        sizes[0]++;
        break;
      case 5:
        sizes[1]++;
        break;
      case 7:
        sizes[2]++;
        break;
      default:
        REQUIRE(false);
    }
  }
  REQUIRE(sizes == std::vector<size_t>({1, 2, 1}));
  delete ntc;
  really_delete_cont(vec);
}

// Testing prefilled TC

TEST_CASE("TC 09: transformation semigroup size 88",
          "[quick][congruence][tc][prefill][finite][09]") {
  std::vector<Element*> gens = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
                                new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(TC_REPORT);
  really_delete_cont(gens);

  REQUIRE(S.size() == 88);
  REQUIRE(S.nrrules() == 18);
  REQUIRE(S.degree() == 5);

  Element* t1 = new Transformation<u_int16_t>({3, 4, 4, 4, 4});
  Element* t2 = new Transformation<u_int16_t>({3, 1, 3, 3, 3});
  word_t   w1, w2;
  S.factorisation(w1, S.position(t1));
  S.factorisation(w2, S.position(t2));
  std::vector<relation_t> extra({relation_t(w1, w2)});
  Congruence*             cong = new Congruence("twosided", &S, extra);
  cong->force_tc_prefill();
  cong->set_report(TC_REPORT);

  REQUIRE(cong->nr_classes() == 21);
  REQUIRE(cong->nr_classes() == 21);
  Element* t3 = new Transformation<u_int16_t>({1, 3, 1, 3, 3});
  Element* t4 = new Transformation<u_int16_t>({4, 2, 4, 4, 2});
  word_t   w3, w4;
  S.factorisation(w3, S.position(t3));
  S.factorisation(w4, S.position(t4));
  REQUIRE(cong->word_to_class_index(w3) == cong->word_to_class_index(w4));

  t1->really_delete();
  t2->really_delete();
  t3->really_delete();
  t4->really_delete();
  delete t1;
  delete t2;
  delete t3;
  delete t4;
  delete cong;
}

TEST_CASE("TC 10: left congruence on transformation semigroup size 88",
          "[quick][congruence][tc][prefill][finite][10]") {
  std::vector<Element*> gens = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
                                new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(TC_REPORT);
  really_delete_cont(gens);

  REQUIRE(S.size() == 88);
  REQUIRE(S.degree() == 5);
  Element* t1 = new Transformation<u_int16_t>({3, 4, 4, 4, 4});
  Element* t2 = new Transformation<u_int16_t>({3, 1, 3, 3, 3});
  word_t   w1, w2;
  S.factorisation(w1, S.position(t1));
  S.factorisation(w2, S.position(t2));
  std::vector<relation_t> extra({relation_t(w1, w2)});
  Congruence*             cong = new Congruence("left", &S, extra);
  cong->force_tc_prefill();
  cong->set_report(TC_REPORT);

  REQUIRE(cong->nr_classes() == 69);
  REQUIRE(cong->nr_classes() == 69);
  Element* t3 = new Transformation<u_int16_t>({1, 3, 1, 3, 3});
  Element* t4 = new Transformation<u_int16_t>({4, 2, 4, 4, 2});
  word_t   w3, w4;
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
  delete cong;
}

TEST_CASE("TC 11: right congruence on transformation semigroup size 88",
          "[quick][congruence][tc][prefill][finite][11]") {
  std::vector<Element*> gens = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
                                new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(TC_REPORT);
  really_delete_cont(gens);

  REQUIRE(S.size() == 88);
  REQUIRE(S.nrrules() == 18);
  REQUIRE(S.degree() == 5);
  Element* t1 = new Transformation<u_int16_t>({3, 4, 4, 4, 4});
  Element* t2 = new Transformation<u_int16_t>({3, 1, 3, 3, 3});
  word_t   w1, w2;
  S.factorisation(w1, S.position(t1));
  S.factorisation(w2, S.position(t2));
  std::vector<relation_t> extra({relation_t(w1, w2)});
  Congruence*             cong = new Congruence("right", &S, extra);
  cong->force_tc_prefill();
  cong->set_report(TC_REPORT);

  REQUIRE(cong->nr_classes() == 72);
  REQUIRE(cong->nr_classes() == 72);
  Element* t3 = new Transformation<u_int16_t>({1, 3, 3, 3, 3});
  Element* t4 = new Transformation<u_int16_t>({4, 2, 4, 4, 2});
  Element* t5 = new Transformation<u_int16_t>({2, 4, 2, 2, 2});
  Element* t6 = new Transformation<u_int16_t>({2, 3, 3, 3, 3});
  word_t   w3, w4, w5, w6;
  S.factorisation(w3, S.position(t3));
  S.factorisation(w4, S.position(t4));
  S.factorisation(w5, S.position(t5));
  S.factorisation(w6, S.position(t6));
  REQUIRE(cong->word_to_class_index(w3) != cong->word_to_class_index(w4));
  //  REQUIRE(cong->word_to_class_index(w5) == cong->word_to_class_index(w6));
  REQUIRE(cong->word_to_class_index(w3) != cong->word_to_class_index(w6));

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
  delete cong;
}

TEST_CASE("TC 12: finite fp-semigroup, dihedral group of order 6",
          "[quick][fpsemigroup][tc][finite][12]") {
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

  Congruence cong("twosided", 5, rels, extra);
  cong.force_tc();
  cong.set_report(TC_REPORT);

  REQUIRE(cong.nr_classes() == 6);
  REQUIRE(cong.word_to_class_index({1}) == cong.word_to_class_index({2}));
}

TEST_CASE("TC 13: finite fp-semigroup, size 16",
          "[quick][fpsemigroup][tc][finite][13]") {
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

  Congruence cong("twosided", 4, rels, extra);
  cong.force_tc();
  cong.set_report(TC_REPORT);

  REQUIRE(cong.nr_classes() == 16);
  REQUIRE(cong.word_to_class_index({2}) == cong.word_to_class_index({3}));
}

TEST_CASE("TC 14: finite fp-semigroup, size 16",
          "[quick][fpsemigroup][tc][finite][14]") {
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

  Congruence cong("twosided", 11, rels, extra);
  cong.force_tc();
  cong.set_report(TC_REPORT);

  REQUIRE(cong.nr_classes() == 16);
  REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({5}));
  REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({10}));
  REQUIRE(cong.word_to_class_index({1}) == cong.word_to_class_index({2}));
  REQUIRE(cong.word_to_class_index({1}) == cong.word_to_class_index({7}));
  REQUIRE(cong.word_to_class_index({3}) == cong.word_to_class_index({4}));
  REQUIRE(cong.word_to_class_index({3}) == cong.word_to_class_index({6}));
  REQUIRE(cong.word_to_class_index({3}) == cong.word_to_class_index({8}));
  REQUIRE(cong.word_to_class_index({3}) == cong.word_to_class_index({9}));
}

TEST_CASE("TC 15: test prefilling of the table manually",
          "[quick][tc][finite][15]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({7, 3, 5, 3, 4, 2, 7, 7}),
         new Transformation<u_int16_t>({3, 6, 3, 4, 0, 6, 0, 7})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(TC_REPORT);
  really_delete_cont(gens);

  // Copy the right Cayley graph of S for prefilling
  Semigroup<>::cayley_graph_t const* right = S.right_cayley_graph_copy();

  // These are defining relations for S
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

  Congruence cong("twosided", 2, rels, std::vector<relation_t>());
  cong.set_report(TC_REPORT);
  cong.set_prefill(*right);
  delete right;
  REQUIRE(cong.nr_classes() == S.size() - 1);
  // This is not really a valid way of prefilling, since there is no "identity"
  // coset at the start of the table. This is why there is a - 1 in the REQUIRE
  // above. Anyway, this tests the relevant parts of the code.
}

TEST_CASE("TC 16: test packing phase", "[quick][tc][finite][16]") {
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

  Congruence cong1("twosided", 2, rels, std::vector<relation_t>());
  cong1.set_report(TC_REPORT);
  cong1.force_tc();
  cong1.set_pack(10);
  cong1.set_report_interval(10);
  REQUIRE(cong1.nr_classes() == 78);

  Congruence cong2("left", 2, rels, std::vector<relation_t>());
  cong2.set_report(TC_REPORT);
  cong2.force_tc();
  cong2.set_pack(10);
  cong2.set_report_interval(10);
  REQUIRE(cong2.nr_classes() == 78);
}

TEST_CASE("TC 17: Example 6.6 in Sims (see also RWS 13)",
          "[quick][tc][finite][17]") {
  std::vector<relation_t> rels
      = {relation_t({0, 0}, {0}),
         relation_t({1, 0}, {1}),
         relation_t({0, 1}, {1}),
         relation_t({2, 0}, {2}),
         relation_t({0, 2}, {2}),
         relation_t({3, 0}, {3}),
         relation_t({0, 3}, {3}),
         relation_t({1, 1}, {0}),
         relation_t({2, 3}, {0}),
         relation_t({2, 2, 2}, {0}),
         relation_t({1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2}, {0}),
         relation_t({1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3,
                     1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3},
                    {0})};
  Congruence cong("twosided", 4, rels, std::vector<relation_t>());
  cong.set_report(TC_REPORT);
  cong.force_tc();
  REQUIRE(cong.nr_classes() == 10752);  // Verified with GAP
}
