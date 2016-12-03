//
// Semigroups++ - C/C++ library for computing with semigroups and monoids
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

#include "../semigroups.h"
#include "catch.hpp"

using namespace semigroupsplusplus;

template <typename T> static inline void really_delete_cont(T cont) {
  for (Element* x : cont) {
    x->really_delete();
    delete x;
  }
}

static inline size_t evaluate_reduct(Semigroup& S, word_t const& word) {
  letter_t out = S.genslookup(word[0]);
  for (auto it = word.cbegin() + 1; it < word.cend(); ++it) {
    out = S.right_cayley_graph()->get(out, *it);
  }
  return out;
}

TEST_CASE("Semigroup: small transformation semigroup", "[small]") {
  std::vector<Element*> gens = {new Transformation<u_int16_t>({0, 1, 0}),
                                new Transformation<u_int16_t>({0, 1, 2})};
  Semigroup S = Semigroup(gens);
  really_delete_cont(gens);

  REQUIRE(S.size(false) == 2);
  REQUIRE(S.degree() == 3);
  REQUIRE(S.nr_idempotents(false) == 2);
  REQUIRE(S.nrgens() == 2);
  REQUIRE(S.nrrules(false) == 4);

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
  REQUIRE(S.position(x) == Semigroup::UNDEFINED);
  REQUIRE(!S.test_membership(x));
  x->really_delete();
  delete x;
}

