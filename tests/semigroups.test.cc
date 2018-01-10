//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2016 James D. Mitchell
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

#include "../src/bmat.h"
#include "../src/semigroups.h"
#include "catch.hpp"

#define SEMIGROUPS_REPORT false

using namespace libsemigroups;

// TODO template
static inline size_t evaluate_reduct(Semigroup<>& S, word_t const& word) {
  letter_t out = S.letter_to_pos(word[0]);
  for (auto it = word.cbegin() + 1; it < word.cend(); ++it) {
    out = S.right(out, *it);
  }
  return out;
}

// TODO template
static inline size_t evaluate_reduct(Semigroup<>* S, word_t const& word) {
  return evaluate_reduct(*S, word);
}

// TODO template
static inline void test_idempotent(Semigroup<>& S, Element* x) {
  REQUIRE(S.is_idempotent(S.position(x)));
  Element* y = x->really_copy();
  y->redefine(x, x);
  REQUIRE(*x == *y);
  REQUIRE(S.fast_product(S.position(x), S.position(x)) == S.position(x));
  y->really_delete();
  delete y;
}

TEST_CASE("Semigroup 01: small transformation semigroup",
          "[quick][semigroup][finite][01]") {
  std::vector<Element*> gens = {new Transformation<u_int16_t>({0, 1, 0}),
                                new Transformation<u_int16_t>({0, 1, 2})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);

  REQUIRE(S.size() == 2);
  REQUIRE(S.degree() == 3);
  REQUIRE(S.nridempotents() == 2);
  REQUIRE(S.nrgens() == 2);
  REQUIRE(S.nrrules() == 4);

  Element* expected = new Transformation<u_int16_t>({0, 1, 0});
  REQUIRE(*S[0] == *expected);
  expected->really_delete();
  delete expected;

  expected = new Transformation<u_int16_t>({0, 1, 2});
  REQUIRE(*S[1] == *expected);
  expected->really_delete();
  delete expected;

  Element* x = new Transformation<u_int16_t>({0, 1, 0});
  REQUIRE(S.position(x) == 0);
  REQUIRE(S.test_membership(x));
  x->really_delete();
  delete x;

  x = new Transformation<u_int16_t>({0, 1, 2});
  REQUIRE(S.position(x) == 1);
  REQUIRE(S.test_membership(x));
  x->really_delete();
  delete x;

  x = new Transformation<u_int16_t>({0, 0, 0});
  REQUIRE(S.position(x) == Semigroup<>::UNDEFINED);
  REQUIRE(!S.test_membership(x));
  x->really_delete();
  delete x;
}

