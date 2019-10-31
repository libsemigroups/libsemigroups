//
// libsemigroups - C/C++ library for semigroups and monoids
// Copyright (C) 2019 Michael Torpey
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
// of the set of integers {0, ..., n - 1 }

#include <cstddef>  // for size_t
#include <vector>   // for operator==, vector

#include "catch.hpp"      // for REQUIRE
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE
#include "uf.hpp"         // for UF, UF::blocks_type, UF::table_type

namespace libsemigroups {
  using detail::UF;

  LIBSEMIGROUPS_TEST_CASE("UF", "001", "constructor by size", "[quick]") {
    UF uf(7);
    REQUIRE(uf.get_size() == 7);
    REQUIRE(*uf.get_table() == UF::table_type({0, 1, 2, 3, 4, 5, 6}));
  }

  LIBSEMIGROUPS_TEST_CASE("UF", "002", "constructor by table", "[quick]") {
    UF::table_type* tab = new UF::table_type({0, 1, 2, 2, 3, 4, 2, 2, 6, 5, 0});
    UF              uf(*tab);
    REQUIRE(uf.get_size() == 11);
    REQUIRE(*uf.get_table() == *tab);
    delete tab;
    REQUIRE(*uf.get_table()
            == UF::table_type({0, 1, 2, 2, 3, 4, 2, 2, 6, 5, 0}));
  }

  LIBSEMIGROUPS_TEST_CASE("UF", "003", "copy constructor", "[quick]") {
    UF::table_type tab = {0, 1, 2, 2, 3, 4, 2, 2, 6, 5, 0};
    UF             uf(tab);
    REQUIRE(uf.get_size() == 11);
    UF uf2(uf);
    REQUIRE(uf2.get_size() == 11);
    REQUIRE(*uf2.get_table()
            == UF::table_type({0, 1, 2, 2, 3, 4, 2, 2, 6, 5, 0}));
  }

  LIBSEMIGROUPS_TEST_CASE("UF",
                          "004",
                          "copy constructor with blocks",
                          "[quick]") {
    UF               uf({0, 0, 1, 2, 4, 5, 3});
    UF::blocks_type* b = uf.get_blocks();

    UF               uf2(uf);
    UF::blocks_type* b2 = uf2.get_blocks();

    REQUIRE(b->size() == b2->size());
    for (size_t i = 0; i < b->size(); i++) {
      if (b->at(i) == nullptr) {
        REQUIRE(b2->at(i) == nullptr);
      } else {
        REQUIRE(*b->at(i) == *b2->at(i));
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("UF", "005", "nr_blocks", "[quick]") {
    UF uf({0, 0, 2, 1, 2, 5, 6, 7, 8, 8, 4, 9});
    REQUIRE(uf.nr_blocks() == 6);
  }

  LIBSEMIGROUPS_TEST_CASE("UF", "006", "get_blocks", "[quick]") {
    UF               uf({0, 0, 2, 1, 2, 5, 6, 7, 8, 8, 4, 9});
    UF::blocks_type* b = uf.get_blocks();
    REQUIRE(b->size() == 12);
    REQUIRE(b->at(0)->size() == 3);
    REQUIRE(b->at(1) == nullptr);
    REQUIRE(b->at(2)->size() == 3);
    REQUIRE(b->at(3) == nullptr);
    REQUIRE(b->at(5)->size() == 1);
    REQUIRE(b->at(8)->size() == 3);
    REQUIRE(b->at(11) == nullptr);
  }

  LIBSEMIGROUPS_TEST_CASE("UF", "007", "find", "[quick]") {
    UF uf({0, 0, 2, 1, 2, 5, 6, 7, 8, 8, 4, 9});
    REQUIRE(uf.find(0) == 0);
    REQUIRE(uf.find(1) == 0);
    REQUIRE(uf.find(4) == 2);
    REQUIRE(uf.find(6) == 6);
    REQUIRE(uf.find(8) == 8);
    REQUIRE(uf.find(11) == 8);
  }

  LIBSEMIGROUPS_TEST_CASE("UF", "008", "unite", "[quick]") {
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

  LIBSEMIGROUPS_TEST_CASE("UF", "009", "flatten", "[quick]") {
    UF uf1({0, 0, 2, 1, 2, 5, 6, 7, 8, 8, 4, 9});
    REQUIRE(*uf1.get_table()
            == UF::table_type({0, 0, 2, 1, 2, 5, 6, 7, 8, 8, 4, 9}));
    uf1.flatten();
    REQUIRE(*uf1.get_table()
            == UF::table_type({0, 0, 2, 0, 2, 5, 6, 7, 8, 8, 2, 8}));

    UF uf2({0, 1, 2, 2, 3, 4, 2, 2, 6, 5, 0});
    REQUIRE(*uf2.get_table()
            == UF::table_type({0, 1, 2, 2, 3, 4, 2, 2, 6, 5, 0}));
    uf2.flatten();
    REQUIRE(*uf2.get_table()
            == UF::table_type({0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 0}));
  }

  LIBSEMIGROUPS_TEST_CASE("UF", "010", "add_entry", "[quick]") {
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

  LIBSEMIGROUPS_TEST_CASE("UF", "011", "add_entry with blocks", "[quick]") {
    UF               uf({0, 0, 2, 3, 3, 5});
    UF::blocks_type* b = uf.get_blocks();
    REQUIRE(b->size() == 6);
    uf.add_entry();
    REQUIRE(b->size() == 7);
    REQUIRE(*b->at(6) == UF::table_type({6}));
  }

  LIBSEMIGROUPS_TEST_CASE("UF", "012", "big chain", "[no-valgrind][quick]") {
    UF::table_type tab;
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
    UF::table_type* t = uf.get_table();
    for (size_t i = 0; i < 100001; i++) {
      REQUIRE(t->at(i) == 0);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("UF", "013", "empty table", "[quick]") {
    UF               uf(0);
    UF::blocks_type* b = uf.get_blocks();
    REQUIRE(b->size() == 0);
    REQUIRE(uf.nr_blocks() == 0);
    uf.add_entry();
    REQUIRE(uf.get_size() == 1);
    REQUIRE(uf.nr_blocks() == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("UF", "014", "next_rep", "[quick]") {
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

  LIBSEMIGROUPS_TEST_CASE("UF", "015", "join", "[quick]") {
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
}  // namespace libsemigroups
