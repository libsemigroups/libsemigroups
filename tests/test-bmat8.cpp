// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 Finn Smith
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

#include <cstddef>        // for size_t
#include <cstdint>        // for uint64_t
#include <iosfwd>         // for ostream, ostringstream, stringbuf
#include <set>            // for set
#include <unordered_set>  // for unordered_set
#include <vector>         // for vector

#include "catch.hpp"  // for REQUIRE, REQUIRE_THROWS_AS, REQUIRE_NOTHROW
#include "libsemigroups/bmat8.hpp"         // for BMat8, operator<<
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin
#include "libsemigroups/timer.hpp"         // for Timer
#include "test-main.hpp"                   // for LIBSEMIGROUPS_TEST_CASE

namespace libsemigroups {
  constexpr bool REPORT = false;

  // Forward decl
  struct LibsemigroupsException;

  LIBSEMIGROUPS_TEST_CASE("BMat8", "001", "transpose", "[quick]") {
    auto  rg = ReportGuard(REPORT);
    BMat8 bm1(0);
    REQUIRE(bm1.transpose() == bm1);

    BMat8 bm2({{1, 1}, {0, 1}});
    REQUIRE(bm2.transpose() == BMat8({{1, 0}, {1, 1}}));

    BMat8 bm3({{0, 0, 0, 1, 0, 0, 1, 1},
               {1, 1, 1, 1, 1, 1, 0, 1},
               {0, 1, 1, 1, 0, 1, 0, 1},
               {1, 1, 0, 1, 1, 1, 1, 1},
               {0, 0, 1, 0, 0, 1, 1, 1},
               {1, 1, 0, 0, 0, 0, 0, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {0, 1, 1, 1, 1, 0, 1, 0}});

    REQUIRE(bm3.transpose()
            == BMat8({{0, 1, 0, 1, 0, 1, 0, 0},
                      {0, 1, 1, 1, 0, 1, 1, 1},
                      {0, 1, 1, 0, 1, 0, 0, 1},
                      {1, 1, 1, 1, 0, 0, 0, 1},
                      {0, 1, 0, 1, 0, 0, 0, 1},
                      {0, 1, 1, 1, 1, 0, 0, 0},
                      {1, 0, 0, 1, 1, 0, 1, 1},
                      {1, 1, 1, 1, 1, 1, 1, 0}}));
  }

  LIBSEMIGROUPS_TEST_CASE("BMat8", "002", "multiplication", "[quick]") {
    auto  rg = ReportGuard(REPORT);
    BMat8 bm({{0, 0, 0, 1, 0, 0, 1, 1},
              {1, 1, 1, 1, 1, 1, 0, 1},
              {0, 1, 1, 1, 0, 1, 0, 1},
              {1, 1, 0, 1, 1, 1, 1, 1},
              {0, 0, 1, 0, 0, 1, 1, 1},
              {1, 1, 0, 0, 0, 0, 0, 1},
              {0, 1, 0, 0, 0, 0, 1, 1},
              {0, 1, 1, 1, 1, 0, 1, 0}});

    BMat8 tmp = bm * bm.one();
    REQUIRE(tmp == bm);
    REQUIRE(tmp == bm * bm.one());

    tmp = bm.one() * bm;
    REQUIRE(tmp == bm);
    REQUIRE(tmp == bm.one() * bm);

    tmp = bm * BMat8(0);
    REQUIRE(tmp == BMat8(0));

    BMat8 bm2({{0, 0, 0, 1, 0, 0, 1, 1},
               {0, 0, 1, 0, 0, 1, 0, 1},
               {1, 1, 0, 0, 1, 1, 0, 1},
               {1, 1, 0, 0, 0, 0, 0, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {0, 1, 0, 1, 1, 1, 1, 1},
               {0, 1, 0, 1, 0, 1, 0, 1},
               {0, 1, 0, 0, 0, 0, 1, 0}});

    tmp = bm * bm2;

    BMat8 bm3({{1, 1, 0, 1, 0, 1, 1, 1},
               {1, 1, 1, 1, 1, 1, 1, 1},
               {1, 1, 1, 1, 1, 1, 1, 1},
               {1, 1, 1, 1, 1, 1, 1, 1},
               {1, 1, 0, 1, 1, 1, 1, 1},
               {0, 1, 1, 1, 0, 1, 1, 1},
               {0, 1, 1, 1, 0, 1, 1, 1},
               {1, 1, 1, 1, 1, 1, 1, 1}});

    REQUIRE(tmp == bm3);
    REQUIRE(tmp == bm * bm2);
  }

  LIBSEMIGROUPS_TEST_CASE("BMat8", "003", "identity matrix", "[quick]") {
    auto  rg = ReportGuard(REPORT);
    BMat8 bm({{0, 1, 1, 1, 0, 1, 0, 1},
              {0, 0, 0, 0, 0, 0, 0, 1},
              {1, 1, 1, 1, 1, 1, 0, 1},
              {1, 1, 0, 1, 1, 1, 1, 1},
              {0, 0, 1, 0, 0, 1, 1, 1},
              {1, 1, 0, 0, 0, 0, 0, 1},
              {0, 1, 0, 0, 0, 0, 1, 1},
              {0, 1, 1, 1, 1, 0, 1, 0}});

    BMat8 id({{1, 0, 0, 0, 0, 0, 0, 0},
              {0, 1, 0, 0, 0, 0, 0, 0},
              {0, 0, 1, 0, 0, 0, 0, 0},
              {0, 0, 0, 1, 0, 0, 0, 0},
              {0, 0, 0, 0, 1, 0, 0, 0},
              {0, 0, 0, 0, 0, 1, 0, 0},
              {0, 0, 0, 0, 0, 0, 1, 0},
              {0, 0, 0, 0, 0, 0, 0, 1}});

    REQUIRE(bm.one() == id);
  }

  LIBSEMIGROUPS_TEST_CASE("BMat8", "004", "random", "[quick]") {
    auto rg = ReportGuard(REPORT);
    for (size_t d = 1; d < 9; ++d) {
      BMat8 bm = BMat8::random(d);
      for (size_t i = d; i < 8; ++i) {
        for (size_t j = 0; j < 8; ++j) {
          REQUIRE(bm.get(i, j) == 0);
          REQUIRE(bm.get(j, i) == 0);
        }
      }
    }
    REQUIRE_THROWS_AS(BMat8::random(9), LibsemigroupsException);
    REQUIRE_THROWS_AS(BMat8::random(9), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("BMat8", "005", "call operator", "[quick]") {
    auto                           rg  = ReportGuard(REPORT);
    std::vector<std::vector<bool>> mat = {{0, 0, 0, 1, 0, 0, 1},
                                          {0, 1, 1, 1, 0, 1, 0},
                                          {1, 1, 0, 1, 1, 1, 1},
                                          {0, 0, 1, 0, 0, 1, 1},
                                          {1, 1, 0, 0, 0, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 1},
                                          {0, 1, 1, 1, 1, 0, 1}};
    BMat8                          bm(mat);

    for (size_t i = 0; i < 7; ++i) {
      for (size_t j = 0; j < 7; ++j) {
        REQUIRE(static_cast<size_t>(bm.get(i, j)) == mat[i][j]);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("BMat8", "006", "operator<<", "[quick]") {
    auto               rg = ReportGuard(REPORT);
    std::ostringstream oss;
    oss << BMat8::random();  // Does not do anything visible

    std::stringbuf buff;
    std::ostream   os(&buff);
    os << BMat8::random();  // Also does not do anything visible
  }

  LIBSEMIGROUPS_TEST_CASE("BMat8", "007", "set", "[quick]") {
    auto  rg = ReportGuard(REPORT);
    BMat8 bm({{0, 1, 1, 1, 0, 1, 0, 1},
              {0, 0, 0, 0, 0, 0, 0, 1},
              {1, 1, 1, 1, 1, 1, 0, 1},
              {1, 1, 0, 1, 1, 1, 1, 1},
              {0, 0, 1, 0, 0, 1, 1, 1},
              {1, 1, 0, 0, 0, 0, 0, 1},
              {0, 1, 0, 0, 0, 0, 1, 1},
              {0, 1, 1, 1, 1, 0, 1, 0}});

    BMat8 bm2({{1, 1, 1, 1, 0, 1, 0, 1},
               {0, 0, 0, 0, 0, 0, 0, 1},
               {1, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 0, 1, 1, 1, 1, 1},
               {0, 0, 1, 0, 0, 1, 1, 1},
               {1, 1, 0, 0, 0, 0, 0, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {0, 1, 1, 1, 1, 0, 1, 0}});

    BMat8 bm3({{1, 0, 1, 1, 0, 1, 0, 1},
               {0, 0, 0, 0, 0, 0, 0, 1},
               {1, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 0, 1, 1, 1, 1, 1},
               {0, 0, 1, 0, 0, 1, 1, 1},
               {1, 1, 0, 0, 0, 0, 0, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {0, 1, 1, 1, 1, 0, 1, 0}});

    BMat8 bm4({{1, 0, 1, 1, 0, 1, 0, 1},
               {0, 0, 0, 0, 0, 0, 0, 1},
               {1, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 0, 1, 1, 1, 1, 1},
               {0, 0, 1, 0, 0, 1, 1, 1},
               {1, 1, 0, 0, 0, 0, 1, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {0, 1, 1, 1, 1, 0, 1, 0}});

    BMat8 bm5({{1, 0, 1, 1, 0, 1, 0, 1},
               {0, 0, 0, 0, 0, 0, 0, 1},
               {1, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 0, 1, 1, 1, 1, 1},
               {0, 0, 1, 0, 0, 1, 1, 1},
               {1, 1, 0, 0, 0, 0, 1, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {0, 1, 1, 1, 1, 0, 1, 1}});

    bm.set(0, 0, 1);
    REQUIRE(bm == bm2);

    bm.set(0, 1, false);
    REQUIRE(bm == bm3);

    bm.set(5, 6, true);
    REQUIRE(bm == bm4);

    bm.set(7, 7, true);
    REQUIRE(bm == bm5);

    for (size_t i = 0; i < 8; ++i) {
      for (size_t j = 0; j < 8; ++j) {
        bm.set(i, j, true);
      }
    }

    BMat8 ones(0xffffffffffffffff);
    REQUIRE(bm == ones);

    for (size_t i = 0; i < 8; ++i) {
      for (size_t j = 0; j < 8; ++j) {
        bm.set(i, j, false);
      }
    }

    BMat8 zeros(0);
    REQUIRE(bm == zeros);

    REQUIRE_THROWS_AS(zeros.set(0, 8, true), LibsemigroupsException);
    REQUIRE_THROWS_AS(zeros.set(8, 0, true), LibsemigroupsException);
    REQUIRE_THROWS_AS(zeros.set(8, 8, true), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("BMat8", "008", "row space basis", "[quick]") {
    auto  rg = ReportGuard(REPORT);
    BMat8 bm({{0, 1, 1, 1, 0, 1, 0, 1},
              {0, 0, 0, 0, 0, 0, 0, 1},
              {1, 1, 1, 1, 1, 1, 0, 1},
              {1, 1, 0, 1, 1, 1, 1, 1},
              {0, 0, 1, 0, 0, 1, 1, 1},
              {1, 1, 0, 0, 0, 0, 0, 1},
              {0, 1, 0, 0, 0, 0, 1, 1},
              {0, 1, 1, 1, 1, 0, 1, 0}});

    BMat8 bm2({{1, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 0, 1, 1, 1, 1, 1},
               {1, 1, 0, 0, 0, 0, 0, 1},
               {0, 1, 1, 1, 1, 0, 1, 0},
               {0, 1, 1, 1, 0, 1, 0, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {0, 0, 1, 0, 0, 1, 1, 1},
               {0, 0, 0, 0, 0, 0, 0, 1}});

    REQUIRE(bm.row_space_basis() == bm2.row_space_basis());

    BMat8 bm3({{1, 1, 1, 1, 0, 1, 0, 1},
               {0, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 1, 0, 0, 1, 0, 1},
               {1, 1, 0, 0, 0, 1, 1, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {1, 0, 0, 0, 0, 1, 0, 0}});

    BMat8 bm4({{1, 1, 1, 1, 0, 1, 0, 1},
               {1, 1, 1, 0, 0, 1, 0, 1},
               {1, 0, 0, 0, 0, 1, 0, 0},
               {0, 1, 1, 1, 1, 1, 0, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {0, 0, 0, 0, 0, 0, 0, 0},
               {0, 0, 0, 0, 0, 0, 0, 0},
               {0, 0, 0, 0, 0, 0, 0, 0}});

    REQUIRE(bm3.row_space_basis() == bm4);
    REQUIRE(bm4.row_space_basis() == bm4);

    BMat8 bm5(0xff00000000000000);

    uint64_t data = 0xffffffffffffffff;

    for (size_t i = 0; i < 7; ++i) {
      REQUIRE(BMat8(data).row_space_basis() == bm5);
      data = data >> 8;
    }

    for (size_t i = 0; i < 1000; ++i) {
      bm = BMat8::random();
      REQUIRE(bm.row_space_basis().row_space_basis() == bm.row_space_basis());
    }
  }

  LIBSEMIGROUPS_TEST_CASE("BMat8", "009", "col space basis", "[quick]") {
    auto  rg = ReportGuard(REPORT);
    BMat8 bm({{0, 1, 1, 1, 0, 1, 0, 1},
              {0, 0, 0, 0, 0, 0, 0, 1},
              {1, 1, 1, 1, 1, 1, 0, 1},
              {1, 1, 0, 1, 1, 1, 1, 1},
              {0, 0, 1, 0, 0, 1, 1, 1},
              {1, 1, 0, 0, 0, 0, 0, 1},
              {0, 1, 0, 0, 0, 0, 1, 1},
              {0, 1, 1, 1, 1, 0, 1, 0}});

    BMat8 bm2({{1, 1, 1, 1, 1, 0, 0, 0},
               {1, 0, 0, 0, 0, 0, 0, 0},
               {1, 1, 1, 1, 1, 1, 1, 0},
               {1, 1, 1, 1, 0, 1, 1, 1},
               {1, 1, 0, 0, 1, 0, 0, 1},
               {1, 0, 1, 0, 0, 1, 0, 0},
               {1, 0, 1, 0, 0, 0, 0, 1},
               {0, 0, 1, 1, 1, 0, 1, 1}});

    REQUIRE(bm.col_space_basis() == bm2);

    BMat8 bm3({{1, 1, 1, 1, 0, 1, 0, 1},
               {0, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 1, 0, 0, 1, 0, 1},
               {1, 1, 0, 0, 0, 1, 1, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {1, 0, 0, 0, 0, 1, 0, 0}});

    BMat8 bm4({{1, 1, 1, 0, 0, 0, 0, 0},
               {1, 1, 0, 1, 0, 0, 0, 0},
               {1, 1, 1, 1, 0, 0, 0, 0},
               {1, 1, 1, 1, 0, 0, 0, 0},
               {1, 0, 1, 0, 0, 0, 0, 0},
               {0, 0, 1, 0, 1, 0, 0, 0},
               {0, 0, 0, 0, 1, 0, 0, 0},
               {0, 0, 1, 0, 0, 0, 0, 0}});

    REQUIRE(bm3.col_space_basis() == bm4);

    uint64_t col = 0x8080808080808080;
    BMat8    bm5(col);

    uint64_t data = 0xffffffffffffffff;

    for (size_t i = 0; i < 7; ++i) {
      REQUIRE(BMat8(data).col_space_basis() == bm5);
      data &= ~(col >> i);
    }

    for (size_t i = 0; i < 1000; ++i) {
      bm = BMat8::random();
      REQUIRE(bm.col_space_basis().col_space_basis() == bm.col_space_basis());
    }
  }

  LIBSEMIGROUPS_TEST_CASE("BMat8", "010", "row space basis", "[quick]") {
    auto                     rg = ReportGuard(REPORT);
    detail::Timer            t;
    const std::vector<BMat8> gens
        = {BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};

    int lg = 0;
    using std::unordered_set;
    unordered_set<BMat8> res;
    res.insert(BMat8::one().row_space_basis());

    std::vector<BMat8> todo, newtodo;
    todo.push_back(BMat8::one().row_space_basis());
    while (todo.size()) {
      newtodo.clear();
      lg++;
      for (auto v : todo) {
        for (auto g : gens) {
          auto el = (v * g).row_space_basis();
          if (res.insert(el).second)
            newtodo.push_back(el);
        }
      }
      std::swap(todo, newtodo);
      // std::cout << lg << ", todo = " << todo.size() << ", res = " <<
      // res.size()
      //     << ", #Bucks = " << res.bucket_count() << std::endl;
    }
    // std::cout << "res =  " << res.size() << std::endl;
    // std::cout << t;
  }

  LIBSEMIGROUPS_TEST_CASE("BMat8", "011", "nr_rows, nr_cols", "[quick]") {
    auto  rg    = ReportGuard(REPORT);
    BMat8 idem1 = BMat8::one();
    BMat8 idem2 = BMat8::one();
    BMat8 one   = BMat8::one();

    REQUIRE(one.nr_rows() == 8);
    REQUIRE(bmat8_helpers::nr_cols(one) == 8);
    for (size_t i = 0; i < 7; ++i) {
      idem1.set(i, i, false);
      idem2.set(7 - i, 7 - i, false);

      REQUIRE(idem1.nr_rows() == 7 - i);
      REQUIRE(bmat8_helpers::nr_cols(idem1) == 7 - i);
      REQUIRE(idem2.nr_rows() == 7 - i);
      REQUIRE(bmat8_helpers::nr_cols(idem2) == 7 - i);
    }

    const std::vector<BMat8> gens
        = {BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};

    FroidurePin<BMat8> S(gens);

    for (auto it = S.begin(); it < S.end(); it++) {
      REQUIRE((*it).nr_rows() <= 8);
      REQUIRE((*it).nr_rows() <= 8);

      REQUIRE((*it).row_space_basis().nr_rows() <= (*it).nr_rows());
      REQUIRE(bmat8_helpers::nr_cols((*it).col_space_basis())
              <= bmat8_helpers::nr_cols(*it));
    }
  }

  LIBSEMIGROUPS_TEST_CASE("BMat8", "012", "row_space, col_space", "[quick]") {
    auto  rg    = ReportGuard(REPORT);
    BMat8 idem1 = BMat8::one();
    BMat8 idem2 = BMat8::one();
    BMat8 one   = BMat8::one();

    REQUIRE(one.row_space_size() == 256);
    REQUIRE(bmat8_helpers::col_space_size(one) == 256);
    for (size_t i = 0; i < 8; ++i) {
      idem1.set(7 - i, 7 - i, false);
      idem2.set(i, i, false);

      REQUIRE(idem1.row_space_size() == pow(2, 7 - i));
      REQUIRE(bmat8_helpers::col_space_size(idem1) == pow(2, 7 - i));
      REQUIRE(idem2.row_space_size() == pow(2, 7 - i));
      REQUIRE(bmat8_helpers::col_space_size(idem2) == pow(2, 7 - i));
    }

    const std::vector<BMat8> gens
        = {BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 1}, {1, 0, 1, 0}, {1, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};

    FroidurePin<BMat8> S(gens);

    for (auto it = S.begin(); it < S.end(); it++) {
      BMat8 x    = *it;
      BMat8 rows = x.row_space_basis();
      BMat8 cols = x.col_space_basis();
      REQUIRE(x.row_space_size() <= 16);
      REQUIRE(bmat8_helpers::col_space_size(x) <= 16);

      REQUIRE(rows.row_space_size() <= pow(2, rows.nr_rows()));
      REQUIRE(bmat8_helpers::col_space_size(cols)
              <= pow(2, bmat8_helpers::nr_cols(cols)));

      for (auto it2 = S.begin(); it2 < S.end(); it2++) {
        BMat8 y = *it2;
        REQUIRE((x * y).row_space_size() <= x.row_space_size());
        REQUIRE(bmat8_helpers::col_space_size(x * y)
                <= bmat8_helpers::col_space_size(x));
      }
    }

    BMat8 bm1({{0, 0, 0, 1, 0, 0, 1, 1},
               {0, 0, 1, 0, 0, 1, 0, 1},
               {1, 1, 0, 0, 1, 1, 0, 1},
               {1, 1, 0, 0, 0, 0, 0, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {0, 1, 0, 1, 1, 1, 1, 1},
               {0, 1, 0, 1, 0, 1, 0, 1},
               {0, 1, 0, 0, 0, 0, 1, 0}});
    BMat8 bmm1({{1, 1, 0, 1, 0, 1, 1, 1},
                {1, 1, 1, 1, 1, 1, 1, 1},
                {1, 1, 1, 1, 1, 1, 1, 1},
                {1, 1, 1, 1, 1, 1, 1, 1},
                {1, 1, 0, 1, 1, 1, 1, 1},
                {0, 1, 1, 1, 0, 1, 1, 1},
                {0, 1, 1, 1, 0, 1, 1, 1},
                {1, 1, 1, 1, 1, 1, 1, 1}});
    BMat8 bm2({{1, 1}, {0, 1}});
    BMat8 bm2t({{1, 0}, {1, 1}});
    BMat8 bm3({{0, 0, 0, 1, 0, 0, 1, 1},
               {1, 1, 1, 1, 1, 1, 0, 1},
               {0, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 0, 1, 1, 1, 1, 1},
               {0, 0, 1, 0, 0, 1, 1, 1},
               {1, 1, 0, 0, 0, 0, 0, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {0, 1, 1, 1, 1, 0, 1, 0}});
    BMat8 bm3t({{0, 1, 0, 1, 0, 1, 0, 0},
                {0, 1, 1, 1, 0, 1, 1, 1},
                {0, 1, 1, 0, 1, 0, 0, 1},
                {1, 1, 1, 1, 0, 0, 0, 1},
                {0, 1, 1, 1, 0, 0, 0, 1},
                {0, 1, 1, 1, 1, 0, 0, 0},
                {1, 0, 0, 1, 1, 0, 1, 1},
                {1, 1, 1, 1, 1, 1, 1, 0}});
    BMat8 bm({{0, 0, 0, 1, 0, 0, 1, 1},
              {1, 1, 1, 1, 1, 1, 0, 1},
              {0, 1, 1, 1, 0, 1, 0, 1},
              {1, 1, 0, 1, 1, 1, 1, 1},
              {0, 0, 1, 0, 0, 1, 1, 1},
              {1, 1, 0, 0, 0, 0, 0, 1},
              {0, 1, 0, 0, 0, 0, 1, 1},
              {0, 1, 1, 1, 1, 0, 1, 0}});

    REQUIRE(22 == bm.row_space_size());
    REQUIRE(31 == bm1.row_space_size());
    REQUIRE(6 == bmm1.row_space_size());
    REQUIRE(3 == bm2.row_space_size());
    REQUIRE(3 == bm2t.row_space_size());
    REQUIRE(21 == bm3.row_space_size());
    REQUIRE(21 == bm3t.row_space_size());
  }

  LIBSEMIGROUPS_TEST_CASE("BMat8", "013", "rows", "[quick]") {
    auto  rg   = ReportGuard(REPORT);
    BMat8 idem = BMat8::one();
    BMat8 one  = BMat8::one();

    std::vector<uint8_t> rows = one.rows();
    REQUIRE(std::set<uint8_t>(rows.begin(), rows.end()).size() == 8);
    for (size_t i = 0; i < 8; ++i) {
      idem.set(7 - i, 7 - i, false);
      rows = idem.rows();
      REQUIRE(std::set<uint8_t>(rows.begin(), rows.end()).size() == 8 - i);
    }

    const std::vector<BMat8> gens
        = {BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 1}, {1, 0, 1, 0}, {1, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};

    FroidurePin<BMat8> S(gens);

    for (auto it = S.begin(); it < S.end(); it++) {
      BMat8 x                         = *it;
      rows                            = x.rows();
      std::vector<uint8_t> basis_rows = x.row_space_basis().rows();
      for (uint8_t row : basis_rows) {
        REQUIRE((row == 0
                 || std::find(rows.begin(), rows.end(), row) != rows.end()));
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("BMat8", "014", "one", "[quick]") {
    auto rg = ReportGuard(REPORT);
    for (size_t i = 1; i <= 8; ++i) {
      BMat8 x = bmat8_helpers::one<BMat8>(i);
      REQUIRE(x * x == x);
      REQUIRE(bmat8_helpers::minimum_dim(x) == i);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("BMat8", "015", "vector constructor", "[quick]") {
    auto  rg = ReportGuard(REPORT);
    BMat8 zero(0);
    REQUIRE(BMat8({{0, 0}, {0, 0}}) == zero);
    REQUIRE(BMat8({{0, 0}, {0, 1}}) != zero);
    REQUIRE(BMat8({{0, 0}, {0, 1}}) == BMat8(uint64_t(1) << 54));

    REQUIRE_THROWS_AS(BMat8({{0, 0}}), LibsemigroupsException);
    REQUIRE_THROWS_AS(BMat8({{0, 1}}), LibsemigroupsException);
    REQUIRE_THROWS_AS(BMat8({{0}, {0}}), LibsemigroupsException);
    REQUIRE_THROWS_AS(BMat8({{1, 0}, {0}}), LibsemigroupsException);
    REQUIRE_THROWS_AS(BMat8({{0, 0, 0, 1, 0, 0, 1, 1, 0},
                             {1, 1, 1, 1, 1, 1, 0, 1, 0},
                             {0, 1, 1, 1, 0, 1, 0, 1, 0},
                             {1, 1, 0, 1, 1, 1, 1, 1, 0},
                             {0, 0, 1, 0, 0, 1, 1, 1, 0},
                             {1, 1, 0, 0, 0, 0, 0, 1, 0},
                             {1, 1, 0, 0, 0, 0, 0, 1, 0},
                             {0, 1, 0, 0, 0, 0, 1, 1, 0},
                             {0, 1, 1, 1, 1, 0, 1, 0, 0}}),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(BMat8({{0, 0, 0, 1, 0, 0, 1, 1, 0},
                             {1, 1, 1, 1, 1, 1, 0, 1, 0},
                             {0, 1, 1, 1, 0, 1, 0, 1, 0},
                             {1, 1, 0, 1, 1, 1, 1, 1, 0},
                             {0, 0, 1, 0, 0, 1, 1, 1, 0},
                             {1, 1, 0, 0, 0, 0, 0, 1, 0},
                             {0, 1, 0, 0, 0, 0, 1, 1, 0},
                             {0, 1, 1, 1, 1, 0, 1, 0, 0}}),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("BMat8", "016", "operator<", "[quick]") {
    auto  rg = ReportGuard(REPORT);
    BMat8 bm1(0);
    BMat8 bm2({{0, 0, 0, 1, 0, 0, 1, 1},
               {1, 1, 1, 1, 1, 1, 0, 1},
               {0, 1, 1, 1, 0, 1, 0, 1},
               {1, 1, 0, 1, 1, 1, 1, 1},
               {0, 0, 1, 0, 0, 1, 1, 1},
               {1, 1, 0, 0, 0, 0, 0, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {0, 1, 1, 1, 1, 0, 1, 0}});
    BMat8 bm3({{1, 1}, {0, 1}});
    REQUIRE(!(bm1 < bm1));
    REQUIRE(!(bm2 < bm2));
    REQUIRE(!(bm3 < bm3));
    REQUIRE(bm1 < bm2);
    REQUIRE(bm2 < bm3);
    REQUIRE(bm1 < bm3);
    REQUIRE(!(bm2 < bm1));
    REQUIRE(!(bm3 < bm2));
    REQUIRE(!(bm3 < bm1));
  }

  LIBSEMIGROUPS_TEST_CASE("BMat8", "017", "adapters", "[quick]") {
    auto  rg = ReportGuard(REPORT);
    BMat8 bm1(0);
    REQUIRE(Complexity<BMat8>()(bm1) == 0);
    REQUIRE(Degree<BMat8>()(bm1) == 8);
    REQUIRE_NOTHROW(IncreaseDegree<BMat8>()(bm1));
    REQUIRE(One<BMat8>()(bm1) == bm1.one());
    REQUIRE(One<BMat8>()(4) == bmat8_helpers::one<BMat8>(4));

    BMat8 bm2 = BMat8::random();
    BMat8 bm3, bm4;
    Product<BMat8>()(bm3, bm2, bm2.one(8));
    REQUIRE(bm3 == bm2);
    Product<BMat8>()(bm3, bm2.one(8), bm2);
    REQUIRE(bm3 == bm2);
    Product<BMat8>()(bm3, bm2, bm2);
    REQUIRE(bm3 == bm2 * bm2);

    ImageRightAction<BMat8, BMat8>()(bm3, bm2, bm2.one(8));
    REQUIRE(bm3 == bm2.row_space_basis());

    ImageRightAction<BMat8, BMat8>()(bm3, bm2.one(8), bm2);
    REQUIRE(bm3 == bm2.row_space_basis());

    ImageLeftAction<BMat8, BMat8>()(bm3, bm2, bm2.one(8));
    REQUIRE(bm3 == bm2.col_space_basis());

    ImageLeftAction<BMat8, BMat8>()(bm3, bm2.one(8), bm2);
    REQUIRE(bm3 == bm2.col_space_basis());

    ImageRightAction<BMat8, BMat8>()(bm3, bm2, bm2);
    ImageLeftAction<BMat8, BMat8>()(bm4, bm2.transpose(), bm2.transpose());
    REQUIRE(bm3 == bm4.transpose());

    REQUIRE(Inverse<BMat8>()(bm2.one(8)) == bm2.one(8));
    BMat8 bm5({{0, 1, 0, 0, 0, 0, 0, 0},
               {1, 0, 0, 0, 0, 0, 0, 0},
               {0, 0, 0, 1, 0, 0, 0, 0},
               {0, 0, 1, 0, 0, 0, 0, 0},
               {0, 0, 0, 0, 1, 0, 0, 0},
               {0, 0, 0, 0, 0, 1, 0, 0},
               {0, 0, 0, 0, 0, 0, 1, 0},
               {0, 0, 0, 0, 0, 0, 0, 1}});
    REQUIRE(Inverse<BMat8>()(bm5) == bm5);
    BMat8 bm6({{0, 1, 0, 0, 0, 0, 0, 0},
               {0, 0, 1, 0, 0, 0, 0, 0},
               {1, 0, 0, 0, 0, 0, 0, 0},
               {0, 0, 0, 1, 0, 0, 0, 0},
               {0, 0, 0, 0, 1, 0, 0, 0},
               {0, 0, 0, 0, 0, 1, 0, 0},
               {0, 0, 0, 0, 0, 0, 1, 0},
               {0, 0, 0, 0, 0, 0, 0, 1}});
    REQUIRE(Inverse<BMat8>()(bm6) == bm6 * bm6);
  }

  LIBSEMIGROUPS_TEST_CASE("BMat8", "018", "one", "[quick]") {
    BMat8 bm5({{1, 0, 0, 0, 0, 0, 0, 0},
               {0, 1, 0, 0, 0, 0, 0, 0},
               {0, 0, 1, 0, 0, 0, 0, 0},
               {0, 0, 0, 1, 0, 0, 0, 0},
               {0, 0, 0, 0, 1, 0, 0, 0},
               {0, 0, 0, 0, 0, 0, 0, 0},
               {0, 0, 0, 0, 0, 0, 0, 0},
               {0, 0, 0, 0, 0, 0, 0, 0}});
    REQUIRE(BMat8::one(5) == bm5);
    REQUIRE(BMat8::one(0) == BMat8(0));
    REQUIRE(BMat8::one(8) == BMat8::one());
  }

}  // namespace libsemigroups
