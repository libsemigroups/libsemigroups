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