TEST_CASE("Semigroup 02: small partial perm semigroup",
          "[quick][semigroup][finite][02]") {
  std::vector<Element*> gens
      = {new PartialPerm<u_int16_t>(
             {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 10),
         new PartialPerm<u_int16_t>({4, 5, 0}, {10, 0, 1}, 10)};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);

  REQUIRE(S.size() == 22);
  REQUIRE(S.degree() == 11);
  REQUIRE(S.nridempotents() == 1);
  REQUIRE(S.nrgens() == 2);
  REQUIRE(S.nrrules() == 9);

  Element* expected = new PartialPerm<u_int16_t>(
      {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 10);
  REQUIRE(*S[0] == *expected);
  expected->really_delete();
  delete expected;

  expected = new PartialPerm<u_int16_t>({4, 5, 0}, {10, 0, 1}, 10);
  REQUIRE(*S[1] == *expected);
  expected->really_delete();
  delete expected;

  Element* x = new Transformation<u_int16_t>({0, 1, 0});
  REQUIRE(S.position(x) == Semigroup<>::UNDEFINED);
  REQUIRE(!S.test_membership(x));
  x->really_delete();
  delete x;

  x = new PartialPerm<u_int16_t>({}, {}, 10);
  REQUIRE(S.position(x) == 10);
  REQUIRE(S.test_membership(x));
  x->really_delete();
  delete x;

  x = new PartialPerm<u_int16_t>({}, {}, 9);
  REQUIRE(S.position(x) == Semigroup<>::UNDEFINED);
  REQUIRE(!S.test_membership(x));

  x->really_delete();
  delete x;

  x = new PartialPerm<u_int16_t>(
      {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 10);
  REQUIRE(S.position(x) == 0);
  REQUIRE(S.test_membership(x));

  Element* y = new PartialPerm<u_int16_t>({4, 5, 0}, {10, 0, 1}, 10);
  REQUIRE(S.position(y) == 1);
  REQUIRE(S.test_membership(y));

  y->redefine(x, x);
  REQUIRE(S.position(y) == 2);
  REQUIRE(S.test_membership(y));

  REQUIRE(*y == *S[2]);
  x->really_delete();
  delete x;
  y->really_delete();
  delete y;
}

TEST_CASE("Semigroup 03: small bipartition semigroup",
          "[quick][semigroup][finite][03]") {
  std::vector<Element*> gens
      = {new Bipartition(
             {0, 1, 2, 1, 0, 2, 1, 0, 2, 2, 0, 0, 2, 0, 3, 4, 4, 1, 3, 0}),
         new Bipartition(
             {0, 1, 1, 1, 1, 2, 3, 2, 4, 5, 5, 2, 4, 2, 1, 1, 1, 2, 3, 2}),
         new Bipartition(
             {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  REQUIRE(S.size() == 10);
  REQUIRE(S.degree() == 10);
  REQUIRE(S.nridempotents() == 6);
  REQUIRE(S.nrgens() == 3);
  REQUIRE(S.nrrules() == 14);

  REQUIRE(*S[0] == *gens[0]);
  REQUIRE(*S[1] == *gens[1]);
  REQUIRE(*S[2] == *gens[2]);

  REQUIRE(S.position(gens[0]) == 0);
  REQUIRE(S.test_membership(gens[0]));

  REQUIRE(S.position(gens[1]) == 1);
  REQUIRE(S.test_membership(gens[1]));

  REQUIRE(S.position(gens[2]) == 2);
  REQUIRE(S.test_membership(gens[2]));

  Element* y = new Bipartition(
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
  REQUIRE(S.position(y) == 2);
  REQUIRE(S.test_membership(y));
  y->redefine(gens[0], gens[1]);
  REQUIRE(S.position(y) == 4);
  REQUIRE(S.test_membership(y));
  y->redefine(gens[1], gens[2]);
  REQUIRE(S.position(y) == 7);
  REQUIRE(S.test_membership(y));
  y->really_delete();
  delete y;
  really_delete_cont(gens);
}

TEST_CASE("Semigroup 04: small Boolean matrix semigroup",
          "[quick][semigroup][finite][04]") {
  std::vector<Element*> gens
      = {new BooleanMat({{1, 0, 1}, {0, 1, 0}, {0, 1, 0}}),
         new BooleanMat({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}),
         new BooleanMat({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  REQUIRE(S.size() == 3);
  REQUIRE(S.degree() == 3);
  REQUIRE(S.nridempotents() == 2);
  REQUIRE(S.nrgens() == 3);
  REQUIRE(S.nrrules() == 7);
  REQUIRE(*S[0] == *gens[0]);
  REQUIRE(*S[1] == *gens[1]);
  REQUIRE(*S[1] == *gens[2]);

  REQUIRE(S.position(gens[0]) == 0);
  REQUIRE(S.test_membership(gens[0]));

  REQUIRE(S.position(gens[1]) == 1);
  REQUIRE(S.test_membership(gens[1]));

  REQUIRE(S.position(gens[2]) == 1);
  REQUIRE(S.test_membership(gens[1]));

  Element* y = new BooleanMat({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}});
  y->redefine(gens[0], gens[0]);
  REQUIRE(S.position(y) == 2);
  REQUIRE(S.test_membership(y));
  y->really_delete();
  delete y;
  really_delete_cont(gens);
}

TEST_CASE("Semigroup 05: small projective max plus matrix semigroup",
          "[quick][semigroup][finite][05]") {
  Semiring<int64_t>* sr = new MaxPlusSemiring();
  auto x  = new ProjectiveMaxPlusMatrix({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, sr);
  auto id = x->identity();
  x->really_delete();
  delete x;
  Semigroup<> S = Semigroup<>({id});
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.size() == 1);
  REQUIRE(S.degree() == 3);
  REQUIRE(S.nridempotents() == 1);
  REQUIRE(S.nrgens() == 1);
  REQUIRE(S.nrrules() == 1);
  REQUIRE(*S[0] == *id);

  REQUIRE(S.position(id) == 0);
  REQUIRE(S.test_membership(id));
  id->really_delete();
  delete id;

  x = new ProjectiveMaxPlusMatrix({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  REQUIRE(S.position(x) == Semigroup<>::UNDEFINED);
  REQUIRE(!S.test_membership(x));
  x->really_delete();
  delete x;
  delete sr;
}

TEST_CASE("Semigroup 06: small matrix semigroup [Integers]",
          "[quick][semigroup][finite][06]") {
  Semiring<int64_t>*    sr = new Integers();
  std::vector<Element*> gens
      = {new MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr),
         new MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr)};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.size() == 13);
  REQUIRE(S.degree() == 2);
  REQUIRE(S.nridempotents() == 4);
  REQUIRE(S.nrgens() == 2);
  REQUIRE(S.nrrules() == 6);
  REQUIRE(*S[0] == *(gens)[0]);
  REQUIRE(*S[1] == *(gens)[1]);

  REQUIRE(S.position(gens[0]) == 0);
  REQUIRE(S.test_membership(gens[0]));

  REQUIRE(S.position(gens[1]) == 1);
  REQUIRE(S.test_membership(gens[1]));

  Element* x = new MatrixOverSemiring<int64_t>({{-2, 2}, {-1, 0}}, sr);
  REQUIRE(S.position(x) == Semigroup<>::UNDEFINED);
  REQUIRE(!S.test_membership(x));

  x->redefine(gens[1], gens[1]);
  REQUIRE(S.position(x) == 4);
  REQUIRE(S.test_membership(x));
  x->really_delete();
  delete x;

  x = new MatrixOverSemiring<int64_t>({{-2, 2, 0}, {-1, 0, 0}, {0, 0, 0}}, sr);
  REQUIRE(S.position(x) == Semigroup<>::UNDEFINED);
  REQUIRE(!S.test_membership(x));
  x->really_delete();
  delete x;

  really_delete_cont(gens);
  delete sr;
}

TEST_CASE("Semigroup 07: small matrix semigroup [MaxPlusSemiring]",
          "[quick][semigroup][finite][07]") {
  Semiring<int64_t>*    sr = new MaxPlusSemiring();
  std::vector<Element*> gens
      = {new MatrixOverSemiring<int64_t>({{0, -4}, {-4, -1}}, sr),
         new MatrixOverSemiring<int64_t>({{0, -3}, {-3, -1}}, sr)};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.size() == 26);
  REQUIRE(S.degree() == 2);
  REQUIRE(S.nridempotents() == 4);
  REQUIRE(S.nrgens() == 2);
  REQUIRE(S.nrrules() == 9);
  REQUIRE(*S[0] == *gens[0]);
  REQUIRE(*S[1] == *gens[1]);

  REQUIRE(S.position(gens[0]) == 0);
  REQUIRE(S.test_membership(gens[0]));

  REQUIRE(S.position(gens[1]) == 1);
  REQUIRE(S.test_membership(gens[1]));

  Element* x = new MatrixOverSemiring<int64_t>({{-2, 2}, {-1, 0}}, sr);
  REQUIRE(S.position(x) == Semigroup<>::UNDEFINED);
  REQUIRE(!S.test_membership(x));
  x->redefine(gens[1], gens[1]);
  REQUIRE(S.position(x) == 5);
  REQUIRE(S.test_membership(x));
  x->really_delete();
  delete x;

  x = new MatrixOverSemiring<int64_t>({{-2, 2, 0}, {-1, 0, 0}, {0, 0, 0}}, sr);
  REQUIRE(S.position(x) == Semigroup<>::UNDEFINED);
  REQUIRE(!S.test_membership(x));
  x->really_delete();
  delete x;

  really_delete_cont(gens);
  delete sr;
}

TEST_CASE("Semigroup 08: small matrix semigroup [MinPlusSemiring]",
          "[quick][semigroup][finite][08]") {
  Semiring<int64_t>*    sr = new MinPlusSemiring();
  std::vector<Element*> gens
      = {new MatrixOverSemiring<int64_t>({{1, 0}, {0, LONG_MAX}}, sr)};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.size() == 3);
  REQUIRE(S.degree() == 2);
  REQUIRE(S.nridempotents() == 1);
  REQUIRE(S.nrgens() == 1);
  REQUIRE(S.nrrules() == 1);

  REQUIRE(*S[0] == *gens[0]);
  REQUIRE(S.position(gens[0]) == 0);
  REQUIRE(S.test_membership(gens[0]));

  Element* x = new MatrixOverSemiring<int64_t>({{-2, 2}, {-1, 0}}, sr);
  REQUIRE(S.position(x) == Semigroup<>::UNDEFINED);
  REQUIRE(!S.test_membership(x));
  x->redefine(gens[0], gens[0]);
  REQUIRE(S.position(x) == 1);
  REQUIRE(S.test_membership(x));
  x->really_delete();
  delete x;

  x = new MatrixOverSemiring<int64_t>({{-2, 2, 0}, {-1, 0, 0}, {0, 0, 0}}, sr);
  REQUIRE(S.position(x) == Semigroup<>::UNDEFINED);
  REQUIRE(!S.test_membership(x));
  x->really_delete();
  delete x;

  really_delete_cont(gens);
  delete sr;
}

TEST_CASE("Semigroup 09: small matrix semigroup [TropicalMaxPlusSemiring]",
          "[quick][semigroup][finite][09]") {
  Semiring<int64_t>*    sr   = new TropicalMaxPlusSemiring(33);
  std::vector<Element*> gens = {
      new MatrixOverSemiring<int64_t>({{22, 21, 0}, {10, 0, 0}, {1, 32, 1}},
                                      sr),
      new MatrixOverSemiring<int64_t>({{0, 0, 0}, {0, 1, 0}, {1, 1, 0}}, sr)};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.size() == 119);
  REQUIRE(S.degree() == 3);
  REQUIRE(S.nridempotents() == 1);
  REQUIRE(S.nrgens() == 2);
  REQUIRE(S.nrrules() == 18);

  REQUIRE(*S[0] == *gens[0]);
  REQUIRE(S.position(gens[0]) == 0);
  REQUIRE(S.test_membership(gens[0]));

  Element* x = new MatrixOverSemiring<int64_t>({{2, 2}, {1, 0}}, sr);
  REQUIRE(S.position(x) == Semigroup<>::UNDEFINED);
  REQUIRE(!S.test_membership(x));
  x->really_delete();
  delete x;

  x = new MatrixOverSemiring<int64_t>({{2, 2, 0}, {1, 0, 0}, {0, 0, 0}}, sr);
  REQUIRE(S.position(x) == Semigroup<>::UNDEFINED);
  REQUIRE(!S.test_membership(x));
  x->redefine(gens[0], gens[0]);
  REQUIRE(S.position(x) == 2);
  REQUIRE(S.test_membership(x));
  x->really_delete();
  delete x;

  really_delete_cont(gens);
  delete sr;
}

TEST_CASE("Semigroup 10: small matrix semigroup [TropicalMinPlusSemiring]",
          "[quick][semigroup][finite][10]") {
  Semiring<int64_t>*    sr   = new TropicalMinPlusSemiring(11);
  std::vector<Element*> gens = {
      new MatrixOverSemiring<int64_t>({{2, 1, 0}, {10, 0, 0}, {1, 2, 1}}, sr),
      new MatrixOverSemiring<int64_t>({{10, 0, 0}, {0, 1, 0}, {1, 1, 0}}, sr)};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.size() == 1039);
  REQUIRE(S.degree() == 3);
  REQUIRE(S.nridempotents() == 5);
  REQUIRE(S.nrgens() == 2);
  REQUIRE(S.nrrules() == 38);

  REQUIRE(*S[0] == *gens[0]);
  REQUIRE(S.position(gens[0]) == 0);
  REQUIRE(S.test_membership(gens[0]));

  Element* x = new MatrixOverSemiring<int64_t>({{-2, 2}, {-1, 0}}, sr);
  REQUIRE(S.position(x) == Semigroup<>::UNDEFINED);
  REQUIRE(!S.test_membership(x));
  x->really_delete();
  delete x;

  x = new MatrixOverSemiring<int64_t>({{-2, 2, 0}, {-1, 0, 0}, {0, 0, 0}}, sr);
  REQUIRE(S.position(x) == Semigroup<>::UNDEFINED);
  REQUIRE(!S.test_membership(x));
  x->redefine(gens[0], gens[0]);
  REQUIRE(S.position(x) == 2);
  REQUIRE(S.test_membership(x));
  x->really_delete();
  delete x;

  really_delete_cont(gens);
  delete sr;
}

TEST_CASE("Semigroup 11: small matrix semigroup [NaturalSemiring]",
          "[quick][semigroup][finite][11]") {
  Semiring<int64_t>*    sr   = new NaturalSemiring(11, 3);
  std::vector<Element*> gens = {
      new MatrixOverSemiring<int64_t>({{2, 1, 0}, {10, 0, 0}, {1, 2, 1}}, sr),
      new MatrixOverSemiring<int64_t>({{10, 0, 0}, {0, 1, 0}, {1, 1, 0}}, sr)};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.size() == 86);
  REQUIRE(S.degree() == 3);
  REQUIRE(S.nridempotents() == 10);
  REQUIRE(S.nrgens() == 2);
  REQUIRE(S.nrrules() == 16);

  REQUIRE(*S[0] == *gens[0]);
  REQUIRE(S.position(gens[0]) == 0);
  REQUIRE(S.test_membership(gens[0]));

  Element* x = new MatrixOverSemiring<int64_t>({{-2, 2}, {-1, 0}}, sr);
  REQUIRE(S.position(x) == Semigroup<>::UNDEFINED);
  REQUIRE(!S.test_membership(x));
  x->really_delete();
  delete x;

  x = new MatrixOverSemiring<int64_t>({{-2, 2, 0}, {-1, 0, 0}, {0, 0, 0}}, sr);
  REQUIRE(S.position(x) == Semigroup<>::UNDEFINED);
  REQUIRE(!S.test_membership(x));
  x->redefine(gens[1], gens[0]);
  REQUIRE(S.position(x) == 4);
  REQUIRE(S.test_membership(x));
  x->really_delete();
  delete x;

  really_delete_cont(gens);
  delete sr;
}

TEST_CASE("Semigroup 12: small pbr semigroup",
          "[quick][semigroup][finite][12]") {
  std::vector<Element*> gens
      = {new PBR(new std::vector<std::vector<u_int32_t>>(
             {{1}, {4}, {3}, {1}, {0, 2}, {0, 3, 4, 5}})),
         new PBR(new std::vector<std::vector<u_int32_t>>(
             {{1, 2}, {0, 1}, {0, 2, 3}, {0, 1, 2}, {3}, {0, 3, 4, 5}}))};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.size() == 30);
  REQUIRE(S.degree() == 3);
  REQUIRE(S.nridempotents() == 22);
  REQUIRE(S.nrgens() == 2);
  REQUIRE(S.nrrules() == 11);

  REQUIRE(*S[0] == *gens[0]);
  REQUIRE(S.position(gens[0]) == 0);
  REQUIRE(S.test_membership(gens[0]));
  REQUIRE(*S[1] == *gens[1]);
  REQUIRE(S.position(gens[1]) == 1);
  REQUIRE(S.test_membership(gens[1]));

  Element* x = new PBR(
      new std::vector<std::vector<u_int32_t>>({{}, {}, {}, {}, {}, {}}));
  REQUIRE(S.position(x) == Semigroup<>::UNDEFINED);
  REQUIRE(!S.test_membership(x));
  x->redefine(gens[1], gens[1]);
  REQUIRE(S.position(x) == 5);
  REQUIRE(S.test_membership(x));
  x->really_delete();
  delete x;

  really_delete_cont(gens);
}

TEST_CASE("Semigroup 13: large transformation semigroup",
          "[quick][semigroup][finite][13]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  REQUIRE(S.size() == 7776);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.nridempotents() == 537);
  REQUIRE(S.nrgens() == 5);
  REQUIRE(S.nrrules() == 2459);
  really_delete_cont(gens);
}

TEST_CASE("Semigroup 14: at, position, current_*",
          "[quick][semigroup][finite][14]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  S.set_batch_size(1024);

  Element* expected = new Transformation<u_int16_t>({5, 3, 4, 1, 2, 5});
  REQUIRE(*S.at(100) == *expected);
  REQUIRE(S.current_size() == 1029);
  REQUIRE(S.current_nrrules() == 74);
  REQUIRE(S.current_max_word_length() == 7);
  expected->really_delete();
  delete expected;

  Element* x = new Transformation<u_int16_t>({5, 3, 4, 1, 2, 5});
  REQUIRE(S.position(x) == 100);
  x->really_delete();
  delete x;

  expected = new Transformation<u_int16_t>({5, 4, 3, 4, 1, 5});
  REQUIRE(*S.at(1023) == *expected);
  REQUIRE(S.current_size() == 1029);
  REQUIRE(S.current_nrrules() == 74);
  REQUIRE(S.current_max_word_length() == 7);
  expected->really_delete();
  delete expected;

  x = new Transformation<u_int16_t>({5, 4, 3, 4, 1, 5});
  REQUIRE(S.position(x) == 1023);
  x->really_delete();
  delete x;

  expected = new Transformation<u_int16_t>({5, 3, 5, 3, 4, 5});
  REQUIRE(*S.at(3000) == *expected);
  REQUIRE(S.current_size() == 3001);
  REQUIRE(S.current_nrrules() == 526);
  REQUIRE(S.current_max_word_length() == 9);
  expected->really_delete();
  delete expected;

  x = new Transformation<u_int16_t>({5, 3, 5, 3, 4, 5});
  REQUIRE(S.position(x) == 3000);
  x->really_delete();
  delete x;

  REQUIRE(S.size() == 7776);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.nridempotents() == 537);
  REQUIRE(S.nrgens() == 5);
  REQUIRE(S.nrrules() == 2459);

  really_delete_cont(gens);
}

