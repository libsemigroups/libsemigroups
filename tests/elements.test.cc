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

#include "../src/bmat8.h"
#include "../src/elements.h"

using namespace libsemigroups;

TEST_CASE("Transformation 01: u_int16_t methods",
          "[quick][element][transformation][01]") {
  auto x = Transformation<u_int16_t>({0, 1, 0});
  auto y = Transformation<u_int16_t>({0, 1, 0});
  REQUIRE(x == y);
  REQUIRE(y * y == x);
  REQUIRE((x < y) == false);

  auto z = Transformation<u_int16_t>({0, 1, 0, 3});
  REQUIRE(x < z);

  auto expected = Transformation<u_int16_t>({0, 0, 0});
  REQUIRE(expected < x);

  REQUIRE(x.degree() == 3);
  REQUIRE(y.degree() == 3);
  REQUIRE(x.complexity() == 3);
  REQUIRE(y.complexity() == 3);
  REQUIRE(x.crank() == 2);
  REQUIRE(y.crank() == 2);
  auto id = x.identity();

  expected = Transformation<u_int16_t>({0, 1, 2});
  REQUIRE(id == expected);

  x.increase_deg_by(10);
  REQUIRE(x.degree() == 13);
}

TEST_CASE("Transformation 02: u_int16_t hash",
          "[quick][element][transformation][02]") {
  Element* x = new Transformation<u_int16_t>({9, 7, 3, 5, 3, 4, 2, 7, 7, 1});
  for (size_t i = 0; i < 1000000; i++) {
    x->hash_value();
  }
  delete x;
}

/*TEST_CASE("Transformation 03: u_int16_t delete/copy",
          "[quick][element][transformation][03]") {
  Element* x = new Transformation<u_int16_t>({9, 7, 3, 5, 3, 4, 2, 7, 7, 1});
  Element* y = x->heap_copy();
  delete x;

  Element* expected
      = new Transformation<u_int16_t>({9, 7, 3, 5, 3, 4, 2, 7, 7, 1});

  REQUIRE(*y == *expected);
  Transformation<u_int16_t>& yy = *static_cast<Transformation<u_int16_t>*>(y);
  REQUIRE(yy == *y);
  Transformation<u_int16_t> zz(yy);
  delete y;  // does not delete the _vector in y, yy, or zz
  REQUIRE(zz == *expected);

  x = new Transformation<u_int16_t>({9, 7, 3, 5, 3, 4, 2, 7, 7, 1});
  REQUIRE(*x == *expected);
  yy = *static_cast<Transformation<u_int16_t>*>(x);
  REQUIRE(yy == *expected);
  delete x;
  REQUIRE(yy == *expected);

  delete expected;
}*/

TEST_CASE("Transformation 04: u_int32_t methods",
          "[quick][element][transformation][04]") {
  Element* x = new Transformation<u_int32_t>({0, 1, 0});
  Element* y = new Transformation<u_int32_t>({0, 1, 0});
  REQUIRE(*x == *y);
  x->redefine(y, y);
  REQUIRE(*x == *y);
  REQUIRE((*x < *y) == false);
  Element* expected = new Transformation<u_int32_t>({0, 0, 0});
  REQUIRE(*expected < *x);

  REQUIRE(x->degree() == 3);
  REQUIRE(y->degree() == 3);
  REQUIRE(x->complexity() == 3);
  REQUIRE(y->complexity() == 3);
  REQUIRE(static_cast<Transformation<u_int32_t>*>(x)->crank() == 2);
  REQUIRE(static_cast<Transformation<u_int32_t>*>(y)->crank() == 2);
  Transformation<u_int32_t> id
      = static_cast<Transformation<u_int32_t>*>(x)->identity();

  expected = new Transformation<u_int32_t>({0, 1, 2});
  REQUIRE(id == *expected);
  delete expected;

  delete x;
  delete y;
}

TEST_CASE("Transformation 05: u_int32_t hash",
          "[quick][element][transformation][05]") {
  Element* x = new Transformation<u_int32_t>({9, 7, 3, 5, 3, 4, 2, 7, 7, 1});
  for (size_t i = 0; i < 1000000; i++) {
    x->hash_value();
  }
  delete x;
}

/*TEST_CASE("Transformation 06: u_int32_t delete/copy",
          "[quick][element][transformation][06]") {
  Element* x = new Transformation<u_int32_t>({9, 7, 3, 5, 3, 4, 2, 7, 7, 1});
  Element* y = x->heap_copy();
  delete x;

  Element* expected
      = new Transformation<u_int32_t>({9, 7, 3, 5, 3, 4, 2, 7, 7, 1});

  REQUIRE(*y == *expected);
  Transformation<u_int32_t>& yy = *static_cast<Transformation<u_int32_t>*>(y);
  REQUIRE(yy == *y);
  Transformation<u_int32_t> zz(yy);
  delete y;  // does not delete the _vector in y, yy, or zz
  REQUIRE(zz == *expected);

  x = new Transformation<u_int32_t>({9, 7, 3, 5, 3, 4, 2, 7, 7, 1});
  REQUIRE(*x == *expected);
  yy = *static_cast<Transformation<u_int32_t>*>(x);
  REQUIRE(yy == *expected);
  delete x;
  REQUIRE(yy == *expected);

  delete expected;
}*/

