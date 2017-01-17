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

#include "../semigroups/cong.h"

#include <utility>

#include "catch.hpp"

using namespace libsemigroups;

template <typename T> static inline void really_delete_cont(T cont) {
  for (Element* x : cont) {
    x->really_delete();
    delete x;
  }
}

TEST_CASE("Congruence: 5-parameter constructor", "[constructor]") {
  std::vector<relation_t> rels;
  rels.push_back(std::pair<word_t, word_t>({0, 0, 0}, {0}));  // (a^3, a)
  rels.push_back(std::pair<word_t, word_t>({0}, {1, 1}));     // (a, b^2)
  std::vector<relation_t> extra;

  Congruence* leftcong = new Congruence("left", 2, rels, extra);
  delete leftcong;
}

TEST_CASE("Congruence: Small FP semigroup", "[small]") {
  std::vector<relation_t> rels;
  rels.push_back(std::pair<word_t, word_t>({0, 0, 0}, {0}));  // (a^3, a)
  rels.push_back(std::pair<word_t, word_t>({0}, {1, 1}));     // (a, b^2)
  std::vector<relation_t> extra;

  Congruence* cong = new Congruence("twosided", 2, rels, extra);
  REQUIRE(!cong->is_tc_done());
  cong->todd_coxeter(false);
  REQUIRE(cong->is_tc_done());
  REQUIRE(cong->nr_classes(false) == 5);

  REQUIRE(cong->word_to_coset({0, 0, 1}, false) == 5);
  REQUIRE(cong->word_to_coset({0, 0, 0, 0, 1}, false) == 5);
  REQUIRE(cong->word_to_coset({0, 1, 1, 0, 0, 1}, false) == 5);
  REQUIRE(cong->word_to_coset({0, 0, 0}, false) == 1);
  REQUIRE(cong->word_to_coset({1}, false) == 3);

  delete cong;
}

TEST_CASE("Congruence: Small left congruence on free semigroup", "[small]") {
  std::vector<relation_t> rels;
  rels.push_back(std::pair<word_t, word_t>({0, 0, 0}, {0}));  // (a^3, a)
  rels.push_back(std::pair<word_t, word_t>({0}, {1, 1}));     // (a, b^2)
  std::vector<relation_t> extra;

  Congruence* leftcong = new Congruence("left", 2, rels, extra);
  leftcong->todd_coxeter(false);
  REQUIRE(leftcong->nr_classes(false) == 5);
  delete leftcong;
}

TEST_CASE("Congruence: Small right congruence on free semigroup", "[small]") {
  std::vector<relation_t> rels;
  rels.push_back(std::pair<word_t, word_t>({0, 0, 0}, {0}));  // (a^3, a)
  rels.push_back(std::pair<word_t, word_t>({0}, {1, 1}));     // (a, b^2)
  std::vector<relation_t> extra;

  Congruence* rightcong = new Congruence("right", 2, rels, extra);
  // don't do todd_coxeter explicitly
  REQUIRE(rightcong->nr_classes(false) == 5);
  REQUIRE(rightcong->is_tc_done());
  delete rightcong;
}

TEST_CASE("Congruence: word_to_coset for left congruence on free semigroup",
          "[method]") {
  std::vector<relation_t> rels;
  rels.push_back(std::pair<word_t, word_t>({0, 0, 0}, {0}));  // (a^3, a)
  rels.push_back(std::pair<word_t, word_t>({0}, {1, 1}));     // (a, b^2)
  std::vector<relation_t> extra;

  Congruence* leftcong = new Congruence("left", 2, rels, extra);
  REQUIRE(leftcong->word_to_coset({0, 0, 1}, false) == 5);
  REQUIRE(leftcong->word_to_coset({0, 0, 0, 0, 1}, false) == 5);
  REQUIRE(leftcong->word_to_coset({0, 1, 1, 0, 0, 1}, false) == 5);
  REQUIRE(leftcong->word_to_coset({0, 0, 0}, false) == 1);
  REQUIRE(leftcong->word_to_coset({1}, false) == 3);
  REQUIRE(leftcong->word_to_coset({0, 0, 0, 0}, false) == 2);
  delete leftcong;
}

TEST_CASE("Congruence: word_to_coset for small FP semigroup", "[method]") {
  std::vector<relation_t> rels;
  rels.push_back(std::pair<word_t, word_t>({0, 0, 0}, {0}));  // (a^3, a)
  rels.push_back(std::pair<word_t, word_t>({0}, {1, 1}));     // (a, b^2)
  std::vector<relation_t> extra;
  Congruence*             cong;

  cong = new Congruence("twosided", 2, rels, extra);
  REQUIRE(cong->word_to_coset({0, 0, 1}, false) == 5);
  REQUIRE(cong->word_to_coset({0, 0, 0, 0, 1}, false) == 5);
  REQUIRE(cong->word_to_coset({0, 1, 1, 0, 0, 1}, false) == 5);
  REQUIRE(cong->word_to_coset({0, 0, 0}, false) == 1);
  REQUIRE(cong->word_to_coset({1}, false) == 3);
  delete cong;

  cong = new Congruence("twosided", 2, rels, extra);
  cong->todd_coxeter(false);
  REQUIRE(cong->word_to_coset({0, 0, 0, 0}, false) == 2);
  delete cong;
}