TEST_CASE("Semigroup: small partial perm semigroup", "[small]") {
  std::vector<Element*> gens = {
      new PartialPerm<u_int16_t>(
          {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 10),
      new PartialPerm<u_int16_t>({4, 5, 0}, {10, 0, 1}, 10)};
  Semigroup S = Semigroup(gens);
  really_delete_cont(gens);

  REQUIRE(S.size(false) == 22);
  REQUIRE(S.degree() == 11);
  REQUIRE(S.nr_idempotents(false) == 1);
  REQUIRE(S.nrgens() == 2);
  REQUIRE(S.nrrules(false) == 9);

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
  REQUIRE(S.position(x) == Semigroup::UNDEFINED);
  REQUIRE(!S.test_membership(x));
  x->really_delete();
  delete x;

  x = new PartialPerm<u_int16_t>({}, {}, 10);
  REQUIRE(S.position(x) == 10);
  REQUIRE(S.test_membership(x));
  x->really_delete();
  delete x;

  x = new PartialPerm<u_int16_t>({}, {}, 9);
  REQUIRE(S.position(x) == Semigroup::UNDEFINED);
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

TEST_CASE("Semigroup: small bipartition semigroup", "[small]") {
  std::vector<Element*> gens = {
      new Bipartition(
          {0, 1, 2, 1, 0, 2, 1, 0, 2, 2, 0, 0, 2, 0, 3, 4, 4, 1, 3, 0}),
      new Bipartition(
          {0, 1, 1, 1, 1, 2, 3, 2, 4, 5, 5, 2, 4, 2, 1, 1, 1, 2, 3, 2}),
      new Bipartition(
          {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0})};
  Semigroup S = Semigroup(gens);
  REQUIRE(S.size(false) == 10);
  REQUIRE(S.degree() == 10);
  REQUIRE(S.nr_idempotents(false) == 6);
  REQUIRE(S.nrgens() == 3);
  REQUIRE(S.nrrules(false) == 14);

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

TEST_CASE("Semigroup: small Boolean matrix semigroup", "[small]") {
  std::vector<Element*> gens = {
      new BooleanMat({{1, 0, 1}, {0, 1, 0}, {0, 1, 0}}),
      new BooleanMat({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}),
      new BooleanMat({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}})};
  Semigroup S = Semigroup(gens);
  REQUIRE(S.size(false) == 3);
  REQUIRE(S.degree() == 3);
  REQUIRE(S.nr_idempotents(false) == 2);
  REQUIRE(S.nrgens() == 3);
  REQUIRE(S.nrrules(false) == 7);
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

TEST_CASE("Semigroup: small projective max plus matrix semigroup", "[small]") {
  Semiring* sr = new MaxPlusSemiring();
  auto x  = new ProjectiveMaxPlusMatrix({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, sr);
  auto id = x->identity();
  x->really_delete();
  delete x;
  Semigroup S = Semigroup({id});

  REQUIRE(S.size(false) == 1);
  REQUIRE(S.degree() == 3);
  REQUIRE(S.nr_idempotents(false) == 1);
  REQUIRE(S.nrgens() == 1);
  REQUIRE(S.nrrules(false) == 1);
  REQUIRE(*S[0] == *id);

  REQUIRE(S.position(id) == 0);
  REQUIRE(S.test_membership(id));
  id->really_delete();
  delete id;

  x = new ProjectiveMaxPlusMatrix({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  REQUIRE(S.position(x) == Semigroup::UNDEFINED);
  REQUIRE(!S.test_membership(x));
  x->really_delete();
  delete x;
  delete sr;
}

TEST_CASE("Semigroup: small matrix semigroup [Integers]", "[small]") {
  Semiring*             sr   = new Integers();
  std::vector<Element*> gens = {new MatrixOverSemiring({{0, 0}, {0, 1}}, sr),
                                new MatrixOverSemiring({{0, 1}, {-1, 0}}, sr)};
  Semigroup S = Semigroup(gens);

  REQUIRE(S.size(false) == 13);
  REQUIRE(S.degree() == 2);
  REQUIRE(S.nr_idempotents(false) == 4);
  REQUIRE(S.nrgens() == 2);
  REQUIRE(S.nrrules(false) == 6);
  REQUIRE(*S[0] == *(gens)[0]);
  REQUIRE(*S[1] == *(gens)[1]);

  REQUIRE(S.position(gens[0]) == 0);
  REQUIRE(S.test_membership(gens[0]));

  REQUIRE(S.position(gens[1]) == 1);
  REQUIRE(S.test_membership(gens[1]));

  Element* x = new MatrixOverSemiring({{-2, 2}, {-1, 0}}, sr);
  REQUIRE(S.position(x) == Semigroup::UNDEFINED);
  REQUIRE(!S.test_membership(x));

  x->redefine(gens[1], gens[1]);
  REQUIRE(S.position(x) == 4);
  REQUIRE(S.test_membership(x));
  x->really_delete();
  delete x;

  x = new MatrixOverSemiring({{-2, 2, 0}, {-1, 0, 0}, {0, 0, 0}}, sr);
  REQUIRE(S.position(x) == Semigroup::UNDEFINED);
  REQUIRE(!S.test_membership(x));
  x->really_delete();
  delete x;

  really_delete_cont(gens);
  delete sr;
}

TEST_CASE("Semigroup: small matrix semigroup [MaxPlusSemiring]", "[small]") {
  Semiring*             sr   = new MaxPlusSemiring();
  std::vector<Element*> gens = {
      new MatrixOverSemiring({{0, -4}, {-4, -1}}, sr),
      new MatrixOverSemiring({{0, -3}, {-3, -1}}, sr)};
  Semigroup S = Semigroup(gens);

  REQUIRE(S.size(false) == 26);
  REQUIRE(S.degree() == 2);
  REQUIRE(S.nr_idempotents(false) == 4);
  REQUIRE(S.nrgens() == 2);
  REQUIRE(S.nrrules(false) == 9);
  REQUIRE(*S[0] == *gens[0]);
  REQUIRE(*S[1] == *gens[1]);

  REQUIRE(S.position(gens[0]) == 0);
  REQUIRE(S.test_membership(gens[0]));

  REQUIRE(S.position(gens[1]) == 1);
  REQUIRE(S.test_membership(gens[1]));

  Element* x = new MatrixOverSemiring({{-2, 2}, {-1, 0}}, sr);
  REQUIRE(S.position(x) == Semigroup::UNDEFINED);
  REQUIRE(!S.test_membership(x));
  x->redefine(gens[1], gens[1]);
  REQUIRE(S.position(x) == 5);
  REQUIRE(S.test_membership(x));
  x->really_delete();
  delete x;

  x = new MatrixOverSemiring({{-2, 2, 0}, {-1, 0, 0}, {0, 0, 0}}, sr);
  REQUIRE(S.position(x) == Semigroup::UNDEFINED);
  REQUIRE(!S.test_membership(x));
  x->really_delete();
  delete x;

  really_delete_cont(gens);
  delete sr;
}

TEST_CASE("Semigroup: small matrix semigroup [MinPlusSemiring]", "[small]") {
  Semiring*             sr   = new MinPlusSemiring();
  std::vector<Element*> gens = {
      new MatrixOverSemiring({{1, 0}, {0, LONG_MAX}}, sr)};
  Semigroup S = Semigroup(gens);

  REQUIRE(S.size(false) == 3);
  REQUIRE(S.degree() == 2);
  REQUIRE(S.nr_idempotents(false) == 1);
  REQUIRE(S.nrgens() == 1);
  REQUIRE(S.nrrules(false) == 1);

  REQUIRE(*S[0] == *gens[0]);
  REQUIRE(S.position(gens[0]) == 0);
  REQUIRE(S.test_membership(gens[0]));

  Element* x = new MatrixOverSemiring({{-2, 2}, {-1, 0}}, sr);
  REQUIRE(S.position(x) == Semigroup::UNDEFINED);
  REQUIRE(!S.test_membership(x));
  x->redefine(gens[0], gens[0]);
  REQUIRE(S.position(x) == 1);
  REQUIRE(S.test_membership(x));
  x->really_delete();
  delete x;

  x = new MatrixOverSemiring({{-2, 2, 0}, {-1, 0, 0}, {0, 0, 0}}, sr);
  REQUIRE(S.position(x) == Semigroup::UNDEFINED);
  REQUIRE(!S.test_membership(x));
  x->really_delete();
  delete x;

  really_delete_cont(gens);
  delete sr;
}

TEST_CASE("Semigroup: small matrix semigroup [TropicalMaxPlusSemiring]",
          "[small]") {
  Semiring*             sr   = new TropicalMaxPlusSemiring(33);
  std::vector<Element*> gens = {
      new MatrixOverSemiring({{22, 21, 0}, {10, 0, 0}, {1, 32, 1}}, sr),
      new MatrixOverSemiring({{-100, 0, 0}, {0, 1, 0}, {1, -1, 0}}, sr)};
  Semigroup S = Semigroup(gens);

  REQUIRE(S.size(false) == 121);
  REQUIRE(S.degree() == 3);
  REQUIRE(S.nr_idempotents(false) == 1);
  REQUIRE(S.nrgens() == 2);
  REQUIRE(S.nrrules(false) == 18);

  REQUIRE(*S[0] == *gens[0]);
  REQUIRE(S.position(gens[0]) == 0);
  REQUIRE(S.test_membership(gens[0]));

  Element* x = new MatrixOverSemiring({{-2, 2}, {-1, 0}}, sr);
  REQUIRE(S.position(x) == Semigroup::UNDEFINED);
  REQUIRE(!S.test_membership(x));
  x->really_delete();
  delete x;

  x = new MatrixOverSemiring({{-2, 2, 0}, {-1, 0, 0}, {0, 0, 0}}, sr);
  REQUIRE(S.position(x) == Semigroup::UNDEFINED);
  REQUIRE(!S.test_membership(x));
  x->redefine(gens[0], gens[0]);
  REQUIRE(S.position(x) == 2);
  REQUIRE(S.test_membership(x));
  x->really_delete();
  delete x;

  really_delete_cont(gens);
  delete sr;
}

TEST_CASE("Semigroup: small matrix semigroup [TropicalMinPlusSemiring]",
          "[small]") {
  Semiring*             sr   = new TropicalMinPlusSemiring(11);
  std::vector<Element*> gens = {
      new MatrixOverSemiring({{2, 1, 0}, {10, 0, 0}, {1, 2, 1}}, sr),
      new MatrixOverSemiring({{10, 0, 0}, {0, 1, 0}, {1, 1, 0}}, sr)};
  Semigroup S = Semigroup(gens);

  REQUIRE(S.size(false) == 1039);
  REQUIRE(S.degree() == 3);
  REQUIRE(S.nr_idempotents(false) == 5);
  REQUIRE(S.nrgens() == 2);
  REQUIRE(S.nrrules(false) == 38);

  REQUIRE(*S[0] == *gens[0]);
  REQUIRE(S.position(gens[0]) == 0);
  REQUIRE(S.test_membership(gens[0]));

  Element* x = new MatrixOverSemiring({{-2, 2}, {-1, 0}}, sr);
  REQUIRE(S.position(x) == Semigroup::UNDEFINED);
  REQUIRE(!S.test_membership(x));
  x->really_delete();
  delete x;

  x = new MatrixOverSemiring({{-2, 2, 0}, {-1, 0, 0}, {0, 0, 0}}, sr);
  REQUIRE(S.position(x) == Semigroup::UNDEFINED);
  REQUIRE(!S.test_membership(x));
  x->redefine(gens[0], gens[0]);
  REQUIRE(S.position(x) == 2);
  REQUIRE(S.test_membership(x));
  x->really_delete();
  delete x;

  really_delete_cont(gens);
  delete sr;
}

TEST_CASE("Semigroup: small matrix semigroup [NaturalSemiring]", "[small]") {
  Semiring*             sr   = new NaturalSemiring(11, 3);
  std::vector<Element*> gens = {
      new MatrixOverSemiring({{2, 1, 0}, {10, 0, 0}, {1, 2, 1}}, sr),
      new MatrixOverSemiring({{10, 0, 0}, {0, 1, 0}, {1, 1, 0}}, sr)};
  Semigroup S = Semigroup(gens);

  REQUIRE(S.size(false) == 86);
  REQUIRE(S.degree() == 3);
  REQUIRE(S.nr_idempotents(false) == 10);
  REQUIRE(S.nrgens() == 2);
  REQUIRE(S.nrrules(false) == 16);

  REQUIRE(*S[0] == *gens[0]);
  REQUIRE(S.position(gens[0]) == 0);
  REQUIRE(S.test_membership(gens[0]));

  Element* x = new MatrixOverSemiring({{-2, 2}, {-1, 0}}, sr);
  REQUIRE(S.position(x) == Semigroup::UNDEFINED);
  REQUIRE(!S.test_membership(x));
  x->really_delete();
  delete x;

  x = new MatrixOverSemiring({{-2, 2, 0}, {-1, 0, 0}, {0, 0, 0}}, sr);
  REQUIRE(S.position(x) == Semigroup::UNDEFINED);
  REQUIRE(!S.test_membership(x));
  x->redefine(gens[1], gens[0]);
  REQUIRE(S.position(x) == 4);
  REQUIRE(S.test_membership(x));
  x->really_delete();
  delete x;

  really_delete_cont(gens);
  delete sr;
}

TEST_CASE("Semigroup: small pbr semigroup", "[small]") {
  std::vector<Element*> gens = {
      new PBR(new std::vector<std::vector<u_int32_t>>(
          {{1}, {4}, {3}, {1}, {0, 2}, {0, 3, 4, 5}})),
      new PBR(new std::vector<std::vector<u_int32_t>>(
          {{1, 2}, {0, 1}, {0, 2, 3}, {0, 1, 2}, {3}, {0, 3, 4, 5}}))};
  Semigroup S = Semigroup(gens);

  REQUIRE(S.size(false) == 30);
  REQUIRE(S.degree() == 3);
  REQUIRE(S.nr_idempotents(false) == 22);
  REQUIRE(S.nrgens() == 2);
  REQUIRE(S.nrrules(false) == 11);

  REQUIRE(*S[0] == *gens[0]);
  REQUIRE(S.position(gens[0]) == 0);
  REQUIRE(S.test_membership(gens[0]));
  REQUIRE(*S[1] == *gens[1]);
  REQUIRE(S.position(gens[1]) == 1);
  REQUIRE(S.test_membership(gens[1]));

  Element* x = new PBR(
      new std::vector<std::vector<u_int32_t>>({{}, {}, {}, {}, {}, {}}));
  REQUIRE(S.position(x) == Semigroup::UNDEFINED);
  REQUIRE(!S.test_membership(x));
  x->redefine(gens[1], gens[1]);
  REQUIRE(S.position(x) == 5);
  REQUIRE(S.test_membership(x));
  x->really_delete();
  delete x;

  really_delete_cont(gens);
}

TEST_CASE("Semigroup: large transformation semigroup", "[large]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
      new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup S = Semigroup(gens);
  REQUIRE(S.size(false) == 7776);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.nr_idempotents(false) == 537);
  REQUIRE(S.nrgens() == 5);
  REQUIRE(S.nrrules(false) == 2459);
  really_delete_cont(gens);
}

TEST_CASE("Semigroup: at, position, current_*", "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
      new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup S = Semigroup(gens);
  S.set_batch_size(1024);

  Element* expected = new Transformation<u_int16_t>({5, 3, 4, 1, 2, 5});
  REQUIRE(*S.at(100, false) == *expected);
  REQUIRE(S.current_size() == 1029);
  REQUIRE(S.current_nrrules() == 74);
  REQUIRE(S.current_max_word_length() == 7);
  expected->really_delete();
  delete expected;

  Element* x = new Transformation<u_int16_t>({5, 3, 4, 1, 2, 5});
  REQUIRE(S.position(x, false) == 100);
  x->really_delete();
  delete x;

  expected = new Transformation<u_int16_t>({5, 4, 3, 4, 1, 5});
  REQUIRE(*S.at(1023, false) == *expected);
  REQUIRE(S.current_size() == 1029);
  REQUIRE(S.current_nrrules() == 74);
  REQUIRE(S.current_max_word_length() == 7);
  expected->really_delete();
  delete expected;

  x = new Transformation<u_int16_t>({5, 4, 3, 4, 1, 5});
  REQUIRE(S.position(x, false) == 1023);
  x->really_delete();
  delete x;

  expected = new Transformation<u_int16_t>({5, 3, 5, 3, 4, 5});
  REQUIRE(*S.at(3000, false) == *expected);
  REQUIRE(S.current_size() == 3001);
  REQUIRE(S.current_nrrules() == 526);
  REQUIRE(S.current_max_word_length() == 9);
  expected->really_delete();
  delete expected;

  x = new Transformation<u_int16_t>({5, 3, 5, 3, 4, 5});
  REQUIRE(S.position(x, false) == 3000);
  x->really_delete();
  delete x;

  REQUIRE(S.size(false) == 7776);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.nr_idempotents(false) == 537);
  REQUIRE(S.nrgens() == 5);
  REQUIRE(S.nrrules(false) == 2459);

  really_delete_cont(gens);
}

TEST_CASE("Semigroup: enumerate", "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
      new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup S = Semigroup(gens);
  S.set_batch_size(1024);

  S.enumerate(3000, false);
  REQUIRE(S.current_size() == 3000);
  REQUIRE(S.current_nrrules() == 526);
  REQUIRE(S.current_max_word_length() == 9);

  S.enumerate(3001, false);
  REQUIRE(S.current_size() == 4024);
  REQUIRE(S.current_nrrules() == 999);
  REQUIRE(S.current_max_word_length() == 10);

  S.enumerate(7000, false);
  REQUIRE(S.current_size() == 7000);
  REQUIRE(S.current_nrrules() == 2044);
  REQUIRE(S.current_max_word_length() == 12);

  REQUIRE(S.size(false) == 7776);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.nr_idempotents(false) == 537);
  REQUIRE(S.nrgens() == 5);
  REQUIRE(S.nrrules(false) == 2459);

  really_delete_cont(gens);
}

TEST_CASE("Semigroup: enumerate [many stops and starts]", "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
      new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup S = Semigroup(gens);
  S.set_batch_size(128);

  for (size_t i = 1; !S.is_done(); i++) {
    S.enumerate(i * 128, false);
  }

  REQUIRE(S.size(false) == 7776);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.nr_idempotents(false) == 537);
  REQUIRE(S.nrgens() == 5);
  REQUIRE(S.nrrules(false) == 2459);

  really_delete_cont(gens);
}

TEST_CASE("Semigroup: factorisation, length [1 element]", "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
      new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup S = Semigroup(gens);
  S.set_batch_size(1024);

  word_t result;
  S.factorisation(result, 5537, false);
  word_t expected = {1, 2, 2, 2, 3, 2, 4, 1, 2, 2, 3};
  REQUIRE(result == expected);
  REQUIRE(S.length_const(5537) == 11);
  REQUIRE(S.length_non_const(5537) == 11);
  REQUIRE(S.current_max_word_length() == 11);

  REQUIRE(S.current_size() == 5539);
  REQUIRE(S.current_nrrules() == 1484);
  REQUIRE(S.current_max_word_length() == 11);

  REQUIRE(S.length_non_const(7775, false) == 16);
  REQUIRE(S.current_max_word_length() == 16);

  really_delete_cont(gens);
}

TEST_CASE("Semigroup: factorisation, products [all elements]", "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
      new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup S = Semigroup(gens);
  S.set_batch_size(1024);

  word_t result;
  for (size_t i = 0; i < S.size(false); i++) {
    S.factorisation(result, i, false);
    REQUIRE(evaluate_reduct(S, result) == i);
  }

  really_delete_cont(gens);
}

TEST_CASE("Semigroup: first/final letter, prefix, suffix, products",
          "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
      new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup S = Semigroup(gens);
  S.enumerate(1000, false);  // full enumerates

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
  REQUIRE(S.prefix(0) == Semigroup::UNDEFINED);
  REQUIRE(S.final_letter(0) == 0);
  REQUIRE(S.suffix(0) == Semigroup::UNDEFINED);

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

TEST_CASE("Semigroup: genslookup [standard]", "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
      new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup S = Semigroup(gens);

  REQUIRE(S.genslookup(0) == 0);
  REQUIRE(S.genslookup(1) == 1);
  REQUIRE(S.genslookup(2) == 2);
  REQUIRE(S.genslookup(3) == 3);
  REQUIRE(S.genslookup(4) == 4);

  really_delete_cont(gens);
}

TEST_CASE("Semigroup: genslookup [duplicate gens]", "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
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
  Semigroup S = Semigroup(gens);

  REQUIRE(S.genslookup(0) == 0);
  REQUIRE(S.genslookup(1) == 1);
  REQUIRE(S.genslookup(2) == 1);
  REQUIRE(S.genslookup(3) == 1);
  REQUIRE(S.genslookup(4) == 1);
  REQUIRE(S.genslookup(10) == 1);
  REQUIRE(S.genslookup(12) == 3);

  REQUIRE(S.size(false) == 7776);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.nr_idempotents(false) == 537);
  REQUIRE(S.nrgens() == 32);
  REQUIRE(S.nrrules(false) == 2621);

  really_delete_cont(gens);
}

TEST_CASE("Semigroup: genslookup [after add_generators]", "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
      new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup S = Semigroup({gens[0]});

  REQUIRE(S.size(false) == 1);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.nr_idempotents(false) == 1);
  REQUIRE(S.nrgens() == 1);
  REQUIRE(S.nrrules(false) == 1);

  S.add_generators({gens[1]}, false);
  REQUIRE(S.size(false) == 2);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.nr_idempotents(false) == 1);
  REQUIRE(S.nrgens() == 2);
  REQUIRE(S.nrrules(false) == 4);

  S.add_generators({gens[2]}, false);
  REQUIRE(S.size(false) == 120);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.nr_idempotents(false) == 1);
  REQUIRE(S.nrgens() == 3);
  REQUIRE(S.nrrules(false) == 25);

  S.add_generators({gens[3]}, false);
  REQUIRE(S.size(false) == 1546);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.nr_idempotents(false) == 32);
  REQUIRE(S.nrgens() == 4);
  REQUIRE(S.nrrules(false) == 495);

  S.add_generators({gens[4]}, false);
  REQUIRE(S.size(false) == 7776);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.nr_idempotents(false) == 537);
  REQUIRE(S.nrgens() == 5);
  REQUIRE(S.nrrules(false) == 2459);

  REQUIRE(S.genslookup(0) == 0);
  REQUIRE(S.genslookup(1) == 1);
  REQUIRE(S.genslookup(2) == 2);
  REQUIRE(S.genslookup(3) == 120);
  REQUIRE(S.genslookup(4) == 1546);

  really_delete_cont(gens);
}

