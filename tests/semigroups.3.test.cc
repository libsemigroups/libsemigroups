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

#include "../src/semigroups.h"
#include "catch.hpp"

#define SEMIGROUPS_REPORT false

using namespace libsemigroups;

static inline size_t evaluate_reduct(Semigroup<>& S, word_t const& word) {
  letter_t out = S.letter_to_pos(word[0]);
  for (auto it = word.cbegin() + 1; it < word.cend(); ++it) {
    out = S.right(out, *it);
  }
  return out;
}

static inline size_t evaluate_reduct(Semigroup<>* S, word_t const& word) {
  return evaluate_reduct(*S, word);
}

TEST_CASE("Semigroup 43: relations [copy_closure, duplicate gens]",
          "[quick][semigroup][finite][43]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);

  S.enumerate(Semigroup<>::LIMIT_MAX);
  REQUIRE(S.is_begun());
  REQUIRE(S.is_done());
  REQUIRE(S.nrgens() == 5);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.current_size() == 120);
  REQUIRE(S.size() == 120);
  REQUIRE(S.current_nrrules() == 33);
  REQUIRE(S.nrrules() == 33);
  REQUIRE(S.current_max_word_length() == 11);

  std::vector<Element*> coll
      = {new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup<>* T = S.copy_closure(&coll);
  T->set_report(SEMIGROUPS_REPORT);
  really_delete_cont(coll);

  REQUIRE(T->size() == 7776);
  REQUIRE(T->is_done());
  REQUIRE(T->nridempotents() == 537);
  delete T;
}

TEST_CASE("Semigroup 44: relations [copy_add_generators, duplicate gens]",
          "[quick][semigroup][finite][44]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);

  S.enumerate(Semigroup<>::LIMIT_MAX);
  REQUIRE(S.is_begun());
  REQUIRE(S.is_done());
  REQUIRE(S.nrgens() == 5);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.current_size() == 120);
  REQUIRE(S.size() == 120);
  REQUIRE(S.current_nrrules() == 33);
  REQUIRE(S.nrrules() == 33);
  REQUIRE(S.current_max_word_length() == 11);

  std::vector<Element*> coll
      = {new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup<>* T = S.copy_add_generators(&coll);
  T->set_report(SEMIGROUPS_REPORT);
  really_delete_cont(coll);

  REQUIRE(T->size() == 7776);
  REQUIRE(T->is_done());
  REQUIRE(T->nridempotents() == 537);
  delete T;
}

TEST_CASE("Semigroup 45: relations [from copy, not enumerated]",
          "[quick][semigroup][finite][45]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);

  Semigroup<> T = Semigroup<>(S);
  T.set_report(SEMIGROUPS_REPORT);
  REQUIRE(T.nrrules() == S.nrrules());

  std::vector<size_t> result;
  T.next_relation(result);
  size_t nr = 0;
  while (!result.empty()) {
    word_t lhs, rhs;
    T.factorisation(lhs, result[0]);
    lhs.push_back(result[1]);
    T.factorisation(rhs, result[2]);

    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    T.next_relation(result);
    nr++;
  }
  REQUIRE(T.nrrules() == nr);

  T.reset_next_relation();
  T.next_relation(result);
  nr = 0;

  while (!result.empty()) {
    word_t lhs, rhs;
    T.factorisation(lhs, result[0]);
    lhs.push_back(result[1]);
    T.factorisation(rhs, result[2]);

    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    T.next_relation(result);
    nr++;
  }
  REQUIRE(T.nrrules() == nr);
}

TEST_CASE("Semigroup 46: relations [from copy, partly enumerated]",
          "[quick][semigroup][finite][46]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);

  S.set_batch_size(1023);
  S.enumerate(1000);

  Semigroup<> T = Semigroup<>(S);
  T.set_report(SEMIGROUPS_REPORT);
  REQUIRE(T.nrrules() == S.nrrules());

  std::vector<size_t> result;
  T.next_relation(result);
  size_t nr = 0;
  while (!result.empty()) {
    word_t lhs, rhs;
    T.factorisation(lhs, result[0]);
    lhs.push_back(result[1]);
    T.factorisation(rhs, result[2]);

    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    T.next_relation(result);
    nr++;
  }
  REQUIRE(T.nrrules() == nr);

  T.reset_next_relation();
  T.next_relation(result);
  nr = 0;

  while (!result.empty()) {
    word_t lhs, rhs;
    T.factorisation(lhs, result[0]);
    lhs.push_back(result[1]);
    T.factorisation(rhs, result[2]);

    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    T.next_relation(result);
    nr++;
  }
  REQUIRE(T.nrrules() == nr);
}