TEST_CASE("Semigroup 15: enumerate", "[quick][semigroup][finite][15]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  S.set_batch_size(1024);

  S.enumerate(3000);
  REQUIRE(S.current_size() == 3000);
  REQUIRE(S.current_nrrules() == 526);
  REQUIRE(S.current_max_word_length() == 9);

  S.enumerate(3001);
  REQUIRE(S.current_size() == 4024);
  REQUIRE(S.current_nrrules() == 999);
  REQUIRE(S.current_max_word_length() == 10);

  S.enumerate(7000);
  REQUIRE(S.current_size() == 7000);
  REQUIRE(S.current_nrrules() == 2044);
  REQUIRE(S.current_max_word_length() == 12);

  REQUIRE(S.size() == 7776);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.nridempotents() == 537);
  REQUIRE(S.nrgens() == 5);
  REQUIRE(S.nrrules() == 2459);

  really_delete_cont(gens);
}

TEST_CASE("Semigroup 16: enumerate [many stops and starts]",
          "[quick][semigroup][finite][16]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  S.set_batch_size(128);

  for (size_t i = 1; !S.is_done(); i++) {
    S.enumerate(i * 128);
  }

  REQUIRE(S.size() == 7776);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.nridempotents() == 537);
  REQUIRE(S.nrgens() == 5);
  REQUIRE(S.nrrules() == 2459);

  really_delete_cont(gens);
}

TEST_CASE("Semigroup 17: factorisation, length [1 element]",
          "[quick][semigroup][finite][17]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  S.set_batch_size(1024);

  word_t result;
  S.factorisation(result, 5537);
  word_t expected = {1, 2, 2, 2, 3, 2, 4, 1, 2, 2, 3};
  REQUIRE(result == expected);
  REQUIRE(S.length_const(5537) == 11);
  REQUIRE(S.length_non_const(5537) == 11);
  REQUIRE(S.current_max_word_length() == 11);

  REQUIRE(S.current_size() == 5539);
  REQUIRE(S.current_nrrules() == 1484);
  REQUIRE(S.current_max_word_length() == 11);

  REQUIRE(S.length_non_const(7775) == 16);
  REQUIRE(S.current_max_word_length() == 16);

  really_delete_cont(gens);
}

TEST_CASE("Semigroup 18: factorisation, products [all elements]",
          "[quick][semigroup][finite][18]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  S.set_batch_size(1024);

  word_t result;
  for (size_t i = 0; i < S.size(); i++) {
    S.factorisation(result, i);
    REQUIRE(evaluate_reduct(S, result) == i);
  }

  really_delete_cont(gens);
}

