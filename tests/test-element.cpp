//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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

#include <cstddef>  // for size_t
#include <cstdint>  // for uint32_t, int32_t, int64_t
#include <vector>   // for vector

#include "catch.hpp"                               // for TEST_CASE
#include "libsemigroups/blocks.hpp"                // for Blocks
#include "libsemigroups/bmat8.hpp"                 // for BMat8
#include "libsemigroups/element-adapters.hpp"      // for Degree etc
#include "libsemigroups/element-helper.hpp"        // for TransfHelper
#include "libsemigroups/element.hpp"               // for Bipartition, Element
#include "libsemigroups/libsemigroups-config.hpp"  // for LIBSEMIGROUPS_SIZEOF_VO...
#include "libsemigroups/types.hpp"  // for SmallestInteger, Smalle...
#include "test-main.hpp"            // LIBSEMIGROUPS_TEST_CASE

namespace libsemigroups {
  struct LibsemigroupsException;

  LIBSEMIGROUPS_TEST_CASE("Element",
                          "001",
                          "comparison operators",
                          "[quick][element]") {
    auto x = Transformation<uint16_t>({0, 1, 0});
    auto y = Transformation<uint16_t>({0, 1});
    REQUIRE(x > y);
  }

  LIBSEMIGROUPS_TEST_CASE("Transformation",
                          "001",
                          "uint16_t methods",
                          "[quick][element]") {
    auto x = Transformation<uint16_t>({0, 1, 0});
    auto y = Transformation<uint16_t>({0, 1, 0});
    REQUIRE(x == y);
    REQUIRE(y * y == x);
    REQUIRE((x < y) == false);

    auto z = Transformation<uint16_t>({0, 1, 0, 3});
    REQUIRE(x < z);

    auto expected = Transformation<uint16_t>({0, 0, 0});
    REQUIRE(expected < x);

    REQUIRE(x.degree() == 3);
    REQUIRE(y.degree() == 3);
    REQUIRE(x.complexity() == 3);
    REQUIRE(y.complexity() == 3);
    REQUIRE(x.crank() == 2);
    REQUIRE(y.crank() == 2);
    auto id = x.identity();

    expected = Transformation<uint16_t>({0, 1, 2});
    REQUIRE(id == expected);

    x.increase_degree_by(10);
    REQUIRE(x.degree() == 13);
    REQUIRE(x.end() - x.begin() == 13);
  }

  LIBSEMIGROUPS_TEST_CASE("Transformation",
                          "002",
                          "uint16_t hash",
                          "[quick][element]") {
    Element* x = new Transformation<uint16_t>({9, 7, 3, 5, 3, 4, 2, 7, 7, 1});
    for (size_t i = 0; i < 1000000; i++) {
      x->hash_value();
    }
    delete x;
  }

  // Transformation 003 was deleted

  LIBSEMIGROUPS_TEST_CASE("Transformation",
                          "004",
                          "uint32_t methods",
                          "[quick][element]") {
    Element* x = new Transformation<uint32_t>({0, 1, 0});
    Element* y = new Transformation<uint32_t>({0, 1, 0});
    REQUIRE(*x == *y);
    x->redefine(y, y);
    REQUIRE(*x == *y);
    REQUIRE((*x < *y) == false);
    Element* expected = new Transformation<uint32_t>({0, 0, 0});
    REQUIRE(*expected < *x);

    delete expected;

    REQUIRE(x->degree() == 3);
    REQUIRE(y->degree() == 3);
    REQUIRE(x->complexity() == 3);
    REQUIRE(y->complexity() == 3);
    REQUIRE(static_cast<Transformation<uint32_t>*>(x)->crank() == 2);
    REQUIRE(static_cast<Transformation<uint32_t>*>(y)->crank() == 2);
    Transformation<uint32_t> id
        = static_cast<Transformation<uint32_t>*>(x)->identity();

    expected = new Transformation<uint32_t>({0, 1, 2});
    REQUIRE(id == *expected);
    delete expected;

    delete x;
    delete y;
  }