TEST_CASE("Semigroup 47: relations [from copy, fully enumerated]",
          "[quick][semigroup][finite][47]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);

  S.enumerate(8000);

  Semigroup<> T = Semigroup<>(S);
  T.set_report(SEMIGROUPS_REPORT);
  REQUIRE(T.nrrules() == S.nrrules());

  std::vector<size_t> result;
  T.next_relation(result);
  size_t nr = 0;
  while (!result.empty()) {
    word_t lhs, rhs;
    T.factorisation(lhs, result[0]);
    lhs.push_back(result[1]);
    T.factorisation(rhs, result[2]);

    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    T.next_relation(result);
    nr++;
  }
  REQUIRE(T.nrrules() == nr);

  T.reset_next_relation();
  T.next_relation(result);
  nr = 0;

  while (!result.empty()) {
    word_t lhs, rhs;
    T.factorisation(lhs, result[0]);
    lhs.push_back(result[1]);
    T.factorisation(rhs, result[2]);

    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    T.next_relation(result);
    nr++;
  }
  REQUIRE(T.nrrules() == nr);
}

TEST_CASE("Semigroup 50: relations [from copy_closure, not enumerated]",
          "[quick][semigroup][finite][50]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);

  REQUIRE(!S.is_begun());
  REQUIRE(!S.is_done());

  std::vector<Element*> coll
      = {new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup<>* T = S.copy_closure(&coll);
  T->set_report(SEMIGROUPS_REPORT);
  REQUIRE(*coll[0] == *(T->gens(3)));
  REQUIRE(*coll[1] == *(T->gens(4)));
  really_delete_cont(coll);

  std::vector<size_t> result;
  T->next_relation(result);
  size_t nr = 0;
  while (!result.empty()) {
    word_t lhs, rhs;
    T->factorisation(lhs, result[0]);
    lhs.push_back(result[1]);
    T->factorisation(rhs, result[2]);

    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    T->next_relation(result);
    nr++;
  }
  REQUIRE(T->nrrules() == nr);
  REQUIRE(2459 == nr);

  T->reset_next_relation();
  T->next_relation(result);
  nr = 0;

  while (!result.empty()) {
    word_t lhs, rhs;
    T->factorisation(lhs, result[0]);
    lhs.push_back(result[1]);
    T->factorisation(rhs, result[2]);

    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    T->next_relation(result);
    nr++;
  }
  REQUIRE(T->nrrules() == nr);
  REQUIRE(2459 == nr);
  delete T;
}

TEST_CASE("Semigroup 51: relations [from copy_add_generators, not enumerated]",
          "[quick][semigroup][finite][51]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);

  REQUIRE(!S.is_begun());
  REQUIRE(!S.is_done());

  std::vector<Element*> coll
      = {new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup<>* T = S.copy_add_generators(&coll);
  T->set_report(SEMIGROUPS_REPORT);
  REQUIRE(*coll[0] == *(T->gens(3)));
  REQUIRE(*coll[1] == *(T->gens(4)));
  really_delete_cont(coll);

  std::vector<size_t> result;
  T->next_relation(result);
  size_t nr = 0;
  while (!result.empty()) {
    word_t lhs, rhs;
    T->factorisation(lhs, result[0]);
    lhs.push_back(result[1]);
    T->factorisation(rhs, result[2]);

    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    T->next_relation(result);
    nr++;
  }
  REQUIRE(T->nrrules() == nr);
  REQUIRE(2459 == nr);

  T->reset_next_relation();
  T->next_relation(result);
  nr = 0;

  while (!result.empty()) {
    word_t lhs, rhs;
    T->factorisation(lhs, result[0]);
    lhs.push_back(result[1]);
    T->factorisation(rhs, result[2]);

    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    T->next_relation(result);
    nr++;
  }
  REQUIRE(T->nrrules() == nr);
  REQUIRE(2459 == nr);
  delete T;
}

TEST_CASE("Semigroup 52: relations [from copy_closure, partly enumerated]",
          "[quick][semigroup][finite][52]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);
  S.set_batch_size(100);

  S.enumerate(10);

  REQUIRE(S.is_begun());
  REQUIRE(!S.is_done());

  std::vector<Element*> coll
      = {new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup<>* T = S.copy_closure(&coll);
  T->set_report(SEMIGROUPS_REPORT);
  really_delete_cont(coll);

  std::vector<size_t> result;
  T->next_relation(result);
  size_t nr = 0;
  while (!result.empty()) {
    word_t lhs, rhs;
    T->factorisation(lhs, result[0]);
    lhs.push_back(result[1]);
    T->factorisation(rhs, result[2]);

    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    T->next_relation(result);
    nr++;
  }
  REQUIRE(T->nrrules() == nr);
  REQUIRE(2459 == nr);

  T->reset_next_relation();
  T->next_relation(result);
  nr = 0;

  while (!result.empty()) {
    word_t lhs, rhs;
    T->factorisation(lhs, result[0]);
    lhs.push_back(result[1]);
    T->factorisation(rhs, result[2]);

    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    T->next_relation(result);
    nr++;
  }
  REQUIRE(T->nrrules() == nr);
  REQUIRE(2459 == nr);
  delete T;
}

TEST_CASE(
    "Semigroup 53: relations [from copy_add_generators, partly enumerated]",
    "[quick][semigroup][finite][53]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);
  S.set_batch_size(100);

  S.enumerate(10);

  REQUIRE(S.is_begun());
  REQUIRE(!S.is_done());

  std::vector<Element*> coll
      = {new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup<>* T = S.copy_add_generators(&coll);
  T->set_report(SEMIGROUPS_REPORT);
  really_delete_cont(coll);

  std::vector<size_t> result;
  T->next_relation(result);
  size_t nr = 0;
  while (!result.empty()) {
    word_t lhs, rhs;
    T->factorisation(lhs, result[0]);
    lhs.push_back(result[1]);
    T->factorisation(rhs, result[2]);

    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    T->next_relation(result);
    nr++;
  }
  REQUIRE(T->nrrules() == nr);
  REQUIRE(2459 == nr);

  T->reset_next_relation();
  T->next_relation(result);
  nr = 0;

  while (!result.empty()) {
    word_t lhs, rhs;
    T->factorisation(lhs, result[0]);
    lhs.push_back(result[1]);
    T->factorisation(rhs, result[2]);

    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    T->next_relation(result);
    nr++;
  }
  REQUIRE(T->nrrules() == nr);
  REQUIRE(2459 == nr);
  delete T;
}

TEST_CASE("Semigroup 54: relations [from copy_closure, fully enumerated]",
          "[quick][semigroup][finite][54]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);

  S.enumerate(8000);

  REQUIRE(S.is_begun());
  REQUIRE(S.is_done());

  std::vector<Element*> coll
      = {new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup<>* T = S.copy_closure(&coll);
  T->set_report(SEMIGROUPS_REPORT);
  really_delete_cont(coll);

  std::vector<size_t> result;
  T->next_relation(result);
  size_t nr = 0;
  while (!result.empty()) {
    REQUIRE(result.size() == 3);  // there are no duplicate gens
    word_t lhs, rhs;
    T->factorisation(lhs, result[0]);
    lhs.push_back(result[1]);
    T->factorisation(rhs, result[2]);

    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    T->next_relation(result);
    nr++;
  }
  REQUIRE(T->nrrules() == nr);
  REQUIRE(2459 == nr);

  T->reset_next_relation();
  T->next_relation(result);
  nr = 0;

  while (!result.empty()) {
    REQUIRE(result.size() == 3);
    word_t lhs, rhs;
    T->factorisation(lhs, result[0]);
    lhs.push_back(result[1]);
    T->factorisation(rhs, result[2]);

    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    T->next_relation(result);
    nr++;
  }
  REQUIRE(T->nrrules() == nr);
  REQUIRE(2459 == nr);
  delete T;
}

TEST_CASE(
    "Semigroup 55: relations [from copy_add_generators, fully enumerated]",
    "[quick][semigroup][finite][55]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);

  S.enumerate(8000);

  REQUIRE(S.is_begun());
  REQUIRE(S.is_done());

  std::vector<Element*> coll
      = {new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup<>* T = S.copy_add_generators(&coll);
  T->set_report(SEMIGROUPS_REPORT);
  really_delete_cont(coll);

  std::vector<size_t> result;
  T->next_relation(result);
  size_t nr = 0;
  while (!result.empty()) {
    REQUIRE(result.size() == 3);  // there are no duplicate gens
    word_t lhs, rhs;
    T->factorisation(lhs, result[0]);
    lhs.push_back(result[1]);
    T->factorisation(rhs, result[2]);

    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    T->next_relation(result);
    nr++;
  }
  REQUIRE(T->nrrules() == nr);
  REQUIRE(2459 == nr);

  T->reset_next_relation();
  T->next_relation(result);
  nr = 0;

  while (!result.empty()) {
    REQUIRE(result.size() == 3);
    word_t lhs, rhs;
    T->factorisation(lhs, result[0]);
    lhs.push_back(result[1]);
    T->factorisation(rhs, result[2]);

    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    T->next_relation(result);
    nr++;
  }
  REQUIRE(T->nrrules() == nr);
  REQUIRE(2459 == nr);
  delete T;
}

TEST_CASE("Semigroup 56: add_generators [duplicate generators]",
          "[quick][semigroup][finite][56]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 0, 3, 4, 5}),
         new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({0, 1, 3, 5, 5, 4}),
         new Transformation<u_int16_t>({1, 0, 2, 4, 4, 5}),
         new Transformation<u_int16_t>({4, 3, 3, 1, 0, 5}),
         new Transformation<u_int16_t>({4, 3, 5, 1, 0, 5}),
         new Transformation<u_int16_t>({5, 5, 2, 3, 4, 0})};

  Semigroup<> S = Semigroup<>({gens[0], gens[0]});
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.size() == 1);
  REQUIRE(S.nrgens() == 2);

  S.add_generators(std::vector<Element*>({}));
  REQUIRE(S.size() == 1);
  REQUIRE(S.nrgens() == 2);

  S.add_generators({gens[0]});
  REQUIRE(S.size() == 1);
  REQUIRE(S.nrgens() == 3);

  S.add_generators({gens[1]});
  REQUIRE(S.size() == 2);
  REQUIRE(S.nrgens() == 4);

  S.add_generators({gens[2]});
  REQUIRE(S.size() == 7);
  REQUIRE(S.nrgens() == 5);

  S.add_generators({gens[3]});
  REQUIRE(S.size() == 18);
  REQUIRE(S.nrgens() == 6);

  S.add_generators({gens[4]});
  REQUIRE(S.size() == 87);
  REQUIRE(S.nrgens() == 7);

  S.add_generators({gens[5]});
  REQUIRE(S.size() == 97);
  REQUIRE(S.nrgens() == 8);

  S.add_generators({gens[6]});
  REQUIRE(S.size() == 119);
  REQUIRE(S.nrgens() == 9);
  REQUIRE(S.nrrules() == 213);

  gens[0]->redefine(gens[3], gens[4]);
  S.add_generators({gens[0]});
  REQUIRE(S.size() == 119);
  REQUIRE(S.nrgens() == 10);
  REQUIRE(S.nrrules() == 267);

  REQUIRE(S.letter_to_pos(0) == 0);
  REQUIRE(S.letter_to_pos(1) == 0);
  REQUIRE(S.letter_to_pos(2) == 0);
  REQUIRE(S.letter_to_pos(3) == 1);
  REQUIRE(S.letter_to_pos(4) == 2);
  REQUIRE(S.letter_to_pos(5) == 7);
  REQUIRE(S.letter_to_pos(6) == 18);
  REQUIRE(S.letter_to_pos(7) == 87);
  REQUIRE(S.letter_to_pos(8) == 97);
  REQUIRE(S.letter_to_pos(9) == 21);

  really_delete_cont(gens);
}

TEST_CASE("Semigroup 57: add_generators [incremental 1]",
          "[quick][semigroup][finite][57]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 0, 3, 4, 5}),
         new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({0, 1, 3, 5, 5, 4}),
         new Transformation<u_int16_t>({1, 0, 2, 4, 4, 5}),
         new Transformation<u_int16_t>({4, 3, 3, 1, 0, 5}),
         new Transformation<u_int16_t>({4, 3, 5, 1, 0, 5}),
         new Transformation<u_int16_t>({5, 5, 2, 3, 4, 0})};

  Semigroup<> S = Semigroup<>({gens[0], gens[0]});
  S.set_report(SEMIGROUPS_REPORT);
  S.add_generators(std::vector<Element*>({}));
  S.add_generators({gens[0]});
  S.add_generators({gens[1]});
  S.add_generators({gens[2]});
  S.add_generators({gens[3]});
  REQUIRE(S.size() == 18);
  REQUIRE(S.nrgens() == 6);

  S.add_generators({gens[4]});
  S.add_generators({gens[5]});
  REQUIRE(S.size() == 97);
  REQUIRE(S.nrgens() == 8);
  REQUIRE(S.nrrules() == 126);

  S.add_generators({gens[4], gens[5]});
  S.add_generators({gens[5]});
  S.add_generators({gens[6]});
  S.add_generators({gens[0], gens[0]});
  REQUIRE(S.size() == 119);
  REQUIRE(S.nrgens() == 14);
  REQUIRE(S.nrrules() == 253);

  really_delete_cont(gens);
}

TEST_CASE("Semigroup 58: add_generators [incremental 2]",
          "[quick][semigroup][finite][58]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 0, 3, 4, 5}),
         new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({0, 1, 3, 5, 5, 4}),
         new Transformation<u_int16_t>({1, 0, 2, 4, 4, 5}),
         new Transformation<u_int16_t>({4, 3, 3, 1, 0, 5}),
         new Transformation<u_int16_t>({4, 3, 5, 1, 0, 5}),
         new Transformation<u_int16_t>({5, 5, 2, 3, 4, 0})};

  Semigroup<> T = Semigroup<>(gens);
  T.set_report(SEMIGROUPS_REPORT);
  REQUIRE(T.size() == 119);

  Semigroup<> S = Semigroup<>({gens[0], gens[0]});
  S.set_report(SEMIGROUPS_REPORT);
  S.add_generators(std::vector<Element*>({}));
  S.add_generators({gens[0]});
  S.enumerate(Semigroup<>::LIMIT_MAX);
  S.add_generators({gens[1]});
  S.enumerate(Semigroup<>::LIMIT_MAX);
  S.add_generators({gens[2]});
  S.enumerate(Semigroup<>::LIMIT_MAX);
  REQUIRE(S.current_size() == 7);
  S.add_generators({gens[3], gens[4], gens[5]});
  REQUIRE(S.nrgens() == 8);
  REQUIRE(S.letter_to_pos(5) == 7);
  REQUIRE(S.letter_to_pos(6) == 8);
  REQUIRE(S.letter_to_pos(7) == 9);
  REQUIRE(S.current_size() == 55);

  S.add_generators({S.at(44)});
  REQUIRE(S.nrgens() == 9);
  REQUIRE(S.current_size() == 73);
  REQUIRE(S.size() == 97);

  S.add_generators({S.at(75)});
  REQUIRE(S.nrgens() == 10);
  REQUIRE(S.current_size() == 97);
  REQUIRE(S.size() == 97);

  S.add_generators({gens[6]});
  REQUIRE(S.nrgens() == 11);
  REQUIRE(S.size() == 119);
  really_delete_cont(gens);
}

TEST_CASE("Semigroup 59: closure [duplicate generators]",
          "[quick][semigroup][finite][59]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 0, 3, 4, 5}),
         new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({0, 1, 3, 5, 5, 4}),
         new Transformation<u_int16_t>({1, 0, 2, 4, 4, 5}),
         new Transformation<u_int16_t>({4, 3, 3, 1, 0, 5}),
         new Transformation<u_int16_t>({4, 3, 5, 1, 0, 5}),
         new Transformation<u_int16_t>({5, 5, 2, 3, 4, 0})};

  Semigroup<> S = Semigroup<>({gens[0], gens[0]});
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.size() == 1);
  REQUIRE(S.nrgens() == 2);

  S.closure(std::vector<Element*>({}));
  REQUIRE(S.size() == 1);
  REQUIRE(S.nrgens() == 2);

  S.closure({gens[0]});
  REQUIRE(S.size() == 1);
  REQUIRE(S.nrgens() == 2);

  S.closure({gens[1]});
  REQUIRE(S.size() == 2);
  REQUIRE(S.nrgens() == 3);

  S.closure({gens[2]});
  REQUIRE(S.size() == 7);
  REQUIRE(S.nrgens() == 4);

  S.closure({gens[3]});
  REQUIRE(S.size() == 18);
  REQUIRE(S.nrgens() == 5);

  S.closure({gens[4]});
  REQUIRE(S.size() == 87);
  REQUIRE(S.nrgens() == 6);

  S.closure({gens[5]});
  REQUIRE(S.size() == 97);
  REQUIRE(S.nrgens() == 7);

  S.closure({gens[6]});
  REQUIRE(S.size() == 119);
  REQUIRE(S.nrgens() == 8);

  really_delete_cont(gens);
}