TEST_CASE("Semigroup 19: first/final letter, prefix, suffix, products",
          "[quick][semigroup][finite][19]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  S.enumerate(1000);  // fully enumerates

  REQUIRE(S.first_letter(6377) == 2);
  REQUIRE(S.prefix(6377) == 5049);
  REQUIRE(S.final_letter(6377) == 2);
  REQUIRE(S.suffix(6377) == 5149);
  REQUIRE(S.fast_product(S.prefix(6377), S.final_letter(6377)) == 6377);
  REQUIRE(S.fast_product(S.first_letter(6377), S.suffix(6377)) == 6377);
  REQUIRE(S.product_by_reduction(S.prefix(6377), S.final_letter(6377)) == 6377);
  REQUIRE(S.product_by_reduction(S.first_letter(6377), S.suffix(6377)) == 6377);

  REQUIRE(S.first_letter(2103) == 3);
  REQUIRE(S.prefix(2103) == 1050);
  REQUIRE(S.final_letter(2103) == 1);
  REQUIRE(S.suffix(2103) == 860);
  REQUIRE(S.fast_product(S.prefix(2103), S.final_letter(2103)) == 2103);
  REQUIRE(S.fast_product(S.first_letter(2103), S.suffix(2103)) == 2103);
  REQUIRE(S.product_by_reduction(S.prefix(2103), S.final_letter(2103)) == 2103);
  REQUIRE(S.product_by_reduction(S.first_letter(2103), S.suffix(2103)) == 2103);

  REQUIRE(S.first_letter(3407) == 2);
  REQUIRE(S.prefix(3407) == 1923);
  REQUIRE(S.final_letter(3407) == 3);
  REQUIRE(S.suffix(3407) == 2115);
  REQUIRE(S.fast_product(S.prefix(3407), S.final_letter(3407)) == 3407);
  REQUIRE(S.fast_product(S.first_letter(3407), S.suffix(3407)) == 3407);
  REQUIRE(S.product_by_reduction(S.prefix(3407), S.final_letter(3407)) == 3407);
  REQUIRE(S.product_by_reduction(S.first_letter(3407), S.suffix(3407)) == 3407);

  REQUIRE(S.first_letter(4245) == 2);
  REQUIRE(S.prefix(4245) == 2767);
  REQUIRE(S.final_letter(4245) == 3);
  REQUIRE(S.suffix(4245) == 2319);
  REQUIRE(S.fast_product(S.prefix(4225), S.final_letter(4225)) == 4225);
  REQUIRE(S.fast_product(S.first_letter(4225), S.suffix(4225)) == 4225);
  REQUIRE(S.product_by_reduction(S.prefix(4225), S.final_letter(4225)) == 4225);
  REQUIRE(S.product_by_reduction(S.first_letter(4225), S.suffix(4225)) == 4225);

  REQUIRE(S.first_letter(3683) == 4);
  REQUIRE(S.prefix(3683) == 2246);
  REQUIRE(S.final_letter(3683) == 2);
  REQUIRE(S.suffix(3683) == 1685);
  REQUIRE(S.fast_product(S.prefix(3683), S.final_letter(3683)) == 3683);
  REQUIRE(S.fast_product(S.first_letter(3683), S.suffix(3683)) == 3683);
  REQUIRE(S.product_by_reduction(S.prefix(3683), S.final_letter(3683)) == 3683);
  REQUIRE(S.product_by_reduction(S.first_letter(3683), S.suffix(3683)) == 3683);

  REQUIRE(S.first_letter(0) == 0);
  REQUIRE(S.prefix(0) == Semigroup<>::UNDEFINED);
  REQUIRE(S.final_letter(0) == 0);
  REQUIRE(S.suffix(0) == Semigroup<>::UNDEFINED);

  REQUIRE(S.first_letter(7775) == 1);
  REQUIRE(S.prefix(7775) == 7760);
  REQUIRE(S.final_letter(7775) == 2);
  REQUIRE(S.suffix(7775) == 7768);
  REQUIRE(S.fast_product(S.prefix(7775), S.final_letter(7775)) == 7775);
  REQUIRE(S.fast_product(S.first_letter(7775), S.suffix(7775)) == 7775);
  REQUIRE(S.product_by_reduction(S.prefix(7775), S.final_letter(7775)) == 7775);
  REQUIRE(S.product_by_reduction(S.first_letter(7775), S.suffix(7775)) == 7775);

  really_delete_cont(gens);
}

TEST_CASE("Semigroup 20: letter_to_pos [standard]",
          "[quick][semigroup][finite][20]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.letter_to_pos(0) == 0);
  REQUIRE(S.letter_to_pos(1) == 1);
  REQUIRE(S.letter_to_pos(2) == 2);
  REQUIRE(S.letter_to_pos(3) == 3);
  REQUIRE(S.letter_to_pos(4) == 4);

  really_delete_cont(gens);
}

TEST_CASE("Semigroup 21: letter_to_pos [duplicate gens]",
          "[quick][semigroup][finite][21]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.letter_to_pos(0) == 0);
  REQUIRE(S.letter_to_pos(1) == 1);
  REQUIRE(S.letter_to_pos(2) == 1);
  REQUIRE(S.letter_to_pos(3) == 1);
  REQUIRE(S.letter_to_pos(4) == 1);
  REQUIRE(S.letter_to_pos(10) == 1);
  REQUIRE(S.letter_to_pos(12) == 3);

  REQUIRE(S.size() == 7776);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.nridempotents() == 537);
  REQUIRE(S.nrgens() == 32);
  REQUIRE(S.nrrules() == 2621);

  really_delete_cont(gens);
}

TEST_CASE("Semigroup 22: letter_to_pos [after add_generators]",
          "[quick][semigroup][finite][22]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>({gens[0]});
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.size() == 1);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.nridempotents() == 1);
  REQUIRE(S.nrgens() == 1);
  REQUIRE(S.nrrules() == 1);

  S.add_generators({gens[1]});
  REQUIRE(S.size() == 2);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.nridempotents() == 1);
  REQUIRE(S.nrgens() == 2);
  REQUIRE(S.nrrules() == 4);

  S.add_generators({gens[2]});
  REQUIRE(S.size() == 120);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.nridempotents() == 1);
  REQUIRE(S.nrgens() == 3);
  REQUIRE(S.nrrules() == 25);

  S.add_generators({gens[3]});
  REQUIRE(S.size() == 1546);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.nridempotents() == 32);
  REQUIRE(S.nrgens() == 4);
  REQUIRE(S.nrrules() == 495);

  S.add_generators({gens[4]});
  REQUIRE(S.size() == 7776);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.nridempotents() == 537);
  REQUIRE(S.nrgens() == 5);
  REQUIRE(S.nrrules() == 2459);

  REQUIRE(S.letter_to_pos(0) == 0);
  REQUIRE(S.letter_to_pos(1) == 1);
  REQUIRE(S.letter_to_pos(2) == 2);
  REQUIRE(S.letter_to_pos(3) == 120);
  REQUIRE(S.letter_to_pos(4) == 1546);

  really_delete_cont(gens);
}

TEST_CASE("Semigroup 23: cbegin_idempotents/cend [1 thread]",
          "[quick][semigroup][finite][23]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);

  size_t nr = 0;
  for (auto it = S.cbegin_idempotents(); it < S.cend_idempotents(); it++) {
    test_idempotent(S, *it);
    nr++;
  }
  REQUIRE(nr == S.nridempotents());
  really_delete_cont(gens);
}

TEST_CASE("Semigroup 24: idempotent_cend/cbegin [1 thread]",
          "[quick][semigroup][finite][24]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);

  size_t nr  = 0;
  auto   end = S.cend_idempotents();
  for (auto it = S.cbegin_idempotents(); it < end; it++) {
    test_idempotent(S, *it);
    nr++;
  }
  REQUIRE(nr == S.nridempotents());

  really_delete_cont(gens);
}

TEST_CASE("Semigroup 25: is_idempotent [1 thread]",
          "[quick][semigroup][finite][25]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  S.set_max_threads(1000);
  really_delete_cont(gens);

  size_t nr = 0;
  for (size_t i = 0; i < S.size(); i++) {
    if (S.is_idempotent(i)) {
      nr++;
    }
  }
  REQUIRE(nr == S.nridempotents());
}

