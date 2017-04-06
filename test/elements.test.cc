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

#include "catch.hpp"

#include "../elements.h"

using namespace libsemigroups;

TEST_CASE("Transformation<u_int16_t>: methods",
          "[quick][element][transformation]") {
  Element* x = new Transformation<u_int16_t>({0, 1, 0});
  Element* y
      = new Transformation<u_int16_t>(new std::vector<u_int16_t>({0, 1, 0}));
  REQUIRE(*x == *y);
  x->redefine(y, y);
  REQUIRE(*x == *y);
  REQUIRE((*x < *y) == false);

  Element* z
      = new Transformation<u_int16_t>(new std::vector<u_int16_t>({0, 1, 0, 3}));
  REQUIRE(*x < *z);
  z->really_delete();
  delete z;

  Element* expected = new Transformation<u_int16_t>({0, 0, 0});
  REQUIRE(*expected < *x);
  expected->really_delete();
  delete expected;

  REQUIRE(x->degree() == 3);
  REQUIRE(y->degree() == 3);
  REQUIRE(x->complexity() == 3);
  REQUIRE(y->complexity() == 3);
  REQUIRE(static_cast<Transformation<u_int16_t>*>(x)->crank() == 2);
  REQUIRE(static_cast<Transformation<u_int16_t>*>(y)->crank() == 2);
  Element* id = x->identity();

  expected = new Transformation<u_int16_t>({0, 1, 2});
  REQUIRE(*id == *expected);
  expected->really_delete();
  delete expected;

  Element* a = x->really_copy(10);
  REQUIRE(a->degree() == 13);
  a->really_delete();
  delete a;

  x->really_delete();
  delete x;
  y->really_delete();
  delete y;
  id->really_delete();
  delete id;
}

TEST_CASE("Transformation<u_int16_t>: hash",
          "[quick][element][transformation]") {
  Element* x = new Transformation<u_int16_t>({9, 7, 3, 5, 3, 4, 2, 7, 7, 1});
  for (size_t i = 0; i < 1000000; i++) {
    x->hash_value();
  }
  x->really_delete();
  delete x;
}

TEST_CASE("Transformation<u_int16_t>: delete/copy",
          "[quick][element][transformation]") {
  Element* x = new Transformation<u_int16_t>({9, 7, 3, 5, 3, 4, 2, 7, 7, 1});
  Element* y = x->really_copy();
  x->really_delete();  // deletes _vector in x but not in y
  delete x;

  Element* expected
      = new Transformation<u_int16_t>({9, 7, 3, 5, 3, 4, 2, 7, 7, 1});

  REQUIRE(*y == *expected);
  Transformation<u_int16_t> yy = *static_cast<Transformation<u_int16_t>*>(y);
  REQUIRE(yy == *y);
  Transformation<u_int16_t> zz(yy);
  delete y;  // does not delete the _vector in y, yy, or zz
  REQUIRE(zz == *expected);
  zz.really_delete();

  std::vector<u_int16_t>* imgs
      = new std::vector<u_int16_t>({9, 7, 3, 5, 3, 4, 2, 7, 7, 1});
  x = new Transformation<u_int16_t>(imgs);  // imgs is not copied into x!
  REQUIRE(*x == *expected);
  yy = *static_cast<Transformation<u_int16_t>*>(x);
  REQUIRE(yy == *expected);
  delete x;
  REQUIRE(yy == *expected);
  yy.really_delete();

  expected->really_delete();
  delete expected;
}

TEST_CASE("Transformation<u_int32_t>: methods",
          "[quick][element][transformation]") {
  Element* x = new Transformation<u_int32_t>({0, 1, 0});
  Element* y
      = new Transformation<u_int32_t>(new std::vector<u_int32_t>({0, 1, 0}));
  REQUIRE(*x == *y);
  x->redefine(y, y);
  REQUIRE(*x == *y);
  REQUIRE((*x < *y) == false);
  Element* expected = new Transformation<u_int32_t>({0, 0, 0});
  REQUIRE(*expected < *x);
  expected->really_delete();
  delete expected;

  REQUIRE(x->degree() == 3);
  REQUIRE(y->degree() == 3);
  REQUIRE(x->complexity() == 3);
  REQUIRE(y->complexity() == 3);
  REQUIRE(static_cast<Transformation<u_int32_t>*>(x)->crank() == 2);
  REQUIRE(static_cast<Transformation<u_int32_t>*>(y)->crank() == 2);
  Element* id = x->identity();

  expected = new Transformation<u_int32_t>({0, 1, 2});
  REQUIRE(*id == *expected);
  expected->really_delete();
  delete expected;

  x->really_delete();
  delete x;
  y->really_delete();
  delete y;
  id->really_delete();
  delete id;
}

TEST_CASE("Transformation<u_int32_t>: hash ~8ms",
          "[quick][element][transformation]") {
  Element* x = new Transformation<u_int32_t>({9, 7, 3, 5, 3, 4, 2, 7, 7, 1});
  for (size_t i = 0; i < 1000000; i++) {
    x->hash_value();
  }
  x->really_delete();
  delete x;
}

TEST_CASE("Transformation<u_int32_t>: delete/copy",
          "[quick][element][transformation]") {
  Element* x = new Transformation<u_int32_t>({9, 7, 3, 5, 3, 4, 2, 7, 7, 1});
  Element* y = x->really_copy();
  x->really_delete();  // deletes _vector in x but not in y
  delete x;

  Element* expected
      = new Transformation<u_int32_t>({9, 7, 3, 5, 3, 4, 2, 7, 7, 1});

  REQUIRE(*y == *expected);
  Transformation<u_int32_t> yy = *static_cast<Transformation<u_int32_t>*>(y);
  REQUIRE(yy == *y);
  Transformation<u_int32_t> zz(yy);
  delete y;  // does not delete the _vector in y, yy, or zz
  REQUIRE(zz == *expected);
  zz.really_delete();

  std::vector<u_int32_t>* imgs
      = new std::vector<u_int32_t>({9, 7, 3, 5, 3, 4, 2, 7, 7, 1});
  x = new Transformation<u_int32_t>(imgs);  // imgs is not copied into x!
  REQUIRE(*x == *expected);
  yy = *static_cast<Transformation<u_int32_t>*>(x);
  REQUIRE(yy == *expected);
  delete x;
  REQUIRE(yy == *expected);
  yy.really_delete();

  expected->really_delete();
  delete expected;
}