TEST_CASE("Semigroup: idempotent_cbegin/cend [1 thread]", "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
      new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup S = Semigroup(gens);

  size_t nr = 0;
  for (auto it = S.idempotents_cbegin(false); it < S.idempotents_cend(); it++) {
    REQUIRE(S.fast_product(*it, *it) == *it);
    nr++;
  }
  REQUIRE(nr == S.nr_idempotents());

  really_delete_cont(gens);
}

TEST_CASE("Semigroup: idempotent_cend/cbegin [1 thread]", "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
      new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup S = Semigroup(gens);

  size_t nr  = 0;
  auto   end = S.idempotents_cend(false);
  for (auto it = S.idempotents_cbegin(false); it < end; it++) {
    REQUIRE(S.fast_product(*it, *it) == *it);
    nr++;
  }
  REQUIRE(nr == S.nr_idempotents());

  really_delete_cont(gens);
}

TEST_CASE("Semigroup: is_idempotent [1 thread]", "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
      new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup S = Semigroup(gens);
  really_delete_cont(gens);

  size_t nr = 0;
  for (size_t i = 0; i < S.size(false); i++) {
    if (S.is_idempotent(i, false)) {
      nr++;
    }
  }
  REQUIRE(nr == S.nr_idempotents());
}

#ifndef SKIP_TEST

