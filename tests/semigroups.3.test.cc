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

template <class TElementType>
void delete_gens(std::vector<TElementType>& gens) {
  for (auto& x : gens) {
    delete x;
  }
}

TEST_CASE("Semigroup 043: relations [copy_closure, duplicate gens]",
          "[quick][semigroup][finite][043]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup<> S = Semigroup<>(gens);
  REPORTER.set_report(SEMIGROUPS_REPORT);

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

  Semigroup<>* T = S.copy_closure(coll);
  REPORTER.set_report(SEMIGROUPS_REPORT);

  REQUIRE(T->size() == 7776);
  REQUIRE(T->is_done());
  REQUIRE(T->nridempotents() == 537);
  delete T;
  delete_gens(gens);
  delete_gens(coll);
}

TEST_CASE("Semigroup 044: relations [copy_add_generators, duplicate gens]",
          "[quick][semigroup][finite][044]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup<> S = Semigroup<>(gens);
  REPORTER.set_report(SEMIGROUPS_REPORT);

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

  Semigroup<>* T = S.copy_add_generators(coll);
  REPORTER.set_report(SEMIGROUPS_REPORT);

  REQUIRE(T->size() == 7776);
  REQUIRE(T->is_done());
  REQUIRE(T->nridempotents() == 537);
  delete T;
  delete_gens(gens);
  delete_gens(coll);
}

TEST_CASE("Semigroup 045: relations [from copy, not enumerated]",
          "[quick][semigroup][finite][045]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  REPORTER.set_report(SEMIGROUPS_REPORT);

  Semigroup<> T = Semigroup<>(S);
  REPORTER.set_report(SEMIGROUPS_REPORT);
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
  delete_gens(gens);
}

TEST_CASE("Semigroup 046: relations [from copy, partly enumerated]",
          "[quick][semigroup][finite][046]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  REPORTER.set_report(SEMIGROUPS_REPORT);

  S.set_batch_size(1023);
  S.enumerate(1000);

  Semigroup<> T = Semigroup<>(S);
  REPORTER.set_report(SEMIGROUPS_REPORT);
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
  delete_gens(gens);
}

TEST_CASE("Semigroup 047: relations [from copy, fully enumerated]",
          "[quick][semigroup][finite][047]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  REPORTER.set_report(SEMIGROUPS_REPORT);

  S.enumerate(8000);

  Semigroup<> T = Semigroup<>(S);
  REPORTER.set_report(SEMIGROUPS_REPORT);
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
  delete_gens(gens);
}

