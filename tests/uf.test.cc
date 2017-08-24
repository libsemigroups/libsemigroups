//
// libsemigroups - C/C++ library for semigroups and monoids
// Copyright (C) 2017 Michael Torpey
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

// The purpose of this file is to test the UF class which describes a partition
// of the set of integers {0 .. n-1}

#include <utility>

#include "../src/uf.h"
#include "catch.hpp"

#define UF_REPORT false

using namespace libsemigroups;

TEST_CASE("UF 01: constructor by size", "[quick][uf][01]") {
  UF uf(7);
  REQUIRE(uf.get_size() == 7);
  REQUIRE(*uf.get_table() == UF::table_t({0, 1, 2, 3, 4, 5, 6}));
}

TEST_CASE("UF 02: constructor by table", "[quick][uf][02]") {
  UF::table_t* tab = new UF::table_t({0, 1, 2, 2, 3, 4, 2, 2, 6, 5, 0});
  UF           uf(*tab);
  REQUIRE(uf.get_size() == 11);
  REQUIRE(*uf.get_table() == *tab);
  delete tab;
  REQUIRE(*uf.get_table() == UF::table_t({0, 1, 2, 2, 3, 4, 2, 2, 6, 5, 0}));
}

TEST_CASE("UF 03: copy constructor", "[quick][uf][03]") {
  UF::table_t tab = {0, 1, 2, 2, 3, 4, 2, 2, 6, 5, 0};
  UF          uf(tab);
  REQUIRE(uf.get_size() == 11);
  UF uf2(uf);
  REQUIRE(uf2.get_size() == 11);
  REQUIRE(*uf2.get_table() == UF::table_t({0, 1, 2, 2, 3, 4, 2, 2, 6, 5, 0}));
}

TEST_CASE("UF 04: copy constructor with blocks", "[quick][uf][04]") {
  UF            uf({0, 0, 1, 2, 4, 5, 3});
  UF::blocks_t* b = uf.get_blocks();

  UF            uf2(uf);
  UF::blocks_t* b2 = uf2.get_blocks();

  REQUIRE(b->size() == b2->size());
  for (size_t i = 0; i < b->size(); i++) {
    if (b->at(i) == nullptr) {
      REQUIRE(b2->at(i) == nullptr);
    } else {
      REQUIRE(*b->at(i) == *b2->at(i));
    }
  }
}

TEST_CASE("UF 05: nr_blocks", "[quick][uf][05]") {
  UF uf({0, 0, 2, 1, 2, 5, 6, 7, 8, 8, 4, 9});
  REQUIRE(uf.nr_blocks() == 6);
}

TEST_CASE("UF 06: get_blocks", "[quick][uf][06]") {
  UF            uf({0, 0, 2, 1, 2, 5, 6, 7, 8, 8, 4, 9});
  UF::blocks_t* b = uf.get_blocks();
  REQUIRE(b->size() == 12);
  REQUIRE(b->at(0)->size() == 3);
  REQUIRE(b->at(1) == nullptr);
  REQUIRE(b->at(2)->size() == 3);
  REQUIRE(b->at(3) == nullptr);
  REQUIRE(b->at(5)->size() == 1);
  REQUIRE(b->at(8)->size() == 3);
  REQUIRE(b->at(11) == nullptr);
}

TEST_CASE("UF 07: find", "[quick][uf][07]") {
  UF uf({0, 0, 2, 1, 2, 5, 6, 7, 8, 8, 4, 9});
  REQUIRE(uf.find(0) == 0);
  REQUIRE(uf.find(1) == 0);
  REQUIRE(uf.find(4) == 2);
  REQUIRE(uf.find(6) == 6);
  REQUIRE(uf.find(8) == 8);
  REQUIRE(uf.find(11) == 8);
}

TEST_CASE("UF 08: unite", "[quick][uf][08]") {
  UF uf({0, 0, 2, 1, 2, 5, 6, 7, 8, 8, 4, 9});
  REQUIRE(uf.find(0) == 0);
  REQUIRE(uf.find(8) == 8);
  REQUIRE(uf.find(11) == 8);

  uf.unite(8, 8);
  REQUIRE(uf.find(0) == 0);
  REQUIRE(uf.find(8) == 8);
  REQUIRE(uf.find(11) == 8);

  uf.unite(11, 8);
  REQUIRE(uf.find(0) == 0);
  REQUIRE(uf.find(8) == 8);
  REQUIRE(uf.find(11) == 8);

  uf.unite(11, 0);
  REQUIRE(uf.find(0) == 0);
  REQUIRE(uf.find(8) == 0);
  REQUIRE(uf.find(11) == 0);
}