TEST_CASE("Semigroup: idempotent_cbegin/cend, is_idempotent [2 threads] ~1.7s",
          "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({1, 2, 3, 4, 5, 6, 0}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5, 6}),
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5, 0})};
  Semigroup S = Semigroup(gens);

  size_t nr = 0;
  for (auto it = S.idempotents_cbegin(false, 2); it < S.idempotents_cend();
       it++) {
    REQUIRE(S.fast_product(*it, *it) == *it);
    nr++;
  }
  REQUIRE(nr == S.nr_idempotents());
  REQUIRE(nr == 6322);

  nr = 0;
  for (auto it = S.idempotents_cbegin(false, 2); it < S.idempotents_cend();
       it++) {
    REQUIRE(S.is_idempotent((*it)));
    nr++;
  }
  REQUIRE(nr == S.nr_idempotents());
  REQUIRE(nr == 6322);

  really_delete_cont(gens);
}

#endif

TEST_CASE("Semigroup: is_done, is_begun", "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
      new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup S = Semigroup(gens);

  REQUIRE(!S.is_begun());
  REQUIRE(!S.is_done());

  S.set_batch_size(1024);
  S.enumerate(10, false);
  REQUIRE(S.is_begun());
  REQUIRE(!S.is_done());

  S.enumerate(8000, false);
  REQUIRE(S.is_begun());
  REQUIRE(S.is_done());

  really_delete_cont(gens);
}