TEST_CASE("Semigroup 26: cbegin_idempotents/cend, is_idempotent [2 threads]",
          "[standard][semigroup][finite][multithread][26]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({1, 2, 3, 4, 5, 6, 0}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5, 6}),
         new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5, 0})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  S.set_max_threads(2);

  size_t nr = 0;

  for (auto it = S.cbegin_idempotents(); it < S.cend_idempotents(); it++) {
    test_idempotent(S, *it);
    nr++;
  }
  REQUIRE(nr == S.nridempotents());
  REQUIRE(nr == 6322);

  nr = 0;
  for (auto it = S.cbegin_idempotents(); it < S.cend_idempotents(); it++) {
    test_idempotent(S, *it);
    nr++;
  }
  REQUIRE(nr == S.nridempotents());
  REQUIRE(nr == 6322);

  really_delete_cont(gens);
}

TEST_CASE("Semigroup 27: is_done, is_begun", "[quick][semigroup][finite][27]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(!S.is_begun());
  REQUIRE(!S.is_done());

  S.set_batch_size(1024);
  S.enumerate(10);
  REQUIRE(S.is_begun());
  REQUIRE(!S.is_done());

  S.enumerate(8000);
  REQUIRE(S.is_begun());
  REQUIRE(S.is_done());

  really_delete_cont(gens);
}

TEST_CASE("Semigroup 28: current_position", "[quick][semigroup][finite][28]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.current_position(gens[0]) == 0);
  REQUIRE(S.current_position(gens[1]) == 1);
  REQUIRE(S.current_position(gens[2]) == 2);
  REQUIRE(S.current_position(gens[3]) == 3);
  REQUIRE(S.current_position(gens[4]) == 4);

  S.set_batch_size(1024);
  S.enumerate(1024);

  REQUIRE(S.current_size() == 1029);
  REQUIRE(S.current_nrrules() == 74);
  REQUIRE(S.current_max_word_length() == 7);
  REQUIRE(S.current_position(S.at(1024)) == 1024);

  Element* x = new Transformation<u_int16_t>({5, 1, 5, 5, 2, 5});
  REQUIRE(S.current_position(x) == 1028);
  REQUIRE(S.current_size() == 1029);
  REQUIRE(S.current_nrrules() == 74);
  REQUIRE(S.current_max_word_length() == 7);
  x->really_delete();
  delete x;

  x = new Transformation<u_int16_t>({5, 1, 5, 5, 2, 5, 6});
  REQUIRE(S.current_position(x) == Semigroup<>::UNDEFINED);
  REQUIRE(S.current_size() == 1029);
  REQUIRE(S.current_nrrules() == 74);
  REQUIRE(S.current_max_word_length() == 7);
  x->really_delete();
  delete x;

  x = new Transformation<u_int16_t>({5, 4, 5, 1, 0, 5});
  REQUIRE(S.current_position(x) == Semigroup<>::UNDEFINED);
  REQUIRE(S.current_size() == 1029);
  REQUIRE(S.current_nrrules() == 74);
  REQUIRE(S.current_max_word_length() == 7);
  REQUIRE(S.position(x) == 1029);
  x->really_delete();
  delete x;

  really_delete_cont(gens);
}

TEST_CASE("Semigroup 29: sorted_position, sorted_at",
          "[quick][semigroup][finite][29]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.sorted_position(gens[0]) == 310);
  REQUIRE(*S.sorted_at(310) == *gens[0]);
  REQUIRE(S.sorted_at(310) == S.at(0));

  REQUIRE(S.sorted_position(gens[1]) == 1390);
  REQUIRE(*S.sorted_at(1390) == *gens[1]);
  REQUIRE(S.sorted_at(1390) == S.at(1));

  REQUIRE(S.sorted_position(gens[2]) == 5235);
  REQUIRE(*S.sorted_at(5235) == *gens[2]);
  REQUIRE(S.sorted_at(5235) == S.at(2));

  REQUIRE(S.sorted_position(gens[3]) == 6790);
  REQUIRE(*S.sorted_at(6790) == *gens[3]);
  REQUIRE(S.sorted_at(6790) == S.at(3));

  REQUIRE(S.sorted_position(gens[4]) == 1606);
  REQUIRE(*S.sorted_at(1606) == *gens[4]);
  REQUIRE(S.sorted_at(1606) == S.at(4));

  REQUIRE(S.is_done());

  REQUIRE(S.sorted_position(S.at(1024)) == 6810);
  REQUIRE(S.position_to_sorted_position(1024) == 6810);

  REQUIRE(*S.sorted_at(6810) == *S.at(1024));
  REQUIRE(S.sorted_at(6810) == S.at(1024));

  Element* x = new Transformation<u_int16_t>({5, 1, 5, 5, 2, 5});
  REQUIRE(S.sorted_position(x) == 6908);
  REQUIRE(S.position_to_sorted_position(S.position(x)) == 6908);
  REQUIRE(*S.sorted_at(6908) == *x);
  REQUIRE(S.sorted_at(6908) == S.at(S.position(x)));
  x->really_delete();
  delete x;

  x = new Transformation<u_int16_t>({5, 5, 5, 1, 5, 5, 6});
  REQUIRE(S.sorted_position(x) == Semigroup<>::UNDEFINED);
  x->really_delete();
  delete x;

  REQUIRE(S.sorted_at(100000) == nullptr);
  REQUIRE(S.at(100000) == nullptr);
  REQUIRE(S.position_to_sorted_position(100000) == Semigroup<>::UNDEFINED);

  really_delete_cont(gens);
}

TEST_CASE("Semigroup 30: right/left Cayley graph",
          "[quick][semigroup][finite][30]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.right(0, 0) == 0);
  REQUIRE(S.left(0, 0) == 0);

  Element* tmp = new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5});
  for (auto it = S.cbegin(); it < S.cend(); ++it) {
    for (size_t i = 0; i < 5; ++i) {
      tmp->redefine(*it, S.gens(i));
      REQUIRE(S.position(tmp) == S.right(S.position(*it), i));
      tmp->redefine(S.gens(i), *it);
      REQUIRE(S.position(tmp) == S.left(S.position(*it), i));
    }
  }
  tmp->really_delete();
  delete tmp;
  // TODO(JDM) more tests
  really_delete_cont(gens);
}

TEST_CASE("Semigroup 31: iterator", "[quick][semigroup][finite][31]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  really_delete_cont(gens);
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.current_size() == 5);
  size_t size = S.current_size();
  for (auto it = S.cbegin(); it < S.cend(); it++) {
    size--;
    REQUIRE(S.test_membership(*it));
  }
  REQUIRE(size == 0);

  for (auto it = S.cbegin(); it < S.cend(); ++it) {
    size++;
    REQUIRE(S.test_membership(*it));
  }
  REQUIRE(size == S.current_size());
  REQUIRE(5 == S.current_size());

  S.set_batch_size(1024);
  S.enumerate(1000);
  REQUIRE(S.current_size() < 7776);

  size = S.current_size();
  for (auto it = S.cbegin(); it < S.cend(); it++) {
    size--;
    REQUIRE(S.test_membership(*it));
  }
  REQUIRE(size == 0);

  for (auto it = S.cbegin(); it < S.cend(); ++it) {
    size++;
    REQUIRE(S.test_membership(*it));
  }
  REQUIRE(size == S.current_size());
  REQUIRE(S.current_size() < 7776);

  REQUIRE(S.size() == 7776);
  size = S.size();
  for (auto it = S.cbegin(); it < S.cend(); it++) {
    size--;
    REQUIRE(S.test_membership(*it));
  }
  REQUIRE(size == 0);

  for (auto it = S.cbegin(); it < S.cend(); ++it) {
    size++;
    REQUIRE(S.test_membership(*it));
  }
  REQUIRE(size == S.size());
}