TEST_CASE("PartialPerm<u_int16_t>: methods", "[quick][element][pperm]") {
  Element* x = new PartialPerm<u_int16_t>({4, 5, 0}, {10, 0, 1}, 10);
  Element* y = new PartialPerm<u_int16_t>({4, 5, 0}, {10, 0, 1}, 10);
  REQUIRE(*x == *y);
  x->redefine(y, y);
  PartialPerm<u_int16_t>* xx = static_cast<PartialPerm<u_int16_t>*>(x);
  REQUIRE(xx->at(0) == 65535);
  REQUIRE(xx->at(1) == 65535);
  REQUIRE(xx->at(2) == 65535);
  REQUIRE(xx->at(3) == 65535);
  REQUIRE(xx->at(4) == 65535);
  REQUIRE(xx->at(5) == 1);

  REQUIRE(*x < *y);
  REQUIRE(!(*x < *x));
  Element* expected = new PartialPerm<u_int16_t>({0, 0, 0});
  REQUIRE(*expected < *x);
  expected->really_delete();
  delete expected;

  REQUIRE(x->degree() == 11);
  REQUIRE(y->degree() == 11);
  REQUIRE(x->complexity() == 11);
  REQUIRE(y->complexity() == 11);
  REQUIRE(static_cast<PartialPerm<u_int16_t>*>(x)->crank() == 1);
  REQUIRE(static_cast<PartialPerm<u_int16_t>*>(y)->crank() == 3);
  Element* id = x->identity();

  expected = new PartialPerm<u_int16_t>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
  REQUIRE(*id == *expected);
  expected->really_delete();
  delete expected;

  Element* a = x->really_copy(10);
  REQUIRE(a->degree() == 21);
  a->really_delete();
  delete a;

  x->really_delete();
  delete x;
  y->really_delete();
  delete y;
  id->really_delete();
  delete id;
}