TEST_CASE("Semigroup: current_position", "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
      new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup S = Semigroup(gens);

  REQUIRE(S.current_position(gens[0]) == 0);
  REQUIRE(S.current_position(gens[1]) == 1);
  REQUIRE(S.current_position(gens[2]) == 2);
  REQUIRE(S.current_position(gens[3]) == 3);
  REQUIRE(S.current_position(gens[4]) == 4);

  S.set_batch_size(1024);
  S.enumerate(1024, false);

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
  REQUIRE(S.current_position(x) == Semigroup::UNDEFINED);
  REQUIRE(S.current_size() == 1029);
  REQUIRE(S.current_nrrules() == 74);
  REQUIRE(S.current_max_word_length() == 7);
  x->really_delete();
  delete x;

  x = new Transformation<u_int16_t>({5, 4, 5, 1, 0, 5});
  REQUIRE(S.current_position(x) == Semigroup::UNDEFINED);
  REQUIRE(S.current_size() == 1029);
  REQUIRE(S.current_nrrules() == 74);
  REQUIRE(S.current_max_word_length() == 7);
  REQUIRE(S.position(x, false) == 1029);
  x->really_delete();
  delete x;

  really_delete_cont(gens);
}

TEST_CASE("Semigroup: sorted_position, sorted_at", "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
      new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup S = Semigroup(gens);

  REQUIRE(S.sorted_position(gens[0], false) == 310);
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
  REQUIRE(*S.sorted_at(6810) == *S.at(1024));
  REQUIRE(S.sorted_at(6810) == S.at(1024));

  Element* x = new Transformation<u_int16_t>({5, 1, 5, 5, 2, 5});
  REQUIRE(S.sorted_position(x) == 6908);
  REQUIRE(*S.sorted_at(6908) == *x);
  REQUIRE(S.sorted_at(6908) == S.at(S.position(x)));
  x->really_delete();
  delete x;

  x = new Transformation<u_int16_t>({5, 5, 5, 1, 5, 5, 6});
  REQUIRE(S.sorted_position(x) == Semigroup::UNDEFINED);
  x->really_delete();
  delete x;

  REQUIRE(S.sorted_at(100000) == nullptr);
  REQUIRE(S.at(100000) == nullptr);

  really_delete_cont(gens);
}

TEST_CASE("Semigroup: right/left Cayley graph", "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
      new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup S = Semigroup(gens);

  REQUIRE(S.right_cayley_graph(false) != nullptr);
  REQUIRE(S.left_cayley_graph(false) != nullptr);
  // TODO(JDM) more tests
  really_delete_cont(gens);
}

TEST_CASE("Semigroup: elements, sorted_elements", "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
      new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup S = Semigroup(gens);

  REQUIRE(S.elements(false) != nullptr);
  REQUIRE(S.sorted_elements(false) != nullptr);
  // TODO(JDM) more tests
  really_delete_cont(gens);
}

TEST_CASE("Semigroup: copy [not enumerated]", "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
      new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup S = Semigroup(gens);

  REQUIRE(!S.is_begun());
  REQUIRE(!S.is_done());
  REQUIRE(S.current_size() == 5);
  REQUIRE(S.current_nrrules() == 0);
  REQUIRE(S.current_max_word_length() == 1);
  REQUIRE(S.current_position(gens[1]) == 1);

  really_delete_cont(gens);

  Semigroup T = Semigroup(S);

  REQUIRE(!T.is_begun());
  REQUIRE(!T.is_done());
  REQUIRE(T.nrgens() == 5);
  REQUIRE(T.degree() == 6);
  REQUIRE(T.current_size() == 5);
  REQUIRE(T.current_nrrules() == 0);
  REQUIRE(T.current_max_word_length() == 1);
  REQUIRE(T.current_position(S.gens()->at(1)) == 1);

  REQUIRE(T.size(false) == 7776);
  REQUIRE(T.nr_idempotents(false) == 537);
  REQUIRE(T.nrrules(false) == 2459);
  REQUIRE(T.is_begun());
  REQUIRE(T.is_done());
}

