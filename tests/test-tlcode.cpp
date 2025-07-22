//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2025 James D. Mitchell
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

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for TEST_CASE
#include "test-main.hpp"                       // LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/exception.hpp"  // for LibsemigroupsException
#include "libsemigroups/tlcode.hpp"        // for TLCode
#include "libsemigroups/bipart.hpp"        // for PBR

#include "libsemigroups/detail/string.hpp"  // for to_string

namespace libsemigroups {
  // Forward decl
  struct LibsemigroupsException;

  LIBSEMIGROUPS_TEST_CASE("TLCode",
                          "000",
                          "TLmax method",
                          "[quick][TlCode]") {
    TLCode c{{0, 1, 0, 0}};
    REQUIRE(c.TL_max(0) == 0);
    REQUIRE(c.TL_max(1) == 1);
    REQUIRE(c.TL_max(2) == 1);
    REQUIRE(c.TL_max(3) == 2);
    REQUIRE(c.TL_max(4) == 3);
    c = TLCode({0, 0, 1, 0, 0});
    REQUIRE(c.TL_max(0) == 0);
    REQUIRE(c.TL_max(1) == 1);
    REQUIRE(c.TL_max(2) == 2);
    REQUIRE(c.TL_max(3) == 1);
    REQUIRE(c.TL_max(4) == 2);
    REQUIRE(c.TL_max(5) == 3);
    c = TLCode({0, 0, 1, 0, 1, 0});
    REQUIRE(c.TL_max(0) == 0);
    REQUIRE(c.TL_max(1) == 1);
    REQUIRE(c.TL_max(2) == 2);
    REQUIRE(c.TL_max(3) == 1);
    REQUIRE(c.TL_max(4) == 2);
    REQUIRE(c.TL_max(5) == 1);
    REQUIRE(c.TL_max(6) == 2);
  }