TEST_CASE("Semigroup 66: reverse iterator", "[quick][semigroup][finite][66]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  really_delete_cont(gens);
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.current_size() == 5);
  size_t size = S.current_size();
  for (auto it = S.crbegin(); it < S.crend(); it++) {
    size--;
    REQUIRE(S.test_membership(*it));
  }
  REQUIRE(size == 0);

  for (auto it = S.crbegin(); it < S.crend(); ++it) {
    size++;
    REQUIRE(S.test_membership(*it));
  }
  REQUIRE(size == S.current_size());
  REQUIRE(5 == S.current_size());

  S.set_batch_size(1024);
  S.enumerate(1000);
  REQUIRE(S.current_size() < 7776);

  size = S.current_size();
  for (auto it = S.crbegin(); it < S.crend(); it++) {
    size--;
    REQUIRE(S.test_membership(*it));
  }
  REQUIRE(size == 0);

  for (auto it = S.crbegin(); it < S.crend(); ++it) {
    size++;
    REQUIRE(S.test_membership(*it));
  }
  REQUIRE(size == S.current_size());
  REQUIRE(S.current_size() < 7776);

  REQUIRE(S.size() == 7776);
  size = S.size();
  for (auto it = S.crbegin(); it < S.crend(); it++) {
    size--;
    REQUIRE(S.test_membership(*it));
  }
  REQUIRE(size == 0);

  for (auto it = S.crbegin(); it < S.crend(); ++it) {
    size++;
    REQUIRE(S.test_membership(*it));
  }
  REQUIRE(size == S.size());
}

TEST_CASE("Semigroup 67: iterator arithmetic",
          "[quick][semigroup][finite][67]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  really_delete_cont(gens);
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.size() == 7776);
  auto it = S.cbegin();
  // The next line should not compile (and does not).
  // (*it)->really_delete();

  for (int64_t i = 0; i < static_cast<int64_t>(S.size()); i++) {
    // *it = reinterpret_cast<Element*>(0);
    // This does not and should not compile
    REQUIRE(*(it + i) == S.at(i));
    it += i;
    REQUIRE(*it == S.at(i));
    it -= i;
    REQUIRE(*it == S.at(0));
    REQUIRE(it == S.cbegin());
    auto tmp(it);
    REQUIRE((tmp + i) - i == tmp);
    REQUIRE((i + tmp) - i == tmp);
    tmp += i;
    REQUIRE(tmp - it == i);
    REQUIRE(it - tmp == -i);
    tmp -= i;
    REQUIRE(tmp - it == 0);
    tmp -= i;
    REQUIRE(tmp - it == -i);
    REQUIRE(it - tmp == i);
  }
  for (int64_t i = S.size(); i < static_cast<int64_t>(2 * S.size()); i++) {
    it += i;
    it -= i;
    REQUIRE(*it == S.at(0));
    REQUIRE(it == S.cbegin());
    auto tmp(it);
    REQUIRE((tmp + i) - i == tmp);
    REQUIRE((i + tmp) - i == tmp);
    tmp += i;
    REQUIRE(tmp - it == i);
    REQUIRE(it - tmp == -i);
    tmp -= i;
    REQUIRE(tmp - it == 0);
    tmp -= i;
    REQUIRE(tmp - it == -i);
    REQUIRE(it - tmp == i);
  }
}

TEST_CASE("Semigroup 68: iterator sorted", "[quick][semigroup][finite][68]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  really_delete_cont(gens);
  S.set_report(SEMIGROUPS_REPORT);

  // Calling cbegin/cend_sorted fully enumerates the semigroup
  { auto it = S.cbegin_sorted(); }
  REQUIRE(S.is_done());

  size_t pos = 0;
  for (auto it = S.cbegin_sorted(); it < S.cend_sorted(); it++) {
    REQUIRE(S.sorted_position(*it) == pos);
    REQUIRE(S.position_to_sorted_position(S.position(*it)) == pos);
    pos++;
  }
  REQUIRE(pos == S.size());

  pos = 0;
  for (auto it = S.cbegin_sorted(); it < S.cend_sorted(); ++it) {
    REQUIRE(S.sorted_position(*it) == pos);
    REQUIRE(S.position_to_sorted_position(S.position(*it)) == pos);
    pos++;
  }
  REQUIRE(pos == S.size());

  pos = S.size();
  for (auto it = S.crbegin_sorted(); it < S.crend_sorted(); it++) {
    pos--;
    REQUIRE(S.sorted_position(*it) == pos);
    REQUIRE(S.position_to_sorted_position(S.position(*it)) == pos);
  }
  REQUIRE(pos == 0);

  pos = S.size();
  for (auto it = S.crbegin_sorted(); it < S.crend_sorted(); ++it) {
    pos--;
    REQUIRE(S.sorted_position(*it) == pos);
    REQUIRE(S.position_to_sorted_position(S.position(*it)) == pos);
  }
  REQUIRE(pos == 0);
}

TEST_CASE("Semigroup 69: iterator sorted arithmetic",
          "[quick][semigroup][finite][69]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  really_delete_cont(gens);
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.size() == 7776);
  auto it = S.cbegin_sorted();
  // The next line should not compile (and does not).
  // (*it)->really_delete();

  for (int64_t i = 0; i < static_cast<int64_t>(S.size()); i++) {
    // The next line does not and should not compile
    // *it = reinterpret_cast<Element*>(0);
    REQUIRE(*(it + i) == S.sorted_at(i));
    it += i;
    REQUIRE(*it == S.sorted_at(i));
    it -= i;
    REQUIRE(*it == S.sorted_at(0));
    REQUIRE(it == S.cbegin_sorted());
    auto tmp(it);
    REQUIRE((tmp + i) - i == tmp);
    REQUIRE((i + tmp) - i == tmp);
    tmp += i;
    REQUIRE(tmp - it == i);
    REQUIRE(it - tmp == -i);
    tmp -= i;
    REQUIRE(tmp - it == 0);
    tmp -= i;
    REQUIRE(tmp - it == -i);
    REQUIRE(it - tmp == i);
  }
  for (int64_t i = S.size(); i < static_cast<int64_t>(2 * S.size()); i++) {
    it += i;
    it -= i;
    REQUIRE(*it == S.sorted_at(0));
    REQUIRE(it == S.cbegin_sorted());
    auto tmp(it);
    REQUIRE((tmp + i) - i == tmp);
    REQUIRE((i + tmp) - i == tmp);
    tmp += i;
    REQUIRE(tmp - it == i);
    REQUIRE(it - tmp == -i);
    tmp -= i;
    REQUIRE(tmp - it == 0);
    tmp -= i;
    REQUIRE(tmp - it == -i);
    REQUIRE(it - tmp == i);
  }
}

TEST_CASE("Semigroup 32: copy [not enumerated]",
          "[quick][semigroup][finite][32]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(!S.is_begun());
  REQUIRE(!S.is_done());
  REQUIRE(S.current_size() == 5);
  REQUIRE(S.current_nrrules() == 0);
  REQUIRE(S.current_max_word_length() == 1);
  REQUIRE(S.current_position(gens[1]) == 1);

  really_delete_cont(gens);

  Semigroup<> T = Semigroup<>(S);
  T.set_report(SEMIGROUPS_REPORT);

  REQUIRE(!T.is_begun());
  REQUIRE(!T.is_done());
  REQUIRE(T.nrgens() == 5);
  REQUIRE(T.degree() == 6);
  REQUIRE(T.current_size() == 5);
  REQUIRE(T.current_nrrules() == 0);
  REQUIRE(T.current_max_word_length() == 1);
  REQUIRE(T.current_position(S.gens(1)) == 1);

  REQUIRE(T.size() == 7776);
  REQUIRE(T.nridempotents() == 537);
  REQUIRE(T.nrrules() == 2459);
  REQUIRE(T.is_begun());
  REQUIRE(T.is_done());
}