TEST_CASE("Semigroup: copy and add gens [not enumerated]", "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5})};
  Semigroup S = Semigroup(gens);
  really_delete_cont(gens);

  REQUIRE(!S.is_begun());
  REQUIRE(!S.is_done());
  REQUIRE(S.nrgens() == 2);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.current_size() == 2);
  REQUIRE(S.current_nrrules() == 0);
  REQUIRE(S.current_max_word_length() == 1);

  std::vector<Element*> coll = {
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
      new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup T = Semigroup(S, coll, false);
  really_delete_cont(coll);

  REQUIRE(!T.is_begun());
  REQUIRE(!T.is_done());
  REQUIRE(T.nrgens() == 5);
  REQUIRE(T.degree() == 6);
  REQUIRE(T.current_size() == 5);
  REQUIRE(T.current_nrrules() == 0);
  REQUIRE(T.current_max_word_length() == 1);
  REQUIRE(T.current_position(S.gens()->at(1)) == 1);

  REQUIRE(T.size(false) == 7776);
  REQUIRE(T.is_done());
  REQUIRE(T.nr_idempotents(false) == 537);
  // REQUIRE(T.nrrules(false) == 2177);
  // This is correct since the order of the generators of T is different to
  // that of S. We don't test for it since it might vary since the order is
  // determined by the order in the unordered_set used in the copy and add
  // generators constructor, and this is not fixed or guaranteed.

  coll        = {new Transformation<u_int16_t>({6, 0, 1, 2, 3, 5, 6})};
  Semigroup U = Semigroup(T, coll, false);
  really_delete_cont(coll);

  REQUIRE(U.is_begun());
  REQUIRE(U.is_done());
  REQUIRE(U.nrgens() == 6);
  REQUIRE(U.degree() == 7);
  REQUIRE(U.current_size() == 16807);
  REQUIRE(U.current_max_word_length() == 16);
  REQUIRE(U.nr_idempotents(false) == 1358);
  // REQUIRE(U.nrrules() == 8272); This is correct since the order of the
  // generators of T and hence U is different to that of S. We don't test for
  // it since it might vary since the order is determined by the order in the
  // unordered_set used in the copy and add generators constructor, and this is
  // not fixed or guaranteed.
}

TEST_CASE("Semigroup: copy [partly enumerated]", "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
      new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup S = Semigroup(gens);
  S.set_batch_size(1000);
  S.enumerate(1001, false);

  REQUIRE(S.is_begun());
  REQUIRE(!S.is_done());
  REQUIRE(S.current_size() == 1006);
  REQUIRE(S.current_nrrules() == 70);
  REQUIRE(S.current_max_word_length() == 7);
  REQUIRE(S.current_position(gens[1]) == 1);
  really_delete_cont(gens);

  Semigroup T = Semigroup(S);

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

  REQUIRE(T.size(false) == 7776);
  REQUIRE(T.nr_idempotents(false) == 537);
  REQUIRE(T.nrrules(false) == 2459);
  REQUIRE(T.is_begun());
  REQUIRE(T.is_done());
}

TEST_CASE("Semigroup: copy and add gens [partly enumerated]", "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup S = Semigroup(gens);
  really_delete_cont(gens);
  S.set_batch_size(60);
  S.enumerate(60, false);

  REQUIRE(S.is_begun());
  REQUIRE(!S.is_done());
  REQUIRE(S.nrgens() == 3);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.current_size() == 63);
  REQUIRE(S.current_nrrules() == 11);
  REQUIRE(S.current_max_word_length() == 7);

  std::vector<Element*> coll = {
      new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup T = Semigroup(S, coll, false);
  really_delete_cont(coll);

  // The next two lines may depend on the implementation of unordered_set, and
  // so are commented out, see the comment below about the order of the
  // generators.
  // REQUIRE(*(*coll)[0] == *(T.gens()->at(4)));
  // REQUIRE(*(*coll)[1] == *(T.gens()->at(3)));

  REQUIRE(T.is_begun());
  REQUIRE(!T.is_done());
  REQUIRE(T.nrgens() == 5);
  REQUIRE(T.degree() == 6);
  REQUIRE(T.current_size() == 818);
  REQUIRE(T.current_nrrules() == 54);
  REQUIRE(T.current_max_word_length() == 7);

  REQUIRE(T.size(false) == 7776);
  REQUIRE(T.is_done());
  REQUIRE(T.nr_idempotents(false) == 537);
  // REQUIRE(T.nrrules(false) == 2458);
  // This is correct since the order of the generators of T is different to
  // that of S. We don't test for it since it might vary since the order is
  // determined by the order in the unordered_set used in the copy and add
  // generators constructor, and this is not fixed or guaranteed.
}

TEST_CASE("Semigroup: copy [fully enumerated]", "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
      new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup S = Semigroup(gens);
  really_delete_cont(gens);

  S.enumerate(8000, false);

  REQUIRE(S.is_begun());
  REQUIRE(S.is_done());
  REQUIRE(S.size(false) == 7776);
  REQUIRE(S.nr_idempotents(false) == 537);
  REQUIRE(S.nrrules(false) == 2459);

  Semigroup T = Semigroup(S);

  REQUIRE(T.is_begun());
  REQUIRE(T.is_done());
  REQUIRE(T.nrgens() == 5);
  REQUIRE(T.degree() == 6);
  REQUIRE(T.size(false) == 7776);
  REQUIRE(T.nr_idempotents(false) == 537);
  REQUIRE(T.nrrules(false) == 2459);
}

TEST_CASE("Semigroup: copy and add gens [fully enumerated]", "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup S = Semigroup(gens);
  really_delete_cont(gens);
  S.enumerate(121, false);

  REQUIRE(S.is_begun());
  REQUIRE(S.is_done());
  REQUIRE(S.nrgens() == 3);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.current_size() == 120);
  REQUIRE(S.current_nrrules() == 25);
  REQUIRE(S.current_max_word_length() == 11);

  std::vector<Element*> coll = {
      new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup T = Semigroup(S, coll, false);
  really_delete_cont(coll);

  // The next two lines may depend on the implementation of unordered_set, and
  // so are commented out, see the comment below about the order of the
  // generators.
  // REQUIRE(*(*coll)[0] == *(T.gens()->at(4)));
  // REQUIRE(*(*coll)[1] == *(T.gens()->at(3)));

  REQUIRE(T.is_begun());
  REQUIRE(!T.is_done());
  REQUIRE(T.nrgens() == 5);
  REQUIRE(T.degree() == 6);
  REQUIRE(T.current_size() == 6842);
  REQUIRE(T.current_nrrules() == 1968);
  REQUIRE(T.current_max_word_length() == 12);

  REQUIRE(T.size(false) == 7776);
  REQUIRE(T.is_done());
  REQUIRE(T.nr_idempotents(false) == 537);
  REQUIRE(T.nrrules(false) == 2458);
  // It is ok that T.nrrules() != S.nrrules() since the generators of T are in
  // a different order to those of S.
}

TEST_CASE("Semigroup: copy and add gens [duplicate gens]", "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup S = Semigroup(gens);
  really_delete_cont(gens);

  std::vector<size_t> result;
  S.next_relation(result, false);
  REQUIRE(result.size() == 2);
  REQUIRE(result[0] == 1);
  REQUIRE(result[1] == 0);

  S.next_relation(result, false);
  REQUIRE(result.size() == 2);
  REQUIRE(result[0] == 3);
  REQUIRE(result[1] == 2);

  S.next_relation(result, false);
  size_t nr = 2;
  while (!result.empty()) {
    S.next_relation(result, false);
    nr++;
  }
  REQUIRE(S.nrrules() == nr);

  S.next_relation(result, false);
  REQUIRE(result.empty());
}