  LIBSEMIGROUPS_TEST_CASE("TLCode",
                          "001",
                          "throw_if_entry_out_of_bounds",
                          "[quick][TlCode]") {
    REQUIRE_NOTHROW(TLCode({0, 0, 0, 0}).throw_if_entry_out_of_bounds());
    REQUIRE_NOTHROW(TLCode({0, 1, 0, 0}).throw_if_entry_out_of_bounds());
    REQUIRE_NOTHROW(TLCode({0, 0, 0, 1}).throw_if_entry_out_of_bounds());
    REQUIRE_THROWS_AS(TLCode({1, 0}).throw_if_entry_out_of_bounds(),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(TLCode({0, 2}).throw_if_entry_out_of_bounds(),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(TLCode({0, 1, 0, 3}).throw_if_entry_out_of_bounds(),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(TLCode({0, 0, 0, 4}).throw_if_entry_out_of_bounds(),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("TLCode",
                          "002",
                          "product_by_generator_inplace_no_checks",
                          "[quick][TlCode]") {
    TLCode c{{0, 0, 0, 0}};
    c.product_by_generator_inplace_no_checks(0);
    REQUIRE(c == TLCode({0, 1, 0, 0}));
    c.product_by_generator_inplace_no_checks(0);
    REQUIRE(c == TLCode({0, 1, 0, 0}));

    c = TLCode({0, 0, 0, 0});
    c.product_by_generator_inplace_no_checks(1);
    REQUIRE(c == TLCode({0, 0, 1, 0}));
    c.product_by_generator_inplace_no_checks(1);
    REQUIRE(c == TLCode({0, 0, 1, 0}));

    c = TLCode({0, 0, 0, 0});
    c.product_by_generator_inplace_no_checks(2);
    REQUIRE(c == TLCode({0, 0, 0, 1}));
    c.product_by_generator_inplace_no_checks(2);
    REQUIRE(c == TLCode({0, 0, 0, 1}));

    c.product_by_generator_inplace_no_checks(1);
    REQUIRE(c == TLCode({0, 0, 0, 2}));
    c.product_by_generator_inplace_no_checks(0);
    REQUIRE(c == TLCode({0, 0, 0, 3}));
    c.product_by_generator_inplace_no_checks(0);
    REQUIRE(c == TLCode({0, 0, 0, 3}));
    c.product_by_generator_inplace_no_checks(1);
    REQUIRE(c == TLCode({0, 0, 0, 2}));
    c.product_by_generator_inplace_no_checks(2);
    REQUIRE(c == TLCode({0, 0, 0, 1}));
    c.product_by_generator_inplace_no_checks(0);
    REQUIRE(c == TLCode({0, 1, 0, 1}));

    c = TLCode({0, 0, 0, 3, 2, 0, 0, 5, 4, 1});
    c.product_by_generator_inplace_no_checks(8);
    REQUIRE(c == TLCode({0, 0, 0, 3, 2, 0, 0, 5, 4, 1}));
    c.product_by_generator_inplace_no_checks(5);
    REQUIRE(c == TLCode({0, 0, 0, 3, 2, 0, 0, 5, 3, 1}));
    c.product_by_generator_inplace_no_checks(6);
    REQUIRE(c == TLCode({0, 0, 0, 3, 2, 0, 0, 5, 2, 1}));

    c = TLCode({0, 0, 0, 0, 0, 0, 0, 0, 7, 1});
    c.product_by_generator_inplace_no_checks(4);
    REQUIRE(c == TLCode({0, 0, 0, 2, 0, 0, 0, 0, 4, 1}));
    c = TLCode({0, 0, 0, 0, 0, 0, 0, 0, 7, 1});
    c.product_by_generator_inplace_no_checks(5);
    REQUIRE(c == TLCode({0, 0, 0, 0, 3, 0, 0, 0, 3, 1}));

    c = TLCode({0, 0, 0, 0, 0, 0, 0, 0, 0, 8});
    c.product_by_generator_inplace_no_checks(5);
    REQUIRE(c == TLCode({0, 0, 0, 0, 3, 0, 0, 0, 0, 4}));
  }

  LIBSEMIGROUPS_TEST_CASE("TLCode",
                          "003",
                          "product_inplace_no_checks",
                          "[quick][TlCode]") {
    TLCode c{{0, 0, 0, 0}};
    TLCode d{{0, 0, 2, 2}};
    c.product_inplace_no_checks(d);
    REQUIRE(c == d);

    d = TLCode({0, 0, 0, 3, 2, 0, 0, 4, 3, 1});
    c = tlcode::one(d);
    c.product_inplace_no_checks(d);
    REQUIRE(c == d);
  }

const std::array<TLCode, 132> TL6 = {{
 TLCode {{0, 0, 2, 0, 0, 0}},
 TLCode {{0, 1, 1, 0, 0, 0}},
 TLCode {{0, 0, 1, 0, 0, 0}},
 TLCode {{0, 1, 0, 0, 0, 0}},
 TLCode {{0, 0, 0, 0, 0, 0}},
 TLCode {{0, 1, 0, 1, 0, 0}},
 TLCode {{0, 0, 0, 1, 0, 0}},
 TLCode {{0, 0, 0, 2, 0, 0}},
 TLCode {{0, 0, 1, 1, 0, 0}},
 TLCode {{0, 0, 0, 3, 0, 0}},
 TLCode {{0, 0, 2, 2, 0, 0}},
 TLCode {{0, 1, 0, 2, 0, 0}},
 TLCode {{0, 0, 2, 1, 0, 0}},
 TLCode {{0, 1, 1, 1, 0, 0}},
 TLCode {{0, 0, 2, 0, 1, 0}},
 TLCode {{0, 1, 1, 0, 1, 0}},
 TLCode {{0, 0, 1, 0, 1, 0}},
 TLCode {{0, 1, 0, 0, 1, 0}},
 TLCode {{0, 0, 0, 0, 1, 0}},
 TLCode {{0, 1, 0, 0, 2, 0}},
 TLCode {{0, 0, 0, 0, 2, 0}},
 TLCode {{0, 1, 0, 1, 1, 0}},
 TLCode {{0, 0, 0, 1, 1, 0}},
 TLCode {{0, 0, 0, 0, 3, 0}},
 TLCode {{0, 0, 0, 2, 2, 0}},
 TLCode {{0, 0, 1, 0, 2, 0}},
 TLCode {{0, 0, 0, 2, 1, 0}},
 TLCode {{0, 0, 1, 1, 1, 0}},
 TLCode {{0, 0, 0, 3, 1, 0}},
 TLCode {{0, 0, 2, 2, 1, 0}},
 TLCode {{0, 1, 0, 2, 1, 0}},
 TLCode {{0, 0, 2, 1, 1, 0}},
 TLCode {{0, 1, 1, 1, 1, 0}},
 TLCode {{0, 0, 2, 0, 2, 0}},
 TLCode {{0, 1, 1, 0, 2, 0}},
 TLCode {{0, 1, 0, 0, 3, 0}},
 TLCode {{0, 1, 0, 2, 2, 0}},
 TLCode {{0, 0, 0, 0, 4, 0}},
 TLCode {{0, 0, 0, 3, 3, 0}},
 TLCode {{0, 0, 2, 0, 3, 0}},
 TLCode {{0, 0, 0, 3, 2, 0}},
 TLCode {{0, 0, 2, 2, 2, 0}},
 TLCode {{0, 0, 2, 0, 0, 1}},
 TLCode {{0, 1, 1, 0, 0, 1}},
 TLCode {{0, 0, 1, 0, 0, 1}},
 TLCode {{0, 1, 0, 0, 0, 1}},
 TLCode {{0, 0, 0, 0, 0, 1}},
 TLCode {{0, 1, 0, 1, 0, 1}},
 TLCode {{0, 0, 0, 1, 0, 1}},
 TLCode {{0, 0, 0, 2, 0, 1}},
 TLCode {{0, 0, 1, 1, 0, 1}},
 TLCode {{0, 0, 0, 3, 0, 1}},
 TLCode {{0, 0, 2, 2, 0, 1}},
 TLCode {{0, 1, 0, 2, 0, 1}},
 TLCode {{0, 0, 2, 1, 0, 1}},
 TLCode {{0, 1, 1, 1, 0, 1}},
 TLCode {{0, 0, 2, 0, 0, 2}},
 TLCode {{0, 1, 1, 0, 0, 2}},
 TLCode {{0, 0, 1, 0, 0, 2}},
 TLCode {{0, 1, 0, 0, 0, 2}},
 TLCode {{0, 0, 0, 0, 0, 2}},
 TLCode {{0, 0, 2, 0, 1, 1}},
 TLCode {{0, 1, 1, 0, 1, 1}},
 TLCode {{0, 0, 1, 0, 1, 1}},
 TLCode {{0, 1, 0, 0, 1, 1}},
 TLCode {{0, 0, 0, 0, 1, 1}},
 TLCode {{0, 1, 0, 0, 0, 3}},
 TLCode {{0, 0, 0, 0, 0, 3}},
 TLCode {{0, 1, 0, 0, 2, 2}},
 TLCode {{0, 0, 0, 0, 2, 2}},
 TLCode {{0, 1, 0, 1, 0, 2}},
 TLCode {{0, 0, 0, 1, 0, 2}},
 TLCode {{0, 1, 0, 0, 2, 1}},
 TLCode {{0, 0, 0, 0, 2, 1}},
 TLCode {{0, 1, 0, 1, 1, 1}},
 TLCode {{0, 0, 0, 1, 1, 1}},
 TLCode {{0, 0, 0, 0, 3, 1}},
 TLCode {{0, 0, 0, 2, 2, 1}},
 TLCode {{0, 0, 1, 0, 2, 1}},
 TLCode {{0, 0, 0, 2, 1, 1}},
 TLCode {{0, 0, 1, 1, 1, 1}},
 TLCode {{0, 0, 0, 2, 0, 2}},
 TLCode {{0, 0, 1, 1, 0, 2}},
 TLCode {{0, 0, 1, 0, 0, 3}},
 TLCode {{0, 0, 1, 0, 2, 2}},
 TLCode {{0, 0, 0, 0, 0, 4}},
 TLCode {{0, 0, 0, 0, 3, 3}},
 TLCode {{0, 0, 0, 2, 0, 3}},
 TLCode {{0, 0, 0, 0, 3, 2}},
 TLCode {{0, 0, 0, 2, 2, 2}},
 TLCode {{0, 0, 0, 3, 1, 1}},
 TLCode {{0, 0, 2, 2, 1, 1}},
 TLCode {{0, 1, 0, 2, 1, 1}},
 TLCode {{0, 0, 2, 1, 1, 1}},
 TLCode {{0, 1, 1, 1, 1, 1}},
 TLCode {{0, 0, 2, 0, 2, 1}},
 TLCode {{0, 1, 1, 0, 2, 1}},
 TLCode {{0, 1, 0, 0, 3, 1}},
 TLCode {{0, 1, 0, 2, 2, 1}},
 TLCode {{0, 0, 0, 0, 4, 1}},
 TLCode {{0, 0, 0, 3, 3, 1}},
 TLCode {{0, 0, 2, 0, 3, 1}},
 TLCode {{0, 0, 0, 3, 2, 1}},
 TLCode {{0, 0, 2, 2, 2, 1}},
 TLCode {{0, 0, 0, 3, 0, 2}},
 TLCode {{0, 0, 2, 2, 0, 2}},
 TLCode {{0, 1, 0, 2, 0, 2}},
 TLCode {{0, 0, 2, 1, 0, 2}},
 TLCode {{0, 1, 1, 1, 0, 2}},
 TLCode {{0, 0, 2, 0, 0, 3}},
 TLCode {{0, 1, 1, 0, 0, 3}},
 TLCode {{0, 0, 2, 0, 2, 2}},
 TLCode {{0, 1, 1, 0, 2, 2}},
 TLCode {{0, 1, 0, 0, 0, 4}},
 TLCode {{0, 1, 0, 0, 3, 3}},
 TLCode {{0, 1, 0, 2, 0, 3}},
 TLCode {{0, 1, 0, 0, 3, 2}},
 TLCode {{0, 1, 0, 2, 2, 2}},
 TLCode {{0, 0, 0, 0, 4, 2}},
 TLCode {{0, 0, 0, 3, 3, 2}},
 TLCode {{0, 0, 2, 0, 3, 2}},
 TLCode {{0, 0, 0, 3, 2, 2}},
 TLCode {{0, 0, 2, 2, 2, 2}},
 TLCode {{0, 0, 0, 3, 0, 3}},
 TLCode {{0, 0, 2, 2, 0, 3}},
 TLCode {{0, 0, 2, 0, 0, 4}},
 TLCode {{0, 0, 2, 0, 3, 3}},
 TLCode {{0, 0, 0, 0, 0, 5}},
 TLCode {{0, 0, 0, 0, 4, 4}},
 TLCode {{0, 0, 0, 3, 0, 4}},
 TLCode {{0, 0, 0, 0, 4, 3}},
 TLCode {{0, 0, 0, 3, 3, 3}} }};

  LIBSEMIGROUPS_TEST_CASE("TLCode",
                          "004",
                          "product_big_list",
                          "[quick][TlCode]") {
      TLCode c{0};
      REQUIRE(c * c == c);
      c = TLCode{1};
      REQUIRE(c * c == c);
      for (auto const &x : TL6) {
          for (auto const &y : TL6) {
              for (auto const &z : TL6) {
                  REQUIRE(x * (y * z) == (x * y) * z);
              }
          }
      }
  }

  LIBSEMIGROUPS_TEST_CASE("TLCode",
                          "005",
                          "product_big_list_inplace",
                          "[quick][TlCode]") {
      TLCode a(6), b(6);
      for (auto const &x : TL6) {
          for (auto const &y : TL6) {
              for (auto const &z : TL6) {
                  a = x;
                  b = y;
                  b.product_inplace_no_checks(z);
                  a.product_inplace_no_checks(b);
                  b = x;
                  b.product_inplace_no_checks(y);
                  b.product_inplace_no_checks(z);

                  REQUIRE(a == b);
              }
          }
      }
  }


}  // namespace libsemigroups