TEST_CASE("UF 09: flatten", "[quick][uf][09]") {
  UF uf1({0, 0, 2, 1, 2, 5, 6, 7, 8, 8, 4, 9});
  REQUIRE(*uf1.get_table()
          == UF::table_t({0, 0, 2, 1, 2, 5, 6, 7, 8, 8, 4, 9}));
  uf1.flatten();
  REQUIRE(*uf1.get_table()
          == UF::table_t({0, 0, 2, 0, 2, 5, 6, 7, 8, 8, 2, 8}));

  UF uf2({0, 1, 2, 2, 3, 4, 2, 2, 6, 5, 0});
  REQUIRE(*uf2.get_table() == UF::table_t({0, 1, 2, 2, 3, 4, 2, 2, 6, 5, 0}));
  uf2.flatten();
  REQUIRE(*uf2.get_table() == UF::table_t({0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 0}));
}

TEST_CASE("UF 10: add_entry", "[quick][uf][10]") {
  UF uf({0, 0, 2, 3, 3, 5});
  REQUIRE(uf.get_size() == 6);
  uf.add_entry();
  REQUIRE(uf.get_size() == 7);
  uf.add_entry();
  REQUIRE(uf.get_size() == 8);
  REQUIRE(uf.find(6) == 6);
  REQUIRE(uf.find(7) == 7);
  uf.unite(1, 7);
  REQUIRE(uf.find(7) == 0);
}

TEST_CASE("UF 11: add_entry with blocks", "[quick][uf][11]") {
  UF            uf({0, 0, 2, 3, 3, 5});
  UF::blocks_t* b = uf.get_blocks();
  REQUIRE(b->size() == 6);
  uf.add_entry();
  REQUIRE(b->size() == 7);
  REQUIRE(*b->at(6) == UF::table_t({6}));
}

TEST_CASE("UF 12: big chain", "[quick][uf][12]") {
  UF::table_t tab;
  tab.push_back(0);
  for (size_t i = 0; i < 100000; i++) {
    tab.push_back(i);
  }
  UF uf(tab);
  REQUIRE(uf.get_size() == 100001);
  REQUIRE(uf.find(12345) == 0);
  REQUIRE(uf.find(100000) == 0);
  REQUIRE(uf.nr_blocks() == 1);
  uf.flatten();
  UF::table_t* t = uf.get_table();
  for (size_t i = 0; i < 100001; i++) {
    REQUIRE(t->at(i) == 0);
  }
}

TEST_CASE("UF 13: empty table", "[quick][uf][13]") {
  UF            uf(0);
  UF::blocks_t* b = uf.get_blocks();
  REQUIRE(b->size() == 0);
  REQUIRE(uf.nr_blocks() == 0);
  uf.add_entry();
  REQUIRE(uf.get_size() == 1);
  REQUIRE(uf.nr_blocks() == 1);
}

TEST_CASE("UF 14: next_rep", "[quick][uf][14]") {
  UF uf(10);
  uf.unite(2, 4);
  uf.unite(4, 9);
  uf.unite(1, 7);

  REQUIRE(uf.nr_blocks() == 7);

  uf.reset_next_rep();
  REQUIRE(uf.next_rep() == 0);
  REQUIRE(uf.next_rep() == 1);
  REQUIRE(uf.next_rep() == 2);
  REQUIRE(uf.next_rep() == 3);
  REQUIRE(uf.next_rep() == 5);
  REQUIRE(uf.next_rep() == 6);
  REQUIRE(uf.next_rep() == 8);
}

TEST_CASE("UF 15: join", "[quick][uf][15]") {
  UF uf1(10);
  uf1.unite(2, 4);
  uf1.unite(4, 9);
  uf1.unite(1, 7);

  REQUIRE(uf1.nr_blocks() == 7);

  uf1.join(uf1);
  REQUIRE(uf1.nr_blocks() == 7);

  UF uf2(10);
  uf2.unite(1, 4);
  uf2.unite(3, 9);
  uf2.unite(0, 7);
  REQUIRE(uf2.nr_blocks() == 7);

  uf1.join(uf2);
  REQUIRE(uf2.nr_blocks() == 7);
  REQUIRE(uf1.nr_blocks() == 4);

  uf1.reset_next_rep();
  REQUIRE(uf1.next_rep() == 0);
  REQUIRE(uf1.next_rep() == 5);
  REQUIRE(uf1.next_rep() == 6);
  REQUIRE(uf1.next_rep() == 8);
}