TEST_CASE("PartialPerm<u_int16_t>: hash ~8ms", "[quick][element][pperm]") {
  Element* x = new PartialPerm<u_int16_t>(
      {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 9);
  for (size_t i = 0; i < 1000000; i++) {
    x->hash_value();
  }
  x->really_delete();
  delete x;
}

TEST_CASE("PartialPerm<u_int16_t>: delete/copy", "[quick][element][pperm]") {
  Element* x = new PartialPerm<u_int16_t>(
      {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 9);
  Element* y = x->really_copy();
  x->really_delete();  // deletes _vector in x but not in y
  delete x;

  Element* expected = new PartialPerm<u_int16_t>(
      {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 9);
  REQUIRE(*y == *expected);

  PartialPerm<u_int16_t> yy = *static_cast<PartialPerm<u_int16_t>*>(y);
  REQUIRE(yy == *y);
  PartialPerm<u_int16_t> zz(yy);
  delete y;  // does not delete the _vector in y, yy, or zz
  REQUIRE(zz == *expected);
  zz.really_delete();
  expected->really_delete();
  delete expected;
}

TEST_CASE("PartialPerm<u_int32_t>: methods", "[quick][element][pperm]") {
  Element* x = new PartialPerm<u_int32_t>({4, 5, 0}, {10, 0, 1}, 10);
  Element* y = new PartialPerm<u_int32_t>({4, 5, 0}, {10, 0, 1}, 10);
  REQUIRE(*x == *y);
  x->redefine(y, y);
  PartialPerm<u_int32_t>* xx = static_cast<PartialPerm<u_int32_t>*>(x);
  REQUIRE(xx->at(0) == 4294967295);
  REQUIRE(xx->at(1) == 4294967295);
  REQUIRE(xx->at(2) == 4294967295);
  REQUIRE(xx->at(3) == 4294967295);
  REQUIRE(xx->at(4) == 4294967295);
  REQUIRE(xx->at(5) == 1);
  REQUIRE((*x < *y) == true);

  Element* z = new PartialPerm<u_int32_t>({0, 0, 0});
  REQUIRE(*z < *x);
  z->really_delete();
  delete z;

  REQUIRE(x->degree() == 11);
  REQUIRE(y->degree() == 11);
  REQUIRE(x->complexity() == 11);
  REQUIRE(y->complexity() == 11);
  REQUIRE(static_cast<PartialPerm<u_int32_t>*>(x)->crank() == 1);
  REQUIRE(static_cast<PartialPerm<u_int32_t>*>(y)->crank() == 3);
  Element* id = x->identity();

  Element* expected
      = new PartialPerm<u_int32_t>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
  REQUIRE(*id == *expected);
  expected->really_delete();
  delete expected;

  x->really_delete();
  delete x;
  y->really_delete();
  delete y;
  id->really_delete();
  delete id;
}

TEST_CASE("PartialPerm<u_int32_t>: hash ~8ms", "[quick][element][pperm]") {
  Element* x = new PartialPerm<u_int32_t>(
      {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 9);
  for (size_t i = 0; i < 1000000; i++) {
    x->hash_value();
  }
  x->really_delete();
  delete x;
}

TEST_CASE("PartialPerm<u_int32_t>: delete/copy", "[quick][element][pperm]") {
  Element* x = new PartialPerm<u_int32_t>(
      {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 9);
  Element* y = x->really_copy();
  x->really_delete();  // deletes _vector in x but not in y
  delete x;

  Element* expected = new PartialPerm<u_int32_t>(
      {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 9);
  REQUIRE(*y == *expected);

  PartialPerm<u_int32_t> yy = *static_cast<PartialPerm<u_int32_t>*>(y);
  REQUIRE(yy == *y);
  PartialPerm<u_int32_t> zz(yy);
  delete y;  // does not delete the _vector in y, yy, or zz
  REQUIRE(zz == *expected);

  expected->really_delete();
  delete expected;
  zz.really_delete();
}

TEST_CASE("BooleanMat: methods", "[quick][element][booleanmat]") {
  Element* x = new BooleanMat({{1, 0, 1}, {0, 1, 0}, {0, 1, 0}});
  Element* y = new BooleanMat({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}});
  Element* z = new BooleanMat({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}});
  REQUIRE(*y == *z);
  z->redefine(x, y);
  REQUIRE(*y == *z);
  z->redefine(y, x);
  REQUIRE(*y == *z);
  REQUIRE(!(*y < *z));
  REQUIRE(x->degree() == 3);
  REQUIRE(y->degree() == 3);
  REQUIRE(z->degree() == 3);
  REQUIRE(x->complexity() == 27);
  REQUIRE(y->complexity() == 27);
  REQUIRE(z->complexity() == 27);
  Element* id = x->identity();
  z->redefine(id, x);
  REQUIRE(*z == *x);
  z->redefine(x, id);
  REQUIRE(*z == *x);
  x->really_delete();
  delete x;
  y->really_delete();
  delete y;
  z->really_delete();
  delete z;
  id->really_delete();
  delete id;
}

TEST_CASE("BooleanMat: hash ~8ms", "[quick][element][booleanmat]") {
  Element* x = new BooleanMat({{1, 0, 1}, {0, 1, 0}, {0, 1, 0}});
  for (size_t i = 0; i < 1000000; i++) {
    x->hash_value();
  }
  x->really_delete();
  delete x;
}

TEST_CASE("BooleanMat: delete/copy", "[quick][element][booleanmat]") {
  Element* x = new BooleanMat({{1, 0, 1}, {0, 1, 0}, {0, 1, 0}});
  Element* y = x->really_copy();
  x->really_delete();  // deletes _vector in x but not in y
  delete x;

  Element* expected = new BooleanMat({{1, 0, 1}, {0, 1, 0}, {0, 1, 0}});
  REQUIRE(*y == *expected);

  BooleanMat yy = *static_cast<BooleanMat*>(y);
  REQUIRE(yy == *y);
  BooleanMat zz(yy);
  delete y;  // does not delete the _vector in y, yy, or zz
  REQUIRE(zz == *expected);
  zz.really_delete();
  expected->really_delete();
  delete expected;
}

TEST_CASE("Bipartition: overridden methods", "[quick][element][bipart]") {
  Element* x = new Bipartition(
      {0, 1, 2, 1, 0, 2, 1, 0, 2, 2, 0, 0, 2, 0, 3, 4, 4, 1, 3, 0});
  Element* y = new Bipartition(
      {0, 1, 1, 1, 1, 2, 3, 2, 4, 5, 5, 2, 4, 2, 1, 1, 1, 2, 3, 2});
  Element* z = new Bipartition(
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
  REQUIRE(!(*y == *z));

  z->redefine(x, y, 0);
  Element* expected = new Bipartition(
      {0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1});
  REQUIRE(*z == *expected);
  expected->really_delete();
  delete expected;

  expected = new Bipartition(
      {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 3, 1, 2, 1});
  z->redefine(y, x, 0);
  REQUIRE(*z == *expected);
  expected->really_delete();
  delete expected;

  REQUIRE(!(*y < *z));
  REQUIRE(x->degree() == 10);
  REQUIRE(y->degree() == 10);
  REQUIRE(z->degree() == 10);
  REQUIRE(x->complexity() == 400);
  REQUIRE(y->complexity() == 400);
  REQUIRE(z->complexity() == 400);

  Element* id = x->identity();
  z->redefine(id, x, 0);
  REQUIRE(*z == *x);
  z->redefine(x, id, 0);
  REQUIRE(*z == *x);
  z->redefine(id, y, 0);
  REQUIRE(*z == *y);
  z->redefine(y, id, 0);
  REQUIRE(*z == *y);
  x->really_delete();
  delete x;
  y->really_delete();
  delete y;
  z->really_delete();
  delete z;
  id->really_delete();
  delete id;
}

TEST_CASE("Bipartition: hash ~12ms", "[quick][element][bipart]") {
  Element* x = new Bipartition(
      {0, 1, 2, 1, 0, 2, 1, 0, 2, 2, 0, 0, 2, 0, 3, 4, 4, 1, 3, 0});
  for (size_t i = 0; i < 1000000; i++) {
    x->hash_value();
  }
  x->really_delete();
  delete x;
}

TEST_CASE("Bipartition: non-overridden methods", "[quick][element][bipart]") {
  Bipartition* x = new Bipartition(
      {0, 0, 0, 0, 0, 0, 1, 2, 0, 1, 0, 0, 1, 2, 3, 3, 0, 4, 1, 1});

  REQUIRE(x->rank() == 3);
  REQUIRE(x->block(0) == 0);
  REQUIRE(x->block(6) == 1);
  REQUIRE(x->block(10) == 0);
  REQUIRE(x->const_nr_blocks() == 5);
  REQUIRE(x->nr_blocks() == 5);
  REQUIRE(x->const_nr_blocks() == 5);
  REQUIRE(x->nr_blocks() == 5);
  REQUIRE(x->nr_left_blocks() == 3);
  REQUIRE(x->nr_right_blocks() == 5);
  REQUIRE(x->is_transverse_block(0));
  REQUIRE(x->is_transverse_block(1));
  REQUIRE(x->is_transverse_block(2));
  REQUIRE(!x->is_transverse_block(3));
  REQUIRE(!x->is_transverse_block(4));

  Bipartition* y = new Bipartition(
      {0, 0, 1, 2, 3, 3, 0, 4, 1, 1, 0, 0, 0, 0, 0, 0, 1, 2, 0, 1});

  Blocks* a = x->left_blocks();
  Blocks* b = y->right_blocks();
  REQUIRE(*a == *b);
  delete a;
  delete b;
  a = x->right_blocks();
  b = y->left_blocks();
  REQUIRE(*a == *b);
  delete a;
  delete b;
  x->really_delete();
  delete x;
  y->really_delete();
  delete y;

  x = new Bipartition(
      {0, 0, 0, 0, 0, 0, 1, 2, 0, 1, 0, 0, 1, 2, 3, 3, 0, 4, 1, 1});
  x->set_nr_blocks(5);
  REQUIRE(x->nr_blocks() == 5);
  x->really_delete();
  delete x;

  x = new Bipartition(
      {0, 0, 0, 0, 0, 0, 1, 2, 0, 1, 0, 0, 1, 2, 3, 3, 0, 4, 1, 1});
  x->set_nr_left_blocks(3);
  REQUIRE(x->nr_left_blocks() == 3);
  REQUIRE(x->nr_right_blocks() == 5);
  REQUIRE(x->nr_blocks() == 5);
  x->really_delete();
  delete x;

  x = new Bipartition(
      {0, 0, 0, 0, 0, 0, 1, 2, 0, 1, 0, 0, 1, 2, 3, 3, 0, 4, 1, 1});
  x->set_rank(3);
  REQUIRE(x->rank() == 3);
  x->really_delete();
  delete x;
}

TEST_CASE("Bipartition: delete/copy", "[quick][element][bipart]") {
  Element* x = new Bipartition({0, 0, 0, 0, 0});
  Element* y = x->really_copy();
  x->really_delete();  // deletes _vector in x but not in y
  delete x;

  Element* expected = new Bipartition({0, 0, 0, 0, 0});
  REQUIRE(*y == *expected);

  Bipartition yy = *static_cast<Bipartition*>(y);
  REQUIRE(yy == *y);
  Bipartition zz(yy);
  delete y;  // does not delete the _vector in y, yy, or zz
  REQUIRE(zz == *expected);
  expected->really_delete();
  delete expected;
  zz.really_delete();
}

TEST_CASE("Bipartition: degree 0", "[quick][element][bipart]") {
  Bipartition* x = new Bipartition(std::vector<u_int32_t>({}));
  REQUIRE(x->const_nr_blocks() == 0);
  REQUIRE(x->nr_left_blocks() == 0);

  Blocks* b = x->left_blocks();
  REQUIRE(b->degree() == 0);
  REQUIRE(b->nr_blocks() == 0);
  delete b;

  b = x->right_blocks();
  REQUIRE(b->degree() == 0);
  REQUIRE(b->nr_blocks() == 0);
  delete b;

  x->really_delete();
  delete x;
}

TEST_CASE("ProjectiveMaxPlusMatrix: methods", "[quick][element][matrix]") {
  Semiring* sr = new MaxPlusSemiring();

  Element* x
      = new ProjectiveMaxPlusMatrix({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  Element* expected = new ProjectiveMaxPlusMatrix(
      {{-4, 0, -2}, {-3, -2, -2}, {-1, -5, -1}}, sr);
  REQUIRE(*x == *expected);
  expected->really_delete();
  delete expected;

  REQUIRE(static_cast<MatrixOverSemiring*>(x)->semiring() == sr);

  Element* y = new ProjectiveMaxPlusMatrix(
      {{LONG_MIN, 0, 0}, {0, 1, 0}, {1, -1, 0}}, sr);
  expected = new ProjectiveMaxPlusMatrix(
      {{LONG_MIN, -1, -1}, {-1, 0, -1}, {0, -2, -1}}, sr);
  REQUIRE(*y == *expected);
  REQUIRE(!(*x == *y));
  expected->really_delete();
  delete expected;

  y->redefine(x, x);
  expected = new ProjectiveMaxPlusMatrix(
      {{-2, -1, -1}, {-2, -2, -2}, {-1, 0, -1}}, sr);
  REQUIRE(*y == *expected);
  expected->really_delete();
  delete expected;

  REQUIRE(*x < *y);
  REQUIRE(x->degree() == 3);
  REQUIRE(y->degree() == 3);
  REQUIRE(x->complexity() == 27);
  REQUIRE(y->complexity() == 27);
  Element* id = x->identity();
  y->redefine(id, x);
  REQUIRE(*y == *x);
  y->redefine(x, id);
  REQUIRE(*y == *x);
  x->really_delete();
  delete x;
  y->really_delete();
  delete y;
  id->really_delete();
  delete id;
  delete sr;
}

TEST_CASE("ProjectiveMaxPlusMatrix: hash ~11ms", "[quick][element][matrix]") {
  Semiring* sr = new MaxPlusSemiring();
  Element*  x
      = new ProjectiveMaxPlusMatrix({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  for (size_t i = 0; i < 1000000; i++) {
    x->hash_value();
  }
  x->really_delete();
  delete x;
  delete sr;
}

TEST_CASE("ProjectiveMaxPlusMatrix: delete/copy", "[quick][element][matrix]") {
  Semiring* sr = new MaxPlusSemiring();
  Element*  x
      = new ProjectiveMaxPlusMatrix({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  Element* y = x->really_copy();
  x->really_delete();  // deletes _vector in x but not in y
  delete x;

  Element* expected
      = new ProjectiveMaxPlusMatrix({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  REQUIRE(*y == *expected);
  expected->really_delete();
  delete expected;

  ProjectiveMaxPlusMatrix yy = *static_cast<ProjectiveMaxPlusMatrix*>(y);
  REQUIRE(yy == *y);

  ProjectiveMaxPlusMatrix zz(yy);
  delete y;  // does not delete the _vector in y, yy, or zz
  expected
      = new ProjectiveMaxPlusMatrix({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  REQUIRE(zz == *expected);
  expected->really_delete();
  delete expected;
  zz.really_delete();
  delete sr;
}

TEST_CASE("MatrixOverSemiring [Integers]: methods",
          "[quick][element][matrix]") {
  Semiring* sr = new Integers();

  Element* x = new MatrixOverSemiring({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  Element* expected
      = new MatrixOverSemiring({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  REQUIRE(*x == *expected);
  expected->really_delete();
  delete expected;

  Element* y
      = new MatrixOverSemiring({{-100, 0, 0}, {0, 1, 0}, {1, -1, 0}}, sr);
  REQUIRE(!(*x == *y));

  y->redefine(x, x);
  expected = new MatrixOverSemiring({{2, -4, 0}, {2, -2, 0}, {2, -1, 1}}, sr);
  REQUIRE(*y == *expected);
  expected->really_delete();
  delete expected;

  REQUIRE(*x < *y);
  REQUIRE(x->degree() == 3);
  REQUIRE(y->degree() == 3);
  REQUIRE(x->complexity() == 27);
  REQUIRE(y->complexity() == 27);
  Element* id = x->identity();
  y->redefine(id, x);
  REQUIRE(*y == *x);
  y->redefine(x, id);
  REQUIRE(*y == *x);
  x->really_delete();
  delete x;
  y->really_delete();
  delete y;
  id->really_delete();
  delete id;
  delete sr;
}

TEST_CASE("MatrixOverSemiring [Integers]: hash ~11ms",
          "[quick][element][matrix]") {
  Semiring* sr = new Integers();
  Element* x = new MatrixOverSemiring({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  for (size_t i = 0; i < 1000000; i++) {
    x->hash_value();
  }
  x->really_delete();
  delete x;
  delete sr;
}

TEST_CASE("MatrixOverSemiring [MaxPlusSemiring]: methods",
          "[quick][element][matrix]") {
  Semiring* sr = new MaxPlusSemiring();

  Element* x = new MatrixOverSemiring({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  Element* expected
      = new MatrixOverSemiring({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  REQUIRE(*x == *expected);
  expected->really_delete();
  delete expected;

  Element* y
      = new MatrixOverSemiring({{-100, 0, 0}, {0, 1, 0}, {1, -1, 0}}, sr);
  REQUIRE(!(*x == *y));

  y->redefine(x, x);
  expected = new MatrixOverSemiring({{1, 2, 2}, {1, 1, 1}, {2, 3, 2}}, sr);
  REQUIRE(*y == *expected);
  expected->really_delete();
  delete expected;

  REQUIRE(*x < *y);
  REQUIRE(x->degree() == 3);
  REQUIRE(y->degree() == 3);
  REQUIRE(x->complexity() == 27);
  REQUIRE(y->complexity() == 27);
  Element* id = x->identity();
  y->redefine(id, x);
  REQUIRE(*y == *x);
  y->redefine(x, id);
  REQUIRE(*y == *x);
  x->really_delete();
  delete x;
  y->really_delete();
  delete y;
  id->really_delete();
  delete id;
  delete sr;
}

TEST_CASE("MatrixOverSemiring [MaxPlusSemiring]: hash ~11ms",
          "[quick][element][matrix]") {
  Semiring* sr = new MaxPlusSemiring();
  Element* x = new MatrixOverSemiring({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  for (size_t i = 0; i < 1000000; i++) {
    x->hash_value();
  }
  x->really_delete();
  delete x;
  delete sr;
}

TEST_CASE("MatrixOverSemiring [MinPlusSemiring]: methods",
          "[quick][element][matrix]") {
  Semiring* sr = new MinPlusSemiring();

  Element* x = new MatrixOverSemiring({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  Element* expected
      = new MatrixOverSemiring({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  REQUIRE(*x == *expected);
  expected->really_delete();
  delete expected;

  Element* y
      = new MatrixOverSemiring({{-100, 0, 0}, {0, 1, 0}, {1, -1, 0}}, sr);
  REQUIRE(!(*x == *y));

  y->redefine(x, x);
  expected
      = new MatrixOverSemiring({{-4, -3, -2}, {-3, -3, -1}, {-4, -3, -3}}, sr);
  REQUIRE(*y == *expected);
  expected->really_delete();
  delete expected;

  REQUIRE(!(*x < *y));
  REQUIRE(x->degree() == 3);
  REQUIRE(y->degree() == 3);
  REQUIRE(x->complexity() == 27);
  REQUIRE(y->complexity() == 27);
  Element* id = x->identity();
  y->redefine(id, x);
  REQUIRE(*y == *x);
  y->redefine(x, id);
  REQUIRE(*y == *x);
  x->really_delete();
  delete x;
  y->really_delete();
  delete y;
  id->really_delete();
  delete id;
  delete sr;
}

TEST_CASE("MatrixOverSemiring [MinPlusSemiring]: hash ~11ms",
          "[quick][element][matrix]") {
  Semiring* sr = new MinPlusSemiring();
  Element* x = new MatrixOverSemiring({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  for (size_t i = 0; i < 1000000; i++) {
    x->hash_value();
  }
  x->really_delete();
  delete x;
  delete sr;
}

TEST_CASE("MatrixOverSemiring [TropicalMaxPlusSemiring]: methods",
          "[quick][element][matrix]") {
  Semiring* sr = new TropicalMaxPlusSemiring(33);

  Element* x
      = new MatrixOverSemiring({{22, 21, 0}, {10, 0, 0}, {1, 32, 1}}, sr);
  Element* expected
      = new MatrixOverSemiring({{22, 21, 0}, {10, 0, 0}, {1, 32, 1}}, sr);
  REQUIRE(*x == *expected);
  expected->really_delete();
  delete expected;

  Element* y
      = new MatrixOverSemiring({{-100, 0, 0}, {0, 1, 0}, {1, -1, 0}}, sr);
  REQUIRE(!(*x == *y));

  y->redefine(x, x);
  expected
      = new MatrixOverSemiring({{33, 33, 22}, {32, 32, 10}, {33, 33, 32}}, sr);
  REQUIRE(*y == *expected);
  expected->really_delete();

  delete expected;
  REQUIRE(*x < *y);
  REQUIRE(x->degree() == 3);
  REQUIRE(y->degree() == 3);
  REQUIRE(x->complexity() == 27);
  REQUIRE(y->complexity() == 27);
  Element* id = x->identity();
  y->redefine(id, x);
  REQUIRE(*y == *x);
  y->redefine(x, id);
  REQUIRE(*y == *x);
  x->really_delete();
  delete x;
  y->really_delete();
  delete y;
  id->really_delete();
  delete id;
  delete sr;
}

TEST_CASE("MatrixOverSemiring [TropicalMaxPlusSemiring]: hash ~11ms",
          "[quick][element][matrix]") {
  Semiring* sr = new TropicalMaxPlusSemiring(33);
  Element*  x
      = new MatrixOverSemiring({{22, 21, 0}, {10, 0, 0}, {1, 32, 1}}, sr);
  for (size_t i = 0; i < 1000000; i++) {
    x->hash_value();
  }
  x->really_delete();
  delete x;
  delete sr;
}

TEST_CASE("MatrixOverSemiring [TropicalMinPlusSemiring]: methods",
          "[quick][element][matrix]") {
  Semiring* sr = new TropicalMinPlusSemiring(33);

  Element* x
      = new MatrixOverSemiring({{22, 21, 0}, {10, 0, 0}, {1, 32, 1}}, sr);

  Element* expected
      = new MatrixOverSemiring({{22, 21, 0}, {10, 0, 0}, {1, 32, 1}}, sr);
  REQUIRE(*x == *expected);
  expected->really_delete();
  delete expected;

  Element* y
      = new MatrixOverSemiring({{-100, 0, 0}, {0, 1, 0}, {1, -1, 0}}, sr);
  REQUIRE(!(*x == *y));

  y->redefine(x, x);
  expected = new MatrixOverSemiring({{1, 21, 1}, {1, 0, 0}, {2, 22, 1}}, sr);
  REQUIRE(*y == *expected);
  expected->really_delete();
  delete expected;

  REQUIRE(!(*x < *y));
  REQUIRE(x->degree() == 3);
  REQUIRE(y->degree() == 3);
  REQUIRE(x->complexity() == 27);
  REQUIRE(y->complexity() == 27);
  Element* id = x->identity();
  y->redefine(id, x);
  REQUIRE(*y == *x);
  y->redefine(x, id);
  REQUIRE(*y == *x);
  x->really_delete();
  delete x;
  y->really_delete();
  delete y;
  id->really_delete();
  delete id;
  delete sr;
}

TEST_CASE("MatrixOverSemiring [TropicalMinPlusSemiring]: hash ~11ms",
          "[quick][element][matrix]") {
  Semiring* sr = new TropicalMinPlusSemiring(33);
  Element*  x
      = new MatrixOverSemiring({{22, 21, 0}, {10, 0, 0}, {1, 32, 1}}, sr);
  for (size_t i = 0; i < 1000000; i++) {
    x->hash_value();
  }
  x->really_delete();
  delete x;
  delete sr;
}

TEST_CASE("MatrixOverSemiring [NaturalSemiring]: methods",
          "[quick][element][matrix]") {
  Semiring* sr = new NaturalSemiring(33, 2);

  Element* x
      = new MatrixOverSemiring({{22, 21, 0}, {10, 0, 0}, {1, 32, 1}}, sr);
  Element* expected
      = new MatrixOverSemiring({{22, 21, 0}, {10, 0, 0}, {1, 32, 1}}, sr);
  REQUIRE(*x == *expected);
  expected->really_delete();
  delete expected;

  Element* y
      = new MatrixOverSemiring({{-100, 0, 0}, {0, 1, 0}, {1, -1, 0}}, sr);
  REQUIRE(!(*x == *y));

  y->redefine(x, x);
  expected
      = new MatrixOverSemiring({{34, 34, 0}, {34, 34, 0}, {33, 33, 1}}, sr);
  REQUIRE(*y == *expected);
  expected->really_delete();
  delete expected;

  REQUIRE(*x < *y);
  REQUIRE(x->degree() == 3);
  REQUIRE(y->degree() == 3);
  REQUIRE(x->complexity() == 27);
  REQUIRE(y->complexity() == 27);
  Element* id = x->identity();
  y->redefine(id, x);
  REQUIRE(*y == *x);
  y->redefine(x, id);
  REQUIRE(*y == *x);
  x->really_delete();
  delete x;
  y->really_delete();
  delete y;
  id->really_delete();
  delete id;
  delete sr;
}

TEST_CASE("MatrixOverSemiring [NaturalSemiring]: hash ~11ms",
          "[quick][element][matrix]") {
  Semiring* sr = new NaturalSemiring(33, 2);
  Element*  x
      = new MatrixOverSemiring({{22, 21, 0}, {10, 0, 0}, {1, 32, 1}}, sr);

  for (size_t i = 0; i < 1000000; i++) {
    x->hash_value();
  }
  x->really_delete();
  delete x;
  delete sr;
}

TEST_CASE("MatrixOverSemiring [Integers]: delete/copy",
          "[quick][element][matrix]") {
  Semiring* sr = new Integers();
  Element* x = new MatrixOverSemiring({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  Element* y = x->really_copy();

  x->really_delete();  // deletes _vector in x but not in y
  delete x;
  Element* expected
      = new MatrixOverSemiring({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  REQUIRE(*y == *expected);
  expected->really_delete();
  delete expected;

  MatrixOverSemiring yy = *static_cast<MatrixOverSemiring*>(y);
  REQUIRE(yy == *y);
  MatrixOverSemiring zz(yy);
  delete y;  // does not delete the _vector in y, yy, or zz
  REQUIRE(zz == yy);
  zz.really_delete();  // deletes _vector in yy, zz, and y
  delete sr;
}

TEST_CASE("MatrixOverSemiring [MaxPlusSemiring]: delete/copy",
          "[quick][element][matrix]") {
  Semiring* sr = new MaxPlusSemiring();
  Element* x = new MatrixOverSemiring({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  Element* y = x->really_copy();

  x->really_delete();  // deletes _vector in x but not in y
  delete x;
  Element* expected
      = new MatrixOverSemiring({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  REQUIRE(*y == *expected);
  expected->really_delete();
  delete expected;

  MatrixOverSemiring yy = *static_cast<MatrixOverSemiring*>(y);
  REQUIRE(yy == *y);
  MatrixOverSemiring zz(yy);
  delete y;  // does not delete the _vector in y, yy, or zz
  REQUIRE(zz == yy);
  zz.really_delete();  // deletes _vector in yy, zz, and y
  delete sr;
}

TEST_CASE("MatrixOverSemiring [MinPlusSemiring]: delete/copy",
          "[quick][element][matrix]") {
  Semiring* sr = new MinPlusSemiring();
  Element* x = new MatrixOverSemiring({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  Element* y = x->really_copy();

  x->really_delete();  // deletes _vector in x but not in y
  delete x;
  Element* expected
      = new MatrixOverSemiring({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  REQUIRE(*y == *expected);
  expected->really_delete();
  delete expected;

  MatrixOverSemiring yy = *static_cast<MatrixOverSemiring*>(y);
  REQUIRE(yy == *y);
  MatrixOverSemiring zz(yy);
  delete y;  // does not delete the _vector in y, yy, or zz
  REQUIRE(zz == yy);
  zz.really_delete();  // deletes _vector in yy, zz, and y
  delete sr;
}

TEST_CASE("MatrixOverSemiring [TropicalMaxPlusSemiring]: delete/copy",
          "[quick][element][matrix]") {
  Semiring* sr = new TropicalMaxPlusSemiring(23);
  Element* x = new MatrixOverSemiring({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  Element* y = x->really_copy();

  x->really_delete();  // deletes _vector in x but not in y
  delete x;
  Element* expected
      = new MatrixOverSemiring({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  REQUIRE(*y == *expected);
  expected->really_delete();
  delete expected;

  MatrixOverSemiring yy = *static_cast<MatrixOverSemiring*>(y);
  REQUIRE(yy == *y);
  MatrixOverSemiring zz(yy);
  delete y;  // does not delete the _vector in y, yy, or zz
  REQUIRE(zz == yy);
  zz.really_delete();  // deletes _vector in yy, zz, and y
  delete sr;
}

TEST_CASE("MatrixOverSemiring [TropicalMinPlusSemiring]: delete/copy",
          "[quick][element][matrix]") {
  Semiring* sr = new TropicalMinPlusSemiring(23);
  Element* x = new MatrixOverSemiring({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  Element* y = x->really_copy();

  x->really_delete();  // deletes _vector in x but not in y
  delete x;
  Element* expected
      = new MatrixOverSemiring({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  REQUIRE(*y == *expected);
  expected->really_delete();
  delete expected;

  MatrixOverSemiring yy = *static_cast<MatrixOverSemiring*>(y);
  REQUIRE(yy == *y);
  MatrixOverSemiring zz(yy);
  delete y;  // does not delete the _vector in y, yy, or zz
  REQUIRE(zz == yy);
  zz.really_delete();  // deletes _vector in yy, zz, and y
  delete sr;
}

TEST_CASE("MatrixOverSemiring [NaturalSemiring]: delete/copy",
          "[quick][element][matrix]") {
  Semiring* sr = new NaturalSemiring(23, 1);
  Element* x = new MatrixOverSemiring({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  Element* y = x->really_copy();

  x->really_delete();  // deletes _vector in x but not in y
  delete x;
  Element* expected
      = new MatrixOverSemiring({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  REQUIRE(*y == *expected);
  expected->really_delete();
  delete expected;

  MatrixOverSemiring yy = *static_cast<MatrixOverSemiring*>(y);
  REQUIRE(yy == *y);
  MatrixOverSemiring zz(yy);
  delete y;  // does not delete the _vector in y, yy, or zz
  REQUIRE(zz == yy);
  zz.really_delete();  // deletes _vector in yy, zz, and y
  delete sr;
}

TEST_CASE("PBR: methods", "[quick][element][pbr]") {
  Element* x = new PBR(new std::vector<std::vector<u_int32_t>>(
      {{1}, {4}, {3}, {1}, {0, 2}, {0, 3, 4, 5}}));
  Element* y = new PBR(new std::vector<std::vector<u_int32_t>>(
      {{1, 2}, {0, 1}, {0, 2, 3}, {0, 1, 2}, {3}, {0, 3, 4, 5}}));
  REQUIRE(!(*x == *y));
  y->redefine(x, x);
  Element* z = new PBR(new std::vector<std::vector<u_int32_t>>(
      {{1}, {4}, {0, 2}, {0, 2}, {0, 1, 2, 3, 4}, {1, 3, 4, 5}}));
  REQUIRE(*y == *z);
  z->really_delete();
  delete z;

  REQUIRE(!(*x < *y));
  REQUIRE(x->degree() == 3);
  REQUIRE(y->degree() == 3);
  REQUIRE(x->complexity() == 216);
  REQUIRE(y->complexity() == 216);
  Element* id = x->identity();
  y->redefine(id, x);
  REQUIRE(*y == *x);
  y->redefine(x, id);
  REQUIRE(*y == *x);
  x->really_delete();
  delete x;
  y->really_delete();
  delete y;
  id->really_delete();
  delete id;
}

TEST_CASE("PBR: universal product", "[quick][element][pbr]") {
  Element* x
      = new PBR(new std::vector<std::vector<u_int32_t>>({{5, 3},
                                                         {5, 4, 3, 0, 1, 2},
                                                         {5, 4, 3, 0, 2},
                                                         {5, 3, 0, 1, 2},
                                                         {5, 0, 2},
                                                         {5, 4, 3, 1, 2}}));
  Element* y = new PBR(new std::vector<std::vector<u_int32_t>>({{5, 4, 3, 0},
                                                                {5, 4, 2},
                                                                {5, 1, 2},
                                                                {5, 4, 3, 2},
                                                                {5, 4, 3, 2},
                                                                {4, 1, 2}}));

  Element* z = new PBR(new std::vector<std::vector<u_int32_t>>({{5, 4, 3, 0},
                                                                {5, 4, 2},
                                                                {5, 1, 2},
                                                                {5, 4, 3, 2},
                                                                {5, 4, 3, 2},
                                                                {4, 1, 2}}));
  z->redefine(x, y);

  Element* expected
      = new PBR(new std::vector<std::vector<u_int32_t>>({{0, 1, 2, 3, 4, 5},
                                                         {0, 1, 2, 3, 4, 5},
                                                         {0, 1, 2, 3, 4, 5},
                                                         {0, 1, 2, 3, 4, 5},
                                                         {0, 1, 2, 3, 4, 5},
                                                         {0, 1, 2, 3, 4, 5}}));
  REQUIRE(*z == *expected);

  x->really_delete();
  delete x;
  y->really_delete();
  delete y;
  z->really_delete();
  delete z;
  expected->really_delete();
  delete expected;
}

TEST_CASE("PBR: product [bigger than previous]", "[quick][element][pbr]") {
  Element* x
      = new PBR(new std::vector<std::vector<u_int32_t>>({{5, 3},
                                                         {5, 4, 3, 0, 1, 2},
                                                         {5, 4, 3, 0, 2},
                                                         {5, 3, 0, 1, 2},
                                                         {5, 0, 2},
                                                         {5, 4, 3, 1, 2},
                                                         {},
                                                         {}}));
  Element* y
      = new PBR(new std::vector<std::vector<u_int32_t>>({{5, 3},
                                                         {5, 4, 3, 0, 1, 2},
                                                         {5, 4, 3, 0, 2},
                                                         {5, 3, 0, 1, 2},
                                                         {5, 0, 2},
                                                         {5, 4, 3, 1, 2},
                                                         {},
                                                         {6}}));
  x->redefine(y, y);
  Element* expected
      = new PBR(new std::vector<std::vector<u_int32_t>>({{0, 1, 2, 3, 4, 5},
                                                         {0, 1, 2, 3, 4, 5},
                                                         {0, 1, 2, 3, 4, 5},
                                                         {0, 1, 2, 3, 4, 5},
                                                         {0, 1, 2, 3, 4, 5},
                                                         {0, 1, 2, 3, 4, 5},
                                                         {},
                                                         {6}}));

  REQUIRE(*x == *expected);

  x->really_delete();
  delete x;
  y->really_delete();
  delete y;
  expected->really_delete();
  delete expected;

  x = new PBR(new std::vector<std::vector<u_int32_t>>(
      {{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {7}}));
  y = new PBR(new std::vector<std::vector<u_int32_t>>(
      {{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {7}}));

  x->redefine(y, y);
  expected = new PBR(new std::vector<std::vector<u_int32_t>>(
      {{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {7}}));
  REQUIRE(*x == *expected);

  x->really_delete();
  delete x;
  y->really_delete();
  delete y;
  expected->really_delete();
  delete expected;
}

TEST_CASE("PBR: hash ~28ms", "[quick][element][pbr]") {
  Element* x = new PBR(new std::vector<std::vector<u_int32_t>>(
      {{1}, {4}, {3}, {1}, {0, 2}, {0, 3, 4, 5}}));
  for (size_t i = 0; i < 1000000; i++) {
    x->hash_value();
  }
  x->really_delete();
  delete x;
}

TEST_CASE("PBR: delete/copy", "[quick][element][pbr]") {
  Element* x = new PBR(new std::vector<std::vector<u_int32_t>>(
      {{1}, {4}, {3}, {1}, {0, 2}, {0, 3, 4, 5}}));
  Element* y = x->really_copy();
  x->really_delete();  // deletes _vector in x but not in y
  delete x;
  Element* z = new PBR(new std::vector<std::vector<u_int32_t>>(
      {{1}, {4}, {3}, {1}, {0, 2}, {0, 3, 4, 5}}));
  REQUIRE(*y == *z);
  z->really_delete();
  delete z;
  PBR yy = *static_cast<PBR*>(y);
  REQUIRE(yy == *y);
  PBR zz(yy);
  delete y;  // does not delete the _vector in y, yy, or zz
  Element* a = new PBR(new std::vector<std::vector<u_int32_t>>(
      {{1}, {4}, {3}, {1}, {0, 2}, {0, 3, 4, 5}}));
  REQUIRE(zz == *a);
  zz.really_delete();
  a->really_delete();
  delete a;
}