TEST_CASE("Semigroup: relations", "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
      new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup S = Semigroup(gens);
  really_delete_cont(gens);

  std::vector<size_t> result;
  S.next_relation(result, false);
  size_t nr = 0;
  while (!result.empty()) {
    word_t lhs, rhs;
    S.factorisation(lhs, result[0], false);
    lhs.push_back(result[1]);
    S.factorisation(rhs, result[2], false);

    REQUIRE(evaluate_reduct(S, lhs) == evaluate_reduct(S, rhs));
    S.next_relation(result, false);
    nr++;
  }
  REQUIRE(S.nrrules() == nr);

  S.reset_next_relation();
  S.next_relation(result, false);
  nr = 0;

  while (!result.empty()) {
    word_t lhs, rhs;
    S.factorisation(lhs, result[0], false);
    lhs.push_back(result[1]);
    S.factorisation(rhs, result[2], false);

    REQUIRE(evaluate_reduct(S, lhs) == evaluate_reduct(S, rhs));
    REQUIRE(evaluate_reduct(S, lhs) == evaluate_reduct(S, rhs));
    S.next_relation(result, false);
    nr++;
  }

  REQUIRE(S.nrrules() == nr);
}

TEST_CASE("Semigroup: relations [duplicate gens]", "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup S = Semigroup(gens);
  really_delete_cont(gens);

  S.enumerate(Semigroup::LIMIT_MAX, false);
  REQUIRE(S.is_begun());
  REQUIRE(S.is_done());
  REQUIRE(S.nrgens() == 5);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.current_size() == 120);
  REQUIRE(S.size() == 120);
  REQUIRE(S.current_nrrules() == 33);
  REQUIRE(S.nrrules() == 33);
  REQUIRE(S.current_max_word_length() == 11);

  std::vector<Element*> coll = {
      new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup T = Semigroup(S, coll, false);
  really_delete_cont(coll);

  REQUIRE(T.size(false) == 7776);
  REQUIRE(T.is_done());
  REQUIRE(T.nr_idempotents(false) == 537);
}

TEST_CASE("Semigroup: relations [from copy, not enumerated]", "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
      new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup S = Semigroup(gens);
  really_delete_cont(gens);

  Semigroup T = Semigroup(S);
  REQUIRE(T.nrrules(false) == S.nrrules(false));

  std::vector<size_t> result;
  T.next_relation(result, false);
  size_t nr = 0;
  while (!result.empty()) {
    word_t lhs, rhs;
    T.factorisation(lhs, result[0], false);
    lhs.push_back(result[1]);
    T.factorisation(rhs, result[2], false);

    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    T.next_relation(result, false);
    nr++;
  }
  REQUIRE(T.nrrules() == nr);

  T.reset_next_relation();
  T.next_relation(result, false);
  nr = 0;

  while (!result.empty()) {
    word_t lhs, rhs;
    T.factorisation(lhs, result[0], false);
    lhs.push_back(result[1]);
    T.factorisation(rhs, result[2], false);

    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    T.next_relation(result, false);
    nr++;
  }
  REQUIRE(T.nrrules() == nr);
}

TEST_CASE("Semigroup: relations [from copy, partly enumerated]", "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
      new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup S = Semigroup(gens);
  really_delete_cont(gens);

  S.set_batch_size(1023);
  S.enumerate(1000, false);

  Semigroup T = Semigroup(S);
  REQUIRE(T.nrrules(false) == S.nrrules(false));

  std::vector<size_t> result;
  T.next_relation(result, false);
  size_t nr = 0;
  while (!result.empty()) {
    word_t lhs, rhs;
    T.factorisation(lhs, result[0], false);
    lhs.push_back(result[1]);
    T.factorisation(rhs, result[2], false);

    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    T.next_relation(result, false);
    nr++;
  }
  REQUIRE(T.nrrules() == nr);

  T.reset_next_relation();
  T.next_relation(result, false);
  nr = 0;

  while (!result.empty()) {
    word_t lhs, rhs;
    T.factorisation(lhs, result[0], false);
    lhs.push_back(result[1]);
    T.factorisation(rhs, result[2], false);

    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    T.next_relation(result, false);
    nr++;
  }
  REQUIRE(T.nrrules() == nr);
}

TEST_CASE("Semigroup: relations [from copy, fully enumerated]", "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
      new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup S = Semigroup(gens);
  really_delete_cont(gens);

  S.enumerate(8000, false);

  Semigroup T = Semigroup(S);
  REQUIRE(T.nrrules(false) == S.nrrules(false));

  std::vector<size_t> result;
  T.next_relation(result, false);
  size_t nr = 0;
  while (!result.empty()) {
    word_t lhs, rhs;
    T.factorisation(lhs, result[0], false);
    lhs.push_back(result[1]);
    T.factorisation(rhs, result[2], false);

    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    T.next_relation(result, false);
    nr++;
  }
  REQUIRE(T.nrrules() == nr);

  T.reset_next_relation();
  T.next_relation(result, false);
  nr = 0;

  while (!result.empty()) {
    word_t lhs, rhs;
    T.factorisation(lhs, result[0], false);
    lhs.push_back(result[1]);
    T.factorisation(rhs, result[2], false);

    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    T.next_relation(result, false);
    nr++;
  }
  REQUIRE(T.nrrules() == nr);
}

