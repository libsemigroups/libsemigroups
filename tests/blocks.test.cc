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

#include "../src/blocks.h"
#include "../src/elements.h"

using namespace libsemigroups;

TEST_CASE("Blocks 01: empty blocks", "[quick][blocks][01]") {
  Blocks b1;
  Blocks b2 = Blocks(new std::vector<u_int32_t>({0, 1, 2, 1, 0, 2}),
                     new std::vector<bool>({true, false, true}));
  REQUIRE(b1 == b1);
  REQUIRE(!(b1 == b2));
  REQUIRE(b1 < b2);
  REQUIRE(!(b2 < b1));
  REQUIRE(b1.degree() == 0);
  REQUIRE(b1.lookup() == nullptr);
  REQUIRE(b1.nr_blocks() == 0);
  REQUIRE(b1.rank() == 0);
}

TEST_CASE("Blocks 02: non-empty blocks", "[quick][blocks][02]") {
  Blocks b = Blocks(new std::vector<u_int32_t>({0, 1, 2, 1, 0, 2}),
                    new std::vector<bool>({true, false, true}));
  REQUIRE(b == b);
  REQUIRE(!(b < b));
  REQUIRE(b.degree() == 6);
  REQUIRE(*b.lookup() == std::vector<bool>({true, false, true}));
  REQUIRE(b.nr_blocks() == 3);
  REQUIRE(b.rank() == 2);
  REQUIRE(b.is_transverse_block(0));
  REQUIRE(!b.is_transverse_block(1));
  REQUIRE(b.is_transverse_block(2));
  REQUIRE(b.block(0) == 0);
  REQUIRE(b.block(1) == 1);
  REQUIRE(b.block(2) == 2);
  REQUIRE(b.block(3) == 1);
  REQUIRE(b.block(4) == 0);
  REQUIRE(b.block(5) == 2);
  size_t i = 0;
  for (auto it = b.cbegin(); it < b.cend(); it++) {
    REQUIRE(*it == b.block(i++));
  }
}

TEST_CASE("Blocks 03: left blocks of bipartition", "[quick][blocks][03]") {
  Bipartition x = Bipartition(
      {0, 1, 2, 1, 0, 2, 1, 0, 2, 2, 0, 0, 2, 0, 3, 4, 4, 1, 3, 0});
  Blocks* b = x.left_blocks();
  REQUIRE(b == b);
  REQUIRE(!(b < b));
  REQUIRE(b->degree() == 10);
  REQUIRE(*b->lookup() == std::vector<bool>({true, true, true}));
  REQUIRE(b->nr_blocks() == 3);
  REQUIRE(b->rank() == 3);
  REQUIRE(b->is_transverse_block(0));
  REQUIRE(b->is_transverse_block(1));
  REQUIRE(b->is_transverse_block(2));
  REQUIRE(b->block(0) == 0);
  REQUIRE(b->block(1) == 1);
  REQUIRE(b->block(2) == 2);
  REQUIRE(b->block(3) == 1);
  REQUIRE(b->block(4) == 0);
  REQUIRE(b->block(5) == 2);
  delete b;
  x.really_delete();
}

TEST_CASE("Blocks 04: right blocks of bipartition", "[quick][blocks][04]") {
  Bipartition x = Bipartition(
      {0, 1, 1, 1, 1, 2, 3, 2, 4, 4, 5, 2, 4, 2, 1, 1, 1, 2, 3, 2});
  Blocks* b = x.right_blocks();
  REQUIRE(b == b);
  REQUIRE(!(b < b));
  REQUIRE(b->degree() == 10);
  REQUIRE(*b->lookup() == std::vector<bool>({false, true, true, true, true}));
  REQUIRE(b->nr_blocks() == 5);
  REQUIRE(b->rank() == 4);
  REQUIRE(!b->is_transverse_block(0));
  REQUIRE(b->is_transverse_block(1));
  REQUIRE(b->is_transverse_block(2));
  REQUIRE(b->is_transverse_block(3));
  REQUIRE(b->is_transverse_block(4));
  REQUIRE(b->block(0) == 0);
  REQUIRE(b->block(1) == 1);
  REQUIRE(b->block(2) == 2);
  REQUIRE(b->block(3) == 1);
  REQUIRE(b->block(4) == 3);
  REQUIRE(b->block(5) == 3);
  REQUIRE(b->block(6) == 3);
  REQUIRE(b->block(7) == 1);
  REQUIRE(b->block(8) == 4);
  REQUIRE(b->block(9) == 1);
  delete b;
  x.really_delete();
}

TEST_CASE("Blocks 05: copy [empty blocks]", "[quick][blocks][05]") {
  Blocks* b = new Blocks();
  Blocks  c(*b);

  REQUIRE(b->degree() == 0);
  REQUIRE(b->lookup() == nullptr);
  REQUIRE(b->nr_blocks() == 0);
  REQUIRE(b->rank() == 0);

  REQUIRE(c.degree() == 0);
  REQUIRE(c.lookup() == nullptr);
  REQUIRE(c.nr_blocks() == 0);
  REQUIRE(c.rank() == 0);

  delete b;
}

TEST_CASE("Blocks 06: copy [non-empty blocks]", "[quick][blocks][06]") {
  Blocks* b = new Blocks(
      new std::vector<u_int32_t>({0, 0, 1, 0, 2, 0, 1, 2, 2, 1, 0}),
      new std::vector<bool>({false, true, false}));
  Blocks c(*b);

  REQUIRE(b->degree() == 11);
  REQUIRE(b->lookup() != nullptr);
  REQUIRE(b->nr_blocks() == 3);
  REQUIRE(b->rank() == 1);

  REQUIRE(c.degree() == 11);
  REQUIRE(c.lookup() != nullptr);
  REQUIRE(c.nr_blocks() == 3);
  REQUIRE(c.rank() == 1);

  delete b;
}

TEST_CASE("Blocks 07: hash value", "[quick][blocks][07]") {
  Blocks* b = new Blocks(
      new std::vector<u_int32_t>({0, 0, 1, 0, 2, 0, 1, 2, 2, 1, 0}),
      new std::vector<bool>({false, true, false}));
  Blocks* c = new Blocks(
      new std::vector<u_int32_t>({0, 0, 1, 0, 2, 0, 1, 2, 2, 1, 0}),
      new std::vector<bool>({false, true, true}));
  REQUIRE(b->hash_value() != c->hash_value());
  delete b;
  delete c;

  b = new Blocks();
  REQUIRE(b->hash_value() == 0);
  delete b;
}

TEST_CASE("Blocks 08: operator<", "[quick][blocks][08]") {
  Blocks* b = new Blocks(
      new std::vector<u_int32_t>({0, 0, 1, 0, 2, 0, 1, 2, 2, 1, 0}),
      new std::vector<bool>({false, true, false}));
  Blocks* c = new Blocks(
      new std::vector<u_int32_t>({0, 0, 1, 0, 2, 0, 1, 2, 2, 1, 0}),
      new std::vector<bool>({false, true, true}));
  REQUIRE(*c < *b);
  REQUIRE(!(*b < *c));
  delete c;

  c = new Blocks(new std::vector<u_int32_t>({0, 1, 1, 0, 2, 0, 1, 2, 2, 1, 0}),
                 new std::vector<bool>({false, true, true}));
  REQUIRE(*b < *c);
  REQUIRE(!(*c < *b));
  delete b;

  b = new Blocks();
  REQUIRE(*b < *c);
  delete b;
  delete c;
}