TEST_CASE("Semigroup 050: relations [from copy_closure, not enumerated]",
          "[quick][semigroup][finite][050]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup<> S = Semigroup<>(gens);
  REPORTER.set_report(SEMIGROUPS_REPORT);

  REQUIRE(!S.is_begun());
  REQUIRE(!S.is_done());

  std::vector<Element*> coll
      = {new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup<>* T = S.copy_closure(coll);
  REPORTER.set_report(SEMIGROUPS_REPORT);
  REQUIRE(*coll[0] == *(T->gens(3)));
  REQUIRE(*coll[1] == *(T->gens(4)));
  delete_gens(coll);

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
  delete_gens(gens);
}

TEST_CASE("Semigroup 051: relations [from copy_add_generators, not enumerated]",
          "[quick][semigroup][finite][051]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup<> S = Semigroup<>(gens);
  REPORTER.set_report(SEMIGROUPS_REPORT);

  REQUIRE(!S.is_begun());
  REQUIRE(!S.is_done());

  std::vector<Element*> coll
      = {new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup<>* T = S.copy_add_generators(coll);
  REPORTER.set_report(SEMIGROUPS_REPORT);
  REQUIRE(*coll[0] == *(T->gens(3)));
  REQUIRE(*coll[1] == *(T->gens(4)));
  delete_gens(coll);

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
  delete_gens(gens);
}

TEST_CASE("Semigroup 052: relations [from copy_closure, partly enumerated]",
          "[quick][semigroup][finite][052]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup<> S = Semigroup<>(gens);
  REPORTER.set_report(SEMIGROUPS_REPORT);
  S.set_batch_size(100);

  S.enumerate(10);

  REQUIRE(S.is_begun());
  REQUIRE(!S.is_done());

  std::vector<Element*> coll
      = {new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup<>* T = S.copy_closure(coll);
  REPORTER.set_report(SEMIGROUPS_REPORT);
  delete_gens(coll);

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
  delete_gens(gens);
}

TEST_CASE(
    "Semigroup 053: relations [from copy_add_generators, partly enumerated]",
    "[quick][semigroup][finite][053]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup<> S = Semigroup<>(gens);
  REPORTER.set_report(SEMIGROUPS_REPORT);
  S.set_batch_size(100);

  S.enumerate(10);

  REQUIRE(S.is_begun());
  REQUIRE(!S.is_done());

  std::vector<Element*> coll
      = {new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup<>* T = S.copy_add_generators(coll);
  REPORTER.set_report(SEMIGROUPS_REPORT);
  delete_gens(coll);

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
  delete_gens(gens);
}

TEST_CASE("Semigroup 054: relations [from copy_closure, fully enumerated]",
          "[quick][semigroup][finite][054]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup<> S = Semigroup<>(gens);
  REPORTER.set_report(SEMIGROUPS_REPORT);

  S.enumerate(8000);

  REQUIRE(S.is_begun());
  REQUIRE(S.is_done());

  std::vector<Element*> coll
      = {new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup<>* T = S.copy_closure(coll);
  REPORTER.set_report(SEMIGROUPS_REPORT);
  delete_gens(coll);

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
  delete_gens(gens);
}

TEST_CASE(
    "Semigroup 055: relations [from copy_add_generators, fully enumerated]",
    "[quick][semigroup][finite][055]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup<> S = Semigroup<>(gens);
  REPORTER.set_report(SEMIGROUPS_REPORT);

  S.enumerate(8000);

  REQUIRE(S.is_begun());
  REQUIRE(S.is_done());

  std::vector<Element*> coll
      = {new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup<>* T = S.copy_add_generators(coll);
  REPORTER.set_report(SEMIGROUPS_REPORT);
  delete_gens(coll);

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
  delete_gens(gens);
}

TEST_CASE("Semigroup 056: add_generators [duplicate generators]",
          "[quick][semigroup][finite][056]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 0, 3, 4, 5}),
         new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({0, 1, 3, 5, 5, 4}),
         new Transformation<u_int16_t>({1, 0, 2, 4, 4, 5}),
         new Transformation<u_int16_t>({4, 3, 3, 1, 0, 5}),
         new Transformation<u_int16_t>({4, 3, 5, 1, 0, 5}),
         new Transformation<u_int16_t>({5, 5, 2, 3, 4, 0})};

  Semigroup<> S = Semigroup<>({gens[0], gens[0]});
  REPORTER.set_report(SEMIGROUPS_REPORT);

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
  delete_gens(gens);
}

TEST_CASE("Semigroup 057: add_generators [incremental 1]",
          "[quick][semigroup][finite][057]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 0, 3, 4, 5}),
         new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({0, 1, 3, 5, 5, 4}),
         new Transformation<u_int16_t>({1, 0, 2, 4, 4, 5}),
         new Transformation<u_int16_t>({4, 3, 3, 1, 0, 5}),
         new Transformation<u_int16_t>({4, 3, 5, 1, 0, 5}),
         new Transformation<u_int16_t>({5, 5, 2, 3, 4, 0})};

  Semigroup<> S = Semigroup<>({gens[0], gens[0]});
  REPORTER.set_report(SEMIGROUPS_REPORT);
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
  delete_gens(gens);
}

TEST_CASE("Semigroup 058: add_generators [incremental 2]",
          "[quick][semigroup][finite][058]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 0, 3, 4, 5}),
         new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({0, 1, 3, 5, 5, 4}),
         new Transformation<u_int16_t>({1, 0, 2, 4, 4, 5}),
         new Transformation<u_int16_t>({4, 3, 3, 1, 0, 5}),
         new Transformation<u_int16_t>({4, 3, 5, 1, 0, 5}),
         new Transformation<u_int16_t>({5, 5, 2, 3, 4, 0})};

  Semigroup<> T = Semigroup<>(gens);
  REPORTER.set_report(SEMIGROUPS_REPORT);
  REQUIRE(T.size() == 119);

  Semigroup<> S = Semigroup<>({gens[0], gens[0]});
  REPORTER.set_report(SEMIGROUPS_REPORT);
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
  delete_gens(gens);
}

TEST_CASE("Semigroup 059: closure [duplicate generators]",
          "[quick][semigroup][finite][059]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 0, 3, 4, 5}),
         new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({0, 1, 3, 5, 5, 4}),
         new Transformation<u_int16_t>({1, 0, 2, 4, 4, 5}),
         new Transformation<u_int16_t>({4, 3, 3, 1, 0, 5}),
         new Transformation<u_int16_t>({4, 3, 5, 1, 0, 5}),
         new Transformation<u_int16_t>({5, 5, 2, 3, 4, 0})};

  Semigroup<> S = Semigroup<>({gens[0], gens[0]});
  REPORTER.set_report(SEMIGROUPS_REPORT);

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
  delete_gens(gens);
}

TEST_CASE("Semigroup 060: closure ", "[quick][semigroup][finite][060]") {
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
  REPORTER.set_report(SEMIGROUPS_REPORT);

  S.closure(gens);
  REQUIRE(S.size() == 27);
  REQUIRE(S.nrgens() == 10);
  delete_gens(gens);
}

TEST_CASE("Semigroup 061: factorisation ", "[quick][semigroup][finite][061]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({1, 1, 4, 5, 4, 5}),
         new Transformation<u_int16_t>({2, 3, 2, 3, 5, 5})};

  Semigroup<> S = Semigroup<>(gens);
  REPORTER.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.factorisation(2) == word_t({0, 1}));
  delete_gens(gens);
}