TEST_CASE("Transformation 07: exceptions",
          "[quick][element][transformation][07]") {
  REQUIRE_NOTHROW(Transformation<u_int16_t>(std::vector<u_int16_t>()));
  REQUIRE_NOTHROW(Transformation<u_int16_t>(std::vector<u_int16_t>({0})));
  REQUIRE_THROWS_AS(Transformation<u_int16_t>(std::vector<u_int16_t>({1})),
                    LibsemigroupsException);

  REQUIRE_NOTHROW(Transformation<u_int16_t>(std::vector<u_int16_t>({0, 1, 2})));
  REQUIRE_NOTHROW(
      Transformation<u_int16_t>(std::initializer_list<u_int16_t>({0, 1, 2})));
  // Implicit type initializer lists are not accepted.
  // REQUIRE_NOTHROW(Transformation<u_int16_t>({0, 1, 2})));

  std::vector<u_int16_t> pimgs = {1, 2, 3};
  // REQUIRE_NOTHROW(Transformation<u_int16_t>(pimgs));
  REQUIRE_THROWS_AS(Transformation<u_int16_t>({1, 2, 3}),
                    LibsemigroupsException);
  REQUIRE_THROWS_AS(
      Transformation<u_int16_t>(std::initializer_list<u_int16_t>({1, 2, 3})),
      LibsemigroupsException);

#if !defined(LIBSEMIGROUPS_HAVE_DENSEHASHMAP) \
    || !defined(LIBSEMIGROUPS_USE_DENSEHASHMAP)
  auto& UNDEF
      = PartialTransformation<u_int16_t, PartialPerm<u_int16_t>>::UNDEFINED;
  REQUIRE_THROWS_AS(Transformation<u_int16_t>(std::initializer_list<u_int16_t>(
                        {UNDEF, UNDEF, UNDEF})),
                    LibsemigroupsException);
#endif
}

TEST_CASE("PartialPerm 01: u_int16_t methods", "[quick][element][pperm][01]") {
  auto& UNDEF
      = PartialTransformation<u_int16_t, PartialPerm<u_int16_t>>::UNDEFINED;
  auto x = PartialPerm<u_int16_t>({4, 5, 0}, {9, 0, 1}, 10);
  auto y = PartialPerm<u_int16_t>({4, 5, 0}, {9, 0, 1}, 10);
  REQUIRE(x == y);
  auto yy = x * x;
  REQUIRE(yy.at(0) == UNDEF);
  REQUIRE(yy.at(1) == UNDEF);
  REQUIRE(yy.at(2) == UNDEF);
  REQUIRE(yy.at(3) == UNDEF);
  REQUIRE(yy.at(4) == UNDEF);
  REQUIRE(yy.at(5) == 1);

  REQUIRE(yy < y);
  REQUIRE(!(x < x));
  auto expected = PartialPerm<u_int16_t>({UNDEF, UNDEF, UNDEF});
  REQUIRE(expected < x);

  REQUIRE(x.degree() == 10);
  REQUIRE(y.degree() == 10);
  REQUIRE(x.complexity() == 10);
  REQUIRE(y.complexity() == 10);
  REQUIRE(yy.crank() == 1);
  REQUIRE(y.crank() == 3);
  REQUIRE(x.crank() == 3);
  auto id = x.identity();

  expected = PartialPerm<u_int16_t>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
  REQUIRE(id == expected);

  x.increase_deg_by(10);
  REQUIRE(x.degree() == 20);
}