TEST_CASE("Semigroup 33: copy_closure [not enumerated]",
          "[quick][semigroup][finite][33]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);

  REQUIRE(!S.is_begun());
  REQUIRE(!S.is_done());
  REQUIRE(S.nrgens() == 2);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.current_size() == 2);
  REQUIRE(S.current_nrrules() == 0);
  REQUIRE(S.current_max_word_length() == 1);

  std::vector<Element*> coll
      = {new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup<>* T = S.copy_closure(&coll);
  T->set_report(SEMIGROUPS_REPORT);
  really_delete_cont(coll);

  REQUIRE(T->is_begun());
  REQUIRE(!T->is_done());
  REQUIRE(T->nrgens() == 5);
  REQUIRE(T->degree() == 6);
  REQUIRE(T->current_size() == 7719);
  REQUIRE(T->current_nrrules() == 2418);
  REQUIRE(T->current_max_word_length() == 14);
  REQUIRE(T->current_position(S.gens(1)) == 1);

  REQUIRE(T->size() == 7776);
  REQUIRE(T->is_done());
  REQUIRE(T->nridempotents() == 537);
  REQUIRE(T->nrrules() == 2459);

  coll           = {new Transformation<u_int16_t>({6, 0, 1, 2, 3, 5, 6})};
  Semigroup<>* U = T->copy_closure(&coll);
  U->set_report(SEMIGROUPS_REPORT);
  really_delete_cont(coll);

  REQUIRE(U->is_begun());
  REQUIRE(U->is_done());
  REQUIRE(U->nrgens() == 6);
  REQUIRE(U->degree() == 7);
  REQUIRE(U->current_size() == 16807);
  REQUIRE(U->current_max_word_length() == 16);
  REQUIRE(U->nridempotents() == 1358);
  REQUIRE(U->nrrules() == 7901);

  coll           = std::vector<Element*>();
  Semigroup<>* V = U->copy_closure(&coll);
  V->set_report(SEMIGROUPS_REPORT);
  REQUIRE(V != U);
  REQUIRE(V->is_begun());
  REQUIRE(V->is_done());
  REQUIRE(V->nrgens() == 6);
  REQUIRE(V->degree() == 7);
  REQUIRE(V->current_size() == 16807);
  REQUIRE(V->current_max_word_length() == 16);
  REQUIRE(V->nridempotents() == 1358);
  REQUIRE(V->nrrules() == 7901);

  delete T;
  delete U;
  delete V;
}

TEST_CASE("Semigroup 34: copy_add_generators [not enumerated]",
          "[quick][semigroup][finite][34]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);

  REQUIRE(!S.is_begun());
  REQUIRE(!S.is_done());
  REQUIRE(S.nrgens() == 2);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.current_size() == 2);
  REQUIRE(S.current_nrrules() == 0);
  REQUIRE(S.current_max_word_length() == 1);

  std::vector<Element*> coll
      = {new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup<>* T = S.copy_add_generators(&coll);
  T->set_report(SEMIGROUPS_REPORT);
  really_delete_cont(coll);

  REQUIRE(!T->is_begun());
  REQUIRE(!T->is_done());
  REQUIRE(T->nrgens() == 5);
  REQUIRE(T->degree() == 6);
  REQUIRE(T->current_size() == 5);
  REQUIRE(T->current_nrrules() == 0);
  REQUIRE(T->current_max_word_length() == 1);
  REQUIRE(T->current_position(S.gens(1)) == 1);

  REQUIRE(T->size() == 7776);
  REQUIRE(T->is_done());
  REQUIRE(T->nridempotents() == 537);
  REQUIRE(T->nrrules() == 2459);

  coll           = {new Transformation<u_int16_t>({6, 0, 1, 2, 3, 5, 6})};
  Semigroup<>* U = T->copy_add_generators(&coll);
  U->set_report(SEMIGROUPS_REPORT);
  really_delete_cont(coll);

  REQUIRE(U->is_begun());
  REQUIRE(U->is_done());
  REQUIRE(U->nrgens() == 6);
  REQUIRE(U->degree() == 7);
  REQUIRE(U->current_size() == 16807);
  REQUIRE(U->current_max_word_length() == 16);
  REQUIRE(U->nridempotents() == 1358);
  REQUIRE(U->nrrules() == 7901);

  coll           = std::vector<Element*>();
  Semigroup<>* V = U->copy_add_generators(&coll);
  V->set_report(SEMIGROUPS_REPORT);
  REQUIRE(V != U);
  REQUIRE(V->is_begun());
  REQUIRE(V->is_done());
  REQUIRE(V->nrgens() == 6);
  REQUIRE(V->degree() == 7);
  REQUIRE(V->current_size() == 16807);
  REQUIRE(V->current_max_word_length() == 16);
  REQUIRE(V->nridempotents() == 1358);
  REQUIRE(V->nrrules() == 7901);

  delete T;
  delete U;
  delete V;
}

TEST_CASE("Semigroup 35: copy [partly enumerated]",
          "[quick][semigroup][finite][35]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  S.set_batch_size(1000);
  S.enumerate(1001);

  REQUIRE(S.is_begun());
  REQUIRE(!S.is_done());
  REQUIRE(S.current_size() == 1006);
  REQUIRE(S.current_nrrules() == 70);
  REQUIRE(S.current_max_word_length() == 7);
  REQUIRE(S.current_position(gens[1]) == 1);
  really_delete_cont(gens);

  Semigroup<> T = Semigroup<>(S);
  T.set_report(SEMIGROUPS_REPORT);

  REQUIRE(T.is_begun());
  REQUIRE(!T.is_done());
  REQUIRE(T.nrgens() == 5);
  REQUIRE(T.degree() == 6);
  REQUIRE(T.current_size() == 1006);
  REQUIRE(T.current_nrrules() == 70);
  REQUIRE(T.current_max_word_length() == 7);

  Element* x = new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5});
  REQUIRE(T.current_position(x) == 0);
  x->really_delete();
  delete x;

  x = new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5});
  REQUIRE(T.current_position(x) == 1);
  x->really_delete();
  delete x;

  REQUIRE(T.size() == 7776);
  REQUIRE(T.nridempotents() == 537);
  REQUIRE(T.nrrules() == 2459);
  REQUIRE(T.is_begun());
  REQUIRE(T.is_done());
}

TEST_CASE("Semigroup 36: copy_closure [partly enumerated]",
          "[quick][semigroup][finite][36]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);
  S.set_batch_size(60);
  S.enumerate(60);

  REQUIRE(S.is_begun());
  REQUIRE(!S.is_done());
  REQUIRE(S.nrgens() == 3);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.current_size() == 63);
  REQUIRE(S.current_nrrules() == 11);
  REQUIRE(S.current_max_word_length() == 7);

  std::vector<Element*> coll
      = {new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup<>* T = S.copy_closure(&coll);
  T->set_report(SEMIGROUPS_REPORT);
  REQUIRE(*coll[0] == *(T->gens(3)));
  REQUIRE(*coll[1] == *(T->gens(4)));
  really_delete_cont(coll);

  REQUIRE(T->is_begun());
  REQUIRE(!T->is_done());
  REQUIRE(T->nrgens() == 5);
  REQUIRE(T->degree() == 6);
  REQUIRE(T->current_size() == 7719);
  REQUIRE(T->current_nrrules() == 2418);
  REQUIRE(T->current_max_word_length() == 14);

  REQUIRE(T->size() == 7776);
  REQUIRE(T->is_done());
  REQUIRE(T->nridempotents() == 537);
  REQUIRE(T->nrrules() == 2459);
  delete T;
}

TEST_CASE("Semigroup 37: copy_add_generators [partly enumerated]",
          "[quick][semigroup][finite][37]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);
  S.set_batch_size(60);
  S.enumerate(60);

  REQUIRE(S.is_begun());
  REQUIRE(!S.is_done());
  REQUIRE(S.nrgens() == 3);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.current_size() == 63);
  REQUIRE(S.current_nrrules() == 11);
  REQUIRE(S.current_max_word_length() == 7);

  std::vector<Element*> coll
      = {new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup<>* T = S.copy_add_generators(&coll);
  T->set_report(SEMIGROUPS_REPORT);
  REQUIRE(*coll[0] == *(T->gens(3)));
  REQUIRE(*coll[1] == *(T->gens(4)));
  really_delete_cont(coll);

  REQUIRE(T->is_begun());
  REQUIRE(!T->is_done());
  REQUIRE(T->nrgens() == 5);
  REQUIRE(T->degree() == 6);
  REQUIRE(T->current_size() == 818);
  REQUIRE(T->current_nrrules() == 55);
  REQUIRE(T->current_max_word_length() == 7);

  REQUIRE(T->size() == 7776);
  REQUIRE(T->is_done());
  REQUIRE(T->nridempotents() == 537);
  REQUIRE(T->nrrules() == 2459);
  delete T;
}

TEST_CASE("Semigroup 38: copy [fully enumerated]",
          "[quick][semigroup][finite][38]") {
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

  REQUIRE(S.is_begun());
  REQUIRE(S.is_done());
  REQUIRE(S.size() == 7776);
  REQUIRE(S.nridempotents() == 537);
  REQUIRE(S.nrrules() == 2459);

  Semigroup<> T = Semigroup<>(S);
  T.set_report(SEMIGROUPS_REPORT);

  REQUIRE(T.is_begun());
  REQUIRE(T.is_done());
  REQUIRE(T.nrgens() == 5);
  REQUIRE(T.degree() == 6);
  REQUIRE(T.size() == 7776);
  REQUIRE(T.nridempotents() == 537);
  REQUIRE(T.nrrules() == 2459);
}