TEST_CASE("Semigroup 60: closure ", "[quick][semigroup][finite][60]") {
  std::vector<Element*> gens = {new Transformation<u_int16_t>({0, 0, 0}),
                                new Transformation<u_int16_t>({0, 0, 1}),
                                new Transformation<u_int16_t>({0, 0, 2}),
                                new Transformation<u_int16_t>({0, 1, 0}),
                                new Transformation<u_int16_t>({0, 1, 1}),
                                new Transformation<u_int16_t>({0, 1, 2}),
                                new Transformation<u_int16_t>({0, 2, 0}),
                                new Transformation<u_int16_t>({0, 2, 1}),
                                new Transformation<u_int16_t>({0, 2, 2}),
                                new Transformation<u_int16_t>({1, 0, 0}),
                                new Transformation<u_int16_t>({1, 0, 1}),
                                new Transformation<u_int16_t>({1, 0, 2}),
                                new Transformation<u_int16_t>({1, 1, 0}),
                                new Transformation<u_int16_t>({1, 1, 1}),
                                new Transformation<u_int16_t>({1, 1, 2}),
                                new Transformation<u_int16_t>({1, 2, 0}),
                                new Transformation<u_int16_t>({1, 2, 1}),
                                new Transformation<u_int16_t>({1, 2, 2}),
                                new Transformation<u_int16_t>({2, 0, 0}),
                                new Transformation<u_int16_t>({2, 0, 1}),
                                new Transformation<u_int16_t>({2, 0, 2}),
                                new Transformation<u_int16_t>({2, 1, 0}),
                                new Transformation<u_int16_t>({2, 1, 1}),
                                new Transformation<u_int16_t>({2, 1, 2}),
                                new Transformation<u_int16_t>({2, 2, 0}),
                                new Transformation<u_int16_t>({2, 2, 1}),
                                new Transformation<u_int16_t>({2, 2, 2})};

  Semigroup<> S = Semigroup<>({gens[0]});
  S.set_report(SEMIGROUPS_REPORT);

  S.closure(gens);
  REQUIRE(S.size() == 27);
  REQUIRE(S.nrgens() == 10);
  really_delete_cont(gens);
}