TEST_CASE("PartialPerm 02: u_int16_t hash", "[quick][element][pperm][02]") {
  Element* x = new PartialPerm<u_int16_t>(
      {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 10);
  for (size_t i = 0; i < 1000000; i++) {
    x->hash_value();
  }
  delete x;
}

TEST_CASE("PartialPerm 03: u_int16_t delete/copy",
          "[quick][element][pperm][03]") {
  Element* x = new PartialPerm<u_int16_t>(
      {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 10);
  Element* y = x->heap_copy();
  delete x;

  Element* expected = new PartialPerm<u_int16_t>(
      {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 10);
  REQUIRE(*y == *expected);

  PartialPerm<u_int16_t> yy = *static_cast<PartialPerm<u_int16_t>*>(y);
  REQUIRE(yy == *y);
  PartialPerm<u_int16_t> zz(yy);
  delete y;  // does not delete the _vector in y, yy, or zz
  REQUIRE(zz == *expected);
  delete expected;
}

TEST_CASE("PartialPerm 04: u_int32_t methods", "[quick][element][pperm][04]") {
  auto& UNDEF
      = PartialTransformation<u_int32_t, PartialPerm<u_int32_t>>::UNDEFINED;
  auto x = PartialPerm<u_int32_t>({4, 5, 0}, {10, 0, 1}, 11);
  auto y = PartialPerm<u_int32_t>({4, 5, 0}, {10, 0, 1}, 11);
  REQUIRE(x == y);
  auto xx = x * x;
  REQUIRE(xx.at(0) == UNDEF);
  REQUIRE(xx.at(1) == UNDEF);
  REQUIRE(xx.at(2) == UNDEF);
  REQUIRE(xx.at(3) == UNDEF);
  REQUIRE(xx.at(4) == UNDEF);
  REQUIRE(xx.at(5) == 1);
  REQUIRE((xx < y) == true);

  auto z = PartialPerm<u_int32_t>({UNDEF, UNDEF, UNDEF});
  REQUIRE(z < x);

  REQUIRE(x.degree() == 11);
  REQUIRE(y.degree() == 11);
  REQUIRE(x.complexity() == 11);
  REQUIRE(y.complexity() == 11);
  REQUIRE(xx.crank() == 1);
  REQUIRE(x.crank() == 3);
  REQUIRE(y.crank() == 3);
  auto id = x.identity();

  auto expected = PartialPerm<u_int32_t>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
  REQUIRE(id == expected);
}

TEST_CASE("PartialPerm 05: u_int32_t hash", "[quick][element][pperm][05]") {
  Element* x = new PartialPerm<u_int32_t>(
      {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 10);
  for (size_t i = 0; i < 1000000; i++) {
    x->hash_value();
  }
  delete x;
}

TEST_CASE("PartialPerm 06: u_int32_t delete/copy",
          "[quick][element][pperm][06]") {
  Element* x = new PartialPerm<u_int32_t>(
      {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 10);
  Element* y = x->heap_copy();
  delete x;

  Element* expected = new PartialPerm<u_int32_t>(
      {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 10);
  REQUIRE(*y == *expected);

  PartialPerm<u_int32_t> yy = *static_cast<PartialPerm<u_int32_t>*>(y);
  REQUIRE(yy == *y);
  PartialPerm<u_int32_t> zz(yy);
  delete y;  // does not delete the _vector in y, yy, or zz
  REQUIRE(zz == *expected);

  delete expected;
}

TEST_CASE("PartialPerm 07: exceptions", "[quick][element][pperm][07]") {
  auto& UNDEF = PartialPerm<u_int16_t>::UNDEFINED;
  REQUIRE_NOTHROW(PartialPerm<u_int16_t>(std::vector<u_int16_t>()));
  REQUIRE_NOTHROW(PartialPerm<u_int16_t>(std::vector<u_int16_t>({0})));
  REQUIRE_NOTHROW(PartialPerm<u_int16_t>(std::vector<u_int16_t>({UNDEF})));
  REQUIRE_THROWS_AS(PartialPerm<u_int16_t>(std::vector<u_int16_t>({1})),
                    LibsemigroupsException);

  REQUIRE_NOTHROW(PartialPerm<u_int16_t>(std::vector<u_int16_t>({0, 1, 2})));
  REQUIRE_NOTHROW(
      PartialPerm<u_int16_t>(std::initializer_list<u_int16_t>({0, 1, 2})));
  REQUIRE_NOTHROW(
      PartialPerm<u_int16_t>(std::vector<u_int16_t>({0, UNDEF, 2})));
  REQUIRE_NOTHROW(PartialPerm<u_int16_t>(
      std::vector<u_int16_t>({0, UNDEF, 5, UNDEF, UNDEF, 1})));

  std::vector<u_int16_t> pimgs = {1, 2, 3};
  // REQUIRE_NOTHROW(PartialPerm<u_int16_t>(pimgs));
  REQUIRE_THROWS_AS(PartialPerm<u_int16_t>(std::vector<u_int16_t>({1, 2, 3})),
                    LibsemigroupsException);
  REQUIRE_THROWS_AS(
      PartialPerm<u_int16_t>(std::vector<u_int16_t>({UNDEF, UNDEF, 3})),
      LibsemigroupsException);
  REQUIRE_THROWS_AS(
      PartialPerm<u_int16_t>(std::vector<u_int16_t>({1, UNDEF, 1})),
      LibsemigroupsException);
  REQUIRE_THROWS_AS(PartialPerm<u_int16_t>(
                        std::vector<u_int16_t>({3, UNDEF, 2, 1, UNDEF, 3})),
                    LibsemigroupsException);
  REQUIRE_THROWS_AS(
      PartialPerm<u_int16_t>(std::initializer_list<u_int16_t>({1, 2, 3})),
      LibsemigroupsException);
  REQUIRE_NOTHROW(PartialPerm<u_int16_t>(
      std::vector<u_int16_t>({1, 2}), std::vector<u_int16_t>({0, 3}), 5));
  REQUIRE_NOTHROW(PartialPerm<u_int16_t>(
      std::vector<u_int16_t>({1, 2}), std::vector<u_int16_t>({0, 5}), 6));
  REQUIRE_THROWS_AS(PartialPerm<u_int16_t>(std::vector<u_int16_t>({1, 2}),
                                           std::vector<u_int16_t>({0}),
                                           5),
                    LibsemigroupsException);
  REQUIRE_THROWS_AS(PartialPerm<u_int16_t>(std::vector<u_int16_t>({1, 2}),
                                           std::vector<u_int16_t>({0, 5}),
                                           4),
                    LibsemigroupsException);
  REQUIRE_THROWS_AS(PartialPerm<u_int16_t>(std::vector<u_int16_t>({1, 5}),
                                           std::vector<u_int16_t>({0, 2}),
                                           4),
                    LibsemigroupsException);
}

TEST_CASE("BooleanMat 01: methods", "[quick][element][booleanmat][01]") {
  auto x = BooleanMat({{1, 0, 1}, {0, 1, 0}, {0, 1, 0}});
  auto y = BooleanMat({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}});
  auto z = BooleanMat({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}});
  REQUIRE(y == z);
  z.redefine(x, y);
  REQUIRE(y == z);
  z.redefine(y, x);
  REQUIRE(y == z);
  REQUIRE(!(y < z));
  REQUIRE(x.degree() == 3);
  REQUIRE(y.degree() == 3);
  REQUIRE(z.degree() == 3);
  REQUIRE(x.complexity() == 27);
  REQUIRE(y.complexity() == 27);
  REQUIRE(z.complexity() == 27);
  auto id = x.identity();
  z.redefine(id, x);
  REQUIRE(z == x);
  z.redefine(x, id);
  REQUIRE(z == x);
}

TEST_CASE("BooleanMat 02: hash", "[quick][element][booleanmat][02]") {
  Element* x = new BooleanMat({{1, 0, 1}, {0, 1, 0}, {0, 1, 0}});
  for (size_t i = 0; i < 1000000; i++) {
    x->hash_value();
  }
  delete x;
}

TEST_CASE("BooleanMat 03: delete/copy", "[quick][element][booleanmat][03]") {
  Element* x = new BooleanMat({{1, 0, 1}, {0, 1, 0}, {0, 1, 0}});
  Element* y = x->heap_copy();
  delete x;

  Element* expected = new BooleanMat({{1, 0, 1}, {0, 1, 0}, {0, 1, 0}});
  REQUIRE(*y == *expected);

  BooleanMat& yy = *static_cast<BooleanMat*>(y);
  REQUIRE(yy == *y);
  BooleanMat zz(yy);
  delete y;  // does not delete the _vector in y, yy, or zz
  REQUIRE(zz == *expected);
  delete expected;
}

TEST_CASE("Bipartition 01: overridden methods",
          "[quick][element][bipart][01]") {
  auto x = Bipartition(
      {0, 1, 2, 1, 0, 2, 1, 0, 2, 2, 0, 0, 2, 0, 3, 4, 4, 1, 3, 0});
  auto y = Bipartition(
      {0, 1, 1, 1, 1, 2, 3, 2, 4, 5, 5, 2, 4, 2, 1, 1, 1, 2, 3, 2});
  auto z = Bipartition(
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
  REQUIRE(!(y == z));

  z.redefine(x, y, 0);
  auto expected = Bipartition(
      {0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1});
  REQUIRE(z == expected);

  expected = Bipartition(
      {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 3, 1, 2, 1});
  z.redefine(y, x, 0);
  REQUIRE(z == expected);

  REQUIRE(!(y < z));
  REQUIRE(x.degree() == 10);
  REQUIRE(y.degree() == 10);
  REQUIRE(z.degree() == 10);
  REQUIRE(x.complexity() == 100);
  REQUIRE(y.complexity() == 100);
  REQUIRE(z.complexity() == 100);

  auto id = x.identity();
  z.redefine(id, x, 0);
  REQUIRE(z == x);
  z.redefine(x, id, 0);
  REQUIRE(z == x);
  z.redefine(id, y, 0);
  REQUIRE(z == y);
  z.redefine(y, id, 0);
  REQUIRE(z == y);
}

TEST_CASE("Bipartition 02: hash", "[quick][element][bipart][02]") {
  Element* x = new Bipartition(
      {0, 1, 2, 1, 0, 2, 1, 0, 2, 2, 0, 0, 2, 0, 3, 4, 4, 1, 3, 0});
  for (size_t i = 0; i < 1000000; i++) {
    x->hash_value();
  }
  delete x;
}

TEST_CASE("Bipartition 03: non-overridden methods",
          "[quick][element][bipart][03]") {
  Bipartition* x = new Bipartition(
      {0, 0, 0, 0, 0, 0, 1, 2, 0, 1, 0, 0, 1, 2, 3, 3, 0, 4, 1, 1});

  REQUIRE(x->rank() == 3);
  REQUIRE(x->at(0) == 0);
  REQUIRE(x->at(6) == 1);
  REQUIRE(x->at(10) == 0);
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
  delete x;
  delete y;

  x = new Bipartition(
      {0, 0, 0, 0, 0, 0, 1, 2, 0, 1, 0, 0, 1, 2, 3, 3, 0, 4, 1, 1});
  x->set_nr_blocks(5);
  REQUIRE(x->nr_blocks() == 5);
  delete x;

  x = new Bipartition(
      {0, 0, 0, 0, 0, 0, 1, 2, 0, 1, 0, 0, 1, 2, 3, 3, 0, 4, 1, 1});
  x->set_nr_left_blocks(3);
  REQUIRE(x->nr_left_blocks() == 3);
  REQUIRE(x->nr_right_blocks() == 5);
  REQUIRE(x->nr_blocks() == 5);
  delete x;

  x = new Bipartition(
      {0, 0, 0, 0, 0, 0, 1, 2, 0, 1, 0, 0, 1, 2, 3, 3, 0, 4, 1, 1});
  x->set_rank(3);
  REQUIRE(x->rank() == 3);
  delete x;
}

TEST_CASE("Bipartition 04: delete/copy", "[quick][element][bipart][04]") {
  Element* x = new Bipartition({0, 0, 0, 0});
  Element* y = x->heap_copy();
  delete x;

  Element* expected = new Bipartition({0, 0, 0, 0});
  REQUIRE(*y == *expected);

  Bipartition yy = *static_cast<Bipartition*>(y);
  REQUIRE(yy == *y);
  Bipartition zz(yy);
  delete y;  // does not delete the _vector in y, yy, or zz
  REQUIRE(zz == *expected);
  delete expected;
}

TEST_CASE("Bipartition 05: degree 0", "[quick][element][bipart][05]") {
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

  delete x;
}

TEST_CASE("Bipartition 06: exceptions", "[quick][bipart][06]") {
  REQUIRE_NOTHROW(Bipartition(std::vector<u_int32_t>()));
  REQUIRE_THROWS_AS(Bipartition({0}), LibsemigroupsException);
  REQUIRE_THROWS_AS(Bipartition({1, 0}), LibsemigroupsException);
}

TEST_CASE("ProjectiveMaxPlusMatrix 01: methods",
          "[quick][element][matrix][01]") {
  Semiring<int64_t>* sr = new MaxPlusSemiring();

  auto x = ProjectiveMaxPlusMatrix({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  auto expected
      = ProjectiveMaxPlusMatrix({{-4, 0, -2}, {-3, -2, -2}, {-1, -5, -1}}, sr);
  REQUIRE(x == expected);

  REQUIRE(x.semiring() == sr);

  auto y
      = ProjectiveMaxPlusMatrix({{LONG_MIN, 0, 0}, {0, 1, 0}, {1, -1, 0}}, sr);
  expected = ProjectiveMaxPlusMatrix(
      {{LONG_MIN, -1, -1}, {-1, 0, -1}, {0, -2, -1}}, sr);
  REQUIRE(y == expected);
  REQUIRE(!(x == y));

  y.redefine(x, x);
  expected
      = ProjectiveMaxPlusMatrix({{-2, -1, -1}, {-2, -2, -2}, {-1, 0, -1}}, sr);
  REQUIRE(y == expected);

  REQUIRE(x < y);
  REQUIRE(x.degree() == 3);
  REQUIRE(y.degree() == 3);
  REQUIRE(x.complexity() == 27);
  REQUIRE(y.complexity() == 27);
  auto id = x.identity();
  y.redefine(id, x);
  REQUIRE(y == x);
  y.redefine(x, id);
  REQUIRE(y == x);
  delete sr;
}

TEST_CASE("ProjectiveMaxPlusMatrix 02: hash", "[quick][element][matrix][02]") {
  Semiring<int64_t>* sr = new MaxPlusSemiring();
  Element*           x
      = new ProjectiveMaxPlusMatrix({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  for (size_t i = 0; i < 1000000; i++) {
    x->hash_value();
  }
  delete x;
  delete sr;
}

TEST_CASE("ProjectiveMaxPlusMatrix 03: delete/copy",
          "[quick][element][matrix][03]") {
  Semiring<int64_t>* sr = new MaxPlusSemiring();
  Element*           x
      = new ProjectiveMaxPlusMatrix({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  Element* y = x->heap_copy();
  delete x;

  Element* expected
      = new ProjectiveMaxPlusMatrix({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  REQUIRE(*y == *expected);
  delete expected;

  ProjectiveMaxPlusMatrix yy = *static_cast<ProjectiveMaxPlusMatrix*>(y);
  REQUIRE(yy == *y);

  ProjectiveMaxPlusMatrix zz(yy);
  delete y;  // does not delete the _vector in y, yy, or zz
  expected
      = new ProjectiveMaxPlusMatrix({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  REQUIRE(zz == *expected);
  delete expected;
  delete sr;
}

TEST_CASE("MatrixOverSemiring 01: Integers methods",
          "[quick][element][matrix][01]") {
  Semiring<int64_t>* sr = new Integers();

  auto x
      = MatrixOverSemiring<int64_t>({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  auto expected
      = MatrixOverSemiring<int64_t>({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  REQUIRE(x == expected);

  auto y
      = MatrixOverSemiring<int64_t>({{-100, 0, 0}, {0, 1, 0}, {1, -1, 0}}, sr);
  REQUIRE(!(x == y));

  y.redefine(x, x);
  expected
      = MatrixOverSemiring<int64_t>({{2, -4, 0}, {2, -2, 0}, {2, -1, 1}}, sr);
  REQUIRE(y == expected);

  REQUIRE(x < y);
  REQUIRE(x.degree() == 3);
  REQUIRE(y.degree() == 3);
  REQUIRE(x.complexity() == 27);
  REQUIRE(y.complexity() == 27);
  auto id = x.identity();
  y.redefine(id, x);
  REQUIRE(y == x);
  y.redefine(x, id);
  REQUIRE(y == x);
  delete sr;
}

TEST_CASE("MatrixOverSemiring 02: Integers, hash",
          "[quick][element][matrix][02]") {
  Semiring<int64_t>* sr = new Integers();
  Element*           x  = new MatrixOverSemiring<int64_t>(
      {{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  for (size_t i = 0; i < 1000000; i++) {
    x->hash_value();
  }
  delete x;
  delete sr;
}

TEST_CASE("MatrixOverSemiring 03: MaxPlusSemiring methods",
          "[quick][element][matrix][03]") {
  Semiring<int64_t>* sr = new MaxPlusSemiring();

  auto x
      = MatrixOverSemiring<int64_t>({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  auto expected
      = MatrixOverSemiring<int64_t>({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  REQUIRE(x == expected);

  auto y
      = MatrixOverSemiring<int64_t>({{-100, 0, 0}, {0, 1, 0}, {1, -1, 0}}, sr);
  REQUIRE(!(x == y));

  y.redefine(x, x);
  expected = MatrixOverSemiring<int64_t>({{1, 2, 2}, {1, 1, 1}, {2, 3, 2}}, sr);
  REQUIRE(y == expected);

  REQUIRE(x < y);
  REQUIRE(x.degree() == 3);
  REQUIRE(y.degree() == 3);
  REQUIRE(x.complexity() == 27);
  REQUIRE(y.complexity() == 27);
  auto id = x.identity();
  y.redefine(id, x);
  REQUIRE(y == x);
  y.redefine(x, id);
  REQUIRE(y == x);
  delete sr;
}

TEST_CASE("MatrixOverSemiring 04: MaxPlusSemiring hash",
          "[quick][element][matrix][04]") {
  Semiring<int64_t>* sr = new MaxPlusSemiring();
  Element*           x  = new MatrixOverSemiring<int64_t>(
      {{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  for (size_t i = 0; i < 1000000; i++) {
    x->hash_value();
  }
  delete x;
  delete sr;
}

TEST_CASE("MatrixOverSemiring 05: MinPlusSemiring methods",
          "[quick][element][matrix][05]") {
  Semiring<int64_t>* sr = new MinPlusSemiring();

  auto x
      = MatrixOverSemiring<int64_t>({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  auto expected
      = MatrixOverSemiring<int64_t>({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  REQUIRE(x == expected);

  auto y
      = MatrixOverSemiring<int64_t>({{-100, 0, 0}, {0, 1, 0}, {1, -1, 0}}, sr);
  REQUIRE(!(x == y));

  y.redefine(x, x);
  expected = MatrixOverSemiring<int64_t>(
      {{-4, -3, -2}, {-3, -3, -1}, {-4, -3, -3}}, sr);
  REQUIRE(y == expected);

  REQUIRE(!(x < y));
  REQUIRE(x.degree() == 3);
  REQUIRE(y.degree() == 3);
  REQUIRE(x.complexity() == 27);
  REQUIRE(y.complexity() == 27);
  auto id = x.identity();
  y.redefine(id, x);
  REQUIRE(y == x);
  y.redefine(x, id);
  REQUIRE(y == x);
  delete sr;
}

TEST_CASE("MatrixOverSemiring 06: MinPlusSemiring hash",
          "[quick][element][matrix][06]") {
  Semiring<int64_t>* sr = new MinPlusSemiring();
  Element*           x  = new MatrixOverSemiring<int64_t>(
      {{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  for (size_t i = 0; i < 1000000; i++) {
    x->hash_value();
  }
  delete x;
  delete sr;
}

TEST_CASE("MatrixOverSemiring 07: TropicalMaxPlusSemiring methods",
          "[quick][element][matrix][07]") {
  Semiring<int64_t>* sr = new TropicalMaxPlusSemiring(33);

  auto x
      = MatrixOverSemiring<int64_t>({{22, 21, 0}, {10, 0, 0}, {1, 32, 1}}, sr);
  auto expected
      = MatrixOverSemiring<int64_t>({{22, 21, 0}, {10, 0, 0}, {1, 32, 1}}, sr);
  REQUIRE(x == expected);

  REQUIRE_THROWS_AS(
      MatrixOverSemiring<int64_t>({{-100, 0, 0}, {0, 1, 0}, {1, -1, 0}}, sr),
      LibsemigroupsException);
  auto y = MatrixOverSemiring<int64_t>({{10, 0, 0}, {0, 1, 0}, {1, 1, 0}}, sr);
  REQUIRE(!(x == y));

  y.redefine(x, x);
  expected = MatrixOverSemiring<int64_t>(
      {{33, 33, 22}, {32, 32, 10}, {33, 33, 32}}, sr);
  REQUIRE(y == expected);

  REQUIRE(x < y);
  REQUIRE(x.degree() == 3);
  REQUIRE(y.degree() == 3);
  REQUIRE(x.complexity() == 27);
  REQUIRE(y.complexity() == 27);
  auto id = x.identity();
  y.redefine(id, x);
  REQUIRE(y == x);
  y.redefine(x, id);
  REQUIRE(y == x);
  delete sr;
}

TEST_CASE("MatrixOverSemiring 08: TropicalMaxPlusSemiring hash",
          "[quick][element][matrix][08]") {
  Semiring<int64_t>* sr = new TropicalMaxPlusSemiring(33);
  Element*           x  = new MatrixOverSemiring<int64_t>(
      {{22, 21, 0}, {10, 0, 0}, {1, 32, 1}}, sr);
  for (size_t i = 0; i < 1000000; i++) {
    x->hash_value();
  }
  delete x;
  delete sr;
}

TEST_CASE("MatrixOverSemiring 09: TropicalMinPlusSemiring methods",
          "[quick][element][matrix][09]") {
  Semiring<int64_t>* sr = new TropicalMinPlusSemiring(33);

  auto x
      = MatrixOverSemiring<int64_t>({{22, 21, 0}, {10, 0, 0}, {1, 32, 1}}, sr);

  auto expected
      = MatrixOverSemiring<int64_t>({{22, 21, 0}, {10, 0, 0}, {1, 32, 1}}, sr);
  REQUIRE(x == expected);

  auto y = MatrixOverSemiring<int64_t>({{10, 0, 0}, {0, 1, 0}, {1, 1, 0}}, sr);
  REQUIRE(!(x == y));

  y.redefine(x, x);
  expected
      = MatrixOverSemiring<int64_t>({{1, 21, 1}, {1, 0, 0}, {2, 22, 1}}, sr);
  REQUIRE(y == expected);

  REQUIRE(!(x < y));
  REQUIRE(x.degree() == 3);
  REQUIRE(y.degree() == 3);
  REQUIRE(x.complexity() == 27);
  REQUIRE(y.complexity() == 27);
  auto id = x.identity();
  y.redefine(id, x);
  REQUIRE(y == x);
  y.redefine(x, id);
  REQUIRE(y == x);
  delete sr;
}

TEST_CASE("MatrixOverSemiring 10: TropicalMinPlusSemiring hash",
          "[quick][element][matrix][10]") {
  Semiring<int64_t>* sr = new TropicalMinPlusSemiring(33);
  Element*           x  = new MatrixOverSemiring<int64_t>(
      {{22, 21, 0}, {10, 0, 0}, {1, 32, 1}}, sr);
  for (size_t i = 0; i < 1000000; i++) {
    x->hash_value();
  }
  delete x;
  delete sr;
}

TEST_CASE("MatrixOverSemiring 11: NaturalSemiring methods",
          "[quick][element][matrix][11]") {
  Semiring<int64_t>* sr = new NaturalSemiring(33, 2);

  auto x
      = MatrixOverSemiring<int64_t>({{22, 21, 0}, {10, 0, 0}, {1, 32, 1}}, sr);
  auto expected
      = MatrixOverSemiring<int64_t>({{22, 21, 0}, {10, 0, 0}, {1, 32, 1}}, sr);
  REQUIRE(x == expected);

  auto y = MatrixOverSemiring<int64_t>({{10, 0, 0}, {0, 1, 0}, {1, 1, 0}}, sr);
  REQUIRE(!(x == y));

  y.redefine(x, x);
  expected = MatrixOverSemiring<int64_t>(
      {{34, 34, 0}, {34, 34, 0}, {33, 33, 1}}, sr);
  REQUIRE(y == expected);

  REQUIRE(x < y);
  REQUIRE(x.degree() == 3);
  REQUIRE(y.degree() == 3);
  REQUIRE(x.complexity() == 27);
  REQUIRE(y.complexity() == 27);
  auto id = x.identity();
  y.redefine(id, x);
  REQUIRE(y == x);
  y.redefine(x, id);
  REQUIRE(y == x);
  delete sr;
}

TEST_CASE("MatrixOverSemiring 12: NaturalSemiring hash",
          "[quick][element][matrix][12]") {
  Semiring<int64_t>* sr = new NaturalSemiring(33, 2);
  Element*           x  = new MatrixOverSemiring<int64_t>(
      {{22, 21, 0}, {10, 0, 0}, {1, 32, 1}}, sr);

  for (size_t i = 0; i < 1000000; i++) {
    x->hash_value();
  }
  delete x;
  delete sr;
}

TEST_CASE("MatrixOverSemiring 13: Integers delete/copy",
          "[quick][element][matrix][13]") {
  Semiring<int64_t>* sr = new Integers();
  Element*           x  = new MatrixOverSemiring<int64_t>(
      {{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  Element* y = x->heap_copy();

  delete x;
  Element* expected = new MatrixOverSemiring<int64_t>(
      {{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  REQUIRE(*y == *expected);
  delete expected;

  MatrixOverSemiring<int64_t> yy
      = *static_cast<MatrixOverSemiring<int64_t>*>(y);
  REQUIRE(yy == *y);
  MatrixOverSemiring<int64_t> zz(yy);
  delete y;  // does not delete the _vector in y, yy, or zz
  REQUIRE(zz == yy);
  delete sr;
}

TEST_CASE("MatrixOverSemiring 14: MaxPlusSemiring delete/copy",
          "[quick][element][matrix][14]") {
  Semiring<int64_t>* sr = new MaxPlusSemiring();
  Element*           x  = new MatrixOverSemiring<int64_t>(
      {{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  Element* y = x->heap_copy();

  delete x;
  Element* expected = new MatrixOverSemiring<int64_t>(
      {{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  REQUIRE(*y == *expected);
  delete expected;

  MatrixOverSemiring<int64_t> yy
      = *static_cast<MatrixOverSemiring<int64_t>*>(y);
  REQUIRE(yy == *y);
  MatrixOverSemiring<int64_t> zz(yy);
  delete y;  // does not delete the _vector in y, yy, or zz
  REQUIRE(zz == yy);
  delete sr;
}

TEST_CASE("MatrixOverSemiring 15: MinPlusSemiring delete/copy",
          "[quick][element][matrix][15]") {
  Semiring<int64_t>* sr = new MinPlusSemiring();
  Element*           x  = new MatrixOverSemiring<int64_t>(
      {{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  Element* y = x->heap_copy();

  delete x;
  Element* expected = new MatrixOverSemiring<int64_t>(
      {{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
  REQUIRE(*y == *expected);
  delete expected;

  MatrixOverSemiring<int64_t> yy
      = *static_cast<MatrixOverSemiring<int64_t>*>(y);
  REQUIRE(yy == *y);
  MatrixOverSemiring<int64_t> zz(yy);
  delete y;  // does not delete the _vector in y, yy, or zz
  REQUIRE(zz == yy);
  delete sr;
}

TEST_CASE("MatrixOverSemiring 16: TropicalMaxPlusSemiring delete/copy",
          "[quick][element][matrix][16]") {
  Semiring<int64_t>* sr = new TropicalMaxPlusSemiring(23);
  Element*           x
      = new MatrixOverSemiring<int64_t>({{2, 2, 0}, {1, 0, 0}, {1, 3, 1}}, sr);
  Element* y = x->heap_copy();

  delete x;
  Element* expected
      = new MatrixOverSemiring<int64_t>({{2, 2, 0}, {1, 0, 0}, {1, 3, 1}}, sr);
  REQUIRE(*y == *expected);
  delete expected;

  MatrixOverSemiring<int64_t> yy
      = *static_cast<MatrixOverSemiring<int64_t>*>(y);
  REQUIRE(yy == *y);
  MatrixOverSemiring<int64_t> zz(yy);
  delete y;  // does not delete the _vector in y, yy, or zz
  REQUIRE(zz == yy);
  delete sr;
}

TEST_CASE("MatrixOverSemiring 17: TropicalMinPlusSemiring delete/copy",
          "[quick][element][matrix][17]") {
  Semiring<int64_t>* sr = new TropicalMinPlusSemiring(23);
  Element*           x
      = new MatrixOverSemiring<int64_t>({{2, 2, 0}, {1, 0, 0}, {1, 3, 1}}, sr);
  Element* y = x->heap_copy();

  delete x;
  Element* expected
      = new MatrixOverSemiring<int64_t>({{2, 2, 0}, {1, 0, 0}, {1, 3, 1}}, sr);
  REQUIRE(*y == *expected);
  delete expected;

  MatrixOverSemiring<int64_t> yy
      = *static_cast<MatrixOverSemiring<int64_t>*>(y);
  REQUIRE(yy == *y);
  MatrixOverSemiring<int64_t> zz(yy);
  delete y;  // does not delete the _vector in y, yy, or zz
  REQUIRE(zz == yy);
  delete sr;
}

TEST_CASE("MatrixOverSemiring 18: NaturalSemiring delete/copy",
          "[quick][element][matrix][18]") {
  Semiring<int64_t>* sr = new NaturalSemiring(23, 1);
  Element*           x
      = new MatrixOverSemiring<int64_t>({{2, 2, 0}, {1, 0, 0}, {1, 3, 1}}, sr);
  Element* y = x->heap_copy();

  delete x;
  Element* expected
      = new MatrixOverSemiring<int64_t>({{2, 2, 0}, {1, 0, 0}, {1, 3, 1}}, sr);
  REQUIRE(*y == *expected);
  delete expected;

  MatrixOverSemiring<int64_t> yy
      = *static_cast<MatrixOverSemiring<int64_t>*>(y);
  REQUIRE(yy == *y);
  MatrixOverSemiring<int64_t> zz(yy);
  delete y;  // does not delete the _vector in y, yy, or zz
  REQUIRE(zz == yy);
  delete sr;
}

TEST_CASE("MatrixOverSemiring 19: exceptions", "[quick][element][matrix][18]") {
  Semiring<int64_t>* sr = new NaturalSemiring(23, 1);
  REQUIRE_THROWS_AS(MatrixOverSemiring<int64_t>({{0, 0}, {0, 0}}, nullptr),
                    LibsemigroupsException);
  REQUIRE_THROWS_AS(
      MatrixOverSemiring<int64_t>(std::vector<std::vector<int64_t>>(), sr),
      LibsemigroupsException);
  REQUIRE_THROWS_AS(
      MatrixOverSemiring<int64_t>({{2, 2, 0}, {0, 0}, {1, 3, 1}}, sr),
      LibsemigroupsException);
  delete sr;
}

TEST_CASE("PBR 02: universal product", "[quick][element][pbr][02]") {
  Element* x = new PBR({{5, 3},
                        {5, 4, 3, 0, 1, 2},
                        {5, 4, 3, 0, 2},
                        {5, 3, 0, 1, 2},
                        {5, 0, 2},
                        {5, 4, 3, 1, 2}});
  Element* y = new PBR({{5, 4, 3, 0},
                        {5, 4, 2},
                        {5, 1, 2},
                        {5, 4, 3, 2},
                        {5, 4, 3, 2},
                        {4, 1, 2}});

  Element* z = new PBR({{5, 4, 3, 0},
                        {5, 4, 2},
                        {5, 1, 2},
                        {5, 4, 3, 2},
                        {5, 4, 3, 2},
                        {4, 1, 2}});
  z->redefine(x, y);

  Element* expected = new PBR({{0, 1, 2, 3, 4, 5},
                               {0, 1, 2, 3, 4, 5},
                               {0, 1, 2, 3, 4, 5},
                               {0, 1, 2, 3, 4, 5},
                               {0, 1, 2, 3, 4, 5},
                               {0, 1, 2, 3, 4, 5}});
  REQUIRE(*z == *expected);

  delete x;
  delete y;
  delete z;
  delete expected;
}

TEST_CASE("PBR 03: product [bigger than previous]",
          "[quick][element][pbr][03]") {
  Element* x = new PBR({{5, 3},
                        {5, 4, 3, 0, 1, 2},
                        {5, 4, 3, 0, 2},
                        {5, 3, 0, 1, 2},
                        {5, 0, 2},
                        {5, 4, 3, 1, 2},
                        {},
                        {}});
  Element* y = new PBR({{5, 3},
                        {5, 4, 3, 0, 1, 2},
                        {5, 4, 3, 0, 2},
                        {5, 3, 0, 1, 2},
                        {5, 0, 2},
                        {5, 4, 3, 1, 2},
                        {},
                        {6}});
  x->redefine(y, y);
  Element* expected = new PBR({{0, 1, 2, 3, 4, 5},
                               {0, 1, 2, 3, 4, 5},
                               {0, 1, 2, 3, 4, 5},
                               {0, 1, 2, 3, 4, 5},
                               {0, 1, 2, 3, 4, 5},
                               {0, 1, 2, 3, 4, 5},
                               {},
                               {6}});

  REQUIRE(*x == *expected);

  delete x;
  delete y;
  delete expected;

  x = new PBR(
      {{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {7}});
  y = new PBR(
      {{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {7}});

  x->redefine(y, y);
  expected = new PBR(
      {{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {7}});
  REQUIRE(*x == *expected);

  delete x;
  delete y;
  delete expected;
}

TEST_CASE("PBR 04: hash", "[quick][element][pbr][04]") {
  Element* x = new PBR({{1}, {4}, {3}, {1}, {0, 2}, {0, 3, 4, 5}});
  for (size_t i = 0; i < 1000000; i++) {
    x->hash_value();
  }
  delete x;
}

TEST_CASE("PBR 05: delete/copy", "[quick][element][pbr][05]") {
  Element* x = new PBR({{1}, {4}, {3}, {1}, {0, 2}, {0, 3, 4, 5}});
  Element* y = x->heap_copy();
  delete x;
  Element* z = new PBR({{1}, {4}, {3}, {1}, {0, 2}, {0, 3, 4, 5}});
  REQUIRE(*y == *z);
  delete z;
  PBR yy = *static_cast<PBR*>(y);
  REQUIRE(yy == *y);
  PBR zz(yy);
  delete y;  // does not delete the _vector in y, yy, or zz
  Element* a = new PBR({{1}, {4}, {3}, {1}, {0, 2}, {0, 3, 4, 5}});
  REQUIRE(zz == *a);
  delete a;
}

TEST_CASE("PBR 06: exceptions", "[quick][element][pbr][06]") {
  REQUIRE_THROWS_AS(PBR({{1}, {4}, {3}, {10}, {0, 2}, {0, 3, 4, 5}}),
                    LibsemigroupsException);
  REQUIRE_THROWS_AS(PBR({{4}, {3}, {0}, {0, 2}, {0, 3, 4, 5}}),
                    LibsemigroupsException);
}

template <class T> bool test_inverse(Permutation<T> const& p) {
  return p * p.inverse() == p.identity() && p.inverse() * p == p.identity();
}

TEST_CASE("Permutation 01: inverse", "[quick][element][permutation][01]") {
  // Those two constructor if not passed a vector return an element
  // with _vector set to null (see issue #87).
  // REQUIRE(test_inverse(Permutation<u_int16_t>({})));
  // REQUIRE(test_inverse(Permutation<u_int16_t>({0})));
  REQUIRE(test_inverse(Permutation<u_int16_t>({1, 0})));
  REQUIRE(test_inverse(Permutation<u_int16_t>({0, 1})));
  REQUIRE(test_inverse(Permutation<u_int16_t>({2, 0, 1, 4, 3})));
  REQUIRE(test_inverse(Permutation<u_int16_t>({4, 2, 0, 1, 3})));
  REQUIRE(test_inverse(Permutation<u_int16_t>({0, 1, 2, 3, 4})));
}

TEST_CASE("Permutation 02: exceptions", "[quick][element][permutation][02]") {
  REQUIRE_NOTHROW(Permutation<u_int16_t>(std::vector<u_int16_t>({})));
  REQUIRE_NOTHROW(Permutation<u_int16_t>(std::vector<u_int16_t>({0})));
  REQUIRE_NOTHROW(Permutation<u_int16_t>(std::vector<u_int16_t>({0, 1})));
  REQUIRE_NOTHROW(Permutation<u_int16_t>(std::vector<u_int16_t>({1, 0})));
  REQUIRE_THROWS_AS(PartialPerm<u_int16_t>(std::vector<u_int16_t>({1, 2})),
                    LibsemigroupsException);
  REQUIRE_THROWS_AS(PartialPerm<u_int16_t>(std::vector<u_int16_t>({1, 0, 3})),
                    LibsemigroupsException);
  REQUIRE_NOTHROW(
      Permutation<u_int16_t>(std::vector<u_int16_t>({1, 4, 0, 3, 2})));
  REQUIRE_THROWS_AS(
      PartialPerm<u_int16_t>(std::vector<u_int16_t>({1, 0, 3, 6, 4})),
      LibsemigroupsException);
  REQUIRE_THROWS_AS(
      PartialPerm<u_int16_t>(std::vector<u_int16_t>({1, 5, 0, 3, 2})),
      LibsemigroupsException);
}

TEST_CASE("SmallestInteger 01", "[quick][helpers][01]") {
  REQUIRE(sizeof(SmallestInteger<0>::type) == 1);
  REQUIRE(sizeof(SmallestInteger<255>::type) == 1);
  REQUIRE(sizeof(SmallestInteger<256>::type) == 2);
  REQUIRE(sizeof(SmallestInteger<65535>::type) == 2);
  REQUIRE(sizeof(SmallestInteger<65536>::type) == 4);
#if LIBSEMIGROUPS_SIZEOF_VOID_P == 8
  REQUIRE(sizeof(SmallestInteger<4294967295>::type) == 4);
  REQUIRE(sizeof(SmallestInteger<4294967296>::type) == 8);
#endif
}

TEST_CASE("Transf 02", "[quick][helpers][02]") {
  auto x = Transf<3>::type({0, 1, 2});
  (void) x;
  auto y = PPerm<3>::type({0, 1, 2});
  (void) y;
  auto z = Perm<3>::type({0, 1, 2});
  (void) z;
  auto a = BMat<3>::type({{0, 1}, {0, 1}});
  (void) a;
}