TEST_CASE("Congruence: 6-argument constructor (trivial cong)",
          "[constructor]") {
  std::vector<Element*> gens = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
                                new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
  Semigroup S = Semigroup(gens);
  really_delete_cont(gens);
  REQUIRE(S.size(false) == 88);
  REQUIRE(S.degree() == 5);

  std::vector<relation_t> extra;
  Congruence* cong = new Congruence("twosided", &S, extra, false, false);
  //                                                     prefill, report
  delete cong;
}

TEST_CASE("Congruence: 6-argument constructor (nontrivial cong)",
          "[constructor]") {
  std::vector<Element*> gens = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
                                new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
  Semigroup S = Semigroup(gens);
  really_delete_cont(gens);
  REQUIRE(S.size(false) == 88);
  REQUIRE(S.degree() == 5);

  Element* t1 = new Transformation<u_int16_t>({3, 4, 4, 4, 4});
  Element* t2 = new Transformation<u_int16_t>({3, 1, 3, 3, 3});
  word_t   w1, w2;
  S.factorisation(w1, S.position(t1), false);
  S.factorisation(w2, S.position(t2), false);
  std::vector<relation_t> extra({std::make_pair(w1, w2)});
  Congruence* cong = new Congruence("twosided", &S, extra, false, false);
  //                                                     prefill, report

  t1->really_delete();
  t2->really_delete();
  delete t1;
  delete t2;
  delete cong;
}