TEST_CASE("Semigroup 062: my favourite example with reserve",
          "[standard][semigroup][finite][062]") {
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
  REPORTER.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.size() == 597369);
  delete_gens(gens);
}

TEST_CASE("Semigroup 063: minimal_factorisation ",
          "[quick][semigroup][finite][063]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({1, 1, 4, 5, 4, 5}),
         new Transformation<u_int16_t>({2, 3, 2, 3, 5, 5})};

  Semigroup<> S = Semigroup<>(gens);
  REPORTER.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.minimal_factorisation(gens[0]) == word_t({0}));

  REQUIRE(S.factorisation(gens[0]) == word_t({0}));

  Element* x = new Transformation<u_int16_t>({4, 1, 4, 1, 4, 5});
  REQUIRE_THROWS_AS(S.minimal_factorisation(x), LibsemigroupsException);
  delete x;

  REQUIRE_THROWS_AS(S.minimal_factorisation(10000000), LibsemigroupsException);
  delete_gens(gens);
}

TEST_CASE("Semigroup 064: batch_size (for an extremely large value)",
          "[quick][semigroup][finite][064]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({1, 1, 4, 5, 4, 5}),
         new Transformation<u_int16_t>({2, 3, 2, 3, 5, 5})};
  Semigroup<> S = Semigroup<>(gens);

  REPORTER.set_report(SEMIGROUPS_REPORT);
  S.set_batch_size(Semigroup<>::LIMIT_MAX);
  S.enumerate();

  REQUIRE(S.size() == 5);
  delete_gens(gens);
}

TEST_CASE("Semigroup 065: my favourite example without reserve",
          "[standard][semigroup][finite][065]") {
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

  REPORTER.set_report(SEMIGROUPS_REPORT);
  REQUIRE(S.size() == 597369);
  delete_gens(gens);
}

TEST_CASE("Semigroup 070: number of idempotents",
          "[extreme][semigroup][finite][070]") {
  Semiring<int64_t>* sr = new NaturalSemiring(0, 6);

  std::vector<Element*> gens = {
      new MatrixOverSemiring<int64_t>({{0, 0, 1}, {0, 1, 0}, {1, 1, 0}}, sr),
      new MatrixOverSemiring<int64_t>({{0, 0, 1}, {0, 1, 0}, {2, 0, 0}}, sr),
      new MatrixOverSemiring<int64_t>({{0, 0, 1}, {0, 1, 1}, {1, 0, 0}}, sr),
      new MatrixOverSemiring<int64_t>({{0, 0, 1}, {0, 1, 0}, {3, 0, 0}}, sr)};
  Semigroup<> S(gens);
  S.reserve(10077696);
  REPORTER.set_report(true);
  REQUIRE(S.size() == 10077696);
  REQUIRE(S.nridempotents() == 13688);
  REPORTER.set_report(false);
  delete sr;
  delete_gens(gens);
}

TEST_CASE("Semigroup 071: number of idempotents",
          "[extreme][semigroup][finite][071]") {
  std::vector<Element*> gens
      = {new Bipartition({0, 1, 2, 3, 4, 5, 5, 0, 1, 2, 3, 4}),
         new Bipartition({0, 1, 2, 3, 4, 5, 1, 0, 2, 3, 4, 5}),
         new Bipartition({0, 1, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5}),
         new Bipartition({0, 0, 1, 2, 3, 4, 0, 0, 1, 2, 3, 4})};
  Semigroup<> S(gens);
  S.reserve(4213597);
  REPORTER.set_report(true);
  REQUIRE(S.size() == 4213597);
  REQUIRE(S.nridempotents() == 541254);
  REPORTER.set_report(false);
  delete_gens(gens);
}