TEST_CASE("Semigroup 61: factorisation ", "[quick][semigroup][finite][61]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({1, 1, 4, 5, 4, 5}),
         new Transformation<u_int16_t>({2, 3, 2, 3, 5, 5})};

  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);

  word_t* w = S.factorisation(2);
  REQUIRE(*w == word_t({0, 1}));
  delete w;
}

TEST_CASE("Semigroup 62: my favourite example with reserve",
          "[standard][semigroup][finite][62]") {
  std::vector<Element*> gens
      = {new Transformation<uint_fast8_t>({1, 7, 2, 6, 0, 4, 1, 5}),
         new Transformation<uint_fast8_t>({2, 4, 6, 1, 4, 5, 2, 7}),
         new Transformation<uint_fast8_t>({3, 0, 7, 2, 4, 6, 2, 4}),
         new Transformation<uint_fast8_t>({3, 2, 3, 4, 5, 3, 0, 1}),
         new Transformation<uint_fast8_t>({4, 3, 7, 7, 4, 5, 0, 4}),
         new Transformation<uint_fast8_t>({5, 6, 3, 0, 3, 0, 5, 1}),
         new Transformation<uint_fast8_t>({6, 0, 1, 1, 1, 6, 3, 4}),
         new Transformation<uint_fast8_t>({7, 7, 4, 0, 6, 4, 1, 7})};
  Semigroup<> S(gens);
  S.reserve(597369);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);

  REQUIRE(S.size() == 597369);
}