TEST_CASE("Congruence: transformation semigroup size 88", "[small]") {
  std::vector<Element*> gens = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
                                new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
  Semigroup S = Semigroup(gens);
  really_delete_cont(gens);
  REQUIRE(S.size(false) == 88);
  REQUIRE(S.degree() == 5);

  Element* t1 = new Transformation<u_int16_t>({3, 4, 4, 4, 4});
  Element* t2 = new Transformation<u_int16_t>({3, 1, 3, 3, 3});
  word_t   w1, w2;
  S.factorisation(w1, S.position(t1), false);
  S.factorisation(w2, S.position(t2), false);
  std::vector<relation_t> extra;
  extra.push_back(std::make_pair(w1, w2));
  extra.push_back(std::make_pair(w1, w2));
  Congruence* cong = new Congruence("twosided", &S, extra, false, false);
  //                                                     prefill, report
  REQUIRE(cong->nr_classes(false) == 21);
  REQUIRE(cong->nr_classes(false) == 21);

  Element* t3 = new Transformation<u_int16_t>({1, 3, 1, 3, 3});
  Element* t4 = new Transformation<u_int16_t>({4, 2, 4, 4, 2});
  word_t   w3, w4;
  S.factorisation(w3, S.position(t3), false);
  S.factorisation(w4, S.position(t4), false);
  REQUIRE(cong->word_to_coset(w3, false) == cong->word_to_coset(w4, false));

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

TEST_CASE("Congruence: transformation semigroup size 88 (prefill)",
          "[small]") {
  std::vector<Element*> gens = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
                                new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
  Semigroup S = Semigroup(gens);
  really_delete_cont(gens);
  REQUIRE(S.size(false) == 88);
  REQUIRE(S.degree() == 5);

  Element* t1 = new Transformation<u_int16_t>({3, 4, 4, 4, 4});
  Element* t2 = new Transformation<u_int16_t>({3, 1, 3, 3, 3});
  word_t   w1, w2;
  S.factorisation(w1, S.position(t1), false);
  S.factorisation(w2, S.position(t2), false);
  std::vector<relation_t> extra({std::make_pair(w1, w2)});
  Congruence* cong = new Congruence("twosided", &S, extra, true, false);
  //                                                     prefill, report
  REQUIRE(cong->nr_classes(false) == 21);
  REQUIRE(cong->nr_classes(false) == 21);

  Element* t3 = new Transformation<u_int16_t>({1, 3, 1, 3, 3});
  Element* t4 = new Transformation<u_int16_t>({4, 2, 4, 4, 2});
  word_t   w3, w4;
  S.factorisation(w3, S.position(t3), false);
  S.factorisation(w4, S.position(t4), false);
  REQUIRE(cong->word_to_coset(w3, false) == cong->word_to_coset(w4, false));

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

TEST_CASE("Congruence: left congruence on transformation semigroup size 88",
          "[small]") {
  std::vector<Element*> gens = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
                                new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
  Semigroup S = Semigroup(gens);
  really_delete_cont(gens);
  REQUIRE(S.size(false) == 88);
  REQUIRE(S.degree() == 5);

  Element* t1 = new Transformation<u_int16_t>({3, 4, 4, 4, 4});
  Element* t2 = new Transformation<u_int16_t>({3, 1, 3, 3, 3});
  word_t   w1, w2;
  S.factorisation(w1, S.position(t1), false);
  S.factorisation(w2, S.position(t2), false);
  std::vector<relation_t> extra({std::make_pair(w1, w2)});
  Congruence* cong = new Congruence("left", &S, extra, false, false);
  //                                                   prefill, report
  REQUIRE(cong->nr_classes(false) == 69);
  REQUIRE(cong->nr_classes(false) == 69);

  Element* t3 = new Transformation<u_int16_t>({1, 3, 1, 3, 3});
  Element* t4 = new Transformation<u_int16_t>({1, 1, 1, 1, 1});
  Element* t5 = new Transformation<u_int16_t>({1, 3, 4, 2, 3});
  word_t   w3, w4, w5;
  S.factorisation(w3, S.position(t3), false);
  S.factorisation(w4, S.position(t4), false);
  S.factorisation(w5, S.position(t5), false);
  REQUIRE(cong->word_to_coset(w3, false) == cong->word_to_coset(w4, false));
  REQUIRE(cong->word_to_coset(w3, false) == cong->word_to_coset(w2, false));
  REQUIRE(cong->word_to_coset(w5, false) != cong->word_to_coset(w4, false));

  t1->really_delete();
  t2->really_delete();
  t3->really_delete();
  t4->really_delete();
  t5->really_delete();
  delete t1;
  delete t2;
  delete t3;
  delete t4;
  delete t5;
  delete cong;
}

TEST_CASE("Congruence: left congruence on transformation semigroup size 88 "
          "(prefill)",
          "[small]") {
  std::vector<Element*> gens = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
                                new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
  Semigroup S = Semigroup(gens);
  really_delete_cont(gens);
  REQUIRE(S.size(false) == 88);
  REQUIRE(S.degree() == 5);

  Element* t1 = new Transformation<u_int16_t>({3, 4, 4, 4, 4});
  Element* t2 = new Transformation<u_int16_t>({3, 1, 3, 3, 3});
  word_t   w1, w2;
  S.factorisation(w1, S.position(t1), false);
  S.factorisation(w2, S.position(t2), false);
  std::vector<relation_t> extra({std::make_pair(w1, w2)});

  Congruence* cong = new Congruence("left", &S, extra, true, false);
  //                                                   prefill, report
  REQUIRE(cong->nr_classes(false) == 69);
  REQUIRE(cong->nr_classes(false) == 69);

  Element* t3 = new Transformation<u_int16_t>({1, 3, 1, 3, 3});
  Element* t4 = new Transformation<u_int16_t>({1, 1, 1, 1, 1});
  Element* t5 = new Transformation<u_int16_t>({1, 3, 4, 2, 3});
  word_t   w3, w4, w5;
  S.factorisation(w3, S.position(t3), false);
  S.factorisation(w4, S.position(t4), false);
  S.factorisation(w5, S.position(t5), false);
  REQUIRE(cong->word_to_coset(w3, false) == cong->word_to_coset(w4, false));
  REQUIRE(cong->word_to_coset(w3, false) == cong->word_to_coset(w2, false));
  REQUIRE(cong->word_to_coset(w5, false) != cong->word_to_coset(w4, false));

  t1->really_delete();
  t2->really_delete();
  t3->really_delete();
  t4->really_delete();
  t5->really_delete();
  delete t1;
  delete t2;
  delete t3;
  delete t4;
  delete t5;
  delete cong;
}

#ifndef SKIP_TEST

TEST_CASE("Congruence: transformation semigroup, size 91738, with "
          "parallel_todd_coxeter",
          "[big]") {
  std::vector<Element*> gens;
  gens = {new Transformation<u_int16_t>({1, 4, 3, 0, 7, 2, 0, 1}),
          new Transformation<u_int16_t>({1, 4, 7, 7, 5, 7, 2, 3}),
          new Transformation<u_int16_t>({6, 0, 3, 1, 5, 4, 6, 0})};
  Semigroup S = Semigroup(gens);
  really_delete_cont(gens);
  REQUIRE(S.size(false) == 91738);
  REQUIRE(S.degree() == 8);

  Element* t1 = new Transformation<u_int16_t>({0, 7, 0, 0, 2, 0, 4, 1});
  Element* t2 = new Transformation<u_int16_t>({6, 6, 1, 0, 4, 5, 6, 6});
  word_t   w1, w2;
  S.factorisation(w1, S.position(t1), false);
  S.factorisation(w2, S.position(t2), false);
  std::vector<relation_t> extra({std::make_pair(w1, w2)});
  Congruence* cong = new Congruence("twosided", &S, extra, true, false);
  //                                                     prefill,report
  REQUIRE(cong->nr_classes(false) == 83);
  REQUIRE(cong->nr_classes(false) == 83);

  Element* t3 = new Transformation<u_int16_t>({3, 5, 0, 0, 6, 0, 6, 3});
  Element* t4 = new Transformation<u_int16_t>({5, 4, 5, 5, 7, 5, 4, 3});
  word_t   w3, w4;
  S.factorisation(w3, S.position(t3), false);
  S.factorisation(w4, S.position(t4), false);
  REQUIRE(cong->word_to_coset(w3, false) == cong->word_to_coset(w4, false));

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

#endif
