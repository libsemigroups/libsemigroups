//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-21 James D. Mitchell
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

#include "catch.hpp"      // for TEST_CASE
#include "test-main.hpp"  // LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/libsemigroups-exception.hpp"  // for LibsemigroupsException
#include "libsemigroups/pbr.hpp"                      // for PBR

namespace libsemigroups {
  // Forward decl
  struct LibsemigroupsException;

  LIBSEMIGROUPS_TEST_CASE("PBR",
                          "001",
                          "universal product with convenience constructor",
                          "[quick][pbr]") {
    PBR x({{-3, -1}, {-3, -2, -1, 1, 2, 3}, {-3, -2, -1, 1, 3}},
          {{-3, -1, 1, 2, 3}, {-3, 1, 3}, {-3, -2, -1, 2, 3}});

    PBR y({{-3, -2, -1, 1}, {-3, -2, 3}, {-3, 2, 3}},
          {{-3, -2, -1, 3}, {-3, -2, -1, 3}, {-2, 2, 3}});

    PBR z({{-3, -2, -1, 1}, {-3, -2, 3}, {-3, 2, 3}},
          {{-3, -2, -1, 3}, {-3, -2, -1, 3}, {-2, 2, 3}});

    PBR xx({{3, 5},
            {0, 1, 2, 3, 4, 5},
            {0, 2, 3, 4, 5},
            {0, 1, 2, 3, 5},
            {0, 2, 5},
            {1, 2, 3, 4, 5}});
    PBR yy({{0, 3, 4, 5},
            {2, 4, 5},
            {1, 2, 5},
            {2, 3, 4, 5},
            {2, 3, 4, 5},
            {1, 2, 4}});

    REQUIRE(x == xx);
    REQUIRE(y == yy);

    z.product_inplace(x, y);

    PBR expected({{0, 1, 2, 3, 4, 5},
                  {0, 1, 2, 3, 4, 5},
                  {0, 1, 2, 3, 4, 5},
                  {0, 1, 2, 3, 4, 5},
                  {0, 1, 2, 3, 4, 5},
                  {0, 1, 2, 3, 4, 5}});
    REQUIRE(z == expected);
  }

  LIBSEMIGROUPS_TEST_CASE("PBR", "002", "universal product", "[quick][pbr]") {
    PBR x({{3, 5},
           {0, 1, 2, 3, 4, 5},
           {0, 2, 3, 4, 5},
           {0, 1, 2, 3, 5},
           {0, 2, 5},
           {1, 2, 3, 4, 5}});
    PBR y({{0, 3, 4, 5},
           {2, 4, 5},
           {1, 2, 5},
           {2, 3, 4, 5},
           {2, 3, 4, 5},
           {1, 2, 4}});

    PBR z({{0, 3, 4, 5},
           {2, 4, 5},
           {1, 2, 5},
           {2, 3, 4, 5},
           {2, 3, 4, 5},
           {1, 2, 4}});
    z.product_inplace(x, y);

    PBR expected({{0, 1, 2, 3, 4, 5},
                  {0, 1, 2, 3, 4, 5},
                  {0, 1, 2, 3, 4, 5},
                  {0, 1, 2, 3, 4, 5},
                  {0, 1, 2, 3, 4, 5},
                  {0, 1, 2, 3, 4, 5}});
    REQUIRE(z == expected);
  }

  LIBSEMIGROUPS_TEST_CASE("PBR",
                          "003",
                          "product [bigger than previous]",
                          "[quick][pbr]") {
    PBR x({{3, 5},
           {0, 1, 2, 3, 4, 5},
           {0, 2, 3, 4, 5},
           {0, 1, 2, 3, 5},
           {0, 2, 5},
           {1, 2, 3, 4, 5},
           {},
           {}});
    PBR y({{3, 5},
           {0, 1, 2, 3, 4, 5},
           {0, 2, 3, 4, 5},
           {0, 1, 2, 3, 5},
           {0, 2, 5},
           {1, 2, 3, 4, 5},
           {},
           {6}});
    x.product_inplace(y, y);
    PBR expected({{0, 1, 2, 3, 4, 5},
                  {0, 1, 2, 3, 4, 5},
                  {0, 1, 2, 3, 4, 5},
                  {0, 1, 2, 3, 4, 5},
                  {0, 1, 2, 3, 4, 5},
                  {0, 1, 2, 3, 4, 5},
                  {},
                  {6}});

    REQUIRE(x == expected);

    x = PBR({{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {7}});
    y = PBR({{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {7}});

    x.product_inplace(y, y);
    expected = PBR(
        {{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {7}});
    REQUIRE(x == expected);
  }

  LIBSEMIGROUPS_TEST_CASE("PBR", "004", "hash", "[quick][pbr]") {
    PBR x({{1}, {4}, {3}, {1}, {0, 2}, {0, 3, 4, 5}});
    for (size_t i = 0; i < 1000000; i++) {
      x.hash_value();
    }
  }

  LIBSEMIGROUPS_TEST_CASE("PBR", "005", "delete/copy", "[quick][pbr]") {
    PBR x({{1}, {4}, {3}, {1}, {0, 2}, {0, 3, 4, 5}});
    PBR y(x);
    PBR z({{1}, {4}, {3}, {1}, {0, 2}, {0, 3, 4, 5}});
    REQUIRE(y == z);
    PBR yy(y);
    REQUIRE(yy == y);
    PBR zz(yy);
    PBR a({{1}, {4}, {3}, {1}, {0, 2}, {0, 3, 4, 5}});
    REQUIRE(zz == a);
  }

  LIBSEMIGROUPS_TEST_CASE("PBR", "006", "exceptions", "[quick][pbr]") {
    REQUIRE_THROWS_AS(PBR::make({{1}, {4}, {3}, {10}, {0, 2}, {0, 3, 4, 5}}),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(PBR::make({{4}, {3}, {0}, {0, 2}, {0, 3, 4, 5}}),
                      LibsemigroupsException);

    REQUIRE_NOTHROW(PBR({{-3, -1}, {-3, -2, -1, 1, 2, 3}, {-3, -2, -1, 1, 3}},
                        {{-3, -1, 1, 2, 3}, {-3, 1, 3}, {-3, -2, -1, 2, 3}}));

    REQUIRE_NOTHROW(PBR({{}, {}}));

    REQUIRE_THROWS_AS(
        PBR::make({{-4, -1}, {-3, -2, -1, 1, 2, 3}, {-3, -2, -1, 1, 3}},
                  {{-3, -1, 1, 2, 3}, {-3, 1, 3}, {-3, -2, -1, 2, 3}}),
        LibsemigroupsException);

    REQUIRE_THROWS_AS(
        PBR::make({{-4, -1}, {-3, -2, -1, 1, 2, 3}, {-3, -2, -1, 1, 3}},
                  {{-3, -1, 1, 2, 3}, {-3, 1, 3}, {-3, -2, -1, 2, 3}}),
        LibsemigroupsException);

    REQUIRE_THROWS_AS(
        PBR::make(
            {{-4, -1}, {-3, -2, -1, 1, 2, 3}, {-3, -2, -1, 1, 3}},
            {{-3, -1, 1, 2, 3}, {-3, 1, 3}, {-3, -2, -1, 2, 3}, {-1, -2}}),
        LibsemigroupsException);

    REQUIRE_THROWS_AS(PBR::make({{}, {2}, {1}, {3, 0}}),
                      LibsemigroupsException);
  }
}  // namespace libsemigroups