TEST_CASE("Semigroup 63: minimal_factorisation ",
          "[quick][semigroup][finite][63]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({1, 1, 4, 5, 4, 5}),
         new Transformation<u_int16_t>({2, 3, 2, 3, 5, 5})};

  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);

  word_t* w = S.minimal_factorisation(gens[0]);
  REQUIRE(*w == word_t({0}));
  delete w;

  w = S.factorisation(gens[0]);
  REQUIRE(*w == word_t({0}));
  delete w;
  really_delete_cont(gens);

  Element* x = new Transformation<u_int16_t>({4, 1, 4, 1, 4, 5});
  REQUIRE(S.minimal_factorisation(x) == nullptr);
  x->really_delete();
  delete x;

  REQUIRE(S.minimal_factorisation(10000000) == nullptr);
}

TEST_CASE("Semigroup 64: batch_size (for an extremely large value)",
          "[quick][semigroup][finite][64]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({1, 1, 4, 5, 4, 5}),
         new Transformation<u_int16_t>({2, 3, 2, 3, 5, 5})};
  Semigroup<> S = Semigroup<>(gens);
  really_delete_cont(gens);

  S.set_report(SEMIGROUPS_REPORT);
  S.set_batch_size(Semigroup<>::LIMIT_MAX);
  S.enumerate();

  REQUIRE(S.size() == 5);
}

