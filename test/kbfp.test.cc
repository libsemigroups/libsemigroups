//
// Semigroups++ - C/C++ library for computing with semigroups and monoids
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

// The purpose of this file is to test the Congruence::KBFP class, this is
// achieved by calling cong.force_kbfp() before calculating anything about the
// congruence.

#include <utility>

#include "../cong.h"
#include "catch.hpp"

#define KBFP_REPORT false

using namespace libsemigroups;

template <typename T> static inline void really_delete_cont(T cont) {
  for (Element* x : cont) {
    x->really_delete();
    delete x;
  }
}

static inline void really_delete_partition(Congruence::partition_t part) {
  for (auto& cont : part) {
    for (Element const* x : cont) {
      const_cast<Element*>(x)->really_delete();
      delete x;
    }
  }
}

TEST_CASE("KBFP 01: Small fp semigroup",
          "[quick][congruence][kbfp][fpsemigroup]") {
  std::vector<relation_t> rels;
  rels.push_back(relation_t({0, 0, 0}, {0}));  // (a^3, a)
  rels.push_back(relation_t({0}, {1, 1}));     // (a, b^2)
  std::vector<relation_t> extra;

  Congruence cong("twosided", 2, rels, extra);
  cong.force_kbfp();
  cong.set_report(KBFP_REPORT);

  REQUIRE(!cong.is_done());
  REQUIRE(cong.nr_classes() == 5);
  REQUIRE(cong.is_done());

  REQUIRE(cong.word_to_class_index({0, 0, 1}) == 4);
  REQUIRE(cong.word_to_class_index({0, 0, 0, 0, 1}) == 4);
  REQUIRE(cong.word_to_class_index({0, 1, 1, 0, 0, 1}) == 4);
  REQUIRE(cong.word_to_class_index({0, 0, 0}) == 0);
  REQUIRE(cong.word_to_class_index({1}) == 1);
}

TEST_CASE("KBFP 02: word_to_class_index for fp semigroup",
          "[quick][congruence][kbfp][fpsemigroup]") {
  std::vector<relation_t> rels;
  rels.push_back(relation_t({0, 0, 0}, {0}));  // (a^3, a)
  rels.push_back(relation_t({0}, {1, 1}));     // (a, b^2)
  std::vector<relation_t> extra;

  Congruence cong1("twosided", 2, rels, extra);
  cong1.force_kbfp();
  cong1.set_report(KBFP_REPORT);

  REQUIRE(cong1.word_to_class_index({0, 0, 1}) == 4);
  REQUIRE(cong1.word_to_class_index({0, 0, 0, 0, 1}) == 4);
  REQUIRE(cong1.word_to_class_index({0, 1, 1, 0, 0, 1}) == 4);
  REQUIRE(cong1.word_to_class_index({0, 0, 0}) == 0);
  REQUIRE(cong1.word_to_class_index({1}) == 1);

  Congruence cong2("twosided", 2, rels, extra);
  cong2.force_kbfp();
  cong2.set_report(KBFP_REPORT);

  REQUIRE(cong2.word_to_class_index({0, 0, 0, 0}) == 2);
}

TEST_CASE("KBFP 03: for a finite semigroup",
          "[quick][congruence][kbfp][finite]") {
  std::vector<Element*> gens = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
                                new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
  Semigroup S = Semigroup(gens);
  S.set_report(KBFP_REPORT);
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

  Congruence cong("twosided", &S, extra);
  cong.force_kbfp();
  cong.set_report(KBFP_REPORT);

  REQUIRE(cong.nr_classes() == 21);
  REQUIRE(cong.nr_classes() == 21);

  Element* t3 = new Transformation<u_int16_t>({1, 3, 1, 3, 3});
  Element* t4 = new Transformation<u_int16_t>({4, 2, 4, 4, 2});
  word_t   w3, w4;
  S.factorisation(w3, S.position(t3));
  S.factorisation(w4, S.position(t4));
  REQUIRE(cong.word_to_class_index(w3) == cong.word_to_class_index(w4));

  Congruence::partition_t nontrivial_classes = cong.nontrivial_classes();
  REQUIRE(nontrivial_classes.size() == 1);
  REQUIRE(nontrivial_classes[0].size() == 68);
  really_delete_partition(nontrivial_classes);

  t1->really_delete();
  t2->really_delete();
  t3->really_delete();
  t4->really_delete();
  delete t1;
  delete t2;
  delete t3;
  delete t4;
}