TEST_CASE("Semigroup 39: copy_closure [fully enumerated]",
          "[quick][semigroup][finite][39]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);
  S.enumerate(121);

  REQUIRE(S.is_begun());
  REQUIRE(S.is_done());
  REQUIRE(S.nrgens() == 3);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.current_size() == 120);
  REQUIRE(S.current_nrrules() == 25);
  REQUIRE(S.current_max_word_length() == 11);

  std::vector<Element*> coll
      = {new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup<>* T = S.copy_closure(&coll);
  T->set_report(SEMIGROUPS_REPORT);
  REQUIRE(*coll[0] == *(T->gens(3)));
  REQUIRE(*coll[1] == *(T->gens(4)));
  really_delete_cont(coll);

  REQUIRE(T->is_begun());
  REQUIRE(!T->is_done());
  REQUIRE(T->nrgens() == 5);
  REQUIRE(T->degree() == 6);
  REQUIRE(T->current_size() == 7719);
  REQUIRE(T->current_nrrules() == 2418);
  REQUIRE(T->current_max_word_length() == 14);

  REQUIRE(T->size() == 7776);
  REQUIRE(T->is_done());
  REQUIRE(T->nridempotents() == 537);
  REQUIRE(T->nrrules() == 2459);
  delete T;
}

TEST_CASE("Semigroup 40: copy_add_generators [fully enumerated]",
          "[quick][semigroup][finite][40]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);
  S.enumerate(121);

  REQUIRE(S.is_begun());
  REQUIRE(S.is_done());
  REQUIRE(S.nrgens() == 3);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.current_size() == 120);
  REQUIRE(S.current_nrrules() == 25);
  REQUIRE(S.current_max_word_length() == 11);

  std::vector<Element*> coll
      = {new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup<>* T = S.copy_add_generators(&coll);
  T->set_report(SEMIGROUPS_REPORT);
  REQUIRE(*coll[0] == *(T->gens(3)));
  REQUIRE(*coll[1] == *(T->gens(4)));
  really_delete_cont(coll);

  REQUIRE(T->is_begun());
  REQUIRE(!T->is_done());
  REQUIRE(T->nrgens() == 5);
  REQUIRE(T->degree() == 6);
  REQUIRE(T->current_size() == 6842);
  REQUIRE(T->current_nrrules() == 1970);
  REQUIRE(T->current_max_word_length() == 12);

  REQUIRE(T->size() == 7776);
  REQUIRE(T->is_done());
  REQUIRE(T->nridempotents() == 537);
  REQUIRE(T->nrrules() == 2459);
  delete T;
}

TEST_CASE("Semigroup 41: relations [duplicate gens]",
          "[quick][semigroup][finite][41]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);

  std::vector<size_t> result;
  S.next_relation(result);
  REQUIRE(result.size() == 2);
  REQUIRE(result[0] == 1);
  REQUIRE(result[1] == 0);

  S.next_relation(result);
  REQUIRE(result.size() == 2);
  REQUIRE(result[0] == 3);
  REQUIRE(result[1] == 2);

  S.next_relation(result);
  size_t nr = 2;
  while (!result.empty()) {
    S.next_relation(result);
    nr++;
  }
  REQUIRE(S.nrrules() == nr);

  S.next_relation(result);
  REQUIRE(result.empty());
}

TEST_CASE("Semigroup 42: relations", "[quick][semigroup][finite][42]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);

  std::vector<size_t> result;
  S.next_relation(result);
  size_t nr = 0;
  while (!result.empty()) {
    word_t lhs, rhs;
    S.factorisation(lhs, result[0]);
    lhs.push_back(result[1]);
    S.factorisation(rhs, result[2]);

    REQUIRE(evaluate_reduct(S, lhs) == evaluate_reduct(S, rhs));
    S.next_relation(result);
    nr++;
  }
  REQUIRE(S.nrrules() == nr);

  S.reset_next_relation();
  S.next_relation(result);
  nr = 0;

  while (!result.empty()) {
    word_t lhs, rhs;
    S.factorisation(lhs, result[0]);
    lhs.push_back(result[1]);
    S.factorisation(rhs, result[2]);

    REQUIRE(evaluate_reduct(S, lhs) == evaluate_reduct(S, rhs));
    REQUIRE(evaluate_reduct(S, lhs) == evaluate_reduct(S, rhs));
    S.next_relation(result);
    nr++;
  }

  REQUIRE(S.nrrules() == nr);
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

TEST_CASE("Semigroup 72: regular boolean mat monoid 4 using BMat8",
          "[quick][semigroup][finite][72]") {
  std::vector<BMat8> gens
      = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};

  Semigroup<BMat8> S(gens);
  S.set_report(SEMIGROUPS_REPORT);
  REQUIRE(S.size() == 63904);
  REQUIRE(S.nridempotents() == 2360);
}

TEST_CASE("Semigroup 73: regular boolean mat monoid 4 using BooleanMat",
          "[quick][semigroup][finite][73]") {
  std::vector<Element*> gens = {
      new BooleanMat({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
      new BooleanMat({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
      new BooleanMat({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
      new BooleanMat({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};
  Semigroup<> S(gens);
  really_delete_cont(gens);
  S.set_report(SEMIGROUPS_REPORT);
  REQUIRE(S.size() == 63904);
  REQUIRE(S.nridempotents() == 2360);
}

TEST_CASE("Semigroup 74: regular boolean mat monoid 5 using BMat8",
          "[extreme][semigroup][finite][74]") {
  Semigroup<BMat8> S(
      {BMat8({{0, 1, 0, 0, 0},
              {1, 0, 0, 0, 0},
              {0, 0, 1, 0, 0},
              {0, 0, 0, 1, 0},
              {0, 0, 0, 0, 1}}),
       BMat8({{0, 1, 0, 0, 0},
              {0, 0, 1, 0, 0},
              {0, 0, 0, 1, 0},
              {0, 0, 0, 0, 1},
              {1, 0, 0, 0, 0}}),
       BMat8({{1, 0, 0, 0, 0},
              {0, 1, 0, 0, 0},
              {0, 0, 1, 0, 0},
              {0, 0, 0, 1, 0},
              {1, 0, 0, 0, 1}}),
       BMat8({{1, 0, 0, 0, 0},
              {0, 1, 0, 0, 0},
              {0, 0, 1, 0, 0},
              {0, 0, 0, 1, 0},
              {0, 0, 0, 0, 0}})});
  S.set_report(true);
  REQUIRE(S.size() == 32311832);
  REQUIRE(S.nridempotents() == 73023);
  // FIXME this demonstrates a very bad split with almost all the
  // elements being put in the final thread by init_idempotents
  S.set_report(false);
}

TEST_CASE("Semigroup 75: regular boolean mat monoid 5 using BooleanMat",
          "[extreme][semigroup][finite][75]") {
  std::vector<Element*> gens = {new BooleanMat({{0, 1, 0, 0, 0},
                                                {1, 0, 0, 0, 0},
                                                {0, 0, 1, 0, 0},
                                                {0, 0, 0, 1, 0},
                                                {0, 0, 0, 0, 1}}),
                                new BooleanMat({{0, 1, 0, 0, 0},
                                                {0, 0, 1, 0, 0},
                                                {0, 0, 0, 1, 0},
                                                {0, 0, 0, 0, 1},
                                                {1, 0, 0, 0, 0}}),
                                new BooleanMat({{1, 0, 0, 0, 0},
                                                {0, 1, 0, 0, 0},
                                                {0, 0, 1, 0, 0},
                                                {0, 0, 0, 1, 0},
                                                {1, 0, 0, 0, 1}}),
                                new BooleanMat({{1, 0, 0, 0, 0},
                                                {0, 1, 0, 0, 0},
                                                {0, 0, 1, 0, 0},
                                                {0, 0, 0, 1, 0},
                                                {0, 0, 0, 0, 0}})};
  Semigroup<> S(gens);
  really_delete_cont(gens);
  S.set_report(true);
  REQUIRE(S.size() == 32311832);
  REQUIRE(S.nridempotents() == 73023);
  // FIXME this demonstrates a very bad split with almost all the
  // elements being put in the final thread by init_idempotents
  S.set_report(false);
}