TEST_CASE("Semigroup: relations [from copy and add gens, not enumerated]",
          "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup S = Semigroup(gens);
  really_delete_cont(gens);

  REQUIRE(!S.is_begun());
  REQUIRE(!S.is_done());

  std::vector<Element*> coll = {
      new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup T = Semigroup(S, coll, false);
  really_delete_cont(coll);

  // The next two lines may depend on the implementation of unordered_set, and
  // so are commented out, see the comment below about the order of the
  // generators.
  // REQUIRE(*(*coll)[0] == *(T.gens()->at(4)));
  // REQUIRE(*(*coll)[1] == *(T.gens()->at(3)));

  std::vector<size_t> result;
  T.next_relation(result, false);
  size_t nr = 0;
  while (!result.empty()) {
    word_t lhs, rhs;
    T.factorisation(lhs, result[0], false);
    lhs.push_back(result[1]);
    T.factorisation(rhs, result[2], false);

    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    T.next_relation(result, false);
    nr++;
  }
  REQUIRE(T.nrrules() == nr);
  // REQUIRE(2458 == nr);
  // This is correct since the order of the generators of T is different to
  // that of S. We don't test for it since it might vary since the order is
  // determined by the order in the unordered_set used in the copy and add
  // generators constructor, and this is not fixed or guaranteed.

  T.reset_next_relation();
  T.next_relation(result, false);
  nr = 0;

  while (!result.empty()) {
    word_t lhs, rhs;
    T.factorisation(lhs, result[0], false);
    lhs.push_back(result[1]);
    T.factorisation(rhs, result[2], false);

    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    T.next_relation(result, false);
    nr++;
  }
  REQUIRE(T.nrrules() == nr);
  // REQUIRE(2458 == nr);
  // This is correct since the order of the generators of T is different to
  // that of S. We don't test for it since it might vary since the order is
  // determined by the order in the unordered_set used in the copy and add
  // generators constructor, and this is not fixed or guaranteed.
}

TEST_CASE("Semigroup: relations [from copy and add gens, partly enumerated]",
          "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup S = Semigroup(gens);
  really_delete_cont(gens);
  S.set_batch_size(100);

  S.enumerate(10, false);

  REQUIRE(S.is_begun());
  REQUIRE(!S.is_done());

  std::vector<Element*> coll = {
      new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup T = Semigroup(S, coll, false);
  really_delete_cont(coll);

  std::vector<size_t> result;
  T.next_relation(result, false);
  size_t nr = 0;
  while (!result.empty()) {
    word_t lhs, rhs;
    T.factorisation(lhs, result[0], false);
    lhs.push_back(result[1]);
    T.factorisation(rhs, result[2], false);

    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    T.next_relation(result, false);
    nr++;
  }
  REQUIRE(T.nrrules() == nr);
  // REQUIRE(2458 == nr);
  // This is correct since the order of the generators of T is different to
  // that of S. We don't test for it since it might vary since the order is
  // determined by the order in the unordered_set used in the copy and add
  // generators constructor, and this is not fixed or guaranteed.

  T.reset_next_relation();
  T.next_relation(result, false);
  nr = 0;

  while (!result.empty()) {
    word_t lhs, rhs;
    T.factorisation(lhs, result[0], false);
    lhs.push_back(result[1]);
    T.factorisation(rhs, result[2], false);

    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    T.next_relation(result, false);
    nr++;
  }
  REQUIRE(T.nrrules() == nr);
  // REQUIRE(2458 == nr);
  // This is correct since the order of the generators of T is different to
  // that of S. We don't test for it since it might vary since the order is
  // determined by the order in the unordered_set used in the copy and add
  // generators constructor, and this is not fixed or guaranteed.
}

TEST_CASE("Semigroup: relations [from copy and add gens, fully enumerated]",
          "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup S = Semigroup(gens);
  really_delete_cont(gens);

  S.enumerate(8000, false);

  REQUIRE(S.is_begun());
  REQUIRE(S.is_done());

  std::vector<Element*> coll = {
      new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup T = Semigroup(S, coll, false);
  really_delete_cont(coll);

  std::vector<size_t> result;
  T.next_relation(result, false);
  size_t nr = 0;
  while (!result.empty()) {
    REQUIRE(result.size() == 3);  // there are no duplicate gens
    word_t lhs, rhs;
    T.factorisation(lhs, result[0], false);
    lhs.push_back(result[1]);
    T.factorisation(rhs, result[2], false);

    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    T.next_relation(result, false);
    nr++;
  }
  REQUIRE(T.nrrules() == nr);
  // REQUIRE(2458 == nr);
  // This is correct since the order of the generators of T is different to
  // that of S. We don't test for it since it might vary since the order is
  // determined by the order in the unordered_set used in the copy and add
  // generators constructor, and this is not fixed or guaranteed.

  T.reset_next_relation();
  T.next_relation(result, false);
  nr = 0;

  while (!result.empty()) {
    REQUIRE(result.size() == 3);
    word_t lhs, rhs;
    T.factorisation(lhs, result[0], false);
    lhs.push_back(result[1]);
    T.factorisation(rhs, result[2], false);

    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
    T.next_relation(result, false);
    nr++;
  }
  REQUIRE(T.nrrules() == nr);
  // REQUIRE(2458 == nr);
  // This is correct since the order of the generators of T is different to
  // that of S. We don't test for it since it might vary since the order is
  // determined by the order in the unordered_set used in the copy and add
  // generators constructor, and this is not fixed or guaranteed.
}

TEST_CASE("Semigroup: add_generators [duplicate generators]", "[method]") {
  std::vector<Element*> gens = {
      new Transformation<u_int16_t>({0, 1, 0, 3, 4, 5}),
      new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
      new Transformation<u_int16_t>({0, 1, 3, 5, 5, 4}),
      new Transformation<u_int16_t>({1, 0, 2, 4, 4, 5}),
      new Transformation<u_int16_t>({4, 3, 3, 1, 0, 5}),
      new Transformation<u_int16_t>({4, 3, 5, 1, 0, 5}),
      new Transformation<u_int16_t>({5, 5, 2, 3, 4, 0})};

  Semigroup S = Semigroup({gens[0], gens[0]});

  REQUIRE(S.size(false) == 1);
  REQUIRE(S.nrgens() == 2);

  S.add_generators(std::unordered_set<Element*>({}), false);
  REQUIRE(S.size(false) == 1);
  REQUIRE(S.nrgens() == 2);

  S.add_generators({gens[0]}, false);
  REQUIRE(S.size(false) == 1);
  REQUIRE(S.nrgens() == 2);

  S.add_generators({gens[1]}, false);
  REQUIRE(S.size(false) == 2);
  REQUIRE(S.nrgens() == 3);

  S.add_generators({gens[2]}, false);
  REQUIRE(S.size(false) == 7);
  REQUIRE(S.nrgens() == 4);

  S.add_generators({gens[3]}, false);
  REQUIRE(S.size(false) == 18);
  REQUIRE(S.nrgens() == 5);

  S.add_generators({gens[4]}, false);
  REQUIRE(S.size(false) == 87);
  REQUIRE(S.nrgens() == 6);

  S.add_generators({gens[5]}, false);
  REQUIRE(S.size(false) == 97);
  REQUIRE(S.nrgens() == 7);

  S.add_generators({gens[6]}, false);
  REQUIRE(S.size(false) == 119);
  REQUIRE(S.nrgens() == 8);

  really_delete_cont(gens);
}