TEST_CASE("Semigroup 65: my favourite example without reserve",
          "[standard][semigroup][finite][65]") {
  std::vector<Element*> gens
      = {new Transformation<uint_fast8_t>({1, 7, 2, 6, 0, 4, 1, 5}),
         new Transformation<uint_fast8_t>({2, 4, 6, 1, 4, 5, 2, 7}),
         new Transformation<uint_fast8_t>({3, 0, 7, 2, 4, 6, 2, 4}),
         new Transformation<uint_fast8_t>({3, 2, 3, 4, 5, 3, 0, 1}),
         new Transformation<uint_fast8_t>({4, 3, 7, 7, 4, 5, 0, 4}),
         new Transformation<uint_fast8_t>({5, 6, 3, 0, 3, 0, 5, 1}),
         new Transformation<uint_fast8_t>({6, 0, 1, 1, 1, 6, 3, 4}),
         new Transformation<uint_fast8_t>({7, 7, 4, 0, 6, 4, 1, 7})};

  Semigroup<> S = Semigroup<>(gens);
  really_delete_cont(gens);

  S.set_report(SEMIGROUPS_REPORT);
  REQUIRE(S.size() == 597369);
}

TEST_CASE("Semigroup 70: number of idempotents",
          "[extreme][semigroup][finite][70]") {
  Semiring<int64_t>* sr = new NaturalSemiring(0, 6);

  std::vector<Element*> gens = {
      new MatrixOverSemiring<int64_t>({{0, 0, 1}, {0, 1, 0}, {1, 1, 0}}, sr),
      new MatrixOverSemiring<int64_t>({{0, 0, 1}, {0, 1, 0}, {2, 0, 0}}, sr),
      new MatrixOverSemiring<int64_t>({{0, 0, 1}, {0, 1, 1}, {1, 0, 0}}, sr),
      new MatrixOverSemiring<int64_t>({{0, 0, 1}, {0, 1, 0}, {3, 0, 0}}, sr)};
  Semigroup<> S(gens);
  really_delete_cont(gens);
  S.reserve(10077696);
  S.set_report(true);
  REQUIRE(S.size() == 10077696);
  REQUIRE(S.nridempotents() == 13688);
  S.set_report(false);
  delete sr;
}

TEST_CASE("Semigroup 71: number of idempotents",
          "[extreme][semigroup][finite][71]") {
  std::vector<Element*> gens
      = {new Bipartition({0, 1, 2, 3, 4, 5, 5, 0, 1, 2, 3, 4}),
         new Bipartition({0, 1, 2, 3, 4, 5, 1, 0, 2, 3, 4, 5}),
         new Bipartition({0, 1, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5}),
         new Bipartition({0, 0, 1, 2, 3, 4, 0, 0, 1, 2, 3, 4})};
  Semigroup<> S(gens);
  really_delete_cont(gens);
  S.reserve(4213597);
  S.set_report(true);
  REQUIRE(S.size() == 4213597);
  REQUIRE(S.nridempotents() == 541254);
  S.set_report(false);
}
