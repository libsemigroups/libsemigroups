//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 Florent Hivert
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

#include <array>

#include "../src/alloc.h"
#include "catch.hpp"

using namespace libsemigroups;


TEST_CASE("Alloc 00: allocator<int>",
          "[quick][alloc][00]") {
  allocator<int> alloc {};
  int *p = alloc.allocate(1);
  *p = 1;
  REQUIRE(*p == 1);
  int *q = alloc.allocate(1);
  *q = 2;
  REQUIRE(*q == 2);
  REQUIRE(*p == 1);
  alloc.deallocate_all();

  const size_t tsz = 10000;
  int *tab[tsz];
  for (size_t i = 0; i<tsz; i++) {
    tab[i] = alloc.allocate(1);
    *(tab[i]) = i;
  }
  for (size_t i = 0; i<tsz; i++) {
    REQUIRE(*(tab[i]) == i);
  }
  alloc.deallocate_all();
}



TEST_CASE("Alloc 01: allocator<std::array<int, 16>>",
          "[quick][alloc][01]") {
  using T = std::array<int, 16>;
  allocator<T> alloc {};
  T bla {};
  T *p = alloc.allocate(1);
  (*p)[0] = 1;
  (*p)[15] = 42;
  REQUIRE((*p)[0] == 1);
  REQUIRE((*p)[15] == 42);
  T *q = alloc.allocate(1);
  (*q)[0] = 2;
  REQUIRE((*q)[0] == 2);

  REQUIRE((*p)[0] == 1);
  REQUIRE((*p)[15] == 42);
  alloc.deallocate_all();

  const size_t tsz = 10000;
  T *tab[tsz];
  for (size_t i = 0; i<tsz; i++) {
    tab[i] = alloc.allocate(1);
    (*(tab[i]))[0] = i;
    (*(tab[i]))[15] = 42;
  }
  for (size_t i = 0; i<tsz; i++) {
    REQUIRE((*(tab[i]))[0] == i);
    REQUIRE((*(tab[i]))[15] == 42);
  }
  alloc.deallocate_all();
}