  LIBSEMIGROUPS_TEST_CASE("Transformation",
                          "005",
                          "uint32_t hash",
                          "[quick][element]") {
    Element* x = new Transformation<uint32_t>({9, 7, 3, 5, 3, 4, 2, 7, 7, 1});
    for (size_t i = 0; i < 1000000; i++) {
      x->hash_value();
    }
    delete x;
  }

  // Transformation 006 deleted

  LIBSEMIGROUPS_TEST_CASE("Transformation",
                          "007",
                          "exceptions",
                          "[quick][element]") {
    REQUIRE_NOTHROW(Transformation<uint16_t>(std::vector<uint16_t>()));
    REQUIRE_NOTHROW(Transformation<uint16_t>(std::vector<uint16_t>({0})));
    REQUIRE_THROWS_AS(Transformation<uint16_t>(std::vector<uint16_t>({1})),
                      LibsemigroupsException);

    REQUIRE_NOTHROW(Transformation<uint16_t>(std::vector<uint16_t>({0, 1, 2})));
    REQUIRE_NOTHROW(
        Transformation<uint16_t>(std::initializer_list<uint16_t>({0, 1, 2})));
    // Implicit type initializer lists are not accepted.
    // REQUIRE_NOTHROW(Transformation<uint16_t>({0, 1, 2})));

    std::vector<uint16_t> pimgs = {1, 2, 3};
    // REQUIRE_NOTHROW(Transformation<uint16_t>(pimgs));
    REQUIRE_THROWS_AS(Transformation<uint16_t>({1, 2, 3}),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(
        Transformation<uint16_t>(std::initializer_list<uint16_t>({1, 2, 3})),
        LibsemigroupsException);

    REQUIRE_THROWS_AS(Transformation<uint16_t>(std::initializer_list<uint16_t>(
                          {UNDEFINED, UNDEFINED, UNDEFINED})),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("PartialPerm",
                          "001",
                          "uint16_t methods",
                          "[quick][element]") {
    auto x = PartialPerm<uint16_t>({4, 5, 0}, {9, 0, 1}, 10);
    auto y = PartialPerm<uint16_t>({4, 5, 0}, {9, 0, 1}, 10);
    REQUIRE(x == y);
    auto yy = x * x;
    REQUIRE(yy.at(0) == UNDEFINED);
    REQUIRE(yy.at(1) == UNDEFINED);
    REQUIRE(yy.at(2) == UNDEFINED);
    REQUIRE(yy.at(3) == UNDEFINED);
    REQUIRE(yy.at(4) == UNDEFINED);
    REQUIRE(yy.at(5) == 1);

    REQUIRE(yy < y);
    REQUIRE(!(x < x));
    auto expected = PartialPerm<uint16_t>({UNDEFINED, UNDEFINED, UNDEFINED});
    REQUIRE(expected < x);

    REQUIRE(x.degree() == 10);
    REQUIRE(y.degree() == 10);
    REQUIRE(x.complexity() == 10);
    REQUIRE(y.complexity() == 10);
    REQUIRE(yy.crank() == 1);
    REQUIRE(y.crank() == 3);
    REQUIRE(x.crank() == 3);
    auto id = x.identity();

    expected = PartialPerm<uint16_t>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
    REQUIRE(id == expected);

    x.increase_degree_by(10);
    REQUIRE(x.degree() == 20);
    REQUIRE(x.end() - x.begin() == x.degree());
  }

  LIBSEMIGROUPS_TEST_CASE("PartialPerm",
                          "002",
                          "uint16_t hash",
                          "[quick][element]") {
    Element* x = new PartialPerm<uint16_t>(
        {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 10);
    for (size_t i = 0; i < 1000000; i++) {
      x->hash_value();
    }
    delete x;
  }

  LIBSEMIGROUPS_TEST_CASE("PartialPerm",
                          "003",
                          "uint16_t delete/copy",
                          "[quick][element]") {
    Element* x = new PartialPerm<uint16_t>(
        {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 10);
    Element* y = x->heap_copy();
    delete x;

    Element* expected = new PartialPerm<uint16_t>(
        {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 10);
    REQUIRE(*y == *expected);

    PartialPerm<uint16_t> yy = *static_cast<PartialPerm<uint16_t>*>(y);
    REQUIRE(yy == *y);
    PartialPerm<uint16_t> zz(yy);
    delete y;  // does not delete the _vector in y, yy, or zz
    REQUIRE(zz == *expected);
    delete expected;
  }

  LIBSEMIGROUPS_TEST_CASE("PartialPerm",
                          "004",
                          "uint32_t methods",
                          "[quick][element]") {
    auto x = PartialPerm<uint32_t>({4, 5, 0}, {10, 0, 1}, 11);
    auto y = PartialPerm<uint32_t>({4, 5, 0}, {10, 0, 1}, 11);
    REQUIRE(x == y);
    auto xx = x * x;
    REQUIRE(xx.at(0) == UNDEFINED);
    REQUIRE(xx.at(1) == UNDEFINED);
    REQUIRE(xx.at(2) == UNDEFINED);
    REQUIRE(xx.at(3) == UNDEFINED);
    REQUIRE(xx.at(4) == UNDEFINED);
    REQUIRE(xx.at(5) == 1);
    REQUIRE((xx < y) == true);

    auto z = PartialPerm<uint32_t>({UNDEFINED, UNDEFINED, UNDEFINED});
    REQUIRE(z < x);

    REQUIRE(x.degree() == 11);
    REQUIRE(y.degree() == 11);
    REQUIRE(x.complexity() == 11);
    REQUIRE(y.complexity() == 11);
    REQUIRE(xx.crank() == 1);
    REQUIRE(x.crank() == 3);
    REQUIRE(y.crank() == 3);
    auto id = x.identity();

    auto expected = PartialPerm<uint32_t>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
    REQUIRE(id == expected);
  }

  LIBSEMIGROUPS_TEST_CASE("PartialPerm",
                          "005",
                          "uint32_t hash",
                          "[quick][element]") {
    Element* x = new PartialPerm<uint32_t>(
        {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 10);
    for (size_t i = 0; i < 1000000; i++) {
      x->hash_value();
    }
    delete x;
  }

  LIBSEMIGROUPS_TEST_CASE("PartialPerm",
                          "006",
                          "uint32_t delete/copy",
                          "[quick][element]") {
    Element* x = new PartialPerm<uint32_t>(
        {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 10);
    Element* y = x->heap_copy();
    delete x;

    Element* expected = new PartialPerm<uint32_t>(
        {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 10);
    REQUIRE(*y == *expected);

    PartialPerm<uint32_t> yy = *static_cast<PartialPerm<uint32_t>*>(y);
    REQUIRE(yy == *y);
    PartialPerm<uint32_t> zz(yy);
    delete y;  // does not delete the _vector in y, yy, or zz
    REQUIRE(zz == *expected);

    delete expected;
  }

  LIBSEMIGROUPS_TEST_CASE("PartialPerm",
                          "007",
                          "exceptions",
                          "[quick][element]") {
    REQUIRE_NOTHROW(PartialPerm<uint16_t>(std::vector<uint16_t>()));
    REQUIRE_NOTHROW(PartialPerm<uint16_t>(std::vector<uint16_t>({0})));
    REQUIRE_NOTHROW(PartialPerm<uint16_t>(std::vector<uint16_t>({UNDEFINED})));
    REQUIRE_THROWS_AS(PartialPerm<uint16_t>(std::vector<uint16_t>({1})),
                      LibsemigroupsException);

    REQUIRE_NOTHROW(PartialPerm<uint16_t>(std::vector<uint16_t>({0, 1, 2})));
    REQUIRE_NOTHROW(
        PartialPerm<uint16_t>(std::initializer_list<uint16_t>({0, 1, 2})));
    REQUIRE_NOTHROW(
        PartialPerm<uint16_t>(std::vector<uint16_t>({0, UNDEFINED, 2})));
    REQUIRE_NOTHROW(PartialPerm<uint16_t>(
        std::vector<uint16_t>({0, UNDEFINED, 5, UNDEFINED, UNDEFINED, 1})));

    std::vector<uint16_t> pimgs = {1, 2, 3};
    // REQUIRE_NOTHROW(PartialPerm<uint16_t>(pimgs));
    REQUIRE_THROWS_AS(PartialPerm<uint16_t>(std::vector<uint16_t>({1, 2, 3})),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(
        PartialPerm<uint16_t>(std::vector<uint16_t>({UNDEFINED, UNDEFINED, 3})),
        LibsemigroupsException);
    REQUIRE_THROWS_AS(
        PartialPerm<uint16_t>(std::vector<uint16_t>({1, UNDEFINED, 1})),
        LibsemigroupsException);
    REQUIRE_THROWS_AS(PartialPerm<uint16_t>(std::vector<uint16_t>(
                          {3, UNDEFINED, 2, 1, UNDEFINED, 3})),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(
        PartialPerm<uint16_t>(std::initializer_list<uint16_t>({1, 2, 3})),
        LibsemigroupsException);
    REQUIRE_NOTHROW(PartialPerm<uint16_t>(
        std::vector<uint16_t>({1, 2}), std::vector<uint16_t>({0, 3}), 5));
    REQUIRE_NOTHROW(PartialPerm<uint16_t>(
        std::vector<uint16_t>({1, 2}), std::vector<uint16_t>({0, 5}), 6));
    REQUIRE_THROWS_AS(PartialPerm<uint16_t>(std::vector<uint16_t>({1, 2}),
                                            std::vector<uint16_t>({0}),
                                            5),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(PartialPerm<uint16_t>(std::vector<uint16_t>({1, 2}),
                                            std::vector<uint16_t>({0, 5}),
                                            4),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(PartialPerm<uint16_t>(std::vector<uint16_t>({1, 5}),
                                            std::vector<uint16_t>({0, 2}),
                                            4),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(PartialPerm<uint16_t>(std::vector<uint16_t>({1, 1}),
                                            std::vector<uint16_t>({0, 2}),
                                            3),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Bipartition",
                          "001",
                          "overridden methods",
                          "[quick][element]") {
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

  LIBSEMIGROUPS_TEST_CASE("Bipartition", "002", "hash", "[quick][element]") {
    Element* x = new Bipartition(
        {0, 1, 2, 1, 0, 2, 1, 0, 2, 2, 0, 0, 2, 0, 3, 4, 4, 1, 3, 0});
    for (size_t i = 0; i < 1000000; i++) {
      x->hash_value();
    }
    delete x;
  }

  LIBSEMIGROUPS_TEST_CASE("Bipartition",
                          "003",
                          "non-overridden methods",
                          "[quick][element]") {
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

  LIBSEMIGROUPS_TEST_CASE("Bipartition",
                          "004",
                          "delete/copy",
                          "[quick][element]") {
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

  LIBSEMIGROUPS_TEST_CASE("Bipartition",
                          "005",
                          "degree 0",
                          "[quick][element]") {
    Bipartition* x = new Bipartition(std::vector<uint32_t>({}));
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

  LIBSEMIGROUPS_TEST_CASE("Bipartition",
                          "006",
                          "exceptions",
                          "[quick][element]") {
    REQUIRE_NOTHROW(Bipartition(std::vector<uint32_t>()));
    REQUIRE_THROWS_AS(Bipartition({0}), LibsemigroupsException);
    REQUIRE_THROWS_AS(Bipartition({1, 0}), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Bipartition",
                          "007",
                          "convenience constructor",
                          "[quick][element]") {
    Bipartition* xx = new Bipartition(
        {0, 0, 0, 0, 0, 0, 1, 2, 0, 1, 0, 0, 1, 2, 3, 3, 0, 4, 1, 1});

    Bipartition* x = new Bipartition({{1, 2, 3, 4, 5, 6, 9, -1, -2, -7},
                                      {7, 10, -3, -9, -10},
                                      {8, -4},
                                      {-5, -6},
                                      {-8}});
    REQUIRE(*x == *xx);

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

    Bipartition* yy = new Bipartition(
        {0, 0, 1, 2, 3, 3, 0, 4, 1, 1, 0, 0, 0, 0, 0, 0, 1, 2, 0, 1});

    Bipartition* y = new Bipartition({{1, 2, 7, -1, -2, -3, -4, -5, -6, -9},
                                      {3, 9, 10, -7, -10},
                                      {4, -8},
                                      {5, 6},
                                      {8}});

    REQUIRE(*y == *yy);

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
    delete xx;
    delete yy;

    xx = new Bipartition(
        {0, 0, 0, 0, 0, 0, 1, 2, 0, 1, 0, 0, 1, 2, 3, 3, 0, 4, 1, 1});
    x = new Bipartition({{1, 2, 3, 4, 5, 6, 9, -1, -2, -7},
                         {7, 10, -3, -9, -10},
                         {8, -4},
                         {-5, -6},
                         {-8}});
    REQUIRE(*x == *xx);
    x->set_nr_blocks(5);
    REQUIRE(x->nr_blocks() == 5);
    delete x;
    delete xx;

    xx = new Bipartition(
        {0, 0, 0, 0, 0, 0, 1, 2, 0, 1, 0, 0, 1, 2, 3, 3, 0, 4, 1, 1});
    x = new Bipartition({{1, 2, 3, 4, 5, 6, 9, -1, -2, -7},
                         {7, 10, -3, -9, -10},
                         {8, -4},
                         {-5, -6},
                         {-8}});
    REQUIRE(*x == *xx);
    x->set_nr_left_blocks(3);
    REQUIRE(x->nr_left_blocks() == 3);
    REQUIRE(x->nr_right_blocks() == 5);
    REQUIRE(x->nr_blocks() == 5);
    delete x;
    delete xx;

    x = new Bipartition({{1, 2, 3, 4, 5, 6, 9, -1, -2, -7},
                         {7, 10, -3, -9, -10},
                         {8, -4},
                         {-5, -6},
                         {-8}});
    x->set_rank(3);
    REQUIRE(x->rank() == 3);
    delete x;

    REQUIRE_THROWS_AS(Bipartition({{0, 2, 3, 4, 5, 6, 9, -1, -2, -7},
                                   {7, 10, -3, -9, -10},
                                   {8, -4},
                                   {-5, -6},
                                   {-8}}),
                      LibsemigroupsException);

    REQUIRE_THROWS_AS(Bipartition({{1, 2, 3, 4, 5, 6, 9, 11, -1, -2, -7},
                                   {7, 10, -3, -9, -10},
                                   {8, -4},
                                   {-5, -6},
                                   {-8}}),
                      LibsemigroupsException);

    REQUIRE_THROWS_AS(Bipartition({{1, 2, 3, 4, 5, 6, 11, -1, -2, -7},
                                   {7, 10, -3, -9, -10},
                                   {8, -4},
                                   {-5, -6},
                                   {-8}}),
                      LibsemigroupsException);

    REQUIRE_THROWS_AS(Bipartition({{1, 2, 3, 4, 5, 6, -11, -1, -2, -7},
                                   {7, 10, -3, -9, -10},
                                   {8, -4},
                                   {-5, -6},
                                   {-8}}),
                      LibsemigroupsException);

    REQUIRE_THROWS_AS(Bipartition({{0, 2, 3, 4, 5, 6, 9, -1},
                                   {7, 10, -3, -9, -10},
                                   {8, -4},
                                   {-5, -6},
                                   {-8}}),
                      LibsemigroupsException);

    REQUIRE_THROWS_AS(Bipartition({{0, 2, 3, 4, 5, 6, 9, -1, -2},
                                   {7, 10, -3, -9, -10},
                                   {8, -4},
                                   {-5, -6},
                                   {-8}}),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Bipartition",
                          "008",
                          "force copy constructor over move constructor",
                          "[quick][element]") {
    std::vector<uint32_t> xx(
        {0, 1, 2, 1, 0, 2, 1, 0, 2, 2, 0, 0, 2, 0, 3, 4, 4, 1, 3, 0});
    auto                  x = Bipartition(xx);
    std::vector<uint32_t> yy(
        {0, 1, 1, 1, 1, 2, 3, 2, 4, 5, 5, 2, 4, 2, 1, 1, 1, 2, 3, 2});
    auto                  y = Bipartition(yy);
    std::vector<uint32_t> zz(
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    auto z = Bipartition(zz);
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

  LIBSEMIGROUPS_TEST_CASE("PBR",
                          "001",
                          "universal product with convenience constructor",
                          "[quick][element]") {
    Element* x = new PBR({{-3, -1}, {-3, -2, -1, 1, 2, 3}, {-3, -2, -1, 1, 3}},
                         {{-3, -1, 1, 2, 3}, {-3, 1, 3}, {-3, -2, -1, 2, 3}});

    Element* y = new PBR({{-3, -2, -1, 1}, {-3, -2, 3}, {-3, 2, 3}},
                         {{-3, -2, -1, 3}, {-3, -2, -1, 3}, {-2, 2, 3}});

    Element* z = new PBR({{-3, -2, -1, 1}, {-3, -2, 3}, {-3, 2, 3}},
                         {{-3, -2, -1, 3}, {-3, -2, -1, 3}, {-2, 2, 3}});

    Element* xx = new PBR({{3, 5},
                           {0, 1, 2, 3, 4, 5},
                           {0, 2, 3, 4, 5},
                           {0, 1, 2, 3, 5},
                           {0, 2, 5},
                           {1, 2, 3, 4, 5}});
    Element* yy = new PBR({{0, 3, 4, 5},
                           {2, 4, 5},
                           {1, 2, 5},
                           {2, 3, 4, 5},
                           {2, 3, 4, 5},
                           {1, 2, 4}});

    REQUIRE(*x == *xx);
    REQUIRE(*y == *yy);

    z->redefine(x, y);

    Element* expected = new PBR({{0, 1, 2, 3, 4, 5},
                                 {0, 1, 2, 3, 4, 5},
                                 {0, 1, 2, 3, 4, 5},
                                 {0, 1, 2, 3, 4, 5},
                                 {0, 1, 2, 3, 4, 5},
                                 {0, 1, 2, 3, 4, 5}});
    REQUIRE(*z == *expected);

    delete x;
    delete xx;
    delete y;
    delete yy;
    delete z;
    delete expected;
  }

  LIBSEMIGROUPS_TEST_CASE("PBR",
                          "002",
                          "universal product",
                          "[quick][element]") {
    Element* x = new PBR({{3, 5},
                          {0, 1, 2, 3, 4, 5},
                          {0, 2, 3, 4, 5},
                          {0, 1, 2, 3, 5},
                          {0, 2, 5},
                          {1, 2, 3, 4, 5}});
    Element* y = new PBR({{0, 3, 4, 5},
                          {2, 4, 5},
                          {1, 2, 5},
                          {2, 3, 4, 5},
                          {2, 3, 4, 5},
                          {1, 2, 4}});

    Element* z = new PBR({{0, 3, 4, 5},
                          {2, 4, 5},
                          {1, 2, 5},
                          {2, 3, 4, 5},
                          {2, 3, 4, 5},
                          {1, 2, 4}});
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

  LIBSEMIGROUPS_TEST_CASE("PBR",
                          "003",
                          "product [bigger than previous]",
                          "[quick][element]") {
    Element* x = new PBR({{3, 5},
                          {0, 1, 2, 3, 4, 5},
                          {0, 2, 3, 4, 5},
                          {0, 1, 2, 3, 5},
                          {0, 2, 5},
                          {1, 2, 3, 4, 5},
                          {},
                          {}});
    Element* y = new PBR({{3, 5},
                          {0, 1, 2, 3, 4, 5},
                          {0, 2, 3, 4, 5},
                          {0, 1, 2, 3, 5},
                          {0, 2, 5},
                          {1, 2, 3, 4, 5},
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

  LIBSEMIGROUPS_TEST_CASE("PBR", "004", "hash", "[quick][element]") {
    Element* x = new PBR({{1}, {4}, {3}, {1}, {0, 2}, {0, 3, 4, 5}});
    for (size_t i = 0; i < 1000000; i++) {
      x->hash_value();
    }
    delete x;
  }

  LIBSEMIGROUPS_TEST_CASE("PBR", "005", "delete/copy", "[quick][element]") {
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

  LIBSEMIGROUPS_TEST_CASE("PBR", "006", "exceptions", "[quick][element]") {
    REQUIRE_THROWS_AS(PBR({{1}, {4}, {3}, {10}, {0, 2}, {0, 3, 4, 5}}),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(PBR({{4}, {3}, {0}, {0, 2}, {0, 3, 4, 5}}),
                      LibsemigroupsException);

    REQUIRE_NOTHROW(PBR({{-3, -1}, {-3, -2, -1, 1, 2, 3}, {-3, -2, -1, 1, 3}},
                        {{-3, -1, 1, 2, 3}, {-3, 1, 3}, {-3, -2, -1, 2, 3}}));

    REQUIRE_NOTHROW(PBR({{}, {}}));

    REQUIRE_THROWS_AS(PBR({{-4, -1}, {-3, -2, -1, 1, 2, 3}, {-3, -2, -1, 1, 3}},
                          {{-3, -1, 1, 2, 3}, {-3, 1, 3}, {-3, -2, -1, 2, 3}}),
                      LibsemigroupsException);

    REQUIRE_THROWS_AS(PBR({{-4, -1}, {-3, -2, -1, 1, 2, 3}, {-3, -2, -1, 1, 3}},
                          {{-3, -1, 1, 2, 3}, {-3, 1, 3}, {-3, -2, -1, 2, 3}}),
                      LibsemigroupsException);

    REQUIRE_THROWS_AS(
        PBR({{-4, -1}, {-3, -2, -1, 1, 2, 3}, {-3, -2, -1, 1, 3}},
            {{-3, -1, 1, 2, 3}, {-3, 1, 3}, {-3, -2, -1, 2, 3}, {-1, -2}}),
        LibsemigroupsException);

    REQUIRE_THROWS_AS(PBR({{}, {2}, {1}, {3, 0}}), LibsemigroupsException);
  }

  template <typename T>
  bool test_inverse(Permutation<T> const& p) {
    return p * p.inverse() == p.identity() && p.inverse() * p == p.identity();
  }

  LIBSEMIGROUPS_TEST_CASE("Permutation", "001", "inverse", "[quick][element]") {
    // Those two constructor if not passed a vector return an element
    // with _vector set to null (see issue #87).
    // REQUIRE(test_inverse(Permutation<uint16_t>({})));
    // REQUIRE(test_inverse(Permutation<uint16_t>({0})));
    REQUIRE(test_inverse(Permutation<uint16_t>({1, 0})));
    REQUIRE(test_inverse(Permutation<uint16_t>({0, 1})));
    REQUIRE(test_inverse(Permutation<uint16_t>({2, 0, 1, 4, 3})));
    REQUIRE(test_inverse(Permutation<uint16_t>({4, 2, 0, 1, 3})));
    REQUIRE(test_inverse(Permutation<uint16_t>({0, 1, 2, 3, 4})));
  }

  LIBSEMIGROUPS_TEST_CASE("Permutation",
                          "002",
                          "exceptions",
                          "[quick][element]") {
    REQUIRE_NOTHROW(Permutation<uint16_t>(std::vector<uint16_t>({})));
    REQUIRE_NOTHROW(Permutation<uint16_t>(std::vector<uint16_t>({0})));
    REQUIRE_NOTHROW(Permutation<uint16_t>(std::vector<uint16_t>({0, 1})));
    REQUIRE_NOTHROW(Permutation<uint16_t>(std::vector<uint16_t>({1, 0})));
    REQUIRE_THROWS_AS(PartialPerm<uint16_t>(std::vector<uint16_t>({1, 2})),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(PartialPerm<uint16_t>(std::vector<uint16_t>({1, 0, 3})),
                      LibsemigroupsException);
    REQUIRE_NOTHROW(
        Permutation<uint16_t>(std::vector<uint16_t>({1, 4, 0, 3, 2})));
    REQUIRE_THROWS_AS(
        PartialPerm<uint16_t>(std::vector<uint16_t>({1, 0, 3, 6, 4})),
        LibsemigroupsException);
    REQUIRE_THROWS_AS(
        PartialPerm<uint16_t>(std::vector<uint16_t>({1, 5, 0, 3, 2})),
        LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("SmallestInteger", "001", "", "[quick][element]") {
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

  LIBSEMIGROUPS_TEST_CASE("Transf", "002", "", "[quick][element]") {
    auto x = TransfHelper<3>::type({0, 1, 2});
    (void) x;
    auto y = PPermHelper<3>::type({0, 1, 2});
    (void) y;
    auto z = PermHelper<3>::type({0, 1, 2});
    (void) z;
    auto a = BMatHelper<3>::type({{0, 1}, {0, 1}});
    (void) a;
  }
}  // namespace libsemigroups
